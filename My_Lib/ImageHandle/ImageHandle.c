#include "ImageHandle.h"
#include "ALL_Includes.h"

s16 TmpH=35;
s16 TmpL=5;
s16 StorageTmpH,StorageTmpL;
u16 MaxPoniter=0;
u16 MinPoniter=0;
float MaxTemp,MinTemp;


float SC1[4][4],SC2[4][4],SC3[4][4],SC4[4][4];//平滑系数

u16 POINT_COLOR=WHITE; 

const u8 ImageFilter[9]={1,2,1,2,4,2,1,2,1};

//色条颜色，共256色
const uint16_t camColors[] = {       
0x480F,0x400F,0x400F,0x400F,0x4010,0x3810,0x3810,0x3810,
0x3810,0x3010,0x3010,0x3010,0x2810,0x2810,0x2810,0x2810,  
0x2010,0x2010,0x2010,0x1810,0x1810,0x1811,0x1811,0x1011,
0x1011,0x1011,0x0811,0x0811,0x0811,0x0011,0x0011,0x0011,
0x0011,0x0011,0x0031,0x0031,0x0051,0x0072,0x0072,0x0092,
0x00B2,0x00B2,0x00D2,0x00F2,0x00F2,0x0112,0x0132,0x0152,
0x0152,0x0172,0x0192,0x0192,0x01B2,0x01D2,0x01F3,0x01F3,
0x0213,0x0233,0x0253,0x0253,0x0273,0x0293,0x02B3,0x02D3,
0x02D3,0x02F3,0x0313,0x0333,0x0333,0x0353,0x0373,0x0394,
0x03B4,0x03D4,0x03D4,0x03F4,0x0414,0x0434,0x0454,0x0474,
0x0474,0x0494,0x04B4,0x04D4,0x04F4,0x0514,0x0534,0x0534,
0x0554,0x0554,0x0574,0x0574,0x0573,0x0573,0x0573,0x0572,
0x0572,0x0572,0x0571,0x0591,0x0591,0x0590,0x0590,0x058F,
0x058F,0x058F,0x058E,0x05AE,0x05AE,0x05AD,0x05AD,0x05AD,
0x05AC,0x05AC,0x05AB,0x05CB,0x05CB,0x05CA,0x05CA,0x05CA,
0x05C9,0x05C9,0x05C8,0x05E8,0x05E8,0x05E7,0x05E7,0x05E6,
0x05E6,0x05E6,0x05E5,0x05E5,0x0604,0x0604,0x0604,0x0603,
0x0603,0x0602,0x0602,0x0601,0x0621,0x0621,0x0620,0x0620,
0x0620,0x0620,0x0E20,0x0E20,0x0E40,0x1640,0x1640,0x1E40,
0x1E40,0x2640,0x2640,0x2E40,0x2E60,0x3660,0x3660,0x3E60,
0x3E60,0x3E60,0x4660,0x4660,0x4E60,0x4E80,0x5680,0x5680,
0x5E80,0x5E80,0x6680,0x6680,0x6E80,0x6EA0,0x76A0,0x76A0,
0x7EA0,0x7EA0,0x86A0,0x86A0,0x8EA0,0x8EC0,0x96C0,0x96C0,
0x9EC0,0x9EC0,0xA6C0,0xAEC0,0xAEC0,0xB6E0,0xB6E0,0xBEE0,
0xBEE0,0xC6E0,0xC6E0,0xCEE0,0xCEE0,0xD6E0,0xD700,0xDF00,
0xDEE0,0xDEC0,0xDEA0,0xDE80,0xDE80,0xE660,0xE640,0xE620,
0xE600,0xE5E0,0xE5C0,0xE5A0,0xE580,0xE560,0xE540,0xE520,
0xE500,0xE4E0,0xE4C0,0xE4A0,0xE480,0xE460,0xEC40,0xEC20,
0xEC00,0xEBE0,0xEBC0,0xEBA0,0xEB80,0xEB60,0xEB40,0xEB20,
0xEB00,0xEAE0,0xEAC0,0xEAA0,0xEA80,0xEA60,0xEA40,0xF220,
0xF200,0xF1E0,0xF1C0,0xF1A0,0xF180,0xF160,0xF140,0xF100,
0xF0E0,0xF0C0,0xF0A0,0xF080,0xF060,0xF040,0xF020,0xF800,
0xffff,0x0000
};

