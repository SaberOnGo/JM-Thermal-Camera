#include "KEY.h"
#include "ALL_Includes.h"



u16 KeyCont=0,KeyTrag=0,KeySecond=0;
u16 KeyValue[5][3]={0};
//u8 DispState = DISABLE;//
u8 SetTempState = 0;//设置温度状态，0-非设置，1-设置高温，2-设置低温, 3-设置反射率
u8 emissivity = 95;
u8 RuleAuto=ENABLE;

void MX_GPIO_Init(void)
{
       GPIO_InitTypeDef GPIO_InitStructure;
	
  	RCC_AHB1PeriphClockCmd(    RCC_AHB1Periph_GPIOA |  RCC_AHB1Periph_GPIOB 
  	                                                             |  RCC_AHB1Periph_GPIOC |  RCC_AHB1Periph_GPIOD
  	                                                             |  RCC_AHB1Periph_GPIOE, ENABLE);
	
       //按键管脚初始化
  	GPIO_InitStructure.GPIO_Pin = KeyUp_Pin;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  	GPIO_Init(KeyUp_Port, &GPIO_InitStructure);
  	
       GPIO_InitStructure.GPIO_Pin = KeyDown_Pin;
  	GPIO_Init(KeyDown_Port, &GPIO_InitStructure);

       GPIO_InitStructure.GPIO_Pin = KeySet_Pin;
  	GPIO_Init(KeySet_Port, &GPIO_InitStructure);

       // CHRG 管脚
  	GPIO_InitStructure.GPIO_Pin = BatCHRG_Pin;
  	GPIO_Init(BatCHRG_Port, &GPIO_InitStructure);

       // STDBY 管脚
  	GPIO_InitStructure.GPIO_Pin = BatSTDBY_Pin;
  	GPIO_Init(BatSTDBY_Port, &GPIO_InitStructure);

       //电源按键
       GPIO_InitStructure.GPIO_Pin = KeyDect_Pin;
  	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  	//GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  	GPIO_Init(KeyDect_Port, &GPIO_InitStructure);
  	
	//电源控制脚
	GPIO_InitStructure.GPIO_Pin = Power_Pin;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	//GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  	GPIO_Init(Power_Port, &GPIO_InitStructure);

       //ADC 电源开关
       GPIO_InitStructure.GPIO_Pin = ADC_Ctrl_Pin; // AD检测电源控制引脚配置
  	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	//GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  	GPIO_Init(ADC_Ctrl_Port, &GPIO_InitStructure);
		
       ADC_Ctrl_Open();
}

//按键扫描
void KeyScan(void)
{    
     u8 i;
     u16 ReadData;
    
    for(i=0;i<4;i++)
    {
        switch(i)
        {    
            case 0:ReadData=(~(KeySet_Port->IDR))&KeySet_Pin;break;
            case 1:ReadData=(~(KeyDown_Port->IDR))&KeyDown_Pin;break;
            case 2:ReadData=(~(KeyUp_Port->IDR))&KeyUp_Pin;break;
						case 3:ReadData=(~(KeyDect_Port->IDR))&KeyDect_Pin;break;					
        }
        KeyTrag = KeyValue[i][0];
        KeyCont = KeyValue[i][1];
        KeySecond = KeyTrag & ReadData;
        KeyTrag  = ReadData &(ReadData ^ KeyCont);
        KeyCont  = ReadData;   
        KeyValue[i][0] = KeyTrag;
        KeyValue[i][1] = KeyCont;
        KeyValue[i][2] = KeySecond;
    }
		
		ReadData=(~(BatCHRG_Port->IDR))&BatCHRG_Pin;		
    KeyCont = KeyValue[4][1];
		KeyTrag = ReadData ^ KeyCont;
		KeyCont  = ReadData;
		KeyValue[4][1] = KeyCont;
		KeyValue[4][0] = KeyTrag;
}

