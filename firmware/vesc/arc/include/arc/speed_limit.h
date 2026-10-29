// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Speed limit (ADR-0034): the maximum eRPM, ~75k (TBC), keeps the motor
 * where the controller's control loop has >= 10 samples per electrical
 * cycle (RSK-18). The firmware clamps the configured eRPM limits
 * (l_min_erpm, l_max_erpm) to it, whatever VESC Tool or a configuration
 * message sets. Upstream's limit is a soft one: it cuts back the motor
 * current from l_erpm_start x the limit to zero at the limit, and truncates
 * speed-mode setpoints. It doesn't brake, so it can't stop overspeed driven
 * from outside (rolling downhill, being pushed). How the motor behaves at the
 * limit is confirmed by the sweep on rig R2.
 */

#ifndef ARC_SPEED_LIMIT_H
#define ARC_SPEED_LIMIT_H

/*
 * The largest |eRPM| allowed. TBC (ADR-0034, RSK-18): 10 control-loop
 * samples per electrical cycle at the A50S's 12.5 kHz loop. A limit on motor
 * speed, so it holds at any gearing; confirmed or changed by the sweep on
 * rig R2. The same value as erpm_max in config/cars/nominal.yaml.
 */
#define ARC_SPEED_LIMIT_ERPM_TBC 75000.0f

/* The largest |eRPM| allowed: ARC_SPEED_LIMIT_ERPM_TBC. */
float arc_speed_limit_erpm(void);

/*
 * Clamps a configured pair of eRPM limits (upstream's l_min_erpm, negative,
 * and l_max_erpm) into [-limit, limit]. A tighter configuration is kept; a
 * looser one, an infinity or a NaN becomes the limit.
 */
void arc_speed_limit_apply(float *min_erpm, float *max_erpm);

#endif /* ARC_SPEED_LIMIT_H */
