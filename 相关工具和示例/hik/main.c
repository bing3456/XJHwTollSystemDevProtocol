#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ComDef.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <dlfcn.h>
#include "VPRSDK.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>

#define SERV_PORT 5000
#define MAXLEN (102400*5)
int socket_svr = 0;
int nCount = 0;
int  socket_udp_main = 0;

typedef void *(*pReceiveMsg)(void* argsvr);
void SendToEmrc(char *msg);
void* ReceiveMsg(void* argsvr)
{
    struct sockaddr_in servaddr, cliaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);
    socklen_t len = sizeof(cliaddr);
    char *chImage = (char *)malloc(MAXLEN);
    char chTwo[280];
    char chPlate[16];
    char chFile[255] = {0x00};
	 time_t starttm, endtm;
    for(;;)
    {
        /* waiting for receive data */
		char mesg[4] = {0x00};
		char mm[4] = {1,2,3,4};
		printf("接收照片数据 \n");
		int n = recvfrom(*((int *)argsvr), mesg, 4, 0, (struct sockaddr *)&servaddr, &len);
		if(0 == memcmp(mesg,mm,4))
		{
			int piBinLen = 0;
			int piJpegLen = 0;
			memset(chPlate,0,16);
			//LogCDev("收到车牌");
			if(TRUE == VPR_GetVehicleInfo(chPlate,&piBinLen, (BYTE*)chTwo, &piJpegLen,(BYTE*)chImage))
			{
				printf("plate:%s\n",chPlate);
				memset(chFile,0,255);
				LogCDev("收到车牌%s",chPlate);
                sprintf(chFile,"/EMRCV4/IMAGE/TEMP/3TEMP.JPG");
                FILE *out;
                if((out = fopen(chFile,"wab"))!=NULL)
                {
                    printf("jpeglen:%d\n",piJpegLen);
                    fwrite(chImage,1,piJpegLen,out);
                    fclose(out);
                }
                memset(chFile,0,255);
                sprintf(chFile,"/EMRCV4/IMAGE/TEMP/3TEMP.BIN");
                if((out = fopen(chFile,"wat"))!=NULL)
                {
                    //printf("open%d\n",nSpecialLen);
                    fwrite(chTwo,1,piBinLen,out);
                    fclose(out);
                }
			//	LogCDev("图片已保存，通知主程序%s",chPlate);
				SendToEmrc(chPlate);
				//LogCDev("主程序通知完毕%s",chPlate);
                nCount++;
            }
        }
    }
    free(chImage);
}

void SendToEmrc(char *msg)
{
	struct MSG_TCOCMD tmp_sendtoemrc;
	char tmpmsg[51] ;
	memset(tmpmsg,0x00,51);
	memset((char *)&tmp_sendtoemrc,' ',sizeof(struct MSG_TCOCMD));
	sprintf(tmpmsg,"%-50s",msg);
	strncpy(tmp_sendtoemrc.Param,tmpmsg,sizeof(tmp_sendtoemrc.Param));
	strncpy(tmp_sendtoemrc.TCOType,TCOTYPE_TCOCMD,2);
	memcpy(tmp_sendtoemrc.CMD,"PLATE",sizeof(tmp_sendtoemrc.CMD));
	write(socket_udp_main,(char*)& tmp_sendtoemrc, sizeof(struct MSG_TCOCMD));
}

int createsvr()
{
    struct sockaddr_in servaddr, cliaddr;
    socket_svr = socket(AF_INET, SOCK_DGRAM, 0); /* create a socket */
    if(socket_svr <= 0)
        return -1;
    /* init servaddr */
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);
    /* bind address and port to socket */
    if(bind(socket_svr, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
    {
        perror("bind error");
        return -1;
    }
    pthread_t udpID;
    int *p=(int *)malloc(sizeof(int));
    *p=socket_svr;
    pReceiveMsg receMsg = ReceiveMsg;
    int ret=pthread_create(&udpID,NULL,receMsg,(void *)p);
    if(ret!=0){
        printf("getinfo thread error\n");
        return -1;
    }
    return 0;
}


void sigalrm_fn(int  sig)
{
   VPR_Capture();
    //alarm(2);
}

int  Init_EMRCMsg(void)
{
	printf("SetAlarm Success \r\n");
	if ((socket_udp_main = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		return -1;
	}
	struct sockaddr_in st_srv_addr;
	bzero(&st_srv_addr, sizeof(st_srv_addr));
	st_srv_addr.sin_family = AF_INET;
	st_srv_addr.sin_port = htons (8880);
	if(connect(socket_udp_main, (struct sockaddr *)&st_srv_addr, sizeof(st_srv_addr)) == -1)
	{
		return -1;
	}
}


void Init_VPR_TEST()
{
	createsvr();
	printf("%d  %d  \r\n",SERV_PORT,8888);
	BOOL bResult = VPR_Init(SERV_PORT,8888,(char*)"182.168.70.100");
	Init_EMRCMsg();
	return;

	if(TRUE == bResult)
	{
		printf("init success");
		signal(SIGALRM,sigalrm_fn);
		// alarm(2);
	}
	else
	{
		printf("init fail");
	}
}


int main() 
{
	 printf("hik 140606.6452\r\n");
	
	Init_VPR_TEST();

    printf("开始运行 \n i  初始化 \n h VPR_ComparePlate\n c VPR_Capture  \n s VPR_CheckStatus \n qVPR_Quit");
	LogCDev("初始化完毕");
    while (1)
    {
		sleep(5);
   //     char ch;
		
   //    ch=getchar();
   //     if(ch=='h')
   //     {
   //         int nResult = VPR_ComparePlate(0,4,(char*)"京A12345",(char*)"京A16789");
   //         printf("VPR_ComparePlate = %d\n",nResult);
   //     }
		 //else if(ch=='f')
   //     {
   //         SendToEmrc("黄新A69F88");
   //     }
		 //else if(ch=='d')
   //     {
   //         SendToEmrc("无车牌");
   //     }
   //     else if(ch=='c')
   //     {
   //         // time_t starttm, endtm;
   //         // time(&starttm);
   //         // time(&endtm);
   //         // while( difftime(endtm , starttm ) < 301)
   //         // {
   //             VPR_Capture();
   //             // time(&endtm);
   //             // sleep(1);
   //         // }
   //     }
   //     else if(ch=='s')
   //     {
   //         char chStatus[255] = {0x00};
   //         printf("befor status \n");
   //         VPR_CheckStatus(chStatus);
   //         printf("status is:%s\n",chStatus);
   //     }
   //     if(ch=='q')
   //     {
   //         if(VPR_Quit())
   //         {
   //            printf("quit system success\n");
   //            break;
   //         }
   //         else
   //            printf("quit system failed\n");
   //     }
    }
    return EXIT_SUCCESS;
}


