/*
 * mpu9150.c
 *
 *  Created on: Mar 2, 2015
 *      Author: rohit
 */

//#define DEBUG_I2C
//#define DEBUG_MPU9150

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_uart.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

/* I2C includes  */
#include "driverlib/i2c.h"

#include "mpu9150.h"

_MPU9150 MPU9150 = {
		.address = 0x68,
		.accelRegBaseAdd = 59,
		.gyroRegBaseAdd = 67,
		.init = MPU9150Init,
		.read = MPU9150Read,
		.write = MPU9150Write,
		.getRawAccelData = MPU9150GetRawAccelData,
		.getRawGyroData = MPU9150GetRawGyroData,
		.getFloatAccelData = MPU9150GetFloatAccelData,
		.getFloatGyroData = MPU9150GetFloatGyroData
};

uint32_t ui32_temp;
uint16_t ui16_temp;
uint8_t ui8_temp;

uint8_t MPU9150Read(uint8_t reg)
{
	I2CMasterSlaveAddrSet(I2C0_BASE, MPU9150.address, false);
	I2CMasterDataPut(I2C0_BASE, reg);
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);

	while(I2CMasterBusy(I2C0_BASE));

	uint8_t val = 0x00;

#ifdef DEBUG_I2C
	switch(I2CMasterErr(I2C0_BASE))
	{
	case I2C_MASTER_ERR_ADDR_ACK:
		UARTprintf("\nI2C_MASTER_ERR_ADDR_ACK");
		return 0;

	case I2C_MASTER_ERR_DATA_ACK:
		UARTprintf("\nI2C_MASTER_ERR_DATA_ACK");
		return 0;

	case I2C_MASTER_ERR_ARB_LOST:
		UARTprintf("\nI2C_MASTER_ERR_ARB_LOST");
		return 0;

	case I2C_MASTER_ERR_NONE:
		UARTprintf("\nI2C_MASTER_ERR_NONE");
		break;

	default:
		UARTprintf("\nUnknown I2C Error");
	}
#else
	ui32_temp = I2CMasterErr(I2C0_BASE);
	if (ui32_temp != I2C_MASTER_ERR_NONE){
		UARTprintf("\nI2C_MASTER_ERR_XXXX: 0x%x", ui32_temp);
		return 0;
	}
#endif


	I2CMasterSlaveAddrSet(I2C0_BASE, MPU9150.address, true);
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
	while(I2CMasterBusy(I2C0_BASE));

#ifdef DEBUG_I2C
	switch(I2CMasterErr(I2C0_BASE))
	{
	case I2C_MASTER_ERR_ADDR_ACK:
		UARTprintf("\nI2C_MASTER_ERR_ADDR_ACK");
		return 0;

	case I2C_MASTER_ERR_DATA_ACK:
		UARTprintf("\nI2C_MASTER_ERR_DATA_ACK");
		return 0;

	case I2C_MASTER_ERR_ARB_LOST:
		UARTprintf("\nI2C_MASTER_ERR_ARB_LOST");
		return 0;

	case I2C_MASTER_ERR_NONE:
		UARTprintf("\nI2C_MASTER_ERR_NONE");
		break;

	default:
		UARTprintf("\nUnknown I2C Error");
	}
#else
	ui32_temp = I2CMasterErr(I2C0_BASE);
	if (ui32_temp != I2C_MASTER_ERR_NONE){
		UARTprintf("\nI2C_MASTER_ERR_XXXX: 0x%x", ui32_temp);
		return 0;
	}
#endif

	val = I2CMasterDataGet(I2C0_BASE);
	#ifdef DEBUG_MPU9150
	UARTprintf("\n\tRead  -> Register: 0x%x, Value: 0x%x", reg, val);
	#endif
	return val;
}

uint8_t MPU9150Write(uint8_t reg, uint8_t val)
{
	I2CMasterSlaveAddrSet(I2C0_BASE, MPU9150.address, false);
	I2CMasterDataPut(I2C0_BASE, reg);
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);

	while(I2CMasterBusy(I2C0_BASE));

