#include "MotorControl.h"
u8 keytemp=0;
u16 uiVqrefTemp=0;
u16 uiVqrefOut=0;
u16 uiVqTarget=0;
u8 ucAngle=0;
int uiTargetLocation=0;
u16 uiLeftLength=1260;
u16 uiRigthLength=1260;
u16 uiLengthDifferent=0;
int uiCurrentLocation=0;
u8 ucMotorDrection=0;
u8 ucSteeringDrection=0;
u8 ucCalibrationFlag=0;
u8 ucCalibrationSuccessFlag=0;
u8 uc100msCnt=0;
u16 uc10msCnt=0;
u16 uiSpeedFbk=0;
u16 uiHallCnt=0;
u16 uiHallSpeedCnt=0;
u16 uiHallSpeedBackCnt=0;
u16 uiHallSpeedBackTempCnt=0;
u16 uiHallSpeedTimeCnt=0;
const u8 ucForwardHall[8]={0,3,6,2,5,1,4,0};
const u8 ucReverseHall[8]={0,5,3,1,6,4,2,0};
int uiCurrentLocationHallCnt=0;
u8 HallTemp;
PID_Para SpeedPara;
PID_Para CurrentPara;
u32 ulSpeedArry[42]={0};

DATA_PARA SpeedData;
u8 ucChangeDrectionFlag=0;

u16 uiSpeedSet=0;
u8 ucReadZeroFlag=0;
u16 uiCurrentZero=0;
u16 uiCurrentAD=0;
u8 ucForewardStallFlag=0;
u8 ucReversedStallFlag=0;

void SpeedSumInit(void)
{
    int i=0;
    for(i=0;i<42;i++)
    {
        ulSpeedArry[i]=1488;
    }
    SpeedData.DataSum=62500;
    SpeedData.Index=0;
    SpeedData.NewData=750;
    SpeedData.AimSpeed=9000;

}

void SpeedSum(DATA_PARA *Data)
{
    if(Data->Index>=42)
    {
        Data->Index=0;
    }
    Data->DataSum = Data->DataSum + Data->NewData - ulSpeedArry[Data->Index];
    ulSpeedArry[Data->Index]=Data->NewData;
    Data->Index++;

}

#define MIN_DISTANCE   42
#define MAX_DISTANCE   100//300
#define MIN_SPEED       200   //rpm
#define MAX_SPEED       1125  //rpm
#define Speed_k         (MAX_SPEED-MIN_SPEED)/(MAX_DISTANCE-MIN_DISTANCE)
#define Speed_b         (MIN_SPEED-Speed_k*MIN_DISTANCE)

u8 ucMotorDrectionTemp=0;
void CalculateDistance(void)
{


    if(ucCalibrationSuccessFlag==1)
    {
        if(ucSteeringDrection==1)
        {
            uiTargetLocation=uiLeftLength+(ucAngle*uiRigthLength)/127-21;
        }
        else
        {
            uiTargetLocation=uiLeftLength-(ucAngle*uiLeftLength)/127-21;
        }
        if(uiTargetLocation>uiCurrentLocation)
        {
            uiLengthDifferent=uiTargetLocation-uiCurrentLocation;
            if((ucMotorDrection==1)&&(uiVqrefOut>0))//在运行中,才能判断反向
            {
                ucChangeDrectionFlag=1;
            }
            ucMotorDrectionTemp=0;
            ucReversedStallFlag=0;
        }
        else
        {
            uiLengthDifferent=uiCurrentLocation-uiTargetLocation;
            if((ucMotorDrection==0)&&(uiVqrefOut>0))//在运行中,才能判断反向
            {
                ucChangeDrectionFlag=1;
            }
            ucMotorDrectionTemp=1;
            ucForewardStallFlag=0;
        }
    }
    if((ucCalibrationFlag==1)&&(ucCalibrationSuccessFlag==0))//进行校准中
    {
        uiLengthDifferent=70;//100
        ucMotorDrection=1;
    }
    else
    {
        ucMotorDrection=ucMotorDrectionTemp;
    }
    if(uiLengthDifferent>9)
    {
        keytemp=1;//启动运行
        if(uiLengthDifferent>MAX_DISTANCE)
        {
            uiSpeedSet=1125;
        }
        else if(uiLengthDifferent>MIN_DISTANCE)
        {
            uiSpeedSet=Speed_k*uiLengthDifferent+Speed_b;
        }
        else
        {
            uiSpeedSet=MIN_SPEED;
        }
    }
    else
    {
        uiSpeedSet=0;
    }

    if(ucChangeDrectionFlag==1)//换向减速
    {
        uiSpeedSet=0;
        if(uiVqrefOut<100)
        {
            keytemp=2;
        }
    }

   // if((uiCurrentLocation==uiTargetLocation)||(keytemp==2))
    if((uiLengthDifferent<10)||(keytemp==2)||(ucReadZeroFlag==0)||(ucForewardStallFlag==1)||(ucReversedStallFlag==1)||(ucCalibrationFlag==0))
    //if((uiLengthDifferent<10)||(keytemp==2)||(ucReadZeroFlag==0)||(ucForewardStallFlag==1)||(ucReversedStallFlag==1))
    {
        uiVqrefOut=0;
        ucChangeDrectionFlag=0;
        SpeedParaInit();
        SpeedSumInit();
        CurrentParaInit();
        keytemp=2;

    }
    else
    {
        if(CurrentPara.Out < SpeedPara.Out)
        {
            uiVqrefOut=CurrentPara.Out;
        }
        else
        {
            uiVqrefOut=SpeedPara.Out;
        }

    }

    My_PWM=uiVqrefOut;

}
void MotorControl(void)
{

   if(ucCalibrationFlag==1)//校准0点
   {
       //ucMotorDrection=0;
       uiVqTarget=800;
   }
   if(keytemp==1)
   {
       SpeedPara.Ref=uiSpeedSet;//78<<6;
       SpeedPara.Fbk=uiHallSpeedBackTempCnt*200/7;//1875000/SpeedData.DataSum;//SpeedData.AimSpeed;
       PID_Speed(&SpeedPara);
   }

}
#define MAX_OUT_PWM  500
#define MIN_OUT_PWM  60

