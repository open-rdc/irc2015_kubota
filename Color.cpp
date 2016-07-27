/* Color.cpp */
#include "Function.h"
#include "Color.h"

I2C colorSensor(p9,p10);
int R,G,B,IR; 


/********************************************************************/
/*                     R,G,B,IRを出力する関数                       */
/*                     引数: なし                                   */
/********************************************************************/
void  get_color_value(){
    char data[8];
    int tmp;

    colorSensor.start();        //start condition
    colorSensor.write(0x54);    //device address
    colorSensor.write(0x00);    //controll register
    colorSensor.write(0x84);    //ADC reset enable
    colorSensor.write(0x01);    //manual timing register 01
    colorSensor.write(0x03);    //set manual timing reister 01
    colorSensor.write(0x03);    //manual timing register 02
    colorSensor.write(0xC0);    //set manual timing register 02
    colorSensor.start();        //start condition
    colorSensor.write(0x54);    //device address
    colorSensor.write(0x00);    //controll register
    colorSensor.write(0x04);    //ADC reset disable
    colorSensor.stop();         //stop condition
    wait(1.0);
    colorSensor.start();        //start condition
    colorSensor.write(0x54);    //device address
    colorSensor.write(0x03);    //output databyte set
    colorSensor.read(0x55, data, 8);    //read sensor
        
    for(int i=0; i<8; i=i+2){ 
        tmp = data[i] << 8;  //16bit shift
        tmp += data[i+1];  
        if(i== 0)           R = tmp;
        else if(i == 2)     G = tmp;
        else if(i == 4)     B = tmp;
        else                IR = tmp;
    }
    colorSensor.stop();
}


/********************************************************************/
/*                     色判別関数                                   */
/*                     引数: なし                                   */
/*                     戻り値:　色判別結果                          */
/********************************************************************/
int WhatColor(){
    get_color_value();
    printf("R:%d G:%d B:%d IR:%d\n",R,G,B,IR);

    if(IR >= 17) return NO_BALL;

    if(B <= 22)      return RED;
    else if(R <= 22) return BLUE;
    else if(G >= 44) return YELLO;
    else             return NO_BALL;
}

