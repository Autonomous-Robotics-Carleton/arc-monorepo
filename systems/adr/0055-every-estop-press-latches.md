# ADR-0055: Every e-stop press latches the stop, fault or not

- **Status:** Accepted (2026-10-30)
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

**The latch**

1. **What latches.** The sync MCU latches when its e-stop status input shows the button pressed. It also latches when any corner reports the e-stop (`ESTOP`, ADR-0035), as a backstop for a press its input misses; a corner reporting the e-stop is already a SYS-25 trigger, so the backstop adds nothing new to watch. Once latched, it refuses drive commands, as for a fault stop (ADR-0039 item 1).
2. **Any press latches, however short.** ADR-0053's minimum press, held until the motor-bus cut at T, applies to resetting a fault stop, not to this latch. The input's debounce is TBD.

**What a press is, and its cause**

3. **A press is not a fault.** While the e-stop status input shows the button pressed, corners reporting the e-stop are expected. The latch's cause is the e-stop (`ARC_LATCH_CAUSE_ESTOP`, ICD-sync-link), not a corner (SYS-25).
4. **A corner reporting the e-stop while the input shows the button released** is an e-stop wiring fault, as ADR-0035 and ADR-0053 already record (`ARC_LATCH_CAUSE_ESTOP_WIRING`, a restart under ADR-0039 item 5).
5. **A fault latch already active takes precedence.** If the car is latched on a fault when the button is pressed, the fault stays the reported cause and still needs its reset: the press held past T, then the checks (ADR-0039, ADR-0053). A shorter press doesn't clear it.

**Resuming**

6. **Driving resumes only after all three:**
   - the button is released;
   - the power-on checks pass (ADR-0039 item 3), within ADR-0053's check window (3 s, TBC) from the release;
   - the operator re-arms from the laptop (`ARC_REARM`).
7. **Releasing the button is the e-stop cycle.** An e-stop latch never needs a separate cycle, because the sync MCU already boots latched (ADR-0039 item 1, ADR-0052). That includes a button held at power-on: its release is the cycle.
8. **After the re-arm,** the first commands are zero and the envelope is back at its 3 m/s default (SYS-22), the same as after a fault reset (ADR-0039 item 4).

The hardware stop doesn't change: the controllers still brake on the ramp, and the motor bus is still cut at T (SYS-05, ADR-0012). ADR-0012 item 4 still holds on each controller. Driving can't resume early because the sync MCU sends no drive command until the re-arm.

## Consequences

- **The sync MCU must read the e-stop status input** (ADR-0039 item 2) and latch on it, not only on SYS-25's triggers. With the corners' reports as a backstop (item 1), a press is caught if either the input or any corner sees it.
- **The latch is a safety function** (ADR-0039): its own tests and a second reviewer. Its implementation is #148.
- **A press doesn't count as a fault.** As ADR-0053 already records for corners reporting the e-stop, a press doesn't count toward ADR-0039's "three SYS-25 stops since power-on".
- **The envelope resets on every re-arm from a latch.** An experiment that raised it has to raise it again after an e-stop.
- **Steering:** the sync MCU returns the steering to centre during the stop and holds it there (ADR-0019). It doesn't steer again until the re-arm.
- **A forged re-arm.** Once the button is released and the checks pass, a re-arm sent from the service port could restart the car. That is the same exposure as after a fault reset (ADR-0039) and adds weight to ICD-sync-link open issue 6 (who the sync MCU accepts messages from).
- **ICD-sync-link** describes `ARC_LATCH_CHECKING` as "released after T" and `ARC_LATCH_ESTOP_HELD` as waiting for the cut at T. Releasing an e-stop latch before T now also leads to the checks (item 6), so both descriptions need updating. The enum values themselves are unchanged.
- **The twin** already starts every run latched (ADR-0053). Its emulated e-stop input now has to latch the car mid-run too, so the scenarios for SYS-05 cover the re-arm.
- **Reopen if:** a re-arm after every press gets in the way of normal runs. In that case, fix how the button is being used rather than shorten the latch.

## Open questions

These came up while recording the decisions above. They're not decided here. Each has the owner's proposed answer, chosen on the strict side, for Ashwin's review with the rest of this ADR.

1. **Telling a missed press from a wiring fault.** Item 1 latches on a corner's e-stop report that the input missed, and item 4 calls a corner's report with the input showing released a wiring fault. A press too short for the input's debounce looks like the second case. The input and the corners' telemetry also change at slightly different moments on every press and release. How long they may disagree before it counts as a wiring fault (TBD), and whether a missed press is reported as the e-stop or as a wiring fault, are open.
   - *Proposed:* report it as a wiring fault, so it needs the restart of ADR-0039 item 5 and someone looks at the wiring, rather than the lighter re-arm. The car is latched either way. They may disagree for at most TBC 20 ms, several 1 ms corner reports and short of any stop, before it counts. If this makes ordinary presses restart the car, the input's debounce or the window is wrong; don't relax the reporting.
2. **A button held at power-on and the power-on latch.** Item 7 makes the release the cycle. The power-on latch (`ARC_LATCH_CAUSE_POWER_ON`) is reset like a fault stop, with the press held past T (ADR-0039, ADR-0053). Open: must the button have been held at least T from power-on for its release to reset the power-on latch too, or does any release count?
   - *Proposed:* held at least T from power-on, the same rule as resetting a fault stop (ADR-0039, ADR-0053). A shorter hold clears the e-stop latch only; the power-on latch then still needs its own reset.
