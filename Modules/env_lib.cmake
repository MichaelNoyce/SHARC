# Configures the environmental sensor Driver

# Generates static libraries:
# - SHARC::ENV

set(STM32_HAL_ENV_DRIVER_INCLUDE_DIRECTORIES
	${CMAKE_SOURCE_DIR}/Modules/ICM20649/Inc
)

file(GLOB STM32_HAL_ENV_DRIVER_SOURCES
	${CMAKE_SOURCE_DIR}/Modules/ICM20649/Src/*.c
)

# Workaround - Broken template files should not be compiled.
list(FILTER STM32_HAL_ENV_DRIVER_SOURCES EXCLUDE REGEX ".*_template.*\\.c")

# SD Driver
add_library(STM32_HAL_ENV_DRIVER STATIC
	${STM32_HAL_ENV_DRIVER_SOURCES}
)


target_include_directories(STM32_HAL_ENV_DRIVER SYSTEM
	PUBLIC ${STM32_HAL_ENV_DRIVER_INCLUDE_DIRECTORIES}
)

add_library(SHARC::ENV ALIAS STM32_HAL_ENV_DRIVER)