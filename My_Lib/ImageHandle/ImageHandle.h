#ifndef __ImageHandle_H
#define __ImageHandle_H 

#include "ALL_Includes.h"

#define BackgroundColor   BLACK
#define IR_Resolution	  	768
#define IR_ResolutionX	  24
#define IR_ResolutionY	  32

extern s16 TmpH;
extern s16 TmpL;
extern u16 POINT_COLOR;
extern s16 StorageTmpH,StorageTmpL;
extern u16 MinPoniter;
extern float MaxTemp,MinTemp;


int Tmp2Color(float Tmps);
void SetSmoothingCoefficient(void);
void DispRul(void);
uint16_t swap16(uint16_t a);
void Dispbackground(void);
void SetDispBuf(float *Temp);
void Disp_BigFloatNum(int x,int y,float num,unsigned int color,unsigned int b_color);
void Disp_SmallFloatNum(int x,int y,float num,unsigned int color,unsigned int b_color);
void DispPowerShape(void);
void FindMaxMinTemp(float *Temp);
void DispMaxMinTemp(void);
void DispReflectRate(void);
void DispOneGridPower(u16 color);
void DispTwoGridPower(u16 color);
void DispThreeGridPower(u16 color);
void DispFourGridPower(u16 color);
void DispBatPower(void);
void DispCross(unsigned int x,unsigned int y,unsigned int color);
void AverageFilter(float *Temp);


#endif