//温度转换成颜色
int Tmp2Color(float Tmps)
{
	if(Tmps<TmpL)return 0;
	if(Tmps>TmpH)return 255;
	float T;
	T=(Tmps-TmpL)/(TmpH-TmpL)*255;
	return (int)T;
}

/*******************************************************************************
//平滑系数,扩大图像
T1---X1---Tx1----X2----T2
|          |          |
Y1         |          Y1
|          |          |
+----------T----------+
|          |          |
Y2         |          Y2
|          |          |
T3---X1---Tx2---X2----T3

Tx1=(x-x1)/x*T1+(x-x2)/x*T2
Tx2=(x-x1)/x*T3+(x-x2)/x*T4

T=(y-y1)/y*Tx1+(y-y2)/y*Tx2
 = (y-y1)/y*(x-x1)/x*T1 +(y-y1)/y*(x-x2)/x*T2
  +(y-y2)/y*(x-x1)/x*T3 +(y-y2)/y*(x-x2)/x*T4

*******************************************************************************/
void SetSmoothingCoefficient(void)
{
int x,y;

for(x=0;x<4;x++){
	for(y=0;y<4;y++){
		SC1[x][y]=(4-(float)y)/4*(4-(float)x)/4;
		SC2[x][y]=(4-(float)y)/4*(4-(4-(float)x))/4;
		SC3[x][y]=(4-(4-(float)y))/4*(4-(float)x)/4;
		SC4[x][y]=(4-(4-(float)y))/4*(4-(4-(float)x))/4;
		}
	}
}

//显示温度图像
void SetDispBuf(float *Temp)
{
	int x,y,k,l,t,PosA,PosB;
	float T1,T2,T3,T4,T;
	u16 DispBuf[8][192];
	int i,j;
	u16 color;
	
	LCD_setwindow(24,32,215,287);
	
	if(RuleAuto!=DISABLE)//自动标尺
	{
		TmpH=Temp[MaxPoniter];
		TmpL=Temp[MinPoniter];
	}		
	
	for(y=0;y<IR_ResolutionY;y++)
	{
			for(x=0;x<IR_ResolutionX;x++)
			{
					PosA = x*IR_ResolutionY+y;
					PosB = (x+1)*IR_ResolutionY+y;
			
					T1=Temp[PosA];
					if(x<(IR_ResolutionX-1))//舍去超过边沿点
						T2=Temp[PosB];
					else
						T2=Temp[PosA];
			
					if(y<(IR_ResolutionY-1))//舍去超过768点
						T3=Temp[PosA+1];
					else
						T3=Temp[PosA];
			
					if((x<(IR_ResolutionX-1))&&(y<(IR_ResolutionY-1)))//舍去超过768点
						T4=Temp[PosB+1];
					else 		
						T4=Temp[PosA];
			
					for(k=0;k<4;k++)
					{
						 for(l=0;l<4;l++)
						 {
								T=	T1*SC1[l][k]+
									T2*SC2[l][k]+
									T3*SC3[l][k]+
									T4*SC4[l][k];
								
								t=Tmp2Color(T);
								color=camColors[t];	
							  DispBuf[k*2][x*8+l*2]=color;
							  DispBuf[k*2][x*8+l*2+1]=color;
								DispBuf[k*2+1][x*8+l*2]=color;
							  DispBuf[k*2+1][x*8+l*2+1]=color;							
						 }
					}		
			}
		
			for(i=0;i<8;i++)
			{
				 for(j=0;j<192;j++)
						LLCD_WRITE_DATA(DispBuf[i][j]);
			}		
	}
} 

//色条颜色转换
uint16_t swap16(uint16_t a)
{
	return (a&0x00ff)*0x0100+a/0x0100;
}

/*********************
显示色条
********************/
void Dispbackground(void)
{
	int i;

	//显示色条
	for(i=0;i<256;i++)
	{
		POINT_COLOR=camColors[255-i];
		LCD_DrawLine(0,i+32,10,i+32,POINT_COLOR);
	}

	POINT_COLOR=WHITE; 
	if(RuleAuto==DISABLE)//非自动，显示标尺温度
	{
		LCD_ShowNum(0,14,TmpH,POINT_COLOR,BackgroundColor);
		LCD_ShowNum(0,289,TmpL,POINT_COLOR,BackgroundColor);
	}
	LCD_DrawLine(10,32,10,287,POINT_COLOR);
	DispRul();
}

