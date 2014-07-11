#ifndef VPR_H_
#define VPR_H_
/*typedef enum{TRUE=1,FALSE=!TRUE} BOOL;
typedef unsigned int UINT;
typedef char BYTE;*/

#ifdef _cplusplus
extern "c"
{
#endif

BOOL VPR_Init(UINT uPort);
BOOL VPR_Quit();
BOOL VPR_Capture();
BOOL VPR_GetVehicleInfo(char *pchPlate,int *piByteBinImagLen,BYTE *pByteBinImage,int *piJpegImageLen,BYTE *pByteJpegImage);
int VPR_ComparePlate(int iFullCompare,int iRule,char *chPlateIn,char *chPlateOut);
BOOL VPR_ComparePlateBin(BYTE *lpBinImageIn,BYTE *lpBinImageOut);
BOOL VPR_CheckStatus(char *chVprDevStatus);

#ifdef _cplusplus
}
#endif
#endif
