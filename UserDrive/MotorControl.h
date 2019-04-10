#ifndef _MOTOR_CONTROL_
#define _MOTOR_CONTROL_
#include "stm32f10x.h"
#define LENGTH  240
typedef struct _PID_
{
    int Kp;
    int Ki;
    int Kd;
    int Ref;
    int Fbk;
    int Err;
    int Kc;
    int Up;
    int Ui;
    int Ud;
    int OutPreSat;
    int OutMax;
    int OutMin;
    int Out;
    int SatErr;
    int Upl;
}PID_Para;
typedef struct
{
    u16 NewData;
    u16 Index;
    u16 DataOut;
    u16 AimSpeed;
    u32 DataSum;
}DATA_PARA;
extern DATA_PARA SpeedData;
extern u16 uiHallSpeedTimeCnt;
extern u8 keytemp;
extern u8 uc1msFlag;
extern u16 uiVqrefTemp;
extern u16 uiVqrefOut;
extern u16 uiVqTarget;
extern u8 ucAngle;
extern int uiTargetLocation;
extern u16 uiLeftLength;
extern u16 uiRigthLength;
extern u16 uiLengthDifferent;
extern int uiCurrentLocation;
extern u8 ucMotorDrection;
extern u8 ucSteeringDrection;
extern u8 ucCalibrationFlag;
extern u8 uc100msCnt;
extern u16 uc10msCnt;
extern u16 uiSpeedFbk;
extern u16 uiHallCnt;
extern u16 uiHallSpeedCnt;
extern u16 uiHallSpeedBackCnt;
extern u16 uiHallSpeedBackTempCnt;
extern int uiCurrentLocationHallCnt;
extern const u8 ucForwardHall[8];
extern const u8 ucReverseHall[8];
extern u8 HallTemp;
extern PID_Para SpeedPara;
extern PID_Para CurrentPara;
extern u8 ucReadZeroFlag;
extern u16 uiCurrentZero;
extern u16 uiCurrentAD;
extern u16 uiCurrentFbk;
extern u16 uiLimitCurret;
extern u8 ucForewardStallFlag;
extern u8 ucReversedStallFlag;
extern u8 ucCalibrationSuccessFlag;
extern void PID_Speed(PID_Para *SpeedPara);
extern void PID_AverageCurrent(PID_Para *CurrentPara);
extern void CurrentParaInit(void);
extern u16 My_PWM;
void MotorControl(void);
void SpeedParaInit(void);
void SpeedSum(DATA_PARA *Data);
void SpeedSumInit(void);
void CalculateDistance(void);
#endif
