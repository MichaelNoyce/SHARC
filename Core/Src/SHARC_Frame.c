#include "SHARC_Frame.h"

//==========================0. Variable Declarations===============================
uint8_t Sample_On,sample_count,IMU_On;
uint32_t Sample_Time_Start; //used to measure length of Routine_STATE_SAMPLE
uint32_t Sample_Time; //used to measure length of Routine_STATE_SAMPLE
uint8_t driftbuffer[DRIFTBUFFER_SIZE] = {0};
uint8_t wavebuffer[WAVEBUFFER_SIZE] = {0};

//==========================1. Function Definitions===============================
uint8_t* to_binary_format_GPS(GPS_Data_t gps_data ,uint8_t ID)
{
	//define union to convert 32 bit float to unsigned uint8_t array
	union
	{
		float a;					//for 32 bit floats
		unsigned char bytes[4];		//for 4 byte long unsigned integer arrays
		uint32_t num;				//for long numbers
	} byte_converter;
	//define union to convert uint16_t/int16_t into unsigned 8-bit byte arrays
	union
	{
		int16_t  s_halfword;
		unsigned char bytes[2];
	}halfword_converter;

	/*0. Byte 0: Packet ID*/
	*driftbuffer = ID;
	/*1. Bytes 1 - 4:  Epoch Time*/
	byte_converter.num = gps_data.coordinates.time;
	memcpy(&driftbuffer[1],(byte_converter).bytes,4);

	/* Add coordinates bytes 1 - 5 = lat, bytes 6 - 10 = long*/
	byte_converter.a = gps_data.coordinates.lat;
	memcpy(&driftbuffer[5],byte_converter.bytes,4);
	byte_converter.a = gps_data.coordinates.longi;
	memcpy(&driftbuffer[9],byte_converter.bytes,4);


	//convert HDOP,VDOP, PDOP to 2 bytes big endian
	driftbuffer[13] = gps_data.diag.HDOP.digit;
	driftbuffer[14] = gps_data.diag.HDOP.precision;
	driftbuffer[15] = gps_data.diag.VDOP.digit;
	driftbuffer[16] = gps_data.diag.VDOP.precision;
	driftbuffer[17] = gps_data.diag.PDOP.digit;
	driftbuffer[18] = gps_data.diag.PDOP.precision;
	driftbuffer[19] = (gps_data.diag.num_sats);
	driftbuffer[19] = driftbuffer[19]<<2;
	driftbuffer[19] |= gps_data.diag.fix_type;

	//breakdown temperature into 4 unsigned bytes
	byte_converter.num = (uint32_t)gps_data.env_Temp;
	memcpy(&driftbuffer[20],byte_converter.bytes,sizeof(uint32_t));

	//break down pressure into 4 unsigned bytes
	byte_converter.num = gps_data.atm_Press;
	memcpy(&driftbuffer[24],byte_converter.bytes,sizeof(uint32_t));

	driftbuffer[get_driftBuffer_Size() - 1] = 0xd; //end of packet character

	//return a pointer the the array driftbuffer;
	return &driftbuffer[0];
}

uint8_t  get_driftBuffer_Size(void)
{
	return DRIFTBUFFER_SIZE;
}

uint8_t* to_binary_format_Wave(Wave_Data_t wave_data, uint8_t ID)
{
	//define union to convert 32 bit float to unsigned uint8_t array
	union
	{
		float a;					//for 32 bit floats
		unsigned char bytes[4];		//for 4 byte long unsigned integer arrays
		uint32_t num;				//for long numbers
	} byte_converter;
	//define union to convert uint16_t/int16_t into unsigned 8-bit byte arrays
	union
	{
		int16_t  s_halfword;
		unsigned char bytes[2];
	}halfword_converter;

	/*0. Byte 0: Packet ID*/
	*wavebuffer = ID;
	/*1. Bytes 1 - 4:  Significant wave height*/
	//byte_converter.a = wave_data.Hm0;
	memcpy(&wavebuffer[1], &wave_data.Hm0,4);
	/*2. Bytes 5 - 9:  Root mean square wave height*/
	//byte_converter.a = wave_data.Hrms;
	memcpy(&wavebuffer[5], &wave_data.Hrms,4);
	/*3. Bytes 9 - 13:  Mean zero crossing period */
	//byte_converter.a = wave_data.T0;
	memcpy(&wavebuffer[9], &wave_data.T0,4);
	/*4. Bytes 14 - 17: Peak wave period*/
	//byte_converter.a = wave_data.TM01;
	memcpy(&wavebuffer[13], &wave_data.TM01,4);
	/*5. Bytes 18 - 21:  0th spectral moment*/
	//byte_converter.a = wave_data.Tp;
	memcpy(&wavebuffer[17], &wave_data.Tp,4);
	/*6. Bytes 22 - 25:  1st spectral moment*/
	//byte_converter.a = wave_data.M0;
	memcpy(&wavebuffer[21], &wave_data.M0,4);
	/*7. Bytes 26 - 29:  2nd spectral moment*/
	//byte_converter.a = wave_data.M1;
	memcpy(&wavebuffer[25], &wave_data.M1,4);
	/*8. Bytes 30 - 150:  PSD Spectra */
	//byte_converter.a = wave_data.M2;
	memcpy(&wavebuffer[29], &wave_data.M2,4);


	wavebuffer[WAVEBUFFER_SIZE-1] = 0xd; //end of packet character

	//return a pointer the the array buffer;
	return wavebuffer;
}