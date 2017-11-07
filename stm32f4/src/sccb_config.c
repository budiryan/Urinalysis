#include "sccb_config.h"
//延时函数，时间nTime x 1us
void sccb_delay(__IO uint32_t nTime)
{ 
  uint32_t temp;
  
  SysTick->LOAD  = nTime * 21;//(SystemCoreClock/1000000) - 1;      
  SysTick->VAL   = 0;                                      
  SysTick->CTRL  = 0x01; 

  do
   {
     temp=SysTick->CTRL;//读取当前倒计数值
   }
  while((temp&0x01)&&(!(temp&(1<<16))));//等待时间到达
  
  SysTick->VAL   = 0;                                      
  SysTick->CTRL  = 0;  
}

void OV7670_HW_Init(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB |
                         RCC_AHB1Periph_GPIOC, ENABLE);

  /* Connect DCMI pins to AF13 */
	//PC10:DCMI_D8 PC12:DCMI_D9 PB5:DCMI_D10 PD2:DCMI_D11 STM32F407VG无DCMI_D12、DCMI_D13
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_DCMI);//PA4:HSYNC
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_DCMI);//PA6:PIXCLK
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_DCMI);//PA10:D1

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_DCMI);//PB6:D5
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_DCMI);//PB7:VSYNC
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_DCMI);//PB8:D6
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_DCMI);//PB9:D7
  
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_DCMI);//PC6:D0
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_DCMI);//PC8:D2
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_DCMI);//PC9:D3
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_DCMI);//PC11:D4
  
    /* D0..D4(PC6/8/9/11)*/
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);//PC
  
  /* DCMI GPIO configuration */
  /* D5D6D7(PB6/8/9), VSYNC(PB7) */
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);//PB

  /* PCLK(PA6) */
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4 | GPIO_Pin_6 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);//PA
   
}

void Camera_Powerdown(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
   /* Enable GPIOC  clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;//PA9:PWDN
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_ResetBits(GPIOA,GPIO_Pin_9);
  _delay_us(500);
  GPIO_SetBits(GPIOA,GPIO_Pin_9);
}

void SCCB_GPIO_Config(void)
{
  //配置SCCB SCL和SDA 
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);
   
  GPIO_InitStructure.GPIO_Pin = SCCB_SDA_PIN ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_Init(SCCB_SDA_GPIO,&GPIO_InitStructure); 
  
  GPIO_InitStructure.GPIO_Pin = SCCB_SCL_PIN ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(SCCB_SCL_GPIO,&GPIO_InitStructure); 
   
}

void SCCB_SDA_IN_Config(void)//设置SDA输入
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin =  SCCB_SDA_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //做输入的时候必须上拉
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_Init(SCCB_SDA_GPIO,&GPIO_InitStructure);
}

void SCCB_SDA_OUT_Config(void)//设置SDA输出
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin =  SCCB_SDA_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(SCCB_SDA_GPIO,&GPIO_InitStructure);
}

__inline void SCCB_Stop(void)
{
  _delay_us(5); //结束之前SCL为低
  SCCB_SCL_High;
  _delay_us(5);
  SCCB_SDA_Low;
  _delay_us(5);
  SCCB_SDA_High;
  _delay_us(3);
}

__inline void SCCB_Start(void)
{
  SCCB_SDA_High;
  SCCB_SCL_High;
  _delay_us(5);
  SCCB_SDA_Low;
  _delay_us(5);
  SCCB_SCL_Low; //I2C启动后SDA和SCL都为低的情况结束该函数
}

//进入8位传递函数时要保证SCL为低
__inline void SCCB_data_trans(u8 outdata)
{
  u8 i;         
  for(i=0;i<8;i++)
  { 
    //_delay_us(100);   
    if(outdata & 0x80)
       SCCB_SDA_High;
    else
       SCCB_SDA_Low; //SDA在SCL为低的时候进行变化
    _delay_us(5);
    SCCB_SCL_High; //然后SCL变高，SDA保持不变
    _delay_us(5);
    SCCB_SCL_Low; //SCL变低，等待下一个数据
    outdata=outdata<<1;
  }
    //_delay_us(100);
    SCCB_SDA_IN_Config();//设置SDA为输入
    _delay_us(3);
    SCCB_SCL_High;//SCL变高，等待ACK信号
    //_delay_us(100);
}

__inline u8 SCCB_data_receive(void)
{
  u8 indata=0x00; 
  u8 i;
  SCCB_SDA_IN_Config();
  for(i=0;i<8;i++)
  {
    indata = indata<<1;
    _delay_us(5);
    SCCB_SCL_High; //SCL为高时接收数据
    if(SCCB_SDA_GPIO->IDR & SCCB_SDA_PIN)
      indata=indata+1;  
    _delay_us(5);
    SCCB_SCL_Low;
  }
    SCCB_SDA_OUT_Config();
    SCCB_SDA_High;
    _delay_us(5);
    SCCB_SCL_High; //将SCL拉高等待应答
  return indata;  
}

void SCCB_WriteByte(u8 reg_addr,u8 data)
{
   SCCB_Stop();
   _delay_us(10);
   SCCB_Start();
   SCCB_data_trans(0x42);//发送从机地址
   while(SCCB_ACK_STATE);//等待应答信号
   _delay_us(5);
   SCCB_SDA_OUT_Config(); 
   SCCB_SCL_Low;
   
   SCCB_data_trans(reg_addr);//发送寄存器地址
   while(SCCB_ACK_STATE);//等待应答信号
   _delay_us(5);
   SCCB_SDA_OUT_Config(); 
   SCCB_SCL_Low;
   
   SCCB_data_trans(data);//发送寄存器数据
   while(SCCB_ACK_STATE);
   _delay_us(5);
   SCCB_SCL_Low;
   SCCB_SDA_OUT_Config();
   SCCB_Stop();
}

u8 SCCB_ReadByte(u8 reg_addr)
{   
   u8 temp_data;
   SCCB_Stop();//设置寄存器地址后才开始读
   _delay_us(10);
   SCCB_Start();
   SCCB_data_trans(0x42);//发送从机地址
   while(SCCB_ACK_STATE);//等待应答信号
   _delay_us(5);
   SCCB_SDA_OUT_Config(); 
   SCCB_SCL_Low;
   
   SCCB_data_trans(reg_addr);//发送寄存器地址
   while(SCCB_ACK_STATE);//等待应答信号
   _delay_us(5);
   SCCB_SDA_OUT_Config(); 
   SCCB_SCL_Low;
	
	
	
    
    SCCB_Stop();
    _delay_us(5);
    SCCB_Start();
    SCCB_data_trans(0x43);//发送从机地址,读操作
    while(SCCB_ACK_STATE);//等待应带信号
    _delay_us(5);
    SCCB_SCL_Low;
     
    temp_data=SCCB_data_receive();
    //while(SCCB_ACK_STATE);
    _delay_us(5);
    SCCB_SDA_OUT_Config();
    SCCB_Stop();
    return (temp_data);
}

void XCLK_init_ON(void)//设置PA8口  ov7670时钟范围10MHZ-48MHZ
{
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能PA口
	
	  GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	  RCC_MCO1Config(RCC_MCO1Source_PLLCLK,RCC_MCO1Div_3);//MCO1:84MHz=168/2,帧率0x35
//	  RCC_MCO1Config(RCC_MCO1Source_HSE,RCC_MCO1Div_1);//MCO1:8mhz,帧率0x05,降低频率，成像不会花屏
}	
