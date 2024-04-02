set(CPU -mcpu=cortex-m4)
set(FPU -mfpu=fpv4-sp-d16)
set(FLOAT_ABI -mfloat-abi=hard )

set(CPU_COMPILE_OPTIONS 
	${CPU}
	-mthumb
	${FPU}
	${FLOAT_ABI}
)
