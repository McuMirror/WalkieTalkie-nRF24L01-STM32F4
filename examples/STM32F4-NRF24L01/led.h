#ifndef LED_H
#define LED_H

uint16_t pwm[2];

void initLed();
void initBlink(void);
void ledTurnOn(char *diode);
void ledTurnOff(char *diode);

#endif