#ifdef DEBUG_I2C
	switch(I2CMasterErr(I2C0_BASE))
	{
	case I2C_MASTER_ERR_ADDR_ACK:
		UARTprintf("\nI2C_MASTER_ERR_ADDR_ACK");
		I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
		return 0;

	case I2C_MASTER_ERR_DATA_ACK:
		UARTprintf("\nI2C_MASTER_ERR_DATA_ACK");
		I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
		return 0;

	case I2C_MASTER_ERR_ARB_LOST:
		UARTprintf("\nI2C_MASTER_ERR_ARB_LOST");
		I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
		return 0;

	case I2C_MASTER_ERR_NONE:
		UARTprintf("\nI2C_MASTER_ERR_NONE");
		break;

	default:
		UARTprintf("\nUnknown I2C Error");
	}
#else
	ui32_temp = I2CMasterErr(I2C0_BASE);
	if (ui32_temp != I2C_MASTER_ERR_NONE){
		UARTprintf("\nI2C_MASTER_ERR_XXXX: 0x%x", ui32_temp);
		I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
		return 0;
	}
#endif

	I2CMasterDataPut(I2C0_BASE, val);
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);

	while(I2CMasterBusy(I2C0_BASE));

#ifdef DEBUG_I2C
	switch(I2CMasterErr(I2C0_BASE))
	{
	case I2C_MASTER_ERR_ADDR_ACK:
		UARTprintf("\nI2C_MASTER_ERR_ADDR_ACK");
		I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
		return 0;

	case I2C_MASTER_ERR_DATA_ACK:
		UARTprintf("\nI2C_MASTER_ERR_DATA_ACK");
		I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
		return 0;

	case I2C_MASTER_ERR_ARB_LOST:
		UARTprintf("\nI2C_MASTER_ERR_ARB_LOST");
		I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
		return 0;

	case I2C_MASTER_ERR_NONE:
		UARTprintf("\nI2C_MASTER_ERR_NONE");
		break;

	default:
		UARTprintf("\nUnknown I2C Error");
	}
#else
	ui32_temp = I2CMasterErr(I2C0_BASE);
	if (ui32_temp != I2C_MASTER_ERR_NONE){
		UARTprintf("\nI2C_MASTER_ERR_XXXX: 0x%x", ui32_temp);
		I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
		return 0;
	}
#endif


	#ifdef DEBUG_MPU9150
	UARTprintf("\n\tWrite -> Register: 0x%x, Value: 0x%x", reg, val);
	#endif
	return 0;
}

uint8_t MPU9150Init(void)
{
	UARTprintf("\n\nMPU9150 Initialiation");
	uint8_t whoami = MPU9150Read(0x75);
	if (whoami == 0x68)
	{
		UARTprintf("\nMPU9150 is online...");
		UARTprintf("\n\nWhoami Data: 0x%x", whoami);
	}
	else
	{
		UARTprintf("\nCould not connect to MPU9150: 0x%x", whoami);
	}


	MPU9150Write(0x6b, 0x00);
	MPU9150Write(0x25, 0x00);
	MPU9150Write(0x1a, 0x00);
	MPU9150Write(0x1b, 0x12);
	MPU9150Write(0x1c, 0x02); // 0x02= +-2G, 0x0a = +-4g, 0x12= +-8G, 0x1a= +-16G
	MPU9150Write(0x24, 0x00);

	uint8_t val1, val2, val3, val4, val5, val6;
	val1 = MPU9150Read(0x6b);
	val2 = MPU9150Read(0x25);
	val3 = MPU9150Read(0x1a);
	val4 = MPU9150Read(0x1b);
	val5 = MPU9150Read(0x1c);
	val6 = MPU9150Read(0x24);

	UARTprintf("\nPower Mgmt: 0x%x", val1);
	UARTprintf("\nSMPLRT_DIV: 0x%x", val2);
	UARTprintf("\nCONFIG    : 0x%x", val3);
	UARTprintf("\nGyroConfig: 0x%x", val4);
	UARTprintf("\nAccConfig : 0x%x", val5);
	UARTprintf("\nI2C_MSTCTL: 0x%x", val6);

	return 0;
}

