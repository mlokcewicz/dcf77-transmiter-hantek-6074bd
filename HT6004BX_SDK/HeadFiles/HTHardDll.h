#ifndef _HT_HARD_DLL_H
#define _HT_HARD_DLL_H
//#ifndef DLL_API
//#define DLL_API extern "C" 11// __declspec(dllimport)
//#endif

#define WIN_API __stdcall

#include "windows.h"  
#define HEAD_DATA 0x0174
typedef struct _HT_RELAY_CONTROL
{
        BOOL bCHEnable[MAX_CH_NUM];
        WORD nCHVoltDIV[MAX_CH_NUM];
        WORD nCHCoupling[MAX_CH_NUM];
        BOOL bCHBWLimit[MAX_CH_NUM];
        WORD nTrigSource;
        BOOL bTrigFilt;
        WORD nALT;
}RELAYCONTROL,*PRELAYCONTROL;

DLL_API WORD WINAPI dsoHTSearchDevice(short* pDevInfo);   //��ȡ1-32���˿��Ƿ����USB�豸

DLL_API WORD WINAPI dsoHTDeviceConnect(WORD nDeviceIndex);    //�����豸�Ƿ����

DLL_API WORD WINAPI dsoHTSetCHPos(WORD nDeviceIndex,WORD nVoltDIV,WORD nPos,WORD nCH,WORD nCHMode);  //����CH�źŵĴ�ֱλ��  OUT��16-00-00-00

DLL_API WORD WINAPI dsoHTSetCHDirectLeverPos(WORD nDeviceIndex,WORD nPos,WORD nCH);  //ֱ���õ�ѹֵ���趨�źŵĴ�ֱλ��


DLL_API WORD WINAPI dsoHTSetVTriggerLevel(WORD nDeviceIndex,WORD nPos,WORD nSensitivity);  //���ô�����ֱ��ƽλ��  ʵ�ʵ�ѹ��Χ0-1012 �������0-255����ʾ��ѹ  OUT:16-00-00-01
//���ô������Ⱥ�Ԥ��������//(����Triggerˮƽλ��)-->����nBufferLen���ڴ泤��10k�� ������512����������,�Ҳ���>16M    HTriggerPos��Ԥ�����ٷֱ�
DLL_API WORD WINAPI dsoHTSetHTriggerLength(WORD nDeviceIndex,PCONTROLDATA pControl,WORD nCHMod);  //������Ԥ��������  OUT:0f-00  
//û�����ù���ʹ�����ڴ泤��Ҳû��
DLL_API WORD WINAPI dsoHTSetBufferSize(WORD nDeviceIndex,WORD nBufferSize);   //0d-0f
//���ã�24bit��CH���ء�����ѡ�񡢵�ѹ��λ��   �̵�����8bit����˥����Ϸ�ʽ
DLL_API WORD WINAPI dsoHTSetCHAndTrigger(WORD nDeviceIndex,PRELAYCONTROL pRelayControl,WORD nTimeDIV);  //OUT:12-00-00-00  //ԭ���Ĵ���
 
//����DSO3074 ���������
DLL_API WORD WINAPI dsoHTSetCHAndTriggerDirect(WORD nDeviceIndex,PRELAYCONTROL pRelayControl,USHORT nDirect,WORD nDriverCode);
//���ֻ������dsoHTSetCHAndTriggerһ������
DLL_API WORD WINAPI dsoHTSetCHAndTriggerVB(WORD nDeviceIndex,WORD* pCHEnable,WORD* pCHVoltDIV,WORD* pCHCoupling,WORD* pCHBWLimit,WORD nTriggerSource,WORD nTriggerFilt,WORD nALT,WORD nTimeDIV);


DLL_API WORD WINAPI dsoHTSetTriggerAndSyncOutput(WORD nDeviceIndex,PCONTROLDATA pControl,WORD nTriggerMode,WORD nTriggerSlope,WORD nPWCondition,
												 ULONG nPW,USHORT nVideoStandard,USHORT nVedioSyncSelect,USHORT nVideoHsyncNumOption,WORD nSync);
 //���ô�����ʽ
//���ò�����
DLL_API WORD WINAPI dsoHTSetSampleRate(WORD nDeviceIndex,WORD nYTFormat,PRELAYCONTROL pRelayControl,PCONTROLDATA pControl);  //OUT:0e-00

