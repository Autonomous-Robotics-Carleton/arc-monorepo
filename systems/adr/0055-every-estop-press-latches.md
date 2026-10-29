# ADR-0055: Every e-stop press latches the stop, fault or not

- **Status:** Proposed (2026-10-29). Accepted once Ashwin, who implements the sync MCU's latch (#148), has reviewed it
- **Date:** 2026-10-29
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-05, SYS-22, SYS-25, E-32, ADR-0012, ADR-0019, ADR-0035, ADR-0039, ADR-0052, ADR-0053, ICD-sync-link
- **Amends:** ADR-0039 (a plain e-stop press latches too, not only a SYS-25 fault) and ADR-0012 (releasing the e-stop doesn't let the car drive again until the operator re-arms)

## Context

The e-stop button (E-32) stops the car in hardware: the motor controllers brake on the ramp, then the motor bus is cut at T (SYS-05, ADR-0012). What happens when it's released is set in three places, and none of them covers a plain press:

- **ADR-0012 item 4:** releasing the e-stop doesn't restart motion: each controller waits for fresh commands. The controllers' brake routine (#80) hands control back on the next fresh command.
- **ADR-0039:** the sync MCU latches only on a SYS-25 fault, and an e-stop cycle is how that latch is reset.
- **ADR-0052:** a heartbeat stop is held until the operator re-arms.

So after a press with no fault, nothing on the sync MCU holds the car. An experiment streams commands (ADR-0048). The first one after the release would reach the controllers, and the car would drive off with someone standing at it, the person who just pressed the button.

The owner's position: an e-stop is meant to stop, not to pause.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| A. As today: release, and the next fresh command drives | Nothing to implement | The car can move off as the button is released, with a person at it |
| **B. Every press latches the sync MCU's stop; driving resumes after the release, the power-on checks and the operator's re-arm** | A person at the laptop decides every restart; the same sequence as a fault reset; nothing the experiment streams gets through | One more re-arm after every press |
| C. A press latches like a fault: another e-stop cycle held past T, then the checks and the re-arm | One rule for every latch | A second walk to the car and a second press when nothing is wrong |

## Criteria

1. Releasing the e-stop never restarts motion by itself, whatever is streaming commands.
2. A person decides every restart.
3. The car's hardware stop (SYS-05) still doesn't depend on any software.

## Decision

**Option B.** Every press of the e-stop latches the sync MCU's stop, whether or not there is a fault.

1. **The latch.** When the sync MCU sees the press on its e-stop status input, it latches. The cause is the e-stop (`ARC_LATCH_CAUSE_ESTOP`, ICD-sync-link). From then on it refuses drive commands, as for a fault stop (ADR-0039 item 1).
2. **Any press latches, however short.** ADR-0053's minimum press, held until the motor-bus cut at T, applies to resetting a fault stop, not to this latch.
3. **Driving resumes only after all three:**
   - the button is released;
   - the power-on checks pass (ADR-0039 item 3), within ADR-0053's check window (3 s, TBC) from the release;
   - the operator re-arms from the laptop (`ARC_REARM`).
4. **The first commands after the re-arm are zero,** as ADR-0039 item 4.

The hardware stop doesn't change: the controllers still brake on the ramp, and the motor bus is still cut at T (SYS-05, ADR-0012). ADR-0012 item 4 still holds on each controller. Driving can't resume early because the sync MCU sends no drive command until the re-arm.

## Consequences

- **The sync MCU must read the e-stop status input** (ADR-0039 item 2) and latch on it, not only on SYS-25's triggers. The latch then doesn't depend on the corners' telemetry: a press is caught even if a corner is silent.
- **The latch is a safety function** (ADR-0039): its own tests and a second reviewer. Its implementation is #148.
- **A press doesn't count as a fault.** As ADR-0053 already records for corners reporting the e-stop, a press doesn't count toward ADR-0039's "three SYS-25 stops since power-on".
- **Steering:** the sync MCU returns the steering to centre during the stop and holds it there (ADR-0019). It doesn't steer again until the re-arm.
- **A forged re-arm.** Once the button is released and the checks pass, a re-arm sent from the service port could restart the car. That is the same exposure as after a fault reset (ADR-0039) and adds weight to ICD-sync-link open issue 6 (who the sync MCU accepts messages from).
- **ICD-sync-link** describes `ARC_LATCH_CHECKING` as "released after T" and `ARC_LATCH_ESTOP_HELD` as waiting for the cut at T. With this ADR, releasing a plain e-stop press before T also leads to the checks. The descriptions need updating once open question 1 below is answered. The enum values themselves are unchanged.
- **The twin** already starts every run latched (ADR-0053). Its emulated e-stop input now has to latch the car mid-run too, so the scenarios for SYS-05 cover the re-arm.
- **Reopen if:** a re-arm after every press gets in the way of normal runs. In that case, fix how the button is being used rather than shorten the latch.

## Open questions

1. **A press while a SYS-25 latch is already active, or one that trips SYS-25 itself.** Every corner reports the e-stop during a press, and a corner "reporting e-stop" is a SYS-25 trigger. Two parts are open:
   - Is that SYS-25 stop the press's own stop (cause e-stop, released by this ADR's sequence), or a fault stop whose reset needs the press held past T (ADR-0053)?
   - If a real fault latch is already active and the press is shorter than T, the fault latch stays. What's open is which cause is reported, and in what order.
2. **What the re-arm requires after a plain e-stop latch.** The release of the press that latched the car is the only cycle this ADR asks for. Is a separate e-stop cycle ever needed? For example: the press began before power-on, or the button was already held when the latch began.
3. **The envelope after the re-arm.** After a fault reset, ADR-0039 item 4 returns the envelope to its 3 m/s default (SYS-22). Does a re-arm after a plain e-stop latch do the same, or does it keep the session's envelope?
4. **The shortest press the sync MCU counts.** The debounce on the e-stop status input is TBD. A press that the controllers brake on (their ESTOP inputs) but the sync MCU misses would leave the car unlatched. Open: should the corners' reported e-stop then latch the car as a backstop?
