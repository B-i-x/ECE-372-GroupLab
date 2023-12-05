#ifndef PWM_H
#define PWM_H

void initPWMTimer3();

void changeDutyCycle(double dutycycle1);

void analyzeADC();

void setServoAngle(int angle);
#endif