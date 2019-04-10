#include "Drv8301.h"
#include "GPIO.h"
#include "Delay.h"
#define DRV8301RS_MSG_NUM    8
u16 	SPI1_RxData[DRV8301RS_MSG_NUM];
static const uint16_t SPI1_TxData[DRV8301RS_MSG_NUM] =
{

	DRV8301_SPI_DATA_FORMATTER(DRV8301_CMD_READ,DRV8301_ADDR_STATUS_REGISTER_1,0x00),
	DRV8301_SPI_DATA_FORMATTER(DRV8301_CMD_READ,DRV8301_ADDR_STATUS_REGISTER_2,0x00),
	DRV8301_SPI_DATA_FORMATTER(DRV8301_CMD_READ,DRV8301_ADDR_CONTROL_REGISTER_1,0x00),
	DRV8301_SPI_DATA_FORMATTER(DRV8301_CMD_READ,DRV8301_ADDR_CONTROL_REGISTER_2,0x00),
	DRV8301_SPI_DATA_FORMATTER(DRV8301_CMD_WRITE, DRV8301_ADDR_CONTROL_REGISTER_1, \
					DRV8301_SPI_CONTROL_REGISTER_1_DATA(DRV8301_VdsLevel_1p324_V, \
														DRV8301_OcMode_CurrentLimit, \
														DRV8301_PwmMode_Six_Inputs, \
														DRV8301_Reset_Normal, \
														DRV8301_PeakCurrent_0p25_A)),
	DRV8301_SPI_DATA_FORMATTER(DRV8301_CMD_READ, DRV8301_ADDR_CONTROL_REGISTER_1, 0x00),
	DRV8301_SPI_DATA_FORMATTER(DRV8301_CMD_WRITE, DRV8301_ADDR_CONTROL_REGISTER_2, \
					DRV8301_SPI_CONTROL_REGISTER_2_DATA(DRV8301_OcOffTimeMode_Normal, \
														DRV8301_DcCalMode_Ch2_Load, \
														DRV8301_DcCalMode_Ch1_Load, \
														DRV8301_ShuntAmpGain_10VpV, \
														DRV8301_OcTwMode_Both)),
	DRV8301_SPI_DATA_FORMATTER(DRV8301_CMD_READ, DRV8301_ADDR_CONTROL_REGISTER_2, 0x00),
};

void GateDriverConf(void)
{
	/* Enable 8301*/
	GPIO_SetBits(DRV8301_EN_GPIO_PORT, DRV8301_EN_PIN);
	delay_ms(5);
	for (int i = 0; i < DRV8301RS_MSG_NUM; i++)
	{
		GPIO_ResetBits(DRV8301_CS_PORT, DRV8301_CS_PIN);//Ƭѡ
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}
		SPI_I2S_SendData(SPI1,SPI1_TxData[i]);
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){}  
		SPI1_RxData[i] = SPI_I2S_ReceiveData(SPI1);
		GPIO_SetBits(DRV8301_CS_PORT, DRV8301_CS_PIN);//ȡ��Ƭѡ
		delay_ms(1);
	}
}
