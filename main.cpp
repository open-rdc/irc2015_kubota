/* main.cpp */
#include "Function.h"

#define AIM_NUM             15
#define RIGHT               0
#define LEFT                1
#define ON                  1
#define OFF                 0
#define FREE_BALL           1
#define BALL_SEARCH         2
#define BALL_CATCH          3 
#define BALL_SHOOT          4
#define GO_HOME             5
#define TIMER               6
#define TIME_LIMIT          320 //Second
#define DISTANCE            157
#define DISTANCE_LIMIT      DISTANCE * 7

int main()
{

    int state;
    int ballcolor_1st, ballcolor_2nd;
    int ballcount = 0;
    int deptharea = OFF;
    int inte_distance = 0;
    int end_flag;
    int search_side = RIGHT;

    printf("\nStart the program\n\n");
    wait(2.5);
    ServoInit();
    SwitchMotorCtrl(ON);    

    state = FREE_BALL;
    while(1){
        switch(state){
            case FREE_BALL:
                LineTrace(4);
                ServoBallThrow();
                LineTrace(1);
                state = BALL_SEARCH;
                break;

            case BALL_SEARCH:
                if(timer >= TIME_LIMIT){ 
                    state = TIMER;
                    break;
                }

                if(search_side == RIGHT){
                     SearchTurn_r();
                     if(get_angle1 == 0){
                         search_side = LEFT;
                         break;
                     }
                }
                else{
                    SearchTurn_l();
                    if(get_angle1 == 0){
                        if(inte_distance < DISTANCE_LIMIT){
                            MoveLineTrace(DISTANCE);
                            inte_distance += DISTANCE;
                        }
                        search_side = RIGHT;
                        deptharea = ON;
                        break;
                    }
                }

                state = BALL_CATCH;
                break;

            case BALL_CATCH:
                BallApproach();
                if(cup == OFF){
                    wait(1);
                    ballcolor_1st = WhatColor();
                    if(ballcolor_1st != NO_BALL) ballcount += 1;
                    if(ballcount == AIM_NUM) cup = ON;

                }

                if(ballcolor_1st == NO_BALL){
                    if(get_angle1 <= 0) Turn(-get_angle1 - 5, 130);
                    else Turn(-get_angle1 + 5, 130); 
                    state = BALL_SEARCH; 
                    break;
                }
                else if(cup == OFF){
                    if(get_angle1 <= 0) Turn(-get_angle1 - 7, 110);
                    else Turn(-get_angle1 + 7, 110); 
                    cup = ON;                            
                    state = BALL_SEARCH;                 
                    printf("color = %d\n",ballcolor_1st);
                    break;
                }

                if(search_side == RIGHT) Turn(-168 - get_angle1, 160);
                else Turn(175 - get_angle1, 160);

                if(deptharea == OFF){
                    target_speed_r = target_speed_l = -150;
                    wait(0.6);
                }
                   
                state = BALL_SHOOT;
                break;

            case BALL_SHOOT:
                if(deptharea == ON){LineTrace(ballcolor_1st + 1);}
                else LineTrace(ballcolor_1st);

                if(ballcolor_1st == BLUE){
                    Turn(167, 180);
                    ServoBallThrow();
                    ServoForkUp();
                    wait(1);
                    ballcolor_2nd = WhatColor();
                    printf("color_2nd = %d\n",ballcolor_2nd);
                    if(ballcolor_2nd != NO_BALL) ballcount += 1;

                    if(ballcolor_2nd == BLUE){
                        ServoBallThrow();
                    }
                    else if(ballcolor_2nd == YELLO || ballcolor_2nd == RED){
                        Turn(173, 180);
                        LineTrace(ballcolor_2nd - ballcolor_1st);
                        Turn(80, 180);
                        ServoBallThrow();
                        Turn(80, 180);
                    }
                }

                if(ballcolor_1st == YELLO){
                    Turn(80, 180);
                    ServoBallThrow();
                    ServoForkUp();
                    wait(1);
                    ballcolor_2nd = WhatColor();
                    printf("color_2nd = %d\n",ballcolor_2nd);
                    if(ballcolor_2nd != NO_BALL) ballcount += 1;
               
                    if(ballcolor_2nd == NO_BALL) Turn(80, 180);
                    if(ballcolor_2nd == YELLO){
                        ServoBallThrow();
                        Turn(80, 180);
                    }
                    else if(ballcolor_2nd == BLUE){
                        Turn(80, 180);
                        LineTrace(1);
                        ServoBallThrow();
                    }
                    else if(ballcolor_2nd == RED){
                        Turn(-80, 180);
                        LineTrace(1);
                        Turn(80, 180);
                        ServoBallThrow();
                        Turn(80, 180);
                    }
                }

                if(ballcolor_1st == RED){
                    Turn(80, 180);
                    ServoBallThrow();
                    ServoForkUp();
                    wait(1);
                    ballcolor_2nd = WhatColor();
                    printf("color_2nd = %d\n",ballcolor_2nd);
                    if(ballcolor_2nd != NO_BALL) ballcount += 1;

                    if(ballcolor_2nd == NO_BALL) Turn(80, 180);
                    if(ballcolor_2nd == RED){
                        ServoBallThrow();
                        Turn(80, 180);
                    }
                    else if(ballcolor_2nd == YELLO){
                        Turn(80, 180);
                        LineTrace(1);
                        Turn(-80, 180);
                        ServoBallThrow();
                        Turn(80, 180);
                    }
                    else if(ballcolor_2nd == BLUE){
                        Turn(80, 180);
                        LineTrace(2);
                        ServoBallThrow();
                    }
                }

                if(ballcount >= AIM_NUM){ 
                    state = GO_HOME;
                    break;
                }

                if(ballcolor_2nd == NO_BALL) LineTrace(ballcolor_1st);
                else LineTrace(ballcolor_2nd);

                if(deptharea == ON) MoveLineTrace(inte_distance);
                cup = OFF;
                ballcolor_1st = ballcolor_2nd = NO_BALL;
                state = BALL_SEARCH;
                break;

            case GO_HOME:
                Turn(170,200);
                if(ballcolor_2nd != NO_BALL){
                    if(ballcolor_2nd == BLUE) LineTrace(2);
                    if(ballcolor_2nd == YELLO) LineTrace(1);
                    if(ballcolor_2nd == RED); 
                }
                else {
                    if(ballcolor_1st == BLUE) LineTrace(2);
                    if(ballcolor_1st == YELLO) LineTrace(1);
                    if(ballcolor_1st == RED); 
                }

                target_speed_l = target_speed_r = 200;
                wait(2.2);
                Turn(170,200);
                for(int i = 0; i < 5; i++){
                    ServoForkAppeal();
                    target_speed_r = 100;
                    target_speed_l = -100;
                }
                target_speed_r = target_speed_l = 0;
                end_flag = ON;
                break;

            case TIMER:
                Turn(170, 200);
                if(ballcolor_1st != NO_BALL){
                     state = BALL_SHOOT;
                     ballcount = AIM_NUM;
                     break;
                }
                if(deptharea == ON) LineTrace(4);
                else LineTrace(3);

                target_speed_l = target_speed_r = 200; 
                wait(2.2);
                Turn(170,200);
                for(int i = 0; i < 5; i++){
                    ServoForkAppeal();
                    target_speed_r = 100;
                    target_speed_l = -100;
                }
                target_speed_r = target_speed_l = 0;
                end_flag = ON;
                break;

        }       
        if(end_flag == ON) break;
    }
    printf("\nProgram has finished");
}

