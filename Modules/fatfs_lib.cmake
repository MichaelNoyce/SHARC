# Configures the STM32CubeL4 HAL and FATFS libraries.
# It is expected the https://github.com/STMicroelectronics/STM32CubeL4 is cloned to ${CMAKE_SOURCE_DIR}/Modules/STM32CubeL4.

# Generates static libraries:
# - SHARC::FATFS

set(STM32CubeL4_FATFS_INCLUDE_DIRECTORIES
	${CMAKE_SOURCE_DIR}/Modules/STM32CubeL4/Middlewares/Third_Party/FatFs/src
	${CMAKE_SOURCE_DIR}/Modules/STM32CubeL4/Middlewares/Third_Party/FatFS/src/drivers
	${CMAKE_SOURCE_DIR}/Modules/STM32CubeL4/Middlewares/Third_Party/FatFs/src/option
	${CMAKE_SOURCE_DIR}/Modules/FATFS/App
    ${CMAKE_SOURCE_DIR}/Modules/FATFS/Target
)

file(GLOB STM32CubeL4_FATFS_SOURCES
	${CMAKE_SOURCE_DIR}/Modules/STM32CubeL4/Middlewares/Third_Party/FatFS/src/*.c
	${CMAKE_SOURCE_DIR}/Modules/STM32CubeL4/Middlewares/Third_Party/FatFS/src/drivers/*.c
	${CMAKE_SOURCE_DIR}/Modules/STM32CubeL4/Middlewares/Third_Party/FatFs/src/option/*.c
	${CMAKE_SOURCE_DIR}/Modules/FATFS/App/fatfs.c
    ${CMAKE_SOURCE_DIR}/Modules/FATFS/Target/bsp_driver_sd.c
    ${CMAKE_SOURCE_DIR}/Modules/FATFS/Target/fatfs_platform.c
    ${CMAKE_SOURCE_DIR}/Modules/FATFS/Target/sd_diskio.c
    ${CMAKE_SOURCE_DIR}/Modules/FATFS/Target/user_diskio.c
)

# Workaround - Broken template files should not be compiled.
list(FILTER STM32CubeL4_FATFS_SOURCES EXCLUDE REGEX ".*_template.*\\.c")
# Converters are not needed for the application.
list(FILTER STM32CubeL4_FATFS_SOURCES EXCLUDE REGEX ".*cc.*\\.c")

# FATFS Library
add_library(STM32CubeL4_FATFS STATIC
	${STM32CubeL4_FATFS_SOURCES}
)

set(STM32CUBEL4_HAL_COMPILE_DEFINITIONS
	USE_HAL_DRIVER
	STM32L4R5xx
)


target_compile_definitions(STM32CubeL4_FATFS PUBLIC
	${HAL_COMPILE_DEFINITIONS}
)

target_include_directories(STM32CubeL4_FATFS SYSTEM
	PUBLIC ${STM32CubeL4_FATFS_INCLUDE_DIRECTORIES}
	PUBLIC ${APPLICATION_INCLUDE_DIRECTORIES}
	PUBLIC  ${CMAKE_SOURCE_DIR}/Core/Inc
)

add_library(SHARC::FATFS ALIAS STM32CubeL4_FATFS)