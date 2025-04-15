################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/inita.c \
../Src/leda.c \
../Src/main.c \
../Src/maina.c \
../Src/serial.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/timer.c \
../Src/timer_main.c 

OBJS += \
./Src/inita.o \
./Src/leda.o \
./Src/main.o \
./Src/maina.o \
./Src/serial.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/timer.o \
./Src/timer_main.o 

C_DEPS += \
./Src/inita.d \
./Src/leda.d \
./Src/main.d \
./Src/maina.d \
./Src/serial.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/timer.d \
./Src/timer_main.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F303VCTx -DSTM32 -DSTM32F3 -DSTM32F3DISCOVERY -c -I../Inc -I"C:/Users/user/Desktop/stm32f303-definitions/Core/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/inita.cyclo ./Src/inita.d ./Src/inita.o ./Src/inita.su ./Src/leda.cyclo ./Src/leda.d ./Src/leda.o ./Src/leda.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/maina.cyclo ./Src/maina.d ./Src/maina.o ./Src/maina.su ./Src/serial.cyclo ./Src/serial.d ./Src/serial.o ./Src/serial.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su ./Src/timer.cyclo ./Src/timer.d ./Src/timer.o ./Src/timer.su ./Src/timer_main.cyclo ./Src/timer_main.d ./Src/timer_main.o ./Src/timer_main.su

.PHONY: clean-Src