#define MAX_ERROR    500

#define KP_TEMP      15
#define KI_TEMP      15
//u16 KP_TEMP;
//u16 KI_TEMP;
void PID_Speed(PID_Para *SpeedPara)
{
    SpeedPara->Err = SpeedPara->Ref - SpeedPara->Fbk;
    if(SpeedPara->Err>MAX_ERROR)
    {
        SpeedPara->Err=MAX_ERROR;
    }
    SpeedPara->Up = (SpeedPara->Kp*SpeedPara->Err)/KP_TEMP;
    SpeedPara->Ui = SpeedPara->Ui + ((SpeedPara->Ki*SpeedPara->Err)/KI_TEMP);

    SpeedPara->Out = SpeedPara->Up + SpeedPara->Ui;


    if(SpeedPara->Out > SpeedPara->OutMax)
    {
        SpeedPara->Out = SpeedPara->OutMax;
    }

    if(SpeedPara->Out < SpeedPara->OutMin)
    {
        SpeedPara->Out = SpeedPara->OutMin;
    }
}

void SpeedParaInit(void)
{
    SpeedPara.Kp=1;
    SpeedPara.Ki=1;
    SpeedPara.Ui=0;
    SpeedPara.Out=0;
    SpeedPara.OutMin=MIN_OUT_PWM;
    SpeedPara.OutMax=MAX_OUT_PWM;
}

void CurrentParaInit(void)
{
    CurrentPara.Kp=1;
    CurrentPara.Ki=1;
    CurrentPara.Ui=0;
    CurrentPara.Out=0;
    CurrentPara.OutMin=MIN_OUT_PWM;
    CurrentPara.OutMax=MAX_OUT_PWM;
}

u16 uiLimitCurret=480;//4A = 480
u16 uiCurrentRef=243;
u16 uiCurrentFbk=0;

void PID_AverageCurrent(PID_Para *CurrentPara)
{

    CurrentPara->Err = CurrentPara->Ref - CurrentPara->Fbk;

    CurrentPara->Up = (CurrentPara->Kp*CurrentPara->Err);

    CurrentPara->Ui = CurrentPara->Ui + ((CurrentPara->Ki*CurrentPara->Err));

    CurrentPara->Out = CurrentPara->Up + CurrentPara->Ui;


    if(CurrentPara->Out > CurrentPara->OutMax)
    {
        CurrentPara->Out = CurrentPara->OutMax;
    }

    if(CurrentPara->Out < CurrentPara->OutMin)
    {
        CurrentPara->Out = CurrentPara->OutMin;
    }

    CurrentPara->Ui=CurrentPara->Out;

}
