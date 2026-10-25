"""Byte streams between a fake node and the firmware under test.

Every transport has write(data, t_ns), read() -> bytes (never blocks) and
close(). A pseudo-terminal, a TCP serial port (QEMU) and a UDP socket (the
sync link) all fit, so a node doesn't know what runs the firmware."""

import os
import select
import socket
import time
from pathlib import Path


STALL_S = 0.05  # how long a stalled reader is waited for, once


class _Writer:
    """Non-blocking writes. A reader that stops reading loses bytes, as a UART
    would (overrun), instead of freezing the simulation: the first write that
    can't go out waits STALL_S, then writes drop at once until the reader
    catches up."""

    dropped = 0  # bytes lost because the reader stalled
    stalled = False

    def _send_all(self, data, send, waitable):
        view = memoryview(data)
        deadline = time.monotonic() + (0 if self.stalled else STALL_S)
        while view:
            try:
                view = view[send(view):]
            except BlockingIOError:
                left = deadline - time.monotonic()
                if left <= 0:
                    self.stalled = True
                    self.dropped += len(view)
                    return
                select.select([], [waitable], [], min(left, 0.005))
        self.stalled = False


class MemoryTransport:
    """For tests: records (time_ns, bytes) for every write; inbox is what the
    node will read next."""

    def __init__(self):
        self.writes = []
        self.inbox = b""

    def write(self, data, t_ns):
        self.writes.append((t_ns, bytes(data)))

    def read(self):
        data, self.inbox = self.inbox, b""
        return data

    def close(self):
        pass

    @property
    def stream(self):
        return b"".join(d for _, d in self.writes)


class CaptureTransport(MemoryTransport):
    """Writes the byte stream to a file on close: golden frames for unit tests."""

    def __init__(self, path):
        super().__init__()
        self.path = path

    def close(self):
        Path(self.path).write_bytes(self.stream)


class FdTransport(_Writer):
    """A file descriptor: native_sim's UART pseudo-terminal, a serial port, a
    FIFO. A terminal is put in raw mode so no byte is rewritten."""

    def __init__(self, path):
        self.fd = os.open(path, os.O_RDWR | os.O_NONBLOCK | os.O_NOCTTY)
        if os.isatty(self.fd):
            import tty
            tty.setraw(self.fd)

    def write(self, data, t_ns):
        self._send_all(data, lambda v: os.write(self.fd, v), self.fd)

    def read(self):
        try:
            return os.read(self.fd, 4096)
        except (BlockingIOError, OSError):
            return b""

    def close(self):
        os.close(self.fd)


class TcpTransport(_Writer):
    """Connects to a serial port that something else serves, e.g. QEMU's
    `-serial tcp::5001,server,nowait`. Works on every host OS."""

    def __init__(self, address):
        host, _, port = address.rpartition(":")
        self.sock = socket.create_connection((host or "127.0.0.1", int(port)))
        self.sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
        self.sock.setblocking(False)

    def write(self, data, t_ns):
        self._send_all(data, self.sock.send, self.sock)

    def read(self):
        try:
            return self.sock.recv(4096)
        except (BlockingIOError, OSError):
            return b""

    def close(self):
        self.sock.close()


class UdpTransport:
    """The sync link: one datagram per MAVLink frame (ICD-sync-link)."""

    def __init__(self, local_port, peer_host, peer_port):
        self.peer = (peer_host, int(peer_port))
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind(("0.0.0.0", int(local_port)))
        self.sock.setblocking(False)

    def write(self, data, t_ns):
        self.sock.sendto(bytes(data), self.peer)

    def read(self):
        out = b""
        while True:
            try:
                out += self.sock.recv(2048)
            except (BlockingIOError, OSError):
                return out

    def close(self):
        self.sock.close()


def open_transport(spec):
    """tcp:HOST:PORT | udp:LOCAL_PORT:PEER_HOST:PEER_PORT | a device path."""
    if spec.startswith("tcp:"):
        return TcpTransport(spec[4:])
    if spec.startswith("udp:"):
        local, host, port = spec[4:].split(":")
        return UdpTransport(local, host, port)
    return FdTransport(spec)