/*******************
获取按键值
返回值:
  0--Set
  1--Dwon
  2--Up
	3--Power
	4--BatCHRG
  5--Up&Down
  0xff--No key on
*******************/
u8 GetKeyBit(void)
{
     u8 i;
     
     KeyScan();
	
		 if((KeyValue[1][2]!=0)&&(KeyValue[2][1]!=0))
				return 5;	
		 
     for(i=0;i<4;i++)
     {
         if(KeyValue[i][2]!=0)
            return i;
     }
		 
		 if(KeyValue[4][0]!=0)//检测到充电插入或拔出动作
            return 4;
		 
     return NoKeyOn;
}


//按键动作
void KeyAction(void)
{
		u8 KeySate;
	
		KeySate=GetKeyBit();
		switch(KeySate)
		{
			case 0:SetTemp();break;//SET ON
			case 1:SetDownTemp();break;
			case 2:SetUpTemp();break;
			case 3:
			{
		             #if 0
					delay_ms(3000);
					if(KeyDect_State==0)
					{
							LCD_Fill(0,0,239,319,BLACK);
							LCD_ShowString(90,140,"Closing",WHITE,BackgroundColor);
							LCD_ShowString(20,160,"Pls Release The Power Key",WHITE,BackgroundColor);
							while(KeyDect_State==0);//等待按下松下					
							delay_ms(2000);
							NVIC_SystemReset();//电源键按下，系统复位关机
					}	
					#else
                                   LCD_Snapshot();
					#endif				
			}
			case 4:
			{
					delay_ms(10);
					DispBatPower();
					break;
			}//插拔充电，立即检测显示电量

			#if  (!DEBUG_EN)
			case 5: 
			{				
					UsartModule =!UsartModule;//取反，切换图像或串口模式
					if(UsartModule==DISABLE)
					{							
							DMA_Cmd(DMA1_Stream6, DISABLE);              //关闭DMA传输 
							USART_DMACmd(USART2,USART_DMAReq_Tx,DISABLE);  //关闭串口2的DMA发送
							USART_Cmd(USART2, DISABLE);//关闭串口	
					}		
					else
					{
							USART_Cmd(USART2, ENABLE);//开启串口	
							USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);  //使能串口2的DMA发送
							MYDMA_Enable(DMA1_Stream6,SEND_BUF_SIZE);     //开始一次DMA传输！
					}				
					break;
			}
			#endif
		}
}

//设置温度，标红卡尺，并存储数据
void SetTemp(void)
{
		u16 EepData[5]={EepWrFlag,0x00,0x00,0x00,0x00};	
	
		switch(SetTempState)
		{
				case 0://进入自动/非自动Rule设置
				{
						LCD_Fill(30,8,77,23,BLACK);
						if(RuleAuto==DISABLE)						
								LCD_ShowString(30,8,"NoAuto",RED,BLACK);
						else
								LCD_ShowString(30,8,"Auto",RED,BLACK);
						SetTempState=1;
						break;
				}
					
				case 1://自动/非自动Rule确认
				{
						LCD_Fill(30,8,77,23,BLACK);
						if(RuleAuto==DISABLE)					
						{
								LCD_ShowString(30,8,"NoAuto",WHITE,BLACK);
								LCD_ShowNum(0,14,TmpH,RED,BackgroundColor);
								SetTempState=2;
						}
						else
						{
								LCD_ShowString(30,8,"Auto",WHITE,BLACK);								
								LCD_ShowString(110,8,"0.",RED,BackgroundColor);
								LCD_ShowNum(126,8,emissivity,RED,BackgroundColor);
								SetTempState=4;
						}
						break;
				}
							
				case 2://设置标尺低温
				{
						LCD_ShowNum(0,14,TmpH,WHITE,BackgroundColor);
						LCD_ShowNum(0,289,TmpL,RED,BackgroundColor);						
						SetTempState=3;
						break;
				}
				
				case 3://设置反射率
				{						
						if(RuleAuto==DISABLE)
								LCD_ShowNum(0,289,TmpL,WHITE,BackgroundColor);
						LCD_ShowString(110,8,"0.",RED,BackgroundColor);
						LCD_ShowNum(126,8,emissivity,RED,BackgroundColor);
						SetTempState=4;
						break;
				}
				
				case 4://保存参数
				{
						LCD_ShowString(110,8,"0.",WHITE,BackgroundColor);
						LCD_ShowNum(126,8,emissivity,WHITE,BackgroundColor);
						if(RuleAuto==DISABLE)
						{
								StorageTmpH=TmpH;
								StorageTmpL=TmpL;
						}
						EepData[1]=(u16)StorageTmpH;
						EepData[2]=(u16)StorageTmpL;
						EepData[3]=(u16)emissivity;
						EepData[4]=(u16)RuleAuto;
					
						EEP_Wirte(EepData,5);//写入EEP
						SetTempState=0;
						break;
				}
		}
}

