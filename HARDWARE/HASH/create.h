#ifndef __CREATE_H
#define __CREATE_H

#include "sys.h"
#include <stdio.h>
#include <stdlib.h>

void create_md5(u8 *encrypt, u8 *decrypt_md5);
void create_sha256(u8 *encrypt, u8 *decrypt_sha256);
void StringToHex(char *str, u8 *out,int tmplen);

#endif
