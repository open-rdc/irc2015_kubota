/* Motor.cpp */
#include "Function.h"
#include "Motor.h"
#include "QEI.h"

#define PI                       3.14159
#define ON                       1
#define OFF                      0
#define CONTROL_PERIOD           0.02    //sec
#define TREAD                    132.0
#define PWM_PERIOD               0.00005 // 20kHz
#define MAX_SPEED                900     // 0.9m/sec
#define KP_R                     3.6
#define KI_R                     0.26
#define KD_R                     0.001
#define KP_L                     3.6
#define KI_L                     0.25
#define KD_L                     0.001
#define RUN_FORWARD              0
#define RUN_BACKWARD     　      1
#define PPR                      100 //Pulse per revolution in encoder
#define INSIDE_GEAR_RATIO        4.4 //gear head
#define OUTSIDE_GEAR_RATIO       5.625 //outsaid gear
#define QEI_X2_ENCODING          2.0
#define WHEEL_RADIUS             25.0
#define HAND_DISTANCE            233.0


/* Define PinNo motor */
PwmOut      MotorRight(p23);
PwmOut      MotorLeft(p21);
DigitalOut  HBridgeRight(p24);
DigitalOut  HBridgeLeft(p22);

/* Define PinNo Encoder */
QEI Pulse_r (p29, p30, NC, 100, QEI::X2_ENCODING);
QEI Pulse_l  (p28, p27, NC, 100, QEI::X2_ENCODING);

/* Global variable */
float theta_r, theta_l;
float theta_old_r = 0.0, theta_old_l = 0.0;
float omega, omega_r, omega_l;
float velocity, velocity_r, velocity_l;
float x0, x1, y0, y1, tht0, tht1;
float get_PSD0, get_PSD1, get_angle0, get_angle1; 
float timer = 0;
int cup = OFF;


/********************************************************************/
/*               モーターPID制御関数                                */
/*               引数: 目標車輪速度,現在の車輪の速度                */
/*       目標車輪速度からPID制御の計算を行いPWM出力する             */
/********************************************************************/
void PIDCtrl_R(float target_speed_r, float velocity_r)
{
    static float e1 = 0, e0 = 0;
    static float ei = 0, ed = 0;
    float calc_velocity;    

    e1 = target_speed_r - velocity_r;
    ei += e1;
    ed = e1 - e0;
    e0 = e1;
    if(target_speed_r == 0) ei = 0, ed = 0;
    calc_velocity = (e1 * KP_R + ei * KI_R + ed * KD_R);

    if(calc_velocity > MAX_SPEED) calc_velocity = MAX_SPEED;
    if(calc_velocity > 0) HBridgeRight = 0; // run forward
    if(calc_velocity < -MAX_SPEED) calc_velocity = -MAX_SPEED;
    if(calc_velocity < 0) HBridgeRight = 1; // run backward

    MotorRight.period(PWM_PERIOD);
    MotorRight = fabs(calc_velocity / MAX_SPEED);
}

void PIDCtrl_L(float target_speed_l, float velocity_l)
{
    static float e1 = 0, e0 = 0;
    static float ei = 0, ed = 0;
    float calc_velocity;

    e1 = target_speed_l - velocity_l;
    ei += e1;
    ed = e1 - e0;
    e0 = e1;
    if(target_speed_l == 0) ei = 0, ed = 0;
    calc_velocity = (e1 * KP_L + ei * KI_L + ed * KD_L);

    if(calc_velocity > MAX_SPEED) calc_velocity = MAX_SPEED;
    if(calc_velocity > 0) HBridgeLeft = 0; // run forward
    if(calc_velocity < -MAX_SPEED) calc_velocity = -MAX_SPEED;
    if(calc_velocity < 0) HBridgeLeft = 1; // run backward

    MotorLeft.period(PWM_PERIOD);
    MotorLeft = fabs(calc_velocity / MAX_SPEED);
}


/********************************************************************/
/*            ロボットを目標角度へ超信地旋回させる関数              */
/*            引数: 目標回転角度,ロボットの回転速度                 */
/********************************************************************/
void Turn(float angle,int speed)
{
    x1 = y1 = tht1 = 0;
    x0 = y0 = tht0 = 0;
    wait(0.1);

    if(angle >= 0)
        while(angle >= (tht1 * 180 / PI))
            target_speed_r = speed, target_speed_l = -speed;
    if(angle <= 0)
        while(angle <= (tht1 * 180 / PI))
            target_speed_r = -speed, target_speed_l = speed;
    target_speed_r = target_speed_l = 0;
    wait(0.1);
}


