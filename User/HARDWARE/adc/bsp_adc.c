/***********************************************
公司：轮趣科技（东莞）有限公司
品牌：WHEELTEC
官网：wheeltec.net
淘宝店铺：shop114407458.taobao.com 
速卖通: https://minibalance.aliexpress.com/store/4455017
版本：V1.0
修改时间：2023-03-02

Brand: WHEELTEC
Website: wheeltec.net
Taobao shop: shop114407458.taobao.com 
Aliexpress: https://minibalance.aliexpress.com/store/4455017
Version: V1.0
Update：2023-03-02

All rights reserved
***********************************************/

#include "./adc/bsp_adc.h"

__IO uint16_t ADC_ConvertedValue;
/**************************************************************************
Function: ADCx_GPIO_Config
Input   : none
Output  : none
函数功能：ADC端口初始化
入口参数: 无 
返回  值：无
**************************************************************************/	 
//ADC端口初始化，用于读取电压
static void ADCx_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// 打开 ADC IO端口时钟
	ADC_GPIO_APBxClock_FUN ( ADC_GPIO_CLK, ENABLE );
	
	// 配置 ADC IO 引脚模式
	// 必须为模拟输入
	GPIO_InitStructure.GPIO_Pin = ADC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	
	// 初始化 ADC IO
	GPIO_Init(ADC_PORT, &GPIO_InitStructure);				
}

/**************************************************************************
Function: ADCx_Mode_Config
Input   : none
Output  : none
函数功能：ADC模式初始化
入口参数: 无 
返回  值：无
**************************************************************************/	 
//ADC初始化，用于读取电压、
//中断方式
static void ADCx_Mode_Config(void)
{
	ADC_InitTypeDef ADC_InitStructure;	

	// 打开ADC时钟
	ADC_APBxClock_FUN (ADC_CLK, ENABLE );
	
	//复位ADC2,将外设 ADC2 的全部寄存器重设为缺省值
	ADC_DeInit(ADCx); 

	// ADC 模式配置
	// 只使用一个ADC，属于独立模式
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	
	// 禁止扫描模式，多通道才要，单通道不需要
	ADC_InitStructure.ADC_ScanConvMode = DISABLE ; 

	// 单次转换
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;

	// 不用外部触发转换，软件开启即可
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;

	// 转换结果右对齐
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	
	// 转换通道1个
	ADC_InitStructure.ADC_NbrOfChannel = 1;	
		
	// 初始化ADC
	ADC_Init(ADCx, &ADC_InitStructure);
	
	// 配置ADC时钟为PCLK2的6分频，即12MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
	
	// 配置 ADC 通道转换顺序和采样时间
	ADC_RegularChannelConfig(ADCx, ADC_CHANNEL, 1, 
	                         ADC_SampleTime_239Cycles5);
	
	// ADC 转换结束产生中断，在中断服务程序中读取转换值
	ADC_ITConfig(ADCx, ADC_IT_EOC, ENABLE);
	
	// 开启ADC ，并开始转换
	ADC_Cmd(ADCx, ENABLE);
	
	// 初始化ADC 校准寄存器  
	ADC_ResetCalibration(ADCx);
	// 等待校准寄存器初始化完成
	while(ADC_GetResetCalibrationStatus(ADCx));
	
	// ADC开始校准
	ADC_StartCalibration(ADCx);
	// 等待校准完成
	while(ADC_GetCalibrationStatus(ADCx));
}
/**************************************************************************
Function: Voltage_ADC_Init
Input   : none
Output  : none
函数功能：ADC初始化
入口参数: 无 
返回  值：无
**************************************************************************/	 
//用于读取电压
void Voltage_ADC_Init(void)
{
	ADCx_GPIO_Config();		//端口配置
	ADCx_Mode_Config();		//模式配置
}

