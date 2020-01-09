#include "rng.h"
#include "delay.h"

u8 RNG_Init(void)
{
	u16 i;
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG,ENABLE);
	RNG_Cmd(ENABLE);
	while(RNG_GetFlagStatus(RNG_FLAG_DRDY)==0)
	{
		i++;
		delay_us(100);
		if(i>=10000)
		{
			return 1;
		}			
	}
	return 0;
}

u32 RNG_Get_RandomNum(void)
{
	while(RNG_GetFlagStatus(RNG_FLAG_DRDY)==0);
	return RNG_GetRandomNumber();
}

int RNG_Get_RandomRange(int min,int max)
{
	return min+RNG_Get_RandomNum()%(max-min+1);
}