/*********************
显示标尺
********************/
void DispRul(void)
{
	int y;
	int H,L;
	
	H=(int)TmpH;
	L=(int)TmpL;
	LCD_Fill(11,32,16,287,BLACK);
	POINT_COLOR=WHITE; 
	
	for(y=L;y<H+1;y=y+1)
	{
			LCD_DrawPoint(11,(y-L)*256/(H-L)+32,POINT_COLOR);
	}
	
	for(y=L;y<H+1;y=y+5)
	{
			LCD_DrawPoint(12,(y-L)*256/(H-L)+32,POINT_COLOR);
			LCD_DrawPoint(13,(y-L)*256/(H-L)+32,POINT_COLOR);
			if((y%10)==0)
			{
					LCD_DrawPoint(14,(y-L)*256/(H-L)+32,POINT_COLOR);
					LCD_DrawPoint(15,(y-L)*256/(H-L)+32,POINT_COLOR);
					LCD_DrawPoint(16,(y-L)*256/(H-L)+32,POINT_COLOR);
			}
	}
} 

//显示浮点温度
void Disp_BigFloatNum(int x,int y,float num,unsigned int color,unsigned int b_color)
{
	int TempNum;
	
	TempNum = (int)(num*10);

	if(TempNum<0)
	{
		TempNum = -TempNum;
		LCD_N(x,y,11,color,b_color);
		x+=16;
	}
	if(TempNum>=10000)
	TempNum = 9999;

	if(TempNum>=1000)
	{
		LCD_N(x, y,TempNum/1000, color,b_color);
		x+=16;
		LCD_N(x, y,(TempNum/100)%10, color,b_color);
		x+=16;
		LCD_N(x, y,(TempNum%100)/10, color,b_color);
		x+=16;
		LCD_N(x, y,10, color,b_color);
		x+=16;
		LCD_N(x, y,TempNum%10, color,b_color);
	}
    else if(TempNum>=100)
	{
		LCD_N(x, y,TempNum/100, color,b_color);
		x+=16;
		LCD_N(x, y,(TempNum/10)%10, color,b_color);
		x+=16;
		LCD_N(x, y,10, color,b_color);
		x+=16;
		LCD_N(x, y,TempNum%10, color,b_color);
	}
	else if(TempNum>=10)
	{
		LCD_N(x, y,TempNum/10, color,b_color);
		x+=16;
		LCD_N(x, y,10, color,b_color);
		x+=16;
		LCD_N(x, y,TempNum%10, color,b_color);
	}
	else
	{
		LCD_N(x, y,0, color,b_color);
		x+=16;
		LCD_N(x, y,10, color,b_color);
		x+=16;
		LCD_N(x, y,TempNum, color,b_color);		
	}

	x+=16;
	LCD_N(x, y,13, color,b_color);
}

//显示浮点温度
void Disp_SmallFloatNum(int x,int y,float num,unsigned int color,unsigned int b_color)
{
	int TempNum;

	if(((x+55)<LCD_WidthX)&&((y+15)<LCD_WidthY))
		LCD_Fill(x,y,x+55,y+15,BLACK);
	
	TempNum = (int)(num*10);

	if(TempNum<0)
	{
		TempNum = -TempNum;
		LCD_Showchar(x,y,'-',color,b_color);
		x+=8;
	}
	if(TempNum>=10000)
	TempNum = 9999;

	if(TempNum>=1000)
	{
		LCD_Showchar(x, y,TempNum/1000+'0', color,b_color);
		x+=8;
		LCD_Showchar(x, y,(TempNum/100)%10+'0', color,b_color);
		x+=8;
		LCD_Showchar(x, y,(TempNum%100)/10+'0', color,b_color);
		x+=8;
		LCD_Showchar(x, y,'.', color,b_color);
		x+=8;
		LCD_Showchar(x, y,TempNum%10+'0', color,b_color);
	}
    else if(TempNum>=100)
	{
		LCD_Showchar(x, y,TempNum/100+'0', color,b_color);
		x+=8;
		LCD_Showchar(x, y,(TempNum/10)%10+'0', color,b_color);
		x+=8;
		LCD_Showchar(x, y,'.', color,b_color);
		x+=8;
		LCD_Showchar(x, y,TempNum%10+'0', color,b_color);
	}
	else if(TempNum>=10)
	{
		LCD_Showchar(x, y,TempNum/10+'0', color,b_color);
		x+=8;
		LCD_Showchar(x, y,'.', color,b_color);
		x+=8;
		LCD_Showchar(x, y,TempNum%10+'0', color,b_color);
	}
	else
	{
		LCD_Showchar(x, y,0+'0', color,b_color);
		x+=8;
		LCD_Showchar(x, y,'.', color,b_color);
		x+=8;
		LCD_Showchar(x, y,TempNum, color,b_color);		
	}

	x+=8;
	LCD_ShowString(x, y,"'C", color,b_color);
}


