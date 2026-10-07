/*
 * Vehicle-state sensors: IMUs, wheel, suspension and knuckle encoders
 * (SPI), ride-height ToF (I2C), each timestamped at the pin in the time
 * base (SYS-06, SYS-07, SYS-24, ADR-0008, ADR-0021). Stub.
 */

#ifndef ARC_SENSORS_H
#define ARC_SENSORS_H

int sensors_init(void);

#endif /* ARC_SENSORS_H */
