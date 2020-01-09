#ifndef __QUEUE_H
#define __QUEUE_H

#include "sys.h"

#define BUFFER_MAX  1024 // 32个为一个序列，共32序列存满(0~1023),1024位留空

typedef struct
{
	u16 head;
	u16 tail;
	u16 length;
	u8 Buf[BUFFER_MAX];
}Buffer;

void Buf_init(Buffer* buffer);
u8 Buf_write(u8 data);
u8 Buf_read(u8* pdata);


#endif