uint8_t MPU9150GetRawAccelData(void)
{
	I2CMasterSlaveAddrSet(I2C0_BASE, MPU9150.address, false);
	I2CMasterDataPut(I2C0_BASE, MPU9150.accelRegBaseAdd);
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);

	while(I2CMasterBusy(I2C0_BASE));

	#ifdef DEBUG_I2C
	switch(I2CMasterErr(I2C0_BASE))
	{
	case I2C_MASTER_ERR_ADDR_ACK:
		UARTprintf("\nI2C_MASTER_ERR_ADDR_ACK");
		return 0;

	case I2C_MASTER_ERR_DATA_ACK:
		UARTprintf("\nI2C_MASTER_ERR_DATA_ACK");
		return 0;

	case I2C_MASTER_ERR_ARB_LOST:
		UARTprintf("\nI2C_MASTER_ERR_ARB_LOST");
		return 0;

	case I2C_MASTER_ERR_NONE:
		UARTprintf("\nI2C_MASTER_ERR_NONE");
		break;

	default:
		UARTprintf("\nUnknown I2C Error");
	}
	#else
	ui32_temp = I2CMasterErr(I2C0_BASE);
	if (ui32_temp != I2C_MASTER_ERR_NONE){
		UARTprintf("\nI2C_MASTER_ERR_XXXX: 0x%x", ui32_temp);
		return 0;
	}
	#endif

	uint8_t i = 0x00;

	I2CMasterSlaveAddrSet(I2C0_BASE, MPU9150.address, true);

	for(i=0; i<6; i++)
	{
		if (i==0)
		{
			I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
		}
		else if (i==5)
		{
			I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
		}
		else
		{
			I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
		}

		while(I2CMasterBusy(I2C0_BASE));

#ifdef DEBUG_I2C
		switch(I2CMasterErr(I2C0_BASE))
		{
		case I2C_MASTER_ERR_ADDR_ACK:
			UARTprintf("\nI2C_MASTER_ERR_ADDR_ACK");
			I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP);
			return 0;

		case I2C_MASTER_ERR_DATA_ACK:
			UARTprintf("\nI2C_MASTER_ERR_DATA_ACK");
			I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP);
			return 0;

		case I2C_MASTER_ERR_ARB_LOST:
			UARTprintf("\nI2C_MASTER_ERR_ARB_LOST");
			I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP);
			return 0;

		case I2C_MASTER_ERR_NONE:
			UARTprintf("\nI2C_MASTER_ERR_NONE");
			break;

		default:
			UARTprintf("\nUnknown I2C Error");
		}
#else
		ui32_temp = I2CMasterErr(I2C0_BASE);
		if (ui32_temp != I2C_MASTER_ERR_NONE)
		{
			UARTprintf("\nI2C_MASTER_ERR_XXXX: 0x%x", ui32_temp);
			I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP);
			return 0;
		}
#endif

		MPU9150.ui8_rawAccel[i] = I2CMasterDataGet(I2C0_BASE);
		#ifdef DEBUG_MPU9150
		UARTprintf("\n\tRead  -> Register: 0x%x, Value: 0x%x",
						MPU9150.accelRegBaseAdd + i,
						MPU9150.ui8_rawAccel[i]);
		#endif

	}

	for (i = 0; i < 3; i++)
	{
		MPU9150.ui16_rawAccel[i] = MPU9150.ui8_rawAccel[i << 1] << 8;
		MPU9150.ui16_rawAccel[i] += MPU9150.ui8_rawAccel[(i << 1) + 1];

		MPU9150.i16_rawAccel[i] = (int16_t)MPU9150.ui8_rawAccel[i << 1];
		MPU9150.i16_rawAccel[i] <<= 8;
		MPU9150.i16_rawAccel[i] += MPU9150.ui8_rawAccel[(i << 1) + 1];

	}
	return 0;
}

