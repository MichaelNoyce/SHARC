# Configures the SD Driver

# Generates static libraries:
# - SHARC::SD

set(STM32_HAL_SD_DRIVER_INCLUDE_DIRECTORIES
	${CMAKE_SOURCE_DIR}/Modules/SD/Inc
)

file(GLOB STM32_HAL_SD_DRIVER_SOURCES
	${CMAKE_SOURCE_DIR}/Modules/SD/Src/HAL_SD.c
)

# Workaround - Broken template files should not be compiled.
list(FILTER STM32_HAL_SD_DRIVER_SOURCES EXCLUDE REGEX ".*_template.*\\.c")

# SD Driver
add_library(STM32_HAL_SD_DRIVER STATIC
	${STM32_HAL_SD_DRIVER_SOURCES}
)


target_include_directories(STM32_HAL_SD_DRIVER SYSTEM
	PUBLIC ${STM32_HAL_SD_DRIVER_INCLUDE_DIRECTORIES}
)

add_library(SHARC::SD ALIAS STM32_HAL_SD_DRIVER)