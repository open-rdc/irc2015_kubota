/* PSD.cpp */
#include "Function.h"
#include "PSD.h"
 
#define NUM 9
AnalogIn PSD(p16);     

int comp( const void *c1, const void *c2 );


/********************************************************************/
/*                     PSDセンサ関数                                */
/*                     引数: なし                                   */
/*                     戻り値:　アナログ信号                        */
/********************************************************************/
float PSD_Value()
{
    float sort[NUM] = {0};

    for(int i = 0; i < NUM; i++)
        sort[i] = PSD.read();
    qsort(sort, NUM, sizeof(float), comp);

    return sort[(NUM -1) / 2 ];
}

int comp( const void *c1, const void *c2 )
{
  int tmp1 = *(int *)c1;
  int tmp2 = *(int *)c2;

  if( tmp1 < tmp2 )  return -1;
  if( tmp1 > tmp2 )  return  1;
  return 0;
}