uint8_t MPU9150GetRawGyroData(void)
{
	I2CMasterSlaveAddrSet(I2C0_BASE, MPU9150.address, false);
	I2CMasterDataPut(I2C0_BASE, MPU9150.gyroRegBaseAdd);
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);

	while(I2CMasterBusy(I2C0_BASE));

	#ifdef DEBUG_I2C
	switch(I2CMasterErr(I2C0_BASE))
	{
	case I2C_MASTER_ERR_ADDR_ACK:
		UARTprintf("\nI2C_MASTER_ERR_ADDR_ACK");
		return 0;

	case I2C_MASTER_ERR_DATA_ACK:
		UARTprintf("\nI2C_MASTER_ERR_DATA_ACK");
		return 0;

	case I2C_MASTER_ERR_ARB_LOST:
		UARTprintf("\nI2C_MASTER_ERR_ARB_LOST");
		return 0;

	case I2C_MASTER_ERR_NONE:
		UARTprintf("\nI2C_MASTER_ERR_NONE");
		break;

	default:
		UARTprintf("\nUnknown I2C Error");
	}
	#else
	ui32_temp = I2CMasterErr(I2C0_BASE);
	if (ui32_temp != I2C_MASTER_ERR_NONE){
		UARTprintf("\nI2C_MASTER_ERR_XXXX: 0x%x", ui32_temp);
		return 0;
	}
	#endif

	uint8_t i = 0x00;

	I2CMasterSlaveAddrSet(I2C0_BASE, MPU9150.address, true);

	for(i=0; i<6; i++)
	{
		if (i==0)
		{
			I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
		}
		else if (i==5)
		{
			I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
		}
		else
		{
			I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
		}

		while(I2CMasterBusy(I2C0_BASE));

#ifdef DEBUG_I2C
		switch(I2CMasterErr(I2C0_BASE))
		{
		case I2C_MASTER_ERR_ADDR_ACK:
			UARTprintf("\nI2C_MASTER_ERR_ADDR_ACK");
			I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP);
			return 0;

		case I2C_MASTER_ERR_DATA_ACK:
			UARTprintf("\nI2C_MASTER_ERR_DATA_ACK");
			I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP);
			return 0;

		case I2C_MASTER_ERR_ARB_LOST:
			UARTprintf("\nI2C_MASTER_ERR_ARB_LOST");
			I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP);
			return 0;

		case I2C_MASTER_ERR_NONE:
			UARTprintf("\nI2C_MASTER_ERR_NONE");
			break;

		default:
			UARTprintf("\nUnknown I2C Error");
		}
#else
		ui32_temp = I2CMasterErr(I2C0_BASE);
		if (ui32_temp != I2C_MASTER_ERR_NONE)
		{
			UARTprintf("\nI2C_MASTER_ERR_XXXX: 0x%x", ui32_temp);
			I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP);
			return 0;
		}
#endif

		MPU9150.ui8_rawGyro[i] = I2CMasterDataGet(I2C0_BASE);
		#ifdef DEBUG_MPU9150
		UARTprintf("\n\tRead  -> Register: 0x%x, Value: 0x%x",
						MPU9150.gyroRegBaseAdd + i,
						MPU9150.ui8_rawGyro[i]);
		#endif

	}

	for (i = 0; i < 3; i++)
	{
		MPU9150.ui16_rawGyro[i] = MPU9150.ui8_rawGyro[i << 1] << 8;
		MPU9150.ui16_rawGyro[i] += MPU9150.ui8_rawGyro[(i << 1) + 1];

		MPU9150.i16_rawGyro[i] = (int16_t)MPU9150.ui8_rawGyro[i << 1];
		MPU9150.i16_rawGyro[i] <<= 8;
		MPU9150.i16_rawGyro[i] += MPU9150.ui8_rawGyro[(i << 1) + 1];
	}
	return 0;
}

uint8_t MPU9150GetFloatAccelData(void)
{
	int i=0;
	for (i=0; i<3 ; i++)
	{
		MPU9150.accel[i] = MPU9150.i16_rawAccel[i]/(float)16384;
	}

	return 0;
}
uint8_t MPU9150GetFloatGyroData(void)
{
	return 0;
}
