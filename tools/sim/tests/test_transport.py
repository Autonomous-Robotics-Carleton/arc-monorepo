"""Tests for the transports (real pseudo-terminals and sockets, no firmware)."""

import os
import socket
import time
import unittest

from arcsim.transport import FdTransport, TcpTransport, UdpTransport, open_transport


class Pty(unittest.TestCase):
    def test_bytes_pass_unchanged_in_raw_mode(self):
        master, slave = os.openpty()
        t = FdTransport(os.ttyname(slave))
        payload = bytes(range(256))  # includes CR, LF, XON, XOFF, NUL
        t.write(payload, 0)
        time.sleep(0.05)
        self.assertEqual(os.read(master, 4096), payload)
        os.write(master, b"\xfd\x00\x0d\x0a")
        time.sleep(0.05)
        self.assertEqual(t.read(), b"\xfd\x00\x0d\x0a")
        t.close()

    def test_a_reader_that_stops_reading_loses_bytes_but_does_not_hang(self):
        master, slave = os.openpty()
        t = FdTransport(os.ttyname(slave))
        start = time.monotonic()
        for _ in range(2000):  # far more than a pty buffers
            t.write(b"x" * 56, 0)
        self.assertLess(time.monotonic() - start, 5)
        self.assertGreater(t.dropped, 0)
        t.close()


class Sockets(unittest.TestCase):
    def test_udp_one_datagram_per_write(self):
        peer = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        peer.bind(("127.0.0.1", 0))
        peer.settimeout(1)
        t = UdpTransport(0, "127.0.0.1", peer.getsockname()[1])
        t.write(b"abc", 0)
        t.write(b"de", 0)
        self.assertEqual([peer.recv(100), peer.recv(100)], [b"abc", b"de"])
        t.sock.close()

    def test_tcp_serial_port_as_qemu_serves_it(self):
        server = socket.socket()
        server.bind(("127.0.0.1", 0))
        server.listen(1)
        t = TcpTransport(f"127.0.0.1:{server.getsockname()[1]}")
        conn, _ = server.accept()
        t.write(b"hello", 0)
        conn.settimeout(1)
        self.assertEqual(conn.recv(10), b"hello")
        conn.sendall(b"back")
        time.sleep(0.05)
        self.assertEqual(t.read(), b"back")
        t.close()
        conn.close()
        server.close()

    def test_specs(self):
        server = socket.socket()
        server.bind(("127.0.0.1", 0))
        server.listen(1)
        t = open_transport(f"tcp:127.0.0.1:{server.getsockname()[1]}")
        self.assertIsInstance(t, TcpTransport)
        t.close()
        server.close()


if __name__ == "__main__":
    unittest.main()
