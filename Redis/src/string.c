#include <stdio.h>
#include <stdlib.h>
typedef struct SDS
{
	int len;
	int free;
	char* buf;
}SDS;

//create a SDS including string(C)
SDS sdsnew(void* buf)
{
	char* BufTmp = (char*)buf;
	int len = 0;
	while(*BufTmp != '\0')
	{
		++len;
		BufTmp++;
	}
	SDS sds;
	sds.buf = (char*)malloc(sizeof(char)*len*2);
	sds.buf = (char*)buf;
	sds.len = len;
	sds.free = len;
	return sds;
}

//create a NULL string
SDS sdsempty()
{
	SDS sds;
	sds.len = 0;
	sds.free = 0;
	sds.buf = (char*)malloc(sizeof(char));
	sds.buf[0] = '\0';
	return sds;
}

int sdslen(SDS sds)
{
	return sds.len;
}

int main(void)
{
	char TestStr[20] = "123456789";
	SDS sds = sdsnew(TestStr);
	printf("%s\n",sds.buf);
	printf("%d\n",sdslen(sds));
	sds = sdsempty();
	printf("%s\n",sds.buf);
	return 0;
}
