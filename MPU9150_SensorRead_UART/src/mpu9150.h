/*
 * mpu9150.h
 *
 *  Created on: Mar 2, 2015
 *      Author: rohit
 */

#ifndef MPU9150_H_
#define MPU9150_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

uint8_t MPU9150Read(uint8_t);
uint8_t MPU9150Write(uint8_t, uint8_t);
uint8_t MPU9150Init(void);
uint8_t MPU9150GetRawAccelData(void);
uint8_t MPU9150GetRawGyroData(void);

typedef struct
{
	uint8_t address;
	uint8_t accelRegBaseAdd;
	uint8_t gyroRegBaseAdd;

	// Raw Values exactly as received from MPU9150 registers
	uint8_t ui8_rawAccel[6];
	uint8_t ui8_rawGyro[6];

	// Raw values but Hi-Low Combined
	uint16_t ui16_rawAccel[3];
	uint16_t ui16_rawGyro[3];

	// Signed values
	int16_t i16_rawAccel[3];
	int16_t i16_rawGyro[3];

	float accel[3];
	float gyro[3];

	uint8_t (*init)(void);
	uint8_t (*read)(uint8_t);
	uint8_t (*write)(uint8_t, uint8_t);
	uint8_t (*getRawAccelData)(void);
	uint8_t (*getRawGyroData)(void);

}_MPU9150;

extern _MPU9150 MPU9150;

#endif /* MPU9150_H_ */
