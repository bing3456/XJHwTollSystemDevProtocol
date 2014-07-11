#ifndef CONVERT_H
#define CONVERT_H

#include "iconv.h"
//����ת��:��һ�ֱ���תΪ��һ�ֱ���
int code_convert(char *from_charset,char *to_charset,char *inbuf,int inlen,char *outbuf,int outlen)
{
	iconv_t cd;
	int rc;
	char **pin = &inbuf;
	char **pout = &outbuf;

	cd = iconv_open(to_charset,from_charset);
	if (cd==0)
	{
	    printf("iconv_open error\n");
	    return -1;
	}
	memset(outbuf,0,outlen);
	iconv(cd,pin,(size_t*)&inlen,pout,(size_t*)&outlen);
	iconv_close(cd);
	printf("last:%s\n", outbuf);
	return 0;
}
//UNICODE��תΪGB2312��
int u2g(char *inbuf,int inlen,char *outbuf,int outlen)
{
	return code_convert("utf-8","gb2312",inbuf,inlen,outbuf,outlen);
}
//GB2312��תΪUNICODE��
int g2u(char *inbuf,size_t inlen,char *outbuf,size_t outlen)
{
	return code_convert("gb2312","utf-8",inbuf,inlen,outbuf,outlen);
}

#endif
