#ifndef COMDEF_H_
#define COMDEF_H_
#include "HCNetSDK.h"
#ifndef BOOL
#define BOOL int
#endif



typedef LONG (*tNET_DVR_Login_V30)(char* ,WORD , char *, char *, LPNET_DVR_DEVICEINFO_V30 );
typedef BOOL (*tNET_DVR_Logout_V30)(LONG);
typedef BOOL (*tNET_DVR_ContinuousShoot)(LONG , LPNET_DVR_SNAPCFG );
typedef BOOL (*tNET_DVR_GetDVRConfig)(LONG , DWORD ,LONG , LPVOID , DWORD , LPDWORD );
typedef BOOL (*tNET_DVR_SetDVRMessageCallBack_V30)(MSGCallBack , void* );
typedef LONG (*tNET_DVR_SetupAlarmChan_V30)(LONG );
typedef BOOL (*tNET_DVR_CloseAlarmChan_V30)(LONG );
typedef LONG (*tNET_DVR_GetLastError)();
typedef BOOL (*tNET_DVR_Init)();
typedef BOOL (*tNET_DVR_Cleanup)();

#endif
