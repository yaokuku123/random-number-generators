#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "queue.h"
#include "create.h"
#include "rng.h"


// 参数初始化
Buffer buffer;
u8 i=0;
u32 num=0;
u8 output[32];
u16 Res;
u8 encrypt1[64],encrypt2[64],key[64],input1[64],input2[64];
u8 buff[100];
extern u8 flag;
// MD5与SHA256异或字符串生成函数，修改传入原始字符串函数
void create_hash(u8 encrypt1[],u8 encrypt2[],u8 output[])
{
		u8 i;
		u8 decrypt_md5_1[16],decrypt_md5_2[16],decrypt_md5[32],decrypt_sha256[32];
	
		create_sha256(encrypt1,decrypt_sha256);
		create_md5(encrypt2,decrypt_md5_1);
		create_md5(decrypt_md5_1,decrypt_md5_2);
		for(i=0;i<16;i++)
		{
			decrypt_md5[i] = decrypt_md5_1[i];
			decrypt_md5[i+16] = decrypt_md5_2[i];
		}
		//异或操作
		for(i=0 ; i<32 ; i++)
		{
			output[i] = decrypt_md5[i] ^ decrypt_sha256[i];
		}
		//更改encrypt
		for(i=0;i<32;i++)
		{
			encrypt1[i] = decrypt_md5[i];
			encrypt1[i+32] = decrypt_sha256[i];
			
			encrypt2[i] = decrypt_sha256[i];
			encrypt2[i] = decrypt_md5[i];
		}
}


void device_init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(168);		
	uart_init(115200);	
	LED_Init();		  	
	Buf_init(&buffer);
	RNG_Init();
}


void init(void)
{
	device_init();
	for(i=0 ; i<64 ; i++)
	{
		input1[i] = RNG_Get_RandomRange(0,256);
		input2[i] = RNG_Get_RandomRange(0,256);
		key[i] = RNG_Get_RandomRange(0,256);
	}
	for(i=0 ; i<64 ; i++)
	{
		encrypt1[i] = input1[i] ^ key[i];
		encrypt2[i] = key[i] ^ input2[i];
	}
}


int main(void)
{ 
	init();
	create_hash(encrypt1,encrypt2,output);
	while(1)
	{	
		if(flag==0)
		{
			while(buffer.length<BUFFER_MAX)
			{
				for(i=0;i<32;i++)
				{
					Buf_write(output[i]);
				}
				create_hash(encrypt1,encrypt2,output);
			}

			
			if(++num == 5000000)
			{
				u8 temp_num=0,data;
				num = 0;
				while(buffer.length>32 && temp_num<32)
				{
					Buf_read(&data);
					temp_num++;
				}
				printf("\n");
			}
		}
		
		LED0 = 0;
	}
}
