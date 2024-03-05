if(CMAKE_CROSSCOMPILING)
	include(stm32l4cube.cmake)
	include(fatfs_lib.cmake)
	include(sd_lib.cmake)
	include(imu_lib.cmake)

	target_link_libraries(STM32CubeL4_FATFS PUBLIC STM32CUBEL4_HAL)

	target_link_libraries(STM32_HAL_SD_DRIVER PUBLIC STM32CubeL4_FATFS)

	target_link_libraries(STM32_HAL_IMU_DRIVER PUBLIC STM32CUBEL4_HAL)

endif()