//UP键按下，温度增加设置
void SetUpTemp(void)
{
		switch(SetTempState)
		{
				case 1:
				{
						SetAutoRuleUpDown();
						break;
				}
				
				case 2:
				{
						if(TmpH<300)
						{
								TmpH+=5;
								LCD_Fill(0,14,23,29,BLACK);
								LCD_ShowNum(0,14,TmpH,RED,BackgroundColor);
						}		
						break;
				}
		
				case 3:
				{
						if(TmpL<TmpH-5)
						{
								TmpL+=5;
								LCD_Fill(0,289,23,304,BLACK);
								LCD_ShowNum(0,289,TmpL,RED,BackgroundColor);
						}		
						break;
				}
				
				case 4:
				{
						SetEmissivityUp();
						break;
				}
		}
}

//Down键按下，温度降低设置
void SetDownTemp(void)
{
		switch(SetTempState)
		{
			  case 1:
				{
						SetAutoRuleUpDown();
						break;
				}
				
				case 2:
				{
						if((TmpH>-40)&&(TmpH>TmpL+5))
						{
								TmpH-=5;
								LCD_Fill(0,14,23,29,BLACK);
								LCD_ShowNum(0,14,TmpH,RED,BackgroundColor);						
						}		
						break;
				}
		
				case 3:
				{
						if(TmpL>-40)
						{
								TmpL-=5;
								LCD_Fill(0,289,23,304,BLACK);
								LCD_ShowNum(0,289,TmpL,RED,BackgroundColor);
						}	
						break;		
				}
				
				case 4:
				{
						SetEmissivityDown();
						break;
				}
		}
}
//UP键按下，反射率调高
void SetEmissivityUp(void)
{
		if(emissivity<99)
				emissivity+=1;
		LCD_ShowNum(126,8,emissivity,RED,BackgroundColor);
}
//Down键按下，反射率调低
void SetEmissivityDown(void)
{
		if(emissivity>10)
				emissivity-=1;
		LCD_ShowNum(126,8,emissivity,RED,BackgroundColor);
}

//Up&Down键设置自动标尺
void SetAutoRuleUpDown(void)
{
		LCD_Fill(30,8,77,23,BLACK);
		if(RuleAuto==DISABLE)
		{
				RuleAuto=ENABLE;							
				LCD_ShowString(30,8,"Auto",RED,BLACK);
				LCD_Fill(0,14,23,29,BLACK);
				LCD_Fill(0,289,23,304,BLACK);
		}
		else
		{
				RuleAuto=DISABLE;
				LCD_ShowString(30,8,"NoAuto",RED,BLACK);
				TmpH=StorageTmpH;
				TmpL=StorageTmpL;
				LCD_ShowNum(0,14,TmpH,WHITE,BackgroundColor);
				LCD_ShowNum(0,289,TmpL,WHITE,BackgroundColor);
		}
}