//电量外形显示
void DispPowerShape(void)
{
		LCD_DrawLine(181,6,181,25,WHITE);
		LCD_DrawLine(216,6,216,25,WHITE);
		LCD_DrawLine(181,6,216,6,WHITE);
		LCD_DrawLine(181,25,216,25,WHITE);
		LCD_DrawLine(180,11,180,20,WHITE);
		LCD_DrawLine(179,11,179,20,WHITE);
		LCD_DrawLine(178,11,178,20,WHITE);
}

//查找最高、最低温度
void FindMaxMinTemp(float *Temp)
{	
		u16 i;
	
		MaxTemp=Temp[0];
	  MinTemp=Temp[0];
	  MaxPoniter=0;
		MinPoniter=0;
	
		for(i=0;i<IR_Resolution;i++)
		{
				if(MaxTemp<Temp[i])
				{
						MaxTemp=Temp[i];
						MaxPoniter=i;
				}
				
				if(MinTemp>Temp[i])
				{
						MinTemp=Temp[i];
						MinPoniter=i;
				}
		}					
}

//显示最高、最低温度
void DispMaxMinTemp(void)
{
		DispCross(24+(MaxPoniter/32)*8,32+(MaxPoniter%32)*8,MAGENTA);
		DispCross(24+(MinPoniter/32)*8,32+(MinPoniter%32)*8,GREEN);
		LCD_Fill(102,288,117,319,BLACK);
		Disp_SmallFloatNum(62,288,MaxTemp,MAGENTA,BackgroundColor);
		Disp_SmallFloatNum(62,304,MinTemp,GREEN,BackgroundColor);	
}

//显示反射率
void DispReflectRate(void)
{
		LCD_ShowString(94,8,"e=0.",WHITE,BLACK);
		LCD_ShowNum(126,8,emissivity,WHITE,BackgroundColor);
}

//显示1格电量
void DispOneGridPower(u16 color)
{
		LCD_Fill(183,8,214,23,BLACK);
		LCD_Fill(207,8,214,23,color);
}

//显示2格电量
void DispTwoGridPower(u16 color)
{
		LCD_Fill(183,8,214,23,BLACK);
		LCD_Fill(199,8,214,23,color);
}

//显示3格电量
void DispThreeGridPower(u16 color)
{
		LCD_Fill(183,8,214,23,BLACK);
		LCD_Fill(191,8,214,23,color);
}

//显示4格电量
void DispFourGridPower(u16 color)
{
		LCD_Fill(183,8,214,23,color);
}

//显示电量
void DispBatPower(void)
{
	  u16 BatVal;
		u8  BatGrid;
		u16 DispColor;
	
		BatVal=ADC1_GetVal();
		
		if(BatCHRG_State==0)//充电中，改善充电时电池浮压
		{
			if(BatVal<=380)
				BatVal-=10;
			if((BatVal<=400)&&(BatVal>380))
				BatVal-=7;
			if((BatVal<=405)&&(BatVal>400))
				BatVal-=4;
		}
		
		if(BatVal>=395)
			BatGrid=4;
		else if(BatVal>=383)
			BatGrid=3;
		else if(BatVal>=370)
			BatGrid=2;
		else if(BatVal>=358)
			BatGrid=1;
		else
			BatGrid=0;
		
		if(BatCHRG_State==0)//充电中
				DispColor=GREEN;//绿色
		else
				DispColor=WHITE;
		
		switch(BatGrid)
		{
			case 0:
			{
					if(BatCHRG_State!=0)//非充电，电量不足关机
							PowerOFF;
					break;
			}
			case 1:DispOneGridPower(DispColor);break;
			case 2:DispTwoGridPower(DispColor);break;
			case 3:DispThreeGridPower(DispColor);break;
			case 4:DispFourGridPower(DispColor);break;
		}
}
//显示十字架
void DispCross(unsigned int x,unsigned int y,unsigned int color)
{
		if(x==24)
		{
			  if(y==32)
				{
						LCD_Fill(x,y,x+6,y,color);
						LCD_Fill(x,y,x,y+6,color);
				}
				else
				{
						LCD_Fill(x,y,x+6,y,color);
						LCD_Fill(x,y-6,x,y+6,color);
				}			
		}
		else if(y==32)
		{
				if(x!=24)
				{
						LCD_Fill(x-6,y,x+6,y,color);
						LCD_Fill(x,y,x,y+6,color);
				}
		}
		else
		{
				LCD_Fill(x-6,y,x+6,y,color);
				LCD_Fill(x,y-6,x,y+6,color);	
		}
}

