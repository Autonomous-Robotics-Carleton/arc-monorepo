# Flashing the motor controllers, and recovering one

- **Owner:** TBD
- **Status:** Draft procedure; not yet followed on hardware ([#181](https://github.com/Autonomous-Robotics-Carleton/arc-monorepo/issues/181))
- **Retires:** RSK-20 (a build that breaks uploads can only be fixed over SWD)
- **Traces to:** SYS-05, SYS-13, ADR-0033, ADR-0034, ADR-0035
- **Read first:** [`firmware/vesc/README.md`](../../firmware/vesc/README.md), [A50S arrival](a50s-arrival.md) (the first flash of a new unit, and proving SWD recovery on a spare)

How our VESC firmware goes onto the four A50S controllers after they're installed, and how a controller that no longer takes uploads is recovered. The four controllers drive the wheels, and the e-stop brake and the command timeout run in this firmware, so every flash ends with those checked again.

## Before flashing

1. **The build:** a `.bin` from `npx nx build vesc` on a commit that's merged to `main` (`bldc/build/a50s_v23c_12s/a50s_v23c_12s.bin`), with `npx nx test vesc` passing on that commit. Record the commit.
2. **The car:** on a stand with every wheel clear of the ground, or the controller on the R2 bench with its motor unloaded. Nobody near the wheels.
3. **Power:** the controllers need logic power to take an upload. On the bench, a fused, current-limited supply (setting TBD, as in the [arrival procedure](a50s-arrival.md)). On the car, the motor bus is on; how to power the controllers with the e-stop pressed depends on the Aux supply (TBD, ICD-corner-connector).
4. **Back up each controller's configuration** with VESC Tool (motor and app configuration), named by the controller's label. A firmware whose configuration layout changed loads the defaults instead of the stored values: the backup, and the motor's row in [`firmware/vesc/motors/`](../../firmware/vesc/motors/README.md), are how it's put back.

## Flashing

One controller at a time, the first one fully checked before the next.

1. Connect the controller to VESC Tool over USB and note the firmware version it reports.
2. Firmware page → the custom-file tab → the `.bin` from step 1 → upload. Don't disconnect or power down while it uploads; the controller restarts at the end.
3. Reconnect and confirm the firmware version changed to the new build (with its git commit once the controller reports one, #160).
4. Run the checks below on this controller before flashing the next. Flashing several at once over CAN (VESC Tool can) waits until this procedure has been followed on hardware.

## Checks after every flash

| Check | How | Pass |
| --- | --- | --- |
| Configuration | Read the motor configuration in VESC Tool | It matches the backup and the motor's row in `motors.csv`; if not, load them (`firmware/vesc/motors/README.md`) |
| Speed cap (ADR-0034) | Read the eRPM limits in Motor Settings | Neither is above the cap (75k eRPM, TBC); our firmware clamps them on every control-loop tick |
| E-stop brake (ADR-0012) | With the wheel clear, spin it slowly from VESC Tool, then press the e-stop | Braked on the ramp, no coasting; commands ignored while pressed |
| E-stop release (ADR-0012 item 4) | Release the e-stop, send nothing | The wheel stays braked until a fresh command |
| Command timeout (ADR-0035, once #221 is merged) | Spin it slowly from VESC Tool, then stop sending (close VESC Tool's connection, which also stops its keepalive) | Braked on the ramp within ~150 ms (TBC) |

Record each controller's result in the table at the end. A controller that fails any check is taken off the car until the cause is known.

## Recovering a controller over SWD

A controller that no longer connects over USB or CAN after a flash (RSK-20): the running firmware receives uploads, so a build that breaks them can't upload its own fix.

1. Power it as in *Before flashing*, with the motor disconnected or unloaded.
2. Connect an ST-Link to the SWD contacts on the A50S's 20-pin connector (Triforce's pinout; contact numbers are confirmed in the [arrival procedure](a50s-arrival.md)).
3. Write a known-good build to the controller over SWD. The exact command (upstream's Makefile flash target or OpenOCD, and which image: the `.bin` VESC Tool uploads or the full image with upstream's bootloader) is **TBD**: settle it when SWD recovery is first demonstrated on a spare in the arrival procedure, and write it here.
4. Reconnect over USB, restore the configuration from the backup, and run every check above.

## Results

| Date | Controller | From version | To version | Checks | Who |
| --- | --- | --- | --- | --- | --- |
| TBD | TBD | TBD | TBD | Not run | TBD |
