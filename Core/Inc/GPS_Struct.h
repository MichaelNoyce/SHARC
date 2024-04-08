/**
  ******************************************************************************
  * @file			: GPS_Struct.h
  * @brief			: Data Structures for GPS Data Interfacing
  * @author			: Lawrence Stanton
  ******************************************************************************
  * @attention
  *
  * © M Noyce 2022
  *
  * This file and its content are the copyright property of the author. All
  * rights are reserved. No warranty is given. No liability is assumed.
  * Confidential unless licensed otherwise. If licensed, refer to the
  * accompanying file "LICENCE" for license details.
  *
  ******************************************************************************
  */

#ifndef SRC_GPS_STRUCT_H_
#define SRC_GPS_STRUCT_H_

/*
 * Coordinate Object
 *
 * Stores the Cordinates of GPS in the form DDMM.mmmm
 * where DD - Degrees
 * 	     MM - Minutes
 * 	   mmmm - Fractional minutes
 * Variables:	Name.............Type.................................Description
 * 				lat..............float32_t............................GPS Lattitude
 * 				longi............float32_t............................GPS Longitude
 */
typedef struct{
	float lat;
	float longi;
	uint32_t time;
	uint32_t tic;
}Coord_t;

/*
 * DOP Object
 *
 * Dilation of Precision is a metric of how the elevation, satelite number and satelite spread
 * affects the accuracy of the signal. This object allows for the conversion and storage of a
 * 32 bit float value representing a DOP into an 8 bit digit and 8 bit precision the range of
 * values outputted from the GPS is between 0,0 and 99.9. This will allow for accuracy up to 2
 * decimal places.
 *
 * Variables:	Name.............Type.................................Description
 * 				digit............uint8_t..............................8 bit representation of the whole number
 * 				precision........uint8_t..............................2 significant places representation of the fraction
 *
 */
typedef struct{
	int digit;
	int precision;
}DOP_t;

/*
 * Diagnostic Object
 *
 * Structure to Hold the GPS data signal diagnostics
 *
 * Variables:	Name.............Type.................................Description
 * 				PDOP.............DOP_t................................Positional Dilation of Precision (3D)
 * 				HDOP.............DOP_t................................Horizontal Dilation of Precision
 * 				VDOP.............DOP_t................................Vertical   Dilation of Precision
 * 				num_sats.........uint8_t..............................Number of Satelites used to obtain positional Fix
 * 				fix_type.........uint8_t..............................number between 1-3 describing the type of fix obtained
 *
 * Fix types
 * 1 - No Fix
 * 2 - 2D Fix (No altitude)
 * 3 - 3D Fix
 */

typedef struct{
	DOP_t PDOP;
	DOP_t HDOP;
	DOP_t VDOP;
	int num_sats;
	int fix_type;

	uint32_t time;
}Diagnostic_t;

typedef struct{
	Coord_t  coordinates;	//GPS coordinates
	Diagnostic_t diag;		//Diagnostic information
} M9N_Data_t;


/*
 * @brief: Structure to store data from GPS in an organised format. Note: custom data types from HAL_GPS.h
 */
typedef struct{
	Coord_t  coordinates;	//GPS coordinates
	Diagnostic_t diag;		//Diagnostic information
	int env_Temp;		//environmental temperature
	int atm_Press;		//atmospheric pressure
} GPS_Data_t;


#endif /* SRC_GPS_STRUCT_H_ */