/*****************************
Average Filter
3x3 Array
*****************************/
void AverageFilter(float *Temp)
{
		u16 x,y;
		u16 PosA;
	  float TempFilter[768]={0};
	 // float MidMatrix[9]={0};
		u16 Cnt=0;
		u8 i;
		
		for(i=0;i<9;i++)
		{
				Cnt+=ImageFilter[i];	
		}
		
		//y=0
		for(x=1;x<IR_ResolutionX-1;x++)
		{
				PosA = x*IR_ResolutionY;
				TempFilter[PosA]=Temp[PosA-IR_ResolutionY]*ImageFilter[0]+Temp[PosA]*ImageFilter[1]+Temp[PosA+IR_ResolutionY]*ImageFilter[2]
						+Temp[PosA-IR_ResolutionY]*ImageFilter[3]+Temp[PosA]*ImageFilter[4]+Temp[PosA+IR_ResolutionY]*ImageFilter[5]
						+Temp[PosA+1-IR_ResolutionY]*ImageFilter[6]+Temp[PosA+1]*ImageFilter[7]+Temp[PosA+1+IR_ResolutionY]*ImageFilter[8];
				TempFilter[PosA]=TempFilter[PosA]/Cnt;			
		}
		//Y=31
		for(x=1;x<IR_ResolutionX-1;x++)
		{
				PosA = x*IR_ResolutionY+31;
				TempFilter[PosA]=Temp[PosA-1-IR_ResolutionY]*ImageFilter[0]+Temp[PosA-1]*ImageFilter[1]+Temp[PosA-1+IR_ResolutionY]*ImageFilter[2]
						+Temp[PosA-IR_ResolutionY]*ImageFilter[3]+Temp[PosA]*ImageFilter[4]+Temp[PosA+IR_ResolutionY]*ImageFilter[5]
						+Temp[PosA-IR_ResolutionY]*ImageFilter[6]+Temp[PosA]*ImageFilter[7]+Temp[PosA+IR_ResolutionY]*ImageFilter[8];
				TempFilter[PosA]=TempFilter[PosA]/Cnt;	
		}
		//x=0
		for(y=1;y<IR_ResolutionY-1;y++)
		{
				PosA = y;
				TempFilter[PosA]=Temp[PosA-1]*ImageFilter[0]+Temp[PosA-1]*ImageFilter[1]+Temp[PosA-1+IR_ResolutionY]*ImageFilter[2]
						+Temp[PosA]*ImageFilter[3]+Temp[PosA]*ImageFilter[4]+Temp[PosA+IR_ResolutionY]*ImageFilter[5]
						+Temp[PosA+1]*ImageFilter[6]+Temp[PosA+1]*ImageFilter[7]+Temp[PosA+1+IR_ResolutionY]*ImageFilter[8];
				TempFilter[PosA]=TempFilter[PosA]/Cnt;	
		}
		//X=23
		for(y=1;y<IR_ResolutionY-1;y++)
		{
				PosA = 23*IR_ResolutionY+y;
				TempFilter[PosA]=Temp[PosA-1-IR_ResolutionY]*ImageFilter[0]+Temp[PosA-1]*ImageFilter[1]+Temp[PosA-1]*ImageFilter[2]
						+Temp[PosA-IR_ResolutionY]*ImageFilter[3]+Temp[PosA]*ImageFilter[4]+Temp[PosA]*ImageFilter[5]
						+Temp[PosA+1-IR_ResolutionY]*ImageFilter[6]+Temp[PosA+1]*ImageFilter[7]+Temp[PosA+1]*ImageFilter[8];
				TempFilter[PosA]=TempFilter[PosA]/Cnt;	
		}
		//x=0,y=0
		PosA = 0;
		TempFilter[PosA]=Temp[PosA]*ImageFilter[0]+Temp[PosA]*ImageFilter[1]+Temp[PosA+IR_ResolutionY]*ImageFilter[2]
				+Temp[PosA]*ImageFilter[3]+Temp[PosA]*ImageFilter[4]+Temp[PosA+IR_ResolutionY]*ImageFilter[5]
				+Temp[PosA+1]*ImageFilter[6]+Temp[PosA+1]*ImageFilter[7]+Temp[PosA+1+IR_ResolutionY]*ImageFilter[8];
		TempFilter[PosA]=TempFilter[PosA]/Cnt;		
		
		//x=0,y=31	
		PosA = 31;
		TempFilter[PosA]=Temp[PosA-1]*ImageFilter[0]+Temp[PosA-1]*ImageFilter[1]+Temp[PosA-1+IR_ResolutionY]*ImageFilter[2]
				+Temp[PosA]*ImageFilter[3]+Temp[PosA]*ImageFilter[4]+Temp[PosA+IR_ResolutionY]*ImageFilter[5]
				+Temp[PosA]*ImageFilter[6]+Temp[PosA]*ImageFilter[7]+Temp[PosA+IR_ResolutionY]*ImageFilter[8];
		TempFilter[PosA]=TempFilter[PosA]/Cnt;	

		//y=0,x=23
		PosA = 23*IR_ResolutionY;
		TempFilter[PosA]=Temp[PosA-IR_ResolutionY]*ImageFilter[0]+Temp[PosA]*ImageFilter[1]+Temp[PosA]*ImageFilter[2]
				+Temp[PosA-IR_ResolutionY]*ImageFilter[3]+Temp[PosA]*ImageFilter[4]+Temp[PosA]*ImageFilter[5]
				+Temp[PosA+1-IR_ResolutionY]*ImageFilter[6]+Temp[PosA+1]*ImageFilter[7]+Temp[PosA+1]*ImageFilter[8];
		TempFilter[PosA]=TempFilter[PosA]/Cnt;
		
		//x=23,y=31
		PosA = 23*IR_ResolutionY+31;
		TempFilter[PosA]=Temp[PosA-1-IR_ResolutionY]*ImageFilter[0]+Temp[PosA-1]*ImageFilter[1]+Temp[PosA-1]*ImageFilter[2]
				+Temp[PosA-IR_ResolutionY]*ImageFilter[3]+Temp[PosA]*ImageFilter[4]+Temp[PosA]*ImageFilter[5]
				+Temp[PosA-IR_ResolutionY]*ImageFilter[6]+Temp[PosA]*ImageFilter[7]+Temp[PosA]*ImageFilter[8];
		TempFilter[PosA]=TempFilter[PosA]/Cnt;	
		
		for(y=1;y<IR_ResolutionY-1;y++)
		{
			for(x=1;x<IR_ResolutionX-1;x++)
			{
					PosA = x*IR_ResolutionY+y;
					TempFilter[PosA]=Temp[PosA-1-IR_ResolutionY]*ImageFilter[0]+Temp[PosA-1]*ImageFilter[1]+Temp[PosA-1+IR_ResolutionY]*ImageFilter[2]
							+Temp[PosA-IR_ResolutionY]*ImageFilter[3]+Temp[PosA]*ImageFilter[4]+Temp[PosA+IR_ResolutionY]*ImageFilter[5]
							+Temp[PosA+1-IR_ResolutionY]*ImageFilter[6]+Temp[PosA+1]*ImageFilter[7]+Temp[PosA+1+IR_ResolutionY]*ImageFilter[8];
					TempFilter[PosA]=TempFilter[PosA]/Cnt;			
			}
		}
			
			for(y=0;y<IR_ResolutionY;y++)
			{
				for(x=0;x<IR_ResolutionX;x++)
				{
						PosA = x*IR_ResolutionY+y;
						Temp[PosA]=TempFilter[PosA];
				}
			}
}