/********************************************************************/
/*        PSDセンサを用いたボール探索　90度ずつ左右を探索する       */
/*        ボールを見つけた場合、ボールの方向にロボットを回転させる  */
/*        引数: なし                                                */
/********************************************************************/
void SearchTurn_r()
{
    get_PSD0 = get_PSD1 = 0;
    get_angle0 = get_angle1 = 0; 
    x1 = y1 = tht1 = 0;
    x0 = y0 = tht0 = 0;

    wait(0.1);
    while(-86 <= (tht1 * 180 / PI)){
        target_speed_r = -110;
        target_speed_l = 110;
        get_PSD0 = PSD_Value();
        get_angle0 = tht1 * 180 / PI;
        if(get_PSD1 < get_PSD0){
            get_PSD1 = get_PSD0;
            get_angle1 = get_angle0; 
        }
    }
    if(get_PSD1 < 0.205) get_angle1 = 0.0;
    target_speed_r = target_speed_l = 0;
    wait(0.1);
    if(get_angle1 == 0){
        Turn(84, 150);
        x1 = y1 = tht1 = 0;
        x0 = y0 = tht0 = 0;
    }
    else Turn(86 + get_angle1, 160);
}

void SearchTurn_l()
{
    get_PSD0 = get_PSD1 = 0;
    get_angle0 = get_angle1 = 0; 
    x1 = y1 = tht1 = 0;
    x0 = y0 = tht0 = 0;

    wait(0.1);
    while(85 >= (tht1 * 180 / PI)){ 
        target_speed_r = 110;
        target_speed_l = -110;
        get_PSD0 = PSD_Value();
        get_angle0 = tht1 * 180 / PI;
        if(get_PSD1 < get_PSD0){
            get_PSD1 = get_PSD0;
            get_angle1 = get_angle0; 
        }
    }
    if(get_PSD1 < 0.205) get_angle1 = 0.0; 
    target_speed_r = target_speed_l = 0;
    wait(0.1);
    if(get_angle1 == 0){
        Turn(-84, 150);
        x1 = y1 = tht1 = 0;
        x0 = y0 = tht0 = 0;
    }
    else Turn(-84 + get_angle1, 160);
}


/********************************************************************/
/*                    ボールをキャッチする関数                      */
/*                    引数: なし                                    */
/********************************************************************/
void BallApproach()
{
    float diff;
    float balldistance = 66.294 * pow((double)get_PSD1, -1.218);

    x1 = y1 = tht1 = 0;
    x0 = y0 = tht0 = 0;
    diff = balldistance - HAND_DISTANCE;

    if(diff < 0){
        while(x1 >= diff - 20)
            target_speed_r = target_speed_l = -180;
        target_speed_r = target_speed_l = 0;
        ServoForkDown();
        target_speed_r = target_speed_l = 140;
        wait(1.0);
    }
    else{
        ServoForkDown();
        while(x1 <= diff + 15)
            target_speed_r = target_speed_l = 140;
    }
    target_speed_r = target_speed_l = 0;
    if(cup == OFF) ServoForkUp();
    else ServoForkCatch();

    while(x1 >= 0)
        printf(""),target_speed_r = target_speed_l = -160;
    target_speed_r = target_speed_l = 0;
}


/********************************************************************/
/*                          割り込み関数                            */
/*                          引数: なし                              */
/*   速度、加速度、オドメトリの計算、PID関数の呼び出しを定期的に行う*/
/********************************************************************/
void MotorCtrl()
{
/* Calculation of theta angular velocity, the velocity */
    // theta_r = パルス＊2pi/車輪1回転のパルス数
    theta_r = (Pulse_r.getPulses() * 2 * PI) / (PPR*INSIDE_GEAR_RATIO*OUTSIDE_GEAR_RATIO*QEI_X2_ENCODING);
    theta_l = (-Pulse_l.getPulses() * 2 * PI) / (PPR*INSIDE_GEAR_RATIO*OUTSIDE_GEAR_RATIO*QEI_X2_ENCODING);
    omega_r = (theta_r-theta_old_r)/CONTROL_PERIOD;
    omega_l = (theta_l-theta_old_l)/CONTROL_PERIOD;
    theta_old_r = theta_r;
    theta_old_l = theta_l;
    velocity_r = WHEEL_RADIUS * omega_r;
    velocity_l = WHEEL_RADIUS * omega_l;

/* Calculation of Odometry */
    velocity = (WHEEL_RADIUS / 2) * (omega_r + omega_l);
    omega = (WHEEL_RADIUS / TREAD) * (omega_r - omega_l); 
    tht1 = tht0 + omega * CONTROL_PERIOD;
    x0 = x1;    y0 = y1;    tht0 = tht1;
    x1 = x0 + velocity * CONTROL_PERIOD * cos(tht1);
    y1 = y0 + velocity * CONTROL_PERIOD * sin(tht1);

    PIDCtrl_R(target_speed_r, velocity_r);
    PIDCtrl_L(target_speed_l, velocity_l);

    timer += CONTROL_PERIOD;
}


/* Timer Interrput */
Ticker TimerInterrupt;

void SwitchMotorCtrl(int flag)
{
    if(flag == 1){
        TimerInterrupt.attach(&MotorCtrl, CONTROL_PERIOD);
        /* Reset encoder value */
        Pulse_l.reset();
        Pulse_r.reset();
    }
    else{
        TimerInterrupt.detach();
        /* Reset encoder value */
        Pulse_l.reset();
        Pulse_r.reset();
    }
}

