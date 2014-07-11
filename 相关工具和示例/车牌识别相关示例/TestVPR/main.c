#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
//#include "VPR.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#include <time.h>
#include <signal.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "convert.h"

#define SERV_PORT 5000
#define MAXLEN 102400*5
int socket_svr = 0;
int nCount = 0;
typedef enum{TRUE=1,FALSE=!TRUE} BOOL;
typedef unsigned int UINT;
typedef char BYTE;

BOOL (*VPR_Init)(UINT);
BOOL (*VPR_Quit)();
BOOL (*VPR_Capture)();
BOOL (*VPR_GetVehicleInfo)(char*,int*,BYTE*,int*,BYTE*);
int (*VPR_ComparePlate)(int ,int ,char*,char*);
BOOL (*VPR_ComparePlateBin)(BYTE*,BYTE*);
BOOL (*VPR_CheckStatus)(char*);
void sigalrm_fn(int  sig)
{
    //VPR_Capture();
    //alarm(2);
}
void* g_pdlHandle = NULL;
int InitInterfaces( )
{
	char* pszErr = NULL;
	g_pdlHandle = dlopen( "./libvpr.so", RTLD_LAZY );
	pszErr = dlerror( );
	if ( !g_pdlHandle || pszErr ) {
		printf( "%s\n", pszErr );
		return 0;
	}
	/*void* pHV = dlopen( "/home/linux/libHvDevice.so", RTLD_LAZY );
	//pszErr = dlerror( );
	//printf( "%s\n", pszErr );
	void* phd= dlsym( g_pdlHandle, "HVAPI_OpenEx" ) ;
	if (phd == NULL)
	{
	    pszErr = dlerror( );
	    printf("load fun HVAPI_OpenEx fail:%s\n" ,pszErr);
	}
	else
	{
	    printf("load fun HVAPI_OpenEx sueecss\n");
	}*/

	//VPR_Init = ( BOOL ( * )( UINT ) )dlsym( g_pdlHandle, "VPR_Init" ) ;
	VPR_Init = dlsym( g_pdlHandle, "VPR_Init" ) ;
	if (VPR_Init == NULL)
	{
	    pszErr = dlerror( );
	    printf("load fun VPR_Init fail:%s\n" ,pszErr);
        //return 0;
	}
	VPR_Quit = ( BOOL ( * )(  ) )dlsym( g_pdlHandle, "VPR_Quit" );
	if (VPR_Quit == NULL)
	{
	    pszErr = dlerror( );
	    printf("load fun VPR_Quit fail:%s\n" ,pszErr);
        //return 0;
	}
	VPR_Capture = ( BOOL ( * )( ) )dlsym( g_pdlHandle, "VPR_Capture" );
	if (VPR_Capture == NULL)
	{
	    pszErr = dlerror( );
	    printf("load fun VPR_Capture fail:%s\n" ,pszErr);
        //return 0;
	}
	VPR_ComparePlate = ( BOOL ( * )( int ,int ,char *,char *) )dlsym( g_pdlHandle, "VPR_ComparePlate" );
	if (VPR_ComparePlate == NULL)
	{
	    pszErr = dlerror( );
	    printf("load fun VPR_ComparePlate fail:%s\n" ,pszErr);
        //return 0;
	}
	VPR_GetVehicleInfo = ( BOOL ( * )(char*,int*,BYTE*,int *,BYTE *) )dlsym( g_pdlHandle, "VPR_GetVehicleInfo" );
	if (VPR_GetVehicleInfo == NULL)
	{
	    pszErr = dlerror( );
	    printf("load fun VPR_GetVehicleInfo fail:%s\n" ,pszErr);
        //return 0;
	}
	VPR_ComparePlateBin = ( BOOL ( * )( BYTE *,BYTE * ) )dlsym( g_pdlHandle, "VPR_ComparePlateBin" );
	if (VPR_ComparePlateBin == NULL)
	{
	    pszErr = dlerror( );
	    printf("load fun VPR_ComparePlateBin fail:%s\n" ,pszErr);
        //return 0;
	}
	VPR_CheckStatus = ( int ( * )(char* ) )dlsym( g_pdlHandle, "VPR_CheckStatus" );
    if (VPR_CheckStatus == NULL)
	{
	    pszErr = dlerror( );
	    printf("load fun VPR_CheckStatus fail:%s\n" ,pszErr);
        //return 0;
	}

	if ( !( VPR_Init && VPR_Quit && VPR_Capture && VPR_ComparePlate &&
		VPR_GetVehicleInfo && VPR_ComparePlateBin && VPR_CheckStatus ) )
    {
        printf("load fun fail\n");
		return 0;
	}
	return 1;
}
void  printf_menu()
{
    char szMenu[1024] = {0};
    sprintf(szMenu,"************菜单***************\n");

    strcat(szMenu, "1 连接相机\n");
    strcat(szMenu, "2 车牌号比较\n");
    strcat(szMenu, "3 强制出结果\n");
    strcat(szMenu, "4 获取连接状态\n");
    strcat(szMenu, "5 二值图比较\n");
    strcat(szMenu, "6 退出\n");
    strcat(szMenu, "********************************\n");
    char szTmp[1024] = {0};
    g2u(szMenu, strlen(szMenu), szTmp, 1024);
    printf("%s", szTmp);
}
int main()
{
    printf("TestVpr is running....\n");
    int fLoad = InitInterfaces();
    if (fLoad == 0)
    {
        printf("load dll fail\n");
        return 0;
    }
    while (1)
    {
        printf_menu();
        printf("input a text:\n");
        char ch;
        //ch=getchar();
        scanf("%s", &ch);
        if(ch=='1')
        {
            printf("createsvr 1\n");
            //ch=getchar();
            createsvr();
            printf("VPR_Init 1\n");
            BOOL bResult = VPR_Init(SERV_PORT);
            printf("VPR_Init 2\n");
            if(TRUE == bResult)
            {
                printf("init success\n");
                //signal(SIGALRM,sigalrm_fn);
                //alarm(2);
            }
            else
            {
                printf("init fail\n");
            }
        }
        else if(ch=='2')
        {
            char szP1[32] = {0};
            char szP2[32] = {0};
            printf("input plate1 text:");
            scanf("%s", szP1);
            printf("input plate2 text:");
            scanf("%s", szP2);
            //int nResult = VPR_ComparePlate(1,4,"京A12345","京A16789");
            int nResult = VPR_ComparePlate(0,4,szP1,szP2);
            printf("VPR_ComparePlate = %d\n",nResult);;
        }
        else if(ch=='3')
        {
            //time_t starttm, endtm;
            //time(&starttm);
            //time(&endtm);
           // while( difftime(endtm , starttm ) < 301)
            //{
                VPR_Capture();
               // time(&endtm);
                //sleep(1);
            //}
        }
        else if(ch=='4')
        {
            char chStatus[255] = {0x00};
            printf("befor status \n");
            VPR_CheckStatus(chStatus);
            char szOut[128] = {0};
            g2u(chStatus, strlen(chStatus), szOut,128);
            printf("status is:%s\n",chStatus);
        }
        else if (ch=='g')
        {
            char szPlate[32] = {0};
            char szPlateBin[300] = {0};
            int iBinLen = 0;
            char pImag[1024 * 500] = {0};
            int iBigLen = 0;
            VPR_GetVehicleInfo(szPlate, &iBinLen, szPlateBin, &iBigLen, pImag);
            printf("%s\n", szPlate);
        }
        else if (ch == '5')
        {
            char szb1[280] = {0};
            char szb2[280] = {0};
            int il1 = 0;
            int il2 = 0;
            FILE* pf1 = fopen("./1.bin", "r");
            if (pf1)
            {
                il1 = fread(szb1,1, 280,pf1);
                fclose(pf1);
            }
             FILE* pf2 = fopen("./2.bin", "r");
            if (pf2)
            {
                printf("b2\n");
                il2 = fread(szb2,1, 280,pf2);
                fclose(pf2);
            }
            printf("bin compare:b1=%d,b2=%d\n", il1,il2);
            if (il1 > 0 && il2 > 0)
            {
                BOOL fMatch = VPR_ComparePlateBin(szb1,szb2);
                printf("match=%d\n", fMatch);
            }
        }
        if(ch=='6')
        {
            if(VPR_Quit())
            {
               printf("quit system success\n");
               break;
            }
            else
               printf("quit system failed\n");
        }

    }
    return EXIT_SUCCESS;
}