DLL_API WORD WINAPI dsoHTSetSampleRateVi(WORD nDeviceIndex,WORD* pCHEnable,WORD* pCHVoltDIV,WORD* pCHCoupling,WORD* pCHBWLimit,WORD nTriggerSource,WORD nTriggerFilt,
										 WORD nALT,PCONTROLDATA pControl);

DLL_API WORD WINAPI dsoHTInitSDRam(WORD nDeviceIndex);  ////OUT:18-00

DLL_API WORD WINAPI dsoHTStartCollectData(WORD nDeviceIndex,WORD nStartControl);

DLL_API WORD WINAPI dsoHTStartTrigger(WORD nDeviceIndex);

DLL_API WORD WINAPI dsoHTForceTrigger(WORD nDeviceIndex);

DLL_API WORD WINAPI dsoHTGetState(WORD nDeviceIndex);

DLL_API WORD WINAPI dsoHTGetPackState(WORD nDeviceIndex);

DLL_API WORD WINAPI dsoHTGetSDRamInit(WORD nDeviceIndex);

DLL_API WORD WINAPI dsoHTGetData(WORD nDeviceIndex,WORD* pCH1Data,WORD* pCH2Data,WORD* pCH3Data,WORD* pCH4Data,PCONTROLDATA pControl/*,ULONG* p1,ULONG* p2,ULONG* p3*/);

DLL_API WORD WINAPI dsoHTGetScanData(WORD nDeviceIndex,WORD* pCH1Data,WORD* pCH2Data,WORD* pCH3Data,WORD* pCH4Data,PCONTROLDATA pControl);

DLL_API WORD WINAPI dsoHTReadCalibrationData(WORD nDeviceIndex,WORD* pLevel,WORD nLen);

DLL_API WORD WINAPI dsoHTWriteCalibrationData(WORD nDeviceIndex,WORD* pLevel,WORD nLen);

DLL_API WORD WINAPI dsoSDGetData(WORD nDeviceIndex,WORD* pCH1Data,WORD* pCH2Data,WORD* pCH3Data,WORD* pCH4Data,PCONTROLDATA pControl,WORD nInsertMode);

DLL_API WORD WINAPI dsoSDHTGetRollData(WORD nDeviceIndex,WORD* pCH1Data,WORD* pCH2Data,WORD* pCH3Data,WORD* pCH4Data,PCONTROLDATA pControl);

DLL_API WORD WINAPI dsoSDHTGetScanData(WORD nDeviceIndex,WORD* pCH1Data,WORD* pCH2Data,WORD* pCH3Data,WORD* pCH4Data,PCONTROLDATA pControl);

DLL_API WORD WINAPI dsoHTGetRollData(WORD nDeviceIndex,WORD* pCH1Data,WORD* pCH2Data,WORD* pCH3Data,WORD* pCH4Data,PCONTROLDATA pControl);

DLL_API WORD WINAPI dsoHTOpenRollMode(WORD nDeviceIndex);

DLL_API WORD WINAPI dsoHTCloseRollMode(WORD nDeviceIndex);

DLL_API WORD WINAPI dsoHTSetPeakDetect(WORD nDeviceIndex,WORD nTimeDIV,WORD nYTFormat);

DLL_API WORD WINAPI dsoHTClosePeakDetect(WORD nDeviceIndex);

DLL_API WORD WINAPI dsoHTGetHardFC(WORD nDeviceIndex,PULONG pFreq,PULONG pCount);

DLL_API WORD WINAPI dsoHTSetHardFC(WORD nDeviceIndex,ULONG nTime,WORD nCountSet);

DLL_API WORD WINAPI dsoHTResetCnter(WORD nDeviceIndex,ULONG nTime);

DLL_API WORD WINAPI dsoHTStartRoll(WORD nDeviceIndex);

DLL_API ULONG WINAPI dsoGetHardVersion(WORD DeviceIndex);

DLL_API WORD WINAPI dsoUSBModeSetIPAddr(WORD DeviceIndex,ULONG nIP,ULONG nSubnetMask,ULONG nGateway,USHORT nPort,BYTE* pMac);

DLL_API WORD WINAPI dsoUSBModeGetIPAddr(WORD DeviceIndex,ULONG* pIP,ULONG* pSubMask,ULONG* pGateway,WORD* pPort,BYTE* pMac);

DLL_API WORD WINAPI dsoOpenLan(WORD DeviceIndex,WORD nMode);

DLL_API WORD WINAPI dsoOpenWIFIPower(WORD DeviceIndex);

