#include "VPRSDK.h"
#include <dlfcn.h>
#include <unistd.h> 
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ComDef.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <dlfcn.h>
#include <semaphore.h>
#include <stdarg.h>
#include <time.h>
int g_nloadcount = 0;
int g_lServerID = -1; //登陆ID
int g_lAlarmHandle = -1;
char g_strIniFile[1024] = {0};
NET_DVR_PLATE_RESULT *g_pPlateResult = NULL;
pthread_mutex_t g_mutex;
int  socket_udp = 0;
void *g_phandle = NULL;
static sem_t LogcSync;
tNET_DVR_Init tInit;
tNET_DVR_Cleanup tCleanup;
tNET_DVR_Login_V30 tLogin_V30 ;
tNET_DVR_Logout_V30 tLogout_V30 ;
tNET_DVR_ContinuousShoot tContinuousShoot ;
tNET_DVR_GetDVRConfig tGetDVRConfig ;
tNET_DVR_SetDVRMessageCallBack_V30 tSetDVRMessageCallBack_V30 ;
tNET_DVR_SetupAlarmChan_V30 tSetupAlarmChan_V30 ;
tNET_DVR_CloseAlarmChan_V30 tCloseAlarmChan_V30;
tNET_DVR_GetLastError tGetLastError;

/////////////////////加载和释放动态库////////////////////////////////
BOOL LoadHcNetSDK()
{	
	printf("Enter LoadHcNetSDK \r\n");
	
	g_phandle = dlopen("./libvprctrl.so", RTLD_LAZY);
	if (NULL == g_phandle)
	{
		LogCDev("failed loading library! %s\n", dlerror());
		printf("failed loading library! %s\n", dlerror());
		return FALSE;
	}
	tLogin_V30 = (tNET_DVR_Login_V30)dlsym(g_phandle, "NET_DVR_Login_V30");
	tLogout_V30 = (tNET_DVR_Logout_V30)dlsym(g_phandle, "NET_DVR_Logout_V30");
	tContinuousShoot = (tNET_DVR_ContinuousShoot)dlsym(g_phandle, "NET_DVR_ContinuousShoot");
	tGetDVRConfig = (tNET_DVR_GetDVRConfig)dlsym(g_phandle, "NET_DVR_GetDVRConfig");
	tSetDVRMessageCallBack_V30 = (tNET_DVR_SetDVRMessageCallBack_V30)dlsym(g_phandle, "NET_DVR_SetDVRMessageCallBack_V30");
	tSetupAlarmChan_V30 = (tNET_DVR_SetupAlarmChan_V30)dlsym(g_phandle, "NET_DVR_SetupAlarmChan_V30");
	tCloseAlarmChan_V30 = (tNET_DVR_CloseAlarmChan_V30)dlsym(g_phandle, "NET_DVR_CloseAlarmChan_V30");
	tGetLastError = (tNET_DVR_GetLastError)dlsym(g_phandle, "NET_DVR_GetLastError");
	tInit = (tNET_DVR_Init)dlsym(g_phandle, "NET_DVR_Init");
	tCleanup = (tNET_DVR_Cleanup)dlsym(g_phandle, "NET_DVR_Cleanup");
	LogCDev("lib isok");
	return TRUE;
}

BOOL HcNetSDKFree()
{
	if(g_phandle != NULL)
	{
		dlclose(g_phandle);
		g_phandle = NULL;
		tCleanup();
		g_nloadcount--;
	}
	return TRUE;
}
//////////////////////////////////////////////////////////

BOOL IsIniFileExist(char* strIniFileName)
{
	printf("Enter IsIniFileExist \r\n");
	int i = 0;
	int iLastSperate = 0;
	char achCurPath[272];  

	getcwd(achCurPath,sizeof(achCurPath)-1); 
	printf("achCurPath = %s \r\n",achCurPath);

    strcpy(g_strIniFile, achCurPath);
	strcat(g_strIniFile, "/");
	strcat(g_strIniFile, strIniFileName);
	printf("g_strIniFile = %s \r\n",g_strIniFile);
	return TRUE;
}

