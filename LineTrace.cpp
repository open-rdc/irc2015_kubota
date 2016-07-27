/*LineTrace.cpp*/
#include "Function.h"
#include "LineTrace.h"


/* Define PinNo sensor linetrace */
DigitalIn ss1(p5),ss2(p6),ss3(p7),ss4(p8);

float target_speed_r, target_speed_l; 


/********************************************************************/
/*                     ライントレース関数                           */
/*          引数: 止まりたいクロスラインまでのクロスラインの本数    */
/*                    信号はデジタル出力                            */
/********************************************************************/
void LineTrace(int countxline)
{
    int sw = 0;
    int s1,s2,s3,s4;
    int count = 0;
    while(count != countxline)
    {
       s1 = ss1.read();
       s2 = ss2.read();
       s3 = ss3.read();
       s4 = ss4.read();

       if(s1 == 1 && s2 == 0 && s3 == 1 && s4 == 1)      sw = 1;
       else if(s1 == 1 && s2 == 1 && s3 == 0 && s4 == 1) sw = 2;
       else if(s1 == 0 && s2 == 1 && s3 == 1 && s4 == 1) sw = 3;
       else if(s1 == 1 && s2 == 1 && s3 == 1 && s4 == 0) sw = 4;
       else if(s1 == 0 && s2 == 0 && s3 == 1 && s4 == 1) sw = 5;
       else if(s1 == 1 && s2 == 1 && s3 == 0 && s4 == 0) sw = 6;
       else if(s1 == 0 && s2 == 0 && s3 == 0 && s4 == 0) sw = 7;
       else if(s1 == 0 && s2 == 0 && s3 == 0 && s4 == 1) sw = 7;
       else if(s1 == 1 && s2 == 0 && s3 == 0 && s4 == 0) sw = 7;
       else if(s1 == 1 && s2 == 0 && s3 == 0 && s4 == 1) sw = 7;

       switch(sw)
       {
           case 1:
               target_speed_r = 220, target_speed_l = 480; 
               break;
           case 2:
               target_speed_r = 480, target_speed_l = 220;
               break;
           case 3:
               target_speed_r = 30, target_speed_l = 460;
               break;
           case 4:
               target_speed_r = 460, target_speed_l = 30;
               break;
           case 5:
               target_speed_r = 60, target_speed_l = 460;
               break;
           case 6:
               target_speed_r = 460, target_speed_l = 60;
               break;
           case 7:
               target_speed_r = target_speed_l = 380;
               count += 1;
               if(count == countxline) target_speed_r = target_speed_l = 240;
               wait(0.09);
               break;

           default:
               target_speed_r = target_speed_l = 410;
               break;
       }
    sw = 0;
    } 
    target_speed_r = target_speed_l = 0;
}


/********************************************************************/
/*                 オドメトリを用いたライントレース関数             */
/*                 引数: 進みたい距離[mm]                           */
/********************************************************************/
void MoveLineTrace(float distance)
{
    int sw = 0;
    int s1,s2,s3,s4;

    x1 = y1 = tht1 = 0;
    x0 = y0 = tht0 = 0;
    while(x1 <= distance)
    {
       s1 = ss1.read();
       s2 = ss2.read();
       s3 = ss3.read();
       s4 = ss4.read();

       if(s1 == 1 && s2 == 0 && s3 == 1 && s4 == 1) sw = 1;
       if(s1 == 1 && s2 == 1 && s3 == 0 && s4 == 1) sw = 2;
       if(s1 == 0 && s2 == 1 && s3 == 1 && s4 == 1) sw = 3;
       if(s1 == 1 && s2 == 1 && s3 == 1 && s4 == 0) sw = 4;

       switch(sw)
       {
           case 1:
               target_speed_r = 120, target_speed_l = 320;
               break;
           case 2:
               target_speed_r = 320, target_speed_l = 120;
               break;
           case 3:
               target_speed_r = 50, target_speed_l = 350;
               break;
           case 4:
               target_speed_r = 350, target_speed_l = 50;
               break;
           default:
               target_speed_r = target_speed_l = 300;
               break;
       }
    sw = 0;
    } 
    target_speed_r = target_speed_l = 0;
}
