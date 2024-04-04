# Configures the Wave Driver

# Generates static libraries:
# - SHARC::Wave

set(STM32_WAVE_LIB_INCLUDE_DIRECTORIES
	${CMAKE_SOURCE_DIR}/Modules/WAVE/Inc
)

file(GLOB STM32_WAVE_LIB_SOURCES
	${CMAKE_SOURCE_DIR}/Modules/WAVE/Src/WAVE.c
)

# Workaround - Broken template files should not be compiled.
list(FILTER STM32_WAVE_LIB_SOURCES EXCLUDE REGEX ".*_template.*\\.c")

# SD Driver
add_library(STM32_WAVE_LIB STATIC
	${STM32_WAVE_LIB_SOURCES}
)


target_include_directories(STM32_WAVE_LIB SYSTEM
	PUBLIC ${STM32_WAVE_LIB_INCLUDE_DIRECTORIES}
)

add_library(SHARC::WAVE ALIAS STM32_WAVE_LIB)