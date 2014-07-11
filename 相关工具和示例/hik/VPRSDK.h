#ifndef VPRSDK_H_
#define VPRSDK_H_
#include "HCNetSDK.h"
#include <time.h>

#define TCOTYPE_TCOCMD   "00" //√¸¡Ó
#define TCOTYPE_TCORET    "01" //

struct MSG_TCOCMD
{
	char TCOType[2];
	char NetNo[2];
	char PlazaNo[2];
	char LaneType;
	char LaneNo[3];
	char CMD[10];
	char Param[100];
};
extern "C"
{

extern  int VPR_Init(UINT uPort,int nHWYPort,char *chDevIp);
extern  BOOL VPR_Quit();
extern  BOOL VPR_Capture();
extern  BOOL VPR_GetVehicleInfo(char *pchPlate,int *piByteBinImagLen, BYTE *pByteBinImage,int *piJpegImageLen,BYTE *pByteJpegImage);
extern  int  VPR_ComparePlate(int iFullCompare,int iRule,char *chPlateIn,char *chPlateOut);
extern  BOOL VPR_ComparePlateBin(BYTE *lpBinImageIn, BYTE *lpBinImageOut);
extern  BOOL VPR_CheckStatus(char *chVprDevStatus);
}
void LogC(char *ModName,char *LogType,char *File,int LineNumber,char *Format,...);
#define LogCDev(_Format_,args...)		 	LogC("hik",0 ,"", 0 ,_Format_, ## args)


#define DATETIME_FORMAT_YYYYMMDDHHMMSS          "1yyyyMMddhhmmss1"
#define DATETIME_FORMAT_YYYYMMDDWHHMMSS          "1yyyyMMddwhhmmss1"
#define DATETIME_FORMAT_YYYYMMDDHHOOOO          "1yyyyMMddhhoooo1"
#define DATETIME_FORMAT_YYYYMMDDHHMM            "2yyyyMMddhhmm2"
#define DATETIME_FORMAT_YYYYMMDDHH              "3yyyyMMddhh3"
#define DATETIME_FORMAT_YYYYMMDDHHM				"4yyyyMMddhh4"
#define DATETIME_FORMAT_YYYY_MM_DD_HH_MM_SS     "5yyyy-MM-dd hh:mm:ss5"
#define DATETIME_FORMAT_HH_MM_SS_5			    "5hh:mm:ss5"
#define DATETIME_FORMAT_YYYY_MM_DD_5			"5yyyy-MM-dd5"
#define DATETIME_FORMAT_YYYYMMDD                "6yyyyMMDD6"
#define DATETIME_FORMAT_HHMMSS                  "7HHMMSS7"
#define DATETIME_FORMAT_HHOOOO                  "7HHOOOO7"
#define DATETIME_FORMAT_YYYY                    "8YYYY8"
#define DATETIME_FORMAT_MMDD                    "9MMDD9"
#define DATETIME_FORMAT_YYYY_MM_DD				"10yyyy-MM-dd01"
char *GetTime(char *Format);
char *GetFormatTime(char *Format,time_t FormatTime);
#endif /*VPRSDK_H_*/

