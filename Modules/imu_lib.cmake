# Configures the IMU Driver

# Generates static libraries:
# - SHARC::IMU

set(STM32_HAL_IMU_DRIVER_INCLUDE_DIRECTORIES
	${CMAKE_SOURCE_DIR}/Modules/ICM20649/Inc
)

file(GLOB STM32_HAL_IMU_DRIVER_SOURCES
	${CMAKE_SOURCE_DIR}/Modules/ICM20649/Src/*.c
)

# Workaround - Broken template files should not be compiled.
list(FILTER STM32_HAL_IMU_DRIVER_SOURCES EXCLUDE REGEX ".*_template.*\\.c")

# SD Driver
add_library(STM32_HAL_IMU_DRIVER STATIC
	${STM32_HAL_IMU_DRIVER_SOURCES}
)


target_include_directories(STM32_HAL_IMU_DRIVER SYSTEM
	PUBLIC ${STM32_HAL_IMU_DRIVER_INCLUDE_DIRECTORIES}
)

add_library(SHARC::IMU ALIAS STM32_HAL_IMU_DRIVER)