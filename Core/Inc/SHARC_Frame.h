#ifndef HAL_SHARC_FRAME_H_
#define HAL_SHARC_FRAME_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "WAVE.h"
#include "GPS_Struct.h"

//============================= 0. Definitions and Macros ===============================
//define Size of Drift and Wave data payload
#define DRIFTBUFFER_SIZE 29
#define WAVEBUFFER_SIZE 34

//============================= 1. Global Variables ===============================
extern uint8_t Sample_On,sample_count,IMU_On;
extern uint32_t Sample_Time_Start; //used to measure length of Routine_STATE_SAMPLE
extern uint32_t Sample_Time; //used to measure length of Routine_STATE_SAMPLE
extern uint8_t driftbuffer[DRIFTBUFFER_SIZE];
extern uint8_t wavebuffer[WAVEBUFFER_SIZE];

//============================= 2. Function Prototypes ============================
/*
 * Function Name: uint8_t* to_binary_format(GPS_Data_t gps_data ,uint8_t ID);
 *
 * @brief: Converts the data in the GPS_Data_t and
 */

uint8_t* to_binary_format_GPS(GPS_Data_t gps_data ,uint8_t ID);


/**
 * @brief
 *
 * @param wave_data
 * @param ID
 * @return
 */
uint8_t* to_binary_format_Wave(Wave_Data_t wave_data ,uint8_t ID);

/*
 * Function Name: uint8_t  get_driftBuffer_Size(void);
 */
uint8_t  get_driftBuffer_Size(void);

#ifdef __cplusplus
}
#endif

#endif /* HAL_SHARC_FRAME_H_ */