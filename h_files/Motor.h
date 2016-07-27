/* Motor.h */
#ifndef _MOTOR_H_
#define _MOTOR_H_

extern int cup;
extern float get_angle0, get_angle1;
extern float x0,y0,tht0;
extern float x1,y1,tht1;
extern float timer;

void SwitchMotorCtrl(int flag);
void PIDCtrl_R(float target, float velosity);
void PIDCtrl_L(float target, float velosity);
void Turn(float angle, int speed);
void SearchTurn_r();
void SearchTurn_l();
void BallApproach();

#endif //_MOTOR_H_
