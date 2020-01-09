#include "queue.h"

extern Buffer buffer;

void Buf_init(Buffer *buffer)
{
	buffer->head = 0;
	buffer->tail = 0;
	buffer->length = 0;
}


u8 Buf_write(u8 data)
{
	if(buffer.length>=BUFFER_MAX)
	{
		return 1;
	}
	buffer.Buf[buffer.tail] = data;
	buffer.tail = (buffer.tail+1)%BUFFER_MAX;
	buffer.length++;
	return 0;
}

u8 Buf_read(u8* pdata)
{
	if(buffer.length==0)
		return 1;
	else
	{
		*pdata = buffer.Buf[buffer.head];
		buffer.head = (buffer.head+1)%BUFFER_MAX;
		buffer.length--;
		return 0;
	}
}