DLL_API WORD WINAPI dsoResetWIFI(WORD DeviceIndex);

DLL_API WORD WINAPI dsoGetFPGAVersion(WORD DeviceIndex);

DLL_API WORD WINAPI dsoGetUSBModulVersion(WORD DeviceIndex);

DLL_API BOOL WINAPI dsoSetUSBBus(WORD DeviceIndex);

DLL_API BOOL WINAPI dsoSetSPIBus(WORD DeviceIndex);

DLL_API BOOL WINAPI dsoSetHardInfo(WORD DeviceIndex,UCHAR* pBuffer);

DLL_API BOOL WINAPI dsoGetHardInfo(WORD DeviceIndex,UCHAR* pBuffer);

DLL_API BOOL WINAPI dsoWriteFlash(WORD DeviceIndex,UCHAR* pBuffer);

DLL_API BOOL WINAPI dsoReadFlash(WORD DeviceIndex,UCHAR* pBuffer);

DLL_API BOOL WINAPI dsoGetDeviceName(WORD DeviceIndex,UCHAR* pBuffer);
DLL_API BOOL WINAPI dsoGetDeviceSN(WORD DeviceIndex,UCHAR* pBuffer);
DLL_API BOOL WINAPI dsoGetPCBVersion(WORD DeviceIndex,UCHAR* pBuffer);

DLL_API WORD WINAPI dsoGetDriverVersion(WORD DeviceIndex,UCHAR* pBuffer);

DLL_API WORD WINAPI dsoGetLANEnable(WORD DeviceIndex,short* pEnable);

DLL_API WORD WINAPI dsoSetLANEnable(WORD DeviceIndex,short nEnable);

DLL_API WORD WINAPI dsoWriteIIC(WORD DeviceIndex,PUCHAR pBuffer,ULONG nLenUpdateFileLen);

//2015.02.04 ���� �������ݶ�ȡʱ��ǿ���ж�
//DLL_API WORD WINAPI dsoSDGetData_Suspend(WORD nDeviceIndex,WORD* pCH1Data,WORD* pCH2Data,WORD* pCH3Data,WORD* pCH4Data,PCONTROLDATA pControl,WORD nInsertMode,USHORT* nSuspend);


//-----------DDS--------------
DLL_API ULONG WINAPI ddsSetOnOff(WORD DeviceIndex,short nOnOff);
DLL_API ULONG WINAPI ddsSetFrequency(WORD DeviceIndex,double dbFre,WORD* pWaveNum,WORD* pPeriodNum);
DLL_API ULONG WINAPI ddsDownload(WORD DeviceIndex,WORD iWaveNum, WORD* pData);
DLL_API ULONG WINAPI ddsSetSyncOut(WORD DeviceIndex, BOOL bEnable);
DLL_API ULONG WINAPI ddsSetCmd(WORD DeviceIndex, USHORT nControl);
DLL_API ULONG WINAPI ddsEmitSingle(WORD DeviceIndex);  //zhang 
DLL_API WORD WINAPI ddsSetFAOC(WORD DeviceIndex, double dFre,WORD nAmpVolt,SHORT nOffsetVolt,ULONG nPeriodNum,float fAMDepth,double dbFMMAXOffset);
DLL_API ULONG WINAPI ddsSetSweep(WORD DeviceIndex, WORD nControl);
DLL_API ULONG WINAPI ddsSetAMFMFreq(WORD DeviceIndex,double dbFre);
DLL_API WORD  WINAPI dsoReadIIC(WORD DeviceIndex,UCHAR* pBuffer,ULONG nLen);
//DLL_API ULONG WINAPI ddsSetPowerOn(WORD DeviceIndex,BOOL bEnable);
DLL_API float WINAPI ddsSDKSetFre(WORD DeviceIndex, float dFre);
DLL_API WORD WINAPI ddsSDKSetAmp(WORD DeviceIndex, WORD nAmp);
DLL_API short WINAPI ddsSDKSetOffset(WORD DeviceIndex, short nOffset);
DLL_API WORD WINAPI ddsSDKSetBurstNum(WORD DeviceIndex, WORD nBurstNum);
DLL_API WORD WINAPI ddsSDKSetWaveType(WORD DeviceIndex, WORD nType);
DLL_API float WINAPI ddsSDKSetWavePhase(WORD DeviceIndex, float fPhase);
DLL_API float WINAPI ddsSDKSetWaveDuty(WORD DeviceIndex, float fDuty);

