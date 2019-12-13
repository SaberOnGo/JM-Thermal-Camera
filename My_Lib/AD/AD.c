#include "AD.h"
#include "ALL_Includes.h"


/************************************
获取ADC1的转换值

************************************/
u16 ADC1_GetVal(void)
{
	 u16 AD_Value;
	 u8  AD_i;
	 u32 AD_temp = 0;

	 for(AD_i = 0; AD_i < 5; AD_i++)
	 {	 	
			ADC1->CR2 |=BIT(30);//开始转化规则通道
			while((ADC1->SR&BIT(1))==0);//等待转换完成
			AD_temp	+= (ADC1->DR&0XFFF);
	 }
	 AD_Value =(((AD_temp/5)*330*5/3)>>12);//获取电池电压值*100,
	 
	 return AD_Value;
}

/*****************************
ADC1的0(PC0)

单次转换

*****************************/
void ADC1_Init(void)
{
  ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  GPIO_InitTypeDef      GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  /* Configure ADC Channel 2 pins as analog input ,PA3*/ 
  GPIO_InitStructure.GPIO_Pin       = BAT_ADC_Pin;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_NOPULL ;
  GPIO_Init(BAT_ADC_PORT, &GPIO_InitStructure);

   /* ADC Common Init **********************************************************/
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent; // 独立模式
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; // 不使用多ADC模式的DMA传送
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  /* ADC1 Init ****************************************************************/
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;    // 12bit
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;                  // 不使用扫描模式
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;  // 单次转换
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//不使用外部触发
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;    //  数据右对齐
  ADC_InitStructure.ADC_NbrOfConversion = 1;                             //   多通道转换时应修改此处
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channel10 configuration *************************************/  
  ADC_RegularChannelConfig(ADC1, BAT_ADC_Channel, 1, ADC_SampleTime_56Cycles);

  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
}