/**************************************************************************
Function: Car_Select_ADC_GPIO_Config
Input   : none
Output  : none
函数功能：ADC端口初始化
入口参数: 无 
返回  值：无
**************************************************************************/	 
//车型选择ADC端口初始化，可用于选择车型
static void Car_Select_ADC_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// 打开 ADC IO端口时钟
	RCC_APB2PeriphClockCmd ( CAR_ADC_GPIO_CLK, ENABLE );
	
	// 配置 ADC IO 引脚模式
	// 必须为模拟输入
	GPIO_InitStructure.GPIO_Pin = CAR_ADC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	
	// 初始化 ADC IO
	GPIO_Init(CAR_ADC_PORT, &GPIO_InitStructure);				
}
/**************************************************************************
Function: Car_Select_ADC_Mode_Config
Input   : none
Output  : none
函数功能：ADC模式初始化
入口参数: 无 
返回  值：无
**************************************************************************/	 
//车型选择ADC端口初始化，可用于选择车型
static void Car_Select_ADC_Mode_Config(void)
{
	ADC_InitTypeDef ADC_InitStructure;	

	// 打开ADC时钟
	CAR_ADC_APBxClock_FUN ( CAR_ADC_CLK, ENABLE );
	
	 //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值
	ADC_DeInit(CAR_ADC); 

	// ADC 模式配置
	// 只使用一个ADC，属于独立模式
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	
	// 禁止扫描模式，多通道才要，单通道不需要
	ADC_InitStructure.ADC_ScanConvMode = DISABLE ; 

	// 单次转换
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;

	// 不用外部触发转换，软件开启即可
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;

	// 转换结果右对齐
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	
	// 转换通道1个
	ADC_InitStructure.ADC_NbrOfChannel = 1;	
		
	// 初始化ADC
	ADC_Init(CAR_ADC, &ADC_InitStructure);
	
	// 配置ADC时钟为PCLK2的6分频，即12MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
	
	// 配置 ADC 通道转换顺序和采样时间
	ADC_RegularChannelConfig(CAR_ADC, CAR_ADC_CHANNEL, 1, 
	                         ADC_SampleTime_239Cycles5);
							 
	// 不中断
	ADC_ITConfig(CAR_ADC, ADC_IT_EOC, DISABLE);
	
	// 开启ADC ，并开始转换
	ADC_Cmd(CAR_ADC, ENABLE);
	
	// 初始化ADC 校准寄存器  
	ADC_ResetCalibration(CAR_ADC);
	// 等待校准寄存器初始化完成
	while(ADC_GetResetCalibrationStatus(CAR_ADC));
	
	// ADC开始校准
	ADC_StartCalibration(CAR_ADC);
	// 等待校准完成
	while(ADC_GetCalibrationStatus(CAR_ADC));
	
}

/**************************************************************************
Function: Car_Select_ADC_Init
Input   : none
Output  : none
函数功能：ADC模式初始化
入口参数: 无 
返回  值：无
**************************************************************************/	 
//车型选择ADC初始化，可用于选择车型
void Car_Select_ADC_Init(void)
{
	Car_Select_ADC_GPIO_Config();	//端口配置
	Car_Select_ADC_Mode_Config();	//模式配置
}

/**************************************************************************
Function: AD sampling
Input   : ch：Channel of ADC1
Output  : AD conversion result
函数功能：AD采样
入口参数: ch：ADC2 的通道
返回  值：AD转换结果
**************************************************************************/	 		
u16 Get_Adc2(u8 ch)   
{
	  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC2, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC2,ADC通道,采样时间为239.5周期	  			     
	ADC_SoftwareStartConvCmd(ADC2, ENABLE);		//使能指定的ADC2的软件转换启动功能		 
	while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC ));//等待转换结束
	return ADC_GetConversionValue(ADC2);	//返回最近一次ADC1规则组的转换结果
}

/**************************************************************************
Function: Get_Voltage
Input   : none
Output  : none
函数功能：获取电压
入口参数: 无 
返回  值：无
**************************************************************************/	 
u16 Get_Voltage(void)
{
	u16 Voltage;
	
	Voltage = Max_Voltage/Max_Voltage_ADC*Get_Adc2(Battery_Ch)*Ratio*100;//读取电压，放大100倍储存
	
	return Voltage;
}
/**************************************************************************
Function: Get_Voltage
Input   : none
Output  : none
函数功能：获取ADC的值
入口参数: 无 
返回  值：无
**************************************************************************/	 
//获取ADC的值，主要用于车型的选择
u16 Get_Adc(u8 ch)   
{
	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	__nop();
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束
	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}


/********************************END OF FILE***********************************/