DLL_API WORD WINAPI dsoHTWRAmpCali(WORD nDeviceIndex,WORD* pLevel,WORD nLen);//д�����У׼��Ϣ
DLL_API WORD WINAPI dsoHTRDAmpCali(WORD nDeviceIndex,WORD* pLevel,WORD nLen);//��ȡ����У׼��Ϣ
DLL_API WORD WINAPI dsoHTWRADCCali(WORD nDeviceIndex,WORD* pLevel,WORD nLen);
DLL_API WORD WINAPI dsoHTRDADCCali(WORD nDeviceIndex,WORD* pLevel,WORD nLen);
DLL_API WORD WINAPI dsoInitHard(WORD DeviceIndex);
DLL_API WORD WINAPI dsoHTADCCHModGain(WORD DeviceIndex,WORD nCHMod);
DLL_API WORD WINAPI dsoHTSetAmpCalibrate(WORD nDeviceIndex,WORD nCHSet,WORD nTimeDIV,WORD *nVoltDiv,WORD *pCHPos);
DLL_API WORD WINAPI dsoHTSetRamAndTrigerControl(WORD DeviceIndex,WORD nTimeDiv,WORD nCHset,WORD nTrigerSource,WORD nPeak);//���ô���Դ
DLL_API WORD WINAPI dsoHTSetTrigerMode(WORD m_nDeviceIndex,WORD nTriggerMode,WORD nTriggerSlop,WORD nTriggerCouple);
DLL_API WORD WINAPI dsoHTSetVideoTriger(WORD m_nDeviceIndex,USHORT nStand,USHORT nVedioSyncSelect,USHORT nVideoHsyncNumOption,USHORT nVideoPositive,WORD nLevel,WORD nLogicTriggerSource);
DLL_API WORD WINAPI dsoHTSetPulseTriger(WORD m_nDeviceIndex,ULONG nPW,WORD nPWCondition);

DLL_API BOOL WINAPI dsoSetDDSCali(WORD DeviceIndex,WORD* level,WORD length);
DLL_API BOOL WINAPI dsoGetDDSCali(WORD DeviceIndex,WORD* level,WORD length);
//


DLL_API WORD WINAPI dsoGetInBufferWithoutOpen(HANDLE hInDevice,UINT m_nSize,PUCHAR pBuffer);
DLL_API WORD WINAPI dsoGetInBuffer(WORD DeviceIndex,UINT m_nSize,PUCHAR pBuffer);
DLL_API WORD WINAPI dsoSendOutBuffer(WORD DeviceIndex,UINT m_nSize,PUCHAR pBuffer);
DLL_API WORD WINAPI dsoGetChmod(WORD DeviceIndex);
DLL_API FLOAT WINAPI dsoGetSampleRate(WORD DeviceIndex);


DLL_API WORD WINAPI  dsoHTSetIIC(WORD DeviceIndex, WORD nTriggerAddr, WORD nTriggerType);
DLL_API WORD WINAPI  dsoHTSetSPI(WORD DeviceIndex, WORD nDataBits, ULONG nDataMask);
DLL_API WORD WINAPI  dsoHTSetUart(WORD DeviceIndex, WORD nDataLen, WORD nData, WORD nOdd, WORD nTriggerType);
DLL_API WORD WINAPI  dsoHTSetLinCan(WORD DeviceIndex, WORD nCANType, ULONG nCANID, UCHAR nLINID, UCHAR nLINType);
DLL_API WORD WINAPI  dsoHTSetSeriseData(WORD DeviceIndex, ULONG nTriggerData, UCHAR nShieldMask);
DLL_API WORD WINAPI  dsoHTSetSeriesTriggerCommon(WORD DeviceIndex, WORD nTriggersouce, ULONG nBaudRate, WORD TriggerType,ULONG nTime);
DLL_API WORD WINAPI	 dsoHTGetCanDecode(WORD nDeviceIndex, CAN_DECODE *p);

DLL_API WORD WINAPI  dsoHTGetLinDecode(WORD nDeviceIndex, LIN_DECODE *p);

DLL_API WORD WINAPI dsoHTGetUartDecode(WORD nDeviceIndex, UART_DECODE *p);

DLL_API WORD WINAPI dsoHTGetSPIDecode(WORD nDeviceIndex, SPI_DECODE *p);

DLL_API WORD WINAPI dsoHTGetIICDecode(WORD nDeviceIndex, IIC_DECODE *p);
#endif
