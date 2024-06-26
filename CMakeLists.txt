cmake_minimum_required(VERSION 3.22...3.27)

project(SHARC)

set(TARGET_EXECUTABLE ${CMAKE_PROJECT_NAME})

enable_language(C CXX ASM)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

if(CMAKE_CROSSCOMPILING)
	include(CMake/scripts.cmake)
	include(CMake/mcu.cmake)

	set(CROSSCOMPILE_C_OPTIONS
		-fdata-sections
		-ffunction-sections
	)
	set(CROSSCOMPILE_CXX_OPTIONS
		-fno-rtti
		-fno-exceptions
	)

	set(CROSSCOMPILE_OPTIMISATION_OPTIONS_DEBUG
		-Og
		-ggdb3
	)
	set(CROSSCOMPILE_OPTIMISATION_OPTIONS_RELWITHDEBINFO
		-O3
		-ggdb
	)

	add_compile_options(
		${CPU_COMPILE_OPTIONS}
		${CROSSCOMPILE_C_OPTIONS}
		"$<$<COMPILE_LANGUAGE:CXX>:${CROSSCOMPILE_CXX_OPTIONS}>"
		"$<$<CONFIG:Debug>:${CROSSCOMPILE_OPTIMISATION_OPTIONS_DEBUG}>"
		"$<$<CONFIG:RelWithDebInfo>:${CROSSCOMPILE_OPTIMISATION_OPTIONS_RELWITHDEBINFO}>"
	)

	add_compile_definitions(
		$<$<CONFIG:Debug>:DEBUG>
	)

	add_link_options(
		${CPU_COMPILE_OPTIONS}
		--specs=nano.specs
		-T${LINKER_SCRIPT}

		LINKER:--start-group
		-lc
		-lm
		-lnosys
		LINKER:--end-group

		LINKER:--gc-sections
		LINKER:--print-memory-usage
		#LINKER:--no-warn-rwx-segment # Workaround — Something isn't right in ST's startup code.
	)
endif()

if(NOT CMAKE_CROSSCOMPILING)
	enable_testing()
	set(CMAKE_C_COMPILER "C:/MinGW/bin/gcc.exe")
	set(CMAKE_CXX_COMPILER "C:/MinGW/bin/g++.exe")
endif()

add_subdirectory(Modules)

if(CMAKE_CROSSCOMPILING)
	
	set(APPLICATION_INCLUDE_DIRECTORIES
		${CMAKE_SOURCE_DIR}/Core/Inc
		${CMAKE_SOURCE_DIR}/Core/Inc/HAL_Init
	)
	
	file(GLOB_RECURSE APPLICATION_SOURCES
		${CMAKE_SOURCE_DIR}/Core/Src/*.c
		${CMAKE_SOURCE_DIR}/Core/Src/*.cpp
		${CMAKE_SOURCE_DIR}/Core/Src/HAL_Init/*.c
		${CMAKE_SOURCE_DIR}/Core/Src/HAL_Init/*.cpp
	)

	list(FILTER APPLICATION_SOURCES EXCLUDE REGEX "main.c$")
		
	add_executable(${TARGET_EXECUTABLE}
		${APPLICATION_SOURCES}
		${STARTUP_SCRIPT}
	)


	target_include_directories(${TARGET_EXECUTABLE} PRIVATE
		${APPLICATION_INCLUDE_DIRECTORIES}
	)

	target_compile_options(${TARGET_EXECUTABLE} PRIVATE
		-Wall
		-Wextra
		-Wpedantic
	)

	target_link_libraries(${TARGET_EXECUTABLE} 
		PUBLIC STM32CUBEL4_HAL
		PUBLIC STM32CubeL4_FATFS
		PUBLIC STM32_HAL_SD_DRIVER
		PUBLIC STM32_HAL_IMU_DRIVER
		PUBLIC STM32_WAVE_LIB
		PUBLIC STM32_HAL_SAT_DRIVER
		PUBLIC STM32_HAL_ENV_DRIVER
	)

	target_link_options(${TARGET_EXECUTABLE} PRIVATE
		LINKER:-Map=${CMAKE_PROJECT_NAME}.map,--cref
	)

	add_custom_command(TARGET ${TARGET_EXECUTABLE} POST_BUILD
		COMMENT "Printing ${TARGET_EXECUTABLE} Size..."
		COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${TARGET_EXECUTABLE}>
	)

	add_custom_command(TARGET ${TARGET_EXECUTABLE} POST_BUILD
		COMMENT "Generating ${TARGET_EXECUTABLE}.hex and ${TARGET_EXECUTABLE}.bin..."
		COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${TARGET_EXECUTABLE}> ${TARGET_EXECUTABLE}.hex
		COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${TARGET_EXECUTABLE}> ${TARGET_EXECUTABLE}.bin
	)

	add_custom_command(TARGET ${TARGET_EXECUTABLE} POST_BUILD
		COMMENT "Generating ${TARGET_EXECUTABLE} Disassembly..."
		COMMAND ${CMAKE_OBJDUMP} -D -C $<TARGET_FILE:${TARGET_EXECUTABLE}> > ${TARGET_EXECUTABLE}.s
	)
endif()