////////////////////////回调函数///////////////////////////
void CALLBACK MessageCallback(LONG lCommand, NET_DVR_ALARMER *pAlarmer, char *pAlarmInfo, DWORD dwBufLen, void* pUser)
{
	printf("Enter MessageCallback\r\n");
	LogCDev("进入回调");
	NET_DVR_PLATE_RESULT struPlateResult;
	memset(&struPlateResult, 0, sizeof(NET_DVR_PLATE_RESULT));
	if (COMM_UPLOAD_PLATE_RESULT == lCommand)
	{
		char chData[4] = {1,2,3,4};
		memcpy(&struPlateResult, pAlarmInfo, sizeof(struPlateResult));
		pthread_mutex_lock (&g_mutex);
		memcpy( g_pPlateResult, (const void*)&struPlateResult, sizeof(struPlateResult));
	//	LogCDev("COMM_UPLOAD_PLATE_RESULT_pthread_mutex_lock_通知消息");
		write(socket_udp, chData, 4);
	//	LogCDev("通知完毕");
		pthread_mutex_unlock (&g_mutex);
	}	
}

/////////////////////////////////////回调函数结束//////////////////////////////////


extern int VPR_Init(UINT uPort,int nHWYPort,char *chDevIp)
{
	
	sem_post((sem_t *)&LogcSync);
	printf("Enter VPR_Init \r\n");
	//此处加载动态库
	if (g_nloadcount <= 0)
	{
		if (!LoadHcNetSDK())
		{
			printf("LoadHcNetSDK error \r\n");
			return FALSE;		
		}
		g_pPlateResult = new NET_DVR_PLATE_RESULT;
		if (g_pPlateResult == NULL)
		{
			printf("New NET_DVR_PLATE_RESULT error \r\n");
			return FALSE;
		}	
		//初始化SDK
		tInit();
		pthread_mutex_init (&g_mutex,NULL);
		//设置回调
		if(!tSetDVRMessageCallBack_V30(MessageCallback, NULL))
		{
		int iError = tGetLastError();
		printf("Set CallBack Error ID = %d!\r\n",iError);
		return FALSE;
		}
		g_nloadcount ++ ;
	}

	 
	

	printf("Set CallBack Success \r\n");
 
	NET_DVR_DEVICEINFO_V30 m_DeviceInfo;
	
	//连接相机
	if(g_lServerID < 0)
	{
		g_lServerID = tLogin_V30(chDevIp, //此处IP通过读配置文件得到
		8000, 
		(char*)"admin", 
		(char*)"12345", 
		&m_DeviceInfo);
		if (g_lServerID < 0)
		{
			int iError = tGetLastError();
			printf("Login Error ID = %d!\r\n",iError);
			return FALSE;
		}
	}
	printf("Login Success \r\n");
	//布防
	if(g_lAlarmHandle < 0)
	{
		g_lAlarmHandle = tSetupAlarmChan_V30(g_lServerID);
		if (g_lAlarmHandle < 0)
		{
			printf("SetupAlarm Error !\r\n");
			return FALSE;
		}
	}
	printf("SetAlarm Success \r\n");
	if ((socket_udp = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		return -1;
	}
	struct sockaddr_in st_srv_addr;
	bzero(&st_srv_addr, sizeof(st_srv_addr));
	st_srv_addr.sin_family = AF_INET;
	st_srv_addr.sin_port = htons (uPort);
	if(connect(socket_udp, (struct sockaddr *)&st_srv_addr, sizeof(st_srv_addr)) == -1)
	{
		return -1;
	}
	
	
	return TRUE;

}



int VPR_ComparePlate(int iFullCompare,int iRule,char *chPlateIn,char *chPlateOut)
{
	printf("Enter VPR_ComparePlate\r\n");
    if(chPlateIn == NULL || chPlateOut == NULL )
    {
        return -1;
    }
    if(0 != iFullCompare)
    {
        if(0 == strcmp(chPlateIn,chPlateOut))    
            return 0;
        else
            return 3;    
    }
    else
    {
        int nCount = 0;
        int i = 0;
        for(i = 0;i<strlen(chPlateIn);i++)
        {
            if(((unsigned char)chPlateIn[i])>127)
            {
                if(strlen(chPlateOut) > i+2)
                {
                    if(0 == memcmp(chPlateIn+i,chPlateOut+i,2))
                        nCount++;
                }
                i++;
            }
            else
            {
                if(chPlateOut[i] == chPlateIn[i])
                {
                    nCount++;
                }
            }
        }
        int fl = (float)nCount*100/(float)iRule;
        if(fl >= 100) 
            return 0;
        else if(fl<100 && fl>=85)
            return 1;
        else if(fl<85 && fl>=70)
            return 2;
        else
            return 3;
    }
}

int VPR_Quit()
{
	printf("Enter VPR_Quit\r\n");
	if (g_lAlarmHandle > -1)
	{
		tCloseAlarmChan_V30(g_lAlarmHandle);
	    g_lAlarmHandle = -1;
	}
	if (g_lServerID > -1)
	{
		tLogout_V30(g_lServerID);
		g_lServerID = -1;
	}
	//释放SDK
	HcNetSDKFree();
	if (g_pPlateResult != NULL)
	{
		delete g_pPlateResult;
		g_pPlateResult = NULL;
	}
	return TRUE;
}

int VPR_Capture()
{
	printf("Enter VPR_Capture\r\n");
	if (g_lServerID < 0)
	{
		printf("VPR_Capture Error ! Not Login \r\n");
		return FALSE;
	}
	NET_DVR_SNAPCFG strSnapcfg;
	memset(&strSnapcfg, 0, sizeof(NET_DVR_SNAPCFG));
	strSnapcfg.dwSize = sizeof(NET_DVR_SNAPCFG);
	strSnapcfg.byRelatedDriveWay = 1;
	strSnapcfg.bySnapTimes = 1;
	if (!tContinuousShoot(g_lServerID, &strSnapcfg))
	{
		printf("VPR_Capture Faile! \r\n");
		return FALSE;
	}
	printf("VPR_Capture Success\r\n");
    return TRUE;
}
BOOL VPR_GetVehicleInfo(char *pchPlate,int *piByteBinImagLen,BYTE *pByteBinImage,int *piJpegImageLen,BYTE *pByteJpegImage)
{
	printf("Enter VPR_GetVehicleInfo\r\n");
    if(pchPlate == NULL || piByteBinImagLen == NULL || piJpegImageLen == NULL || pByteJpegImage == NULL)
    {
        return FALSE;
   	}
    pthread_mutex_lock (&g_mutex);
    strcpy(pchPlate,g_pPlateResult->struPlateInfo.sLicense);
	printf("pchPlate = %s \r\n",pchPlate);
	
    int nLen = 280;
    *piByteBinImagLen = nLen;
	if (0 != g_pPlateResult->dwBinPicLen && NULL != g_pPlateResult->pBuffer3)
	{
		memcpy(pByteBinImage,g_pPlateResult->pBuffer3,280);
	}	
    nLen = g_pPlateResult->dwPicLen;
    *piJpegImageLen = nLen;
    if(NULL != g_pPlateResult->pBuffer1 && 0 != g_pPlateResult->dwPicLen)
    {
        memcpy(pByteJpegImage,g_pPlateResult->pBuffer1,g_pPlateResult->dwPicLen);
    }
	pthread_mutex_unlock (&g_mutex);
	printf("VPR_GetVehicleInfo Success\r\n");
    return TRUE;
}

int VPR_ComparePlateBin(BYTE *lpBinImageIn,BYTE *lpBinImageOut)
{
	printf("Enter VPR_ComparePlateBin\r\n");
    if( lpBinImageIn == NULL || lpBinImageOut == NULL )
    {
        return FALSE;
   	}
    int i = 0;
    for(i = 0;i < 280;i++)
    {
        if(lpBinImageIn[i] != lpBinImageOut[i])
        {
            return FALSE;
        }
    }
    return TRUE;
}

int VPR_CheckStatus(char *chVprDevStatus)
{
	printf("Enter VPR_CheckStatus\r\n");
	if (g_lServerID < 0)
	{
		printf("VPR_CheckStatus Error ! Not Login \r\n");
		return FALSE;
	}
	DWORD dwReturn = 0;
	NET_DVR_TIME struDevTime;
	memset(&struDevTime, 0, sizeof(NET_DVR_TIME));
	if (!tGetDVRConfig(g_lServerID, NET_DVR_GET_TIMECFG, 0,
		&struDevTime, sizeof(struDevTime), &dwReturn))
	{
		printf("VPR_CheckStatus Faile\r\n");
		strcpy(chVprDevStatus,"设备不在线");
		return FALSE;
	}
	strcpy(chVprDevStatus,"设备正常");
	return TRUE;
}

void LogC(char *ModName,char *LogType,char *File,int LineNumber,char *Format,...)
{	
	char buffer[20000];
	char LogFileName[200];
	FILE *fp;
	va_list args;
	sem_wait((sem_t *)&LogcSync);	
	memset(buffer,0x00,20000);	
	va_start(args,Format);	
	vsprintf(buffer,Format,args);	
	va_end(args);	
	memset(LogFileName,0x00 ,200);	
	sprintf(LogFileName,"hik.log");	
	if((fp=fopen(LogFileName,"at+"))==NULL)
	{	
		fp=fopen(LogFileName,"wt+");	
	}
	fputs(GetTime(DATETIME_FORMAT_YYYY_MM_DD_HH_MM_SS),fp);	
	fputs(" ",fp);	
	fputs(buffer,fp);	
	fputs("\n",fp);	
	fclose(fp);	
	sem_post((sem_t *)&LogcSync);	
}

char DataTimeStr[25];
char *GetFormatTime(char *Format,time_t FormatTime)
{	
    struct tm *rtc_time;
	memset(DataTimeStr,0x00,sizeof(DataTimeStr));
    rtc_time = localtime(&FormatTime);	
    if(!strncmp(Format,DATETIME_FORMAT_MMDD,strlen(DATETIME_FORMAT_MMDD)))
    {	
        sprintf(DataTimeStr, "%02d%02d%c",rtc_time->tm_mon+1, rtc_time->tm_mday,'\0');	
    }
	
	else if(!strncmp(Format,DATETIME_FORMAT_YYYYMMDDWHHMMSS,strlen(DATETIME_FORMAT_YYYYMMDDWHHMMSS)))
	{	
		sprintf(DataTimeStr, "%04d%02d%02d%d%02d%02d%02d%c",rtc_time->tm_year+1900, rtc_time->tm_mon+1, rtc_time->tm_mday,rtc_time->tm_wday+1, rtc_time->tm_hour, rtc_time->tm_min, rtc_time->tm_sec,'\0');	
	}
    else if(!strncmp(Format,DATETIME_FORMAT_YYYYMMDDHHMMSS,strlen(DATETIME_FORMAT_YYYYMMDDHHMMSS)))
    {	
        sprintf(DataTimeStr, "%04d%02d%02d%02d%02d%02d%c",rtc_time->tm_year+1900, rtc_time->tm_mon+1, rtc_time->tm_mday,rtc_time->tm_hour, rtc_time->tm_min, rtc_time->tm_sec,'\0');	
    }
    else if(!strncmp(Format,DATETIME_FORMAT_YYYYMMDDHHOOOO,strlen(DATETIME_FORMAT_YYYYMMDDHHOOOO)))
    {	
        sprintf(DataTimeStr, "%04d%02d%02d%02d0000%c",rtc_time->tm_year+1900, rtc_time->tm_mon+1, rtc_time->tm_mday,rtc_time->tm_hour,'\0');	
    }
	else if(!strncmp(Format,DATETIME_FORMAT_YYYYMMDDHHMM,strlen(DATETIME_FORMAT_YYYYMMDDHHMM)))
	{	
        sprintf(DataTimeStr, "%04d%02d%02d%02d%02d%c",rtc_time->tm_year+1900, rtc_time->tm_mon+1, rtc_time->tm_mday,rtc_time->tm_hour,rtc_time->tm_min,'\0');	
	}
    else if(!strncmp(Format,DATETIME_FORMAT_YYYYMMDDHH,strlen(DATETIME_FORMAT_YYYYMMDDHH)))
    {	
        sprintf(DataTimeStr, "%04d%02d%02d%02d%c",rtc_time->tm_year+1900, rtc_time->tm_mon+1, rtc_time->tm_mday,rtc_time->tm_hour,'\0');	
    }
	else if(!strncmp(Format,DATETIME_FORMAT_YYYYMMDDHHM,strlen(DATETIME_FORMAT_YYYYMMDDHHM)))
	{	

		sprintf(DataTimeStr, "%04d%02d%02d%02d%02d%c",rtc_time->tm_year+1900, rtc_time->tm_mon+1, rtc_time->tm_mday,rtc_time->tm_hour,rtc_time->tm_min /10,'\0');	
	}
    else if(!strncmp(Format,DATETIME_FORMAT_YYYY_MM_DD_HH_MM_SS,strlen(DATETIME_FORMAT_YYYY_MM_DD_HH_MM_SS)))
    {	
        sprintf(DataTimeStr, "%04d-%02d-%02d %02d:%02d:%02d%c",rtc_time->tm_year+1900, rtc_time->tm_mon+1, rtc_time->tm_mday,rtc_time->tm_hour, rtc_time->tm_min, rtc_time->tm_sec,'\0');	
    }
	 else if(!strncmp(Format,DATETIME_FORMAT_HH_MM_SS_5,strlen(DATETIME_FORMAT_HH_MM_SS_5)))
    {	
        sprintf(DataTimeStr, "%02d:%02d:%02d%c", rtc_time->tm_hour, rtc_time->tm_min, rtc_time->tm_sec,'\0');	
    }
	 else if(!strncmp(Format,DATETIME_FORMAT_YYYY_MM_DD_5,strlen(DATETIME_FORMAT_YYYY_MM_DD_5)))
    {	
        sprintf(DataTimeStr, "%04d-%02d-%02d%c",rtc_time->tm_year+1900, rtc_time->tm_mon+1, rtc_time->tm_mday,'\0');	
    }
    else if(!strncmp(Format,DATETIME_FORMAT_YYYYMMDD,strlen(DATETIME_FORMAT_YYYYMMDD)))
    {	
        sprintf(DataTimeStr, "%04d%02d%02d%c",rtc_time->tm_year+1900, rtc_time->tm_mon+1, rtc_time->tm_mday,'\0');	
    }
    else if(!strncmp(Format,DATETIME_FORMAT_HHMMSS,strlen(DATETIME_FORMAT_HHMMSS)))
    {	
        sprintf(DataTimeStr, "%02d%02d%02d%c",rtc_time->tm_hour, rtc_time->tm_min, rtc_time->tm_sec,'\0');	
    }
    else if(!strncmp(Format,DATETIME_FORMAT_HHOOOO,strlen(DATETIME_FORMAT_HHOOOO)))
	{	
        sprintf(DataTimeStr, "%02d0000%c",rtc_time->tm_hour,'\0');	
	}
    else if(!strncmp(Format,DATETIME_FORMAT_YYYY,strlen(DATETIME_FORMAT_YYYY)))
    {	
        sprintf(DataTimeStr, "%04d%c",rtc_time->tm_year+1900,'\0');	
    }
	else if(!strncmp(Format,DATETIME_FORMAT_YYYY_MM_DD,strlen(DATETIME_FORMAT_YYYY_MM_DD)))
	{	
		sprintf(DataTimeStr, "%04d-%02d-%02d%c",rtc_time->tm_year+1900, rtc_time->tm_mon+1, rtc_time->tm_mday,'\0');	
	}
	return DataTimeStr;
}
/****************************************************************************************
* 获取 格式化 字符串 日期
*****************************************************************************************/
char *GetTime(char *Format)
{	
	time_t Currenttime = time(NULL);	
	return GetFormatTime(Format,Currenttime);	
}