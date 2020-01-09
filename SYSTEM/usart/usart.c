#include "sys.h"
#include "usart.h"	
#include "queue.h"

extern Buffer buffer;
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
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

#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	

//��ʼ��IO ����1 
//bound:������
void uart_init(u32 bound){
   //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9����ΪUSART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10����ΪUSART1
	
	//USART1�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA9��PA10

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
  USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 
	
	//USART_ClearFlag(USART1, USART_FLAG_TC);
	
#if EN_USART1_RX	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

#endif
	
}


//void USART1_IRQHandler(void)                	//����1�жϷ������
//{
//	u8 data;
//	u8 Res;
//	u8 temp=32;
//	
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
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
//	void USART1_IRQHandler(void)                        //����1�жϷ������
//	{
//		if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�
//		{
//				USART_ClearITPendingBit(USART1,USART_IT_RXNE); //����жϱ�־. 
//				Usart1_Buffer[Usart1_Rx] = USART_ReceiveData(USART1); 
//				Usart1_Rx++; 
//				Usart1_Rx &= 0xFF; //�ж��Ƿ���������		
//		} 
//		
//		if(Usart1_Buffer[Usart1_Rx-1] == 0x5A) Usart1_Tx=Usart1_Rx-1;
//		if((Usart1_Buffer[Usart1_Tx] == 0x5A)&&(Usart1_Buffer[Usart1_Rx-1] == 0x59)) //��⵽ͷ������¼�⵽β 
//		{
//			Usart1_Len = Usart1_Rx-1-Usart1_Tx;
//			Usart1_Sta = 1;
//		}
//		if(USART_GetFlagStatus(USART1,USART_FLAG_ORE) == SET) //��� 
//		{ 
//				USART_ClearFlag(USART1,USART_FLAG_ORE); //��SR 
//				USART_ReceiveData(USART1); //��DR 
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


	u8 state=0; //״̬����λ
	u8 Usart1_Err=0; //�����ʶλ
	u8 Usart1_Sta=0; //��־λ
	u8 Usart1_Buffer[1024];  //��������
	u8 Usart1_Rx=0;  //���������±�
	u8 Usart1_ID[8]; //ID��
	u8 Usart1_Type[2]; //����ѡ��
	u8 Len_Type=0;
	u8 Usart1_From_Addr[2]; //��λ����ַ
	u8 Len_From=0;
	u8 Usart1_To_Addr[2]; //��λ����ַ
	u8 Len_To=0;
	u8 Usart1_Len[4]; //Data����
	u16 Len_Data=0;
	u8 Usart1_Data[10000]; //Data
	u16 ID_num=0,Type_num=0,From_num=0,To_num=0,Len_num=0,Data_num=0,temp_num=0;
	u8 flag=0;
	void USART1_IRQHandler(void)                        //����1�жϷ������
	{
		u8 temp=32; //ɢ��ֵ����
		u8 data;
		if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�
		{
			USART_ClearITPendingBit(USART1,USART_IT_RXNE); //����жϱ�־. 
			Usart1_Buffer[Usart1_Rx] = USART_ReceiveData(USART1); 
			Usart1_Rx++; 
			Usart1_Rx &= 0x3FF; //�ж��Ƿ���������		
			
			// ֡ͷ
			if(state==0)
			{
				Usart1_Sta=0;
				ID_num=0,Type_num=0,From_num=0,To_num=0,Data_num=0,Len_num=0;
				Len_Data=0,Len_From=0,Len_To=0,Len_Type=0;
				if(Usart1_Buffer[Usart1_Rx-1] == 0x5A) state=1;
				else state=0;
			}
			//����ID
			else if(state==1)
			{
				Usart1_ID[ID_num++]=Usart1_Buffer[Usart1_Rx-1];
				if(ID_num==8) state=2;
				else state=1;
			}
			//���չ���
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
			//������λ����ַ
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
			//������λ����ַ
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
			//����Data����
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
			//����Data
			else if(state==6||state==7)
			{
				Usart1_Data[Data_num++]=Usart1_Buffer[Usart1_Rx-1];
				if(Data_num==Len_Data) state=8;
				else state=7;
			}
			//����Error
			else if(state==8)
			{
				Usart1_Err=Usart1_Buffer[Usart1_Rx-1];
				state=9;
			}
			//֡β
			else if(state==9)
			{
				if(Usart1_Buffer[Usart1_Rx-1] == 0x59) Usart1_Sta=1;
				else
				{
					state=0;
					Usart1_Rx=0;
				}
			}
			
			if(USART_GetFlagStatus(USART1,USART_FLAG_ORE) == SET) //��� 
			{ 
					USART_ClearFlag(USART1,USART_FLAG_ORE); //��SR 
					USART_ReceiveData(USART1); //��DR 
			} 
			
			if( Usart1_Sta==1 ) 
			{ 
//				//���ֲ�����ӡ��֤
//				//ID��
//				printf("ID: ");
//				for(temp_num=0;temp_num<8;temp_num++)
//				{
//					printf("%c",Usart1_ID[temp_num]);
//				}
//				printf("\r\n");
//				//����
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
				
				
				//����ʵ��
				//����ɢ���ַ�������
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
					if(buffer.length<32) printf("\n"); //��ֹpython����
					while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
				}
				//ֹͣ�����ַ�������
				else if(Len_Type==1)
				{
					flag=1;
				}
				//���������ַ�������
				else if(Len_Type==2)
				{
					flag=0;
				}
				//���ѭ�����л�����
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
