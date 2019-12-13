#include "ALL_Includes.h"//包含所需的头文件
#include "math.h"



void ALL_Config(void);

#define  Rate2HZ   0x02
#define  Rate4HZ   0x03
#define  Rate8HZ   0x04
#define  Rate16HZ  0x05
#define  Rate32HZ  0x06

#define  MLX_I2C_ADDR 0x33
#define	 RefreshRate Rate8HZ 
#define  TA_SHIFT 8 //Default shift for MLX90640 in open air


u8 UsartBuff[SEND_BUF_SIZE];//串口传输图像缓存
extern void USB_Init(void);
extern int32_t flash_initialize(void);
extern uint32_t  PCLK1_CLK; 
extern void FatFs_Demo(void);

uint32_t run_sec;

int main(void)
{
              static uint16_t eeMLX90640[832];  
              uint16_t frame[834];
              float Ta,tr;
              static float mlx90640To[768];
		u16 TimeCount=0;	
		u16 i=0;
	       u32 CheckCode;
		int mode;
		float MidTemp;
	
		ALL_Config();
		UsartBuff[0]=0x5A;
		UsartBuff[1]=0x5A;
		UsartBuff[2]=0x02;
		UsartBuff[3]=0x06;

              os_printf("IR V0.14 %s %s \r\n",  __DATE__, __TIME__);
	       DispPowerShape();

	        while(KeyDect_State!=0)
		{		
		             
				DispBatPower();
				delay_ms(500);		
		}//等待电源键按下		
	       delay_ms(1000);
		PowerON;
		DispBatPower();
		
		if(EEP_ReadWord(TempFlagAddr)==EepWrFlag)//EEP有写过，从EEP调取数据
		{
				TmpH=(s16)EEP_ReadWord(HightTempAddr);
				TmpL=(s16)EEP_ReadWord(LowTempAddr);				
				emissivity=(u8)EEP_ReadWord(EmissivityAddr);		
				RuleAuto=(u8)EEP_ReadWord(AutoRuleAddr);
		}
		StorageTmpH=TmpH;
		StorageTmpL=TmpL;
		
		Dispbackground();//显示色条
		DispReflectRate();//显示反射率	
		LCD_ShowString(30,288,"MAX:",MAGENTA,BLACK);
		LCD_ShowString(30,304,"MIN:",GREEN,BLACK);
		if(RuleAuto==DISABLE)//显示"自动"或"非自动"
			LCD_ShowString(30,8,"NoAuto",WHITE,BLACK);
		else
			LCD_ShowString(30,8,"Auto",WHITE,BLACK);
		
		SetSmoothingCoefficient();//平滑系数
		
              MLX90640_SetRefreshRate(MLX_I2C_ADDR, RefreshRate);//可以不用，系统默认初始化
              MLX90640_SetChessMode(MLX_I2C_ADDR);//可以不用，系统默认初始化
		paramsMLX90640 mlx90640;
              MLX90640_DumpEE(MLX_I2C_ADDR, eeMLX90640);
              MLX90640_ExtractParameters(eeMLX90640, &mlx90640);
		mode = MLX90640_GetCurMode(MLX_I2C_ADDR);
		
		for(i=0;i<3;i++)//丢去开始帧
		{
				MLX90640_GetFrameData(MLX_I2C_ADDR, frame);
				delay_ms(500);
		}
		
		while(1)
		{
				MLX90640_GetFrameData(MLX_I2C_ADDR, frame);
				Ta = MLX90640_GetTa(frame, &mlx90640);
				tr = Ta - TA_SHIFT;
			
				KeyAction();	
					
				MLX90640_CalculateTo(frame, &mlx90640, ((float)emissivity)/100, tr, mlx90640To);							
				MLX90640_BadPixelsCorrection((&mlx90640)->brokenPixels, mlx90640To, mode,  &mlx90640);
				MLX90640_BadPixelsCorrection((&mlx90640)->outlierPixels, mlx90640To,  mode, &mlx90640); 
									  
				if(UsartModule==DISABLE)//显示模式
				{
						MidTemp=mlx90640To[367];
						FindMaxMinTemp(mlx90640To);	
						AverageFilter(mlx90640To);//均值滤波
						SetDispBuf(mlx90640To);
						DispMaxMinTemp();
						LCD_Fill(191,292,222,317,BLACK);//清除中心点温度显示区域
						Disp_BigFloatNum(127,292,MidTemp,WHITE,BackgroundColor);	//中心点温度
						DispCross(112,152,WHITE);	//中心+字标							
				}
				else//串口传输模式
				{			
						CheckCode=0x5A5A;
						CheckCode+=0x0602;		
						for(i=0;i<768;i++)//放大10倍，转换成INT16,先低发送8，后高8，DMA循环发送
						{							
								UsartBuff[i*2+4]= (u16)(mlx90640To[i]*100)&0xFF;
								UsartBuff[i*2+5]= ((u16)(mlx90640To[i]*100)>>8)&0xFF;
								CheckCode+=(u16)(mlx90640To[i]*100);
						}
						UsartBuff[1540]= (u16)(Ta*100)&0xFF;
						UsartBuff[1541]= ((u16)(Ta*100)>>8)&0xFF;
						CheckCode+=(u16)(Ta*100);
						UsartBuff[1542]= (u16)CheckCode&0xFF;
						UsartBuff[1543]= ((u16)CheckCode>>8)&0xFF;
				}
				
				KeyAction();		

				TimeCount++;
				if(TimeCount>=240)//30秒检测一次,8HZ,250ms. 
				{
						TimeCount=0;
						DispBatPower();		
				}	
		}
}

// 外设时钟初始化
void RCC_PeriphInit(void)
{
       RCC_AHB1PeriphClockCmd(    RCC_AHB1Periph_GPIOA |  RCC_AHB1Periph_GPIOB 
  	                                                             |  RCC_AHB1Periph_GPIOC |  RCC_AHB1Periph_GPIOD
  	                                                             |  RCC_AHB1Periph_GPIOE, ENABLE);
}

/************************
函数功能：总初始化
输入参数：无
输出参数：无
备    注：无
************************/



void ALL_Config(void)
{
      RCC_ClocksTypeDef rcc_clocks;
      
       NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组4
       
       SysTick_Init();
	RCC_PeriphInit();
	MX_GPIO_Init();
	RCC_GetClocksFreq(&rcc_clocks);  // read system clock config
	PCLK1_CLK = rcc_clocks.PCLK1_Frequency;
	uart_init(460800);  // 460800
	flash_initialize();
	USB_Init();
	delay_ms(2000);
	LCD_init(); 
	IIC_GPIO_Init();
	ADC1_Init();
	os_printf("IR V0.02 %s %s \r\n",  __DATE__, __TIME__);
	
	
       os_printf("SystemClock = %ld \r\n", SystemCoreClock);
       os_printf("SysClk = %d, HCLK = %d, PCLK1 = %d, PCLK2 = %d\r\n", 
	   	                 rcc_clocks.SYSCLK_Frequency / FREQ_1MHz,  rcc_clocks.HCLK_Frequency / FREQ_1MHz,
	   	                 rcc_clocks.PCLK1_Frequency / FREQ_1MHz,     rcc_clocks.PCLK2_Frequency / FREQ_1MHz);  
	 
       FatFs_Demo();  

       #if (! DEBUG_EN)
	MYDMA_Config(DMA1_Stream6,DMA_Channel_4,(u32)&USART2->DR,(u32)UsartBuff,SEND_BUF_SIZE);//DMA1,STEAM6,CH4,外设为串口2,存储器为SendBuff,长度为:SEND_BUF_SIZE.
	#endif
}