void ReceiveMsg(void* argsvr)
{
    struct sockaddr_in servaddr, cliaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);
    socklen_t len = sizeof(cliaddr);
    unsigned char *chImage = malloc(MAXLEN);
    unsigned char chTwo[280];
    unsigned char chPlate[16];
    unsigned char chFile[255] = {0x00};
    for(;;)
    {
        /* waiting for receive data */
        unsigned char mesg[4] = {0x00};
        unsigned char mm[4] = {1,2,3,4};
        //printf("接收照片数据 \n");
        int n = recvfrom(*((int *)argsvr), mesg, 4, 0, (struct sockaddr *)&servaddr, &len);
        if(0 == memcmp(mesg,mm,4))
        {
            int piBinLen = 0;
            int piJpegLen = 0;
            memset(chPlate,0,16);
            if(TRUE == VPR_GetVehicleInfo(chPlate,&piBinLen,chTwo,&piJpegLen,chImage))
            {
                char szTmp[32]= {0};
                //sprintf(szTmp,"接收:%s\n", chPlate);
               // char szOut[32] = {0};
                //g2u(szTmp, strlen(szTmp), szOut, 32);
                printf("%s\n",chPlate);
                //char sztt[32] = {0};
                //sprintf(sztt, "%s", "京A1234");
                //g2u(sztt, strlen(sztt), szTmp, 16);
                //printf("%s\n",szTmp);
                memset(chFile,0,255);
                //mkdir("/home/linux/image1", S_IREAD | S_IWRITE);
                if (access("./result/", 0) != 0)
                {
                    system("mkdir ./result/");
                }
                sprintf(chFile,"./result/%d.jpg",nCount);
                FILE *out;
                if((out = fopen(chFile,"wb"))!=NULL)
                {
                    printf("jpeglen:%d\n",piJpegLen);
                    fwrite(chImage,1,piJpegLen,out);
                    fclose(out);
                }
                memset(chFile,0,255);
                sprintf(chFile,"./result/%d.bin",nCount);
                if((out = fopen(chFile,"w"))!=NULL)
                {
                    //printf("open%d\n",nSpecialLen);
                    fwrite(chTwo,1,piBinLen,out);
                    fclose(out);
                }
                memset(chFile,0,255);
                sprintf(chFile,"./result/plate.txt",nCount);
                if((out = fopen(chFile,"a+"))!=NULL)
                {
                    //printf("open%d\n",nSpecialLen);
                    fwrite(chPlate,1,strlen(chPlate),out);
                    fwrite("\r\n",1,strlen("\r\n"),out);
                    fclose(out);
                }
                nCount++;
            }
        }
    }
    free(chImage);
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
    int ret=pthread_create(&udpID,NULL,(void *) ReceiveMsg,(void *)p);
    if(ret!=0){
        printf("getinfo thread error\n");
        return -1;
    }
    return 0;
}
