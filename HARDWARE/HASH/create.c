#include "create.h"
#include "md5.h"
#include "sha256.h" 

// 字符串表示的十六位数与对应的十六位数转换函数
void StringToHex(char *str, u8 *out,int tmplen)
{
	char *p = str;
	char high = 0, low = 0;
	int cnt = 0;
	while (cnt < (tmplen / 2))
	{
		high = ((*p > '9') && ((*p <= 'F') || (*p <= 'f'))) ? *p - 48 - 7 : *p - 48;
		low = (*(++p) > '9' && ((*p <= 'F') || (*p <= 'f'))) ? *(p)-48 - 7 : *(p)-48;
		out[cnt] = ((high & 0x0f) << 4 | (low & 0x0f));
		p++;
		cnt++;
	}
	if (tmplen % 2 != 0) out[cnt] = ((*p > '9') && ((*p <= 'F') || (*p <= 'f'))) ? *p - 48 - 7 : *p - 48;
}

// MD5哈希散列生成函数
void create_md5(u8 *encrypt, u8 *decrypt_md5)
{
	unsigned int length=0;
	MD5_CTX md5;
	MD5Init(&md5);
	while(encrypt[length]!='\0')
	{
		length++;
	}
	MD5Update(&md5, encrypt, length);
	MD5Final(&md5, decrypt_md5);
}

// SHA256哈希散列生成函数
void create_sha256(u8 *encrypt, u8 *decrypt_sha256)
{
	char temp[64];
	StrSHA256(encrypt, 64, temp);
  StringToHex(temp,decrypt_sha256,64);
}
