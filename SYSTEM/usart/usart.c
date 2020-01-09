#include "sys.h"
#include "usart.h"	
#include "queue.h"

extern Buffer buffer;
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif

//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif
 
void print(unsigned char output[])
{
	int i;
	for(i=0 ; i<32 ; i++)
	{
		printf("%02X",output[i]);
	}
	printf("\n");
}

#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	

//初始化IO 串口1 
//bound:波特率
void uart_init(u32 bound){
   //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1
	
	//USART1端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
  USART_Cmd(USART1, ENABLE);  //使能串口1 
	
	//USART_ClearFlag(USART1, USART_FLAG_TC);
	
#if EN_USART1_RX	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、

#endif
	
}


//void USART1_IRQHandler(void)                	//串口1中断服务程序
//{
//	u8 data;
//	u8 Res;
//	u8 temp=32;
//	
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
//	{
//		Res =USART_ReceiveData(USART1);

//		while(buffer.length>=32)
//		{
//			while(temp--)
//			{
//				Buf_read(&data);
//				printf("%02X",data);
//				if(temp==0) printf("\n");
//			}
//			temp=32;
//		}
//		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
//	} 
//  
//} 


//	u8 Usart1_Buffer[256];
//	u8 Usart1_Rx=0,Usart1_Tx=0;
//	u8 Usart1_Len=0,Usart1_Sta=0;
//	u8 temp_num=0;
//	void USART1_IRQHandler(void)                        //串口1中断服务程序
//	{
//		if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
//		{
//				USART_ClearITPendingBit(USART1,USART_IT_RXNE); //清除中断标志. 
//				Usart1_Buffer[Usart1_Rx] = USART_ReceiveData(USART1); 
//				Usart1_Rx++; 
//				Usart1_Rx &= 0xFF; //判断是否计数到最大		
//		} 
//		
//		if(Usart1_Buffer[Usart1_Rx-1] == 0x5A) Usart1_Tx=Usart1_Rx-1;
//		if((Usart1_Buffer[Usart1_Tx] == 0x5A)&&(Usart1_Buffer[Usart1_Rx-1] == 0x59)) //检测到头的情况下检测到尾 
//		{
//			Usart1_Len = Usart1_Rx-1-Usart1_Tx;
//			Usart1_Sta = 1;
//		}
//		if(USART_GetFlagStatus(USART1,USART_FLAG_ORE) == SET) //溢出 
//		{ 
//				USART_ClearFlag(USART1,USART_FLAG_ORE); //读SR 
//				USART_ReceiveData(USART1); //读DR 
//		} 
//		
//		
//		if( Usart1_Sta==1 ) 
//		{ 
//				for(temp_num=0;temp_num<Usart1_Len;temp_num++,Usart1_Tx++)
//				{
//					if(temp_num==0) continue;
//					printf("%c",Usart1_Buffer[Usart1_Tx]);
//				}
//				Usart1_Rx=0;
//				Usart1_Tx=0;
//				Usart1_Sta=0;
//		}  
//	} 


	u8 state=0; //状态控制位
	u8 Usart1_Err=0; //错误标识位
	u8 Usart1_Sta=0; //标志位
	u8 Usart1_Buffer[1024];  //缓冲数组
	u8 Usart1_Rx=0;  //缓冲数组下标
	u8 Usart1_ID[8]; //ID号
	u8 Usart1_Type[2]; //功能选择
	u8 Len_Type=0;
	u8 Usart1_From_Addr[2]; //上位机地址
	u8 Len_From=0;
	u8 Usart1_To_Addr[2]; //下位机地址
	u8 Len_To=0;
	u8 Usart1_Len[4]; //Data长度
	u16 Len_Data=0;
	u8 Usart1_Data[10000]; //Data
	u16 ID_num=0,Type_num=0,From_num=0,To_num=0,Len_num=0,Data_num=0,temp_num=0;
	u8 flag=0;
	void USART1_IRQHandler(void)                        //串口1中断服务程序
	{
		u8 temp=32; //散列值长度
		u8 data;
		if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
		{
			USART_ClearITPendingBit(USART1,USART_IT_RXNE); //清除中断标志. 
			Usart1_Buffer[Usart1_Rx] = USART_ReceiveData(USART1); 
			Usart1_Rx++; 
			Usart1_Rx &= 0x3FF; //判断是否计数到最大		
			
			// 帧头
			if(state==0)
			{
				Usart1_Sta=0;
				ID_num=0,Type_num=0,From_num=0,To_num=0,Data_num=0,Len_num=0;
				Len_Data=0,Len_From=0,Len_To=0,Len_Type=0;
				if(Usart1_Buffer[Usart1_Rx-1] == 0x5A) state=1;
				else state=0;
			}
			//接收ID
			else if(state==1)
			{
				Usart1_ID[ID_num++]=Usart1_Buffer[Usart1_Rx-1];
				if(ID_num==8) state=2;
				else state=1;
			}
			//接收功能
			else if(state==2)
			{
				Usart1_Type[Type_num++]=Usart1_Buffer[Usart1_Rx-1];
				if(Type_num==2)
				{
					state=3;
					for(temp_num=0;temp_num<2;temp_num++)
					{
						Len_Type=Len_Type*10+(Usart1_Type[temp_num]-'0');
					}
				}
				else state=2;
			}
			//接收上位机地址
			else if(state==3)
			{
				Usart1_From_Addr[From_num++]=Usart1_Buffer[Usart1_Rx-1];
				if(From_num==2)
				{
					state=4;
					for(temp_num=0;temp_num<2;temp_num++)
					{
						Len_From=Len_From*10+(Usart1_From_Addr[temp_num]-'0');
					}
					if(Len_From!=1) state=0;
				}	
				else state=3;
			}
			//接收下位机地址
			else if(state==4)
			{
				Usart1_To_Addr[To_num++]=Usart1_Buffer[Usart1_Rx-1];
				if(To_num==2)
				{
					state=5;
					for(temp_num=0;temp_num<2;temp_num++)
					{
						Len_To=Len_To*10+(Usart1_To_Addr[temp_num]-'0');
					}
					if(Len_To!=2) state=0;
				}	
				else state=4;
			}
			//接收Data长度
			else if(state==5)
			{
				Usart1_Len[Len_num++]=Usart1_Buffer[Usart1_Rx-1];
				if(Len_num==4)
				{
					state=6;
					for(temp_num=0;temp_num<4;temp_num++)
					{
						Len_Data = Len_Data*10+(Usart1_Len[temp_num]-'0');
					}
				}
				else state=5;
			}
			//接收Data
			else if(state==6||state==7)
			{
				Usart1_Data[Data_num++]=Usart1_Buffer[Usart1_Rx-1];
				if(Data_num==Len_Data) state=8;
				else state=7;
			}
			//接收Error
			else if(state==8)
			{
				Usart1_Err=Usart1_Buffer[Usart1_Rx-1];
				state=9;
			}
			//帧尾
			else if(state==9)
			{
				if(Usart1_Buffer[Usart1_Rx-1] == 0x59) Usart1_Sta=1;
				else
				{
					state=0;
					Usart1_Rx=0;
				}
			}
			
			if(USART_GetFlagStatus(USART1,USART_FLAG_ORE) == SET) //溢出 
			{ 
					USART_ClearFlag(USART1,USART_FLAG_ORE); //读SR 
					USART_ReceiveData(USART1); //读DR 
			} 
			
			if( Usart1_Sta==1 ) 
			{ 
//				//各种操作打印验证
//				//ID号
//				printf("ID: ");
//				for(temp_num=0;temp_num<8;temp_num++)
//				{
//					printf("%c",Usart1_ID[temp_num]);
//				}
//				printf("\r\n");
//				//功能
//				printf("Type: ");
//				printf("%d\r\n",Len_Type);
//				//From_Addr
//				printf("From_Addr: ");
//				printf("%d\r\n",Len_From);
//				//To_Addr
//				printf("To_Addr: ");
//				printf("%d\r\n",Len_To);
//				//Len
//				printf("Len: ");
//				printf("%d\r\n",Len_Data);
//				//Data
//				printf("Data: ");
//				for(temp_num=0;temp_num<Len_num;temp_num++)
//				{
//					printf("%c",Usart1_Data[temp_num]);
//				}
//				printf("\r\n");
//				//Error
//				printf("Error: ");
//				printf("%c\r\n",Usart1_Err);
//				printf("\r\n");
				
				
				//功能实现
				//读出散列字符串序列
				if(Len_Type==0)
				{
					flag=0;
					while(buffer.length>=32)
					{
						while(temp--)
						{
							Buf_read(&data);
							printf("%02X",data);
							if(temp==0) printf("\n");
						}
						temp=32;
					}
					if(buffer.length<32) printf("\n"); //防止python阻塞
					while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
				}
				//停止产生字符串序列
				else if(Len_Type==1)
				{
					flag=1;
				}
				//开启产生字符串序列
				else if(Len_Type==2)
				{
					flag=0;
				}
				//清空循环队列缓冲区
				else if(Len_Type==3)
				{
					while(buffer.length>=1)
					{	
						Buf_read(&data);
					}
				}
				
				state=0;
				Usart1_Rx=0;
			}  
		} 
	} 
#endif	
