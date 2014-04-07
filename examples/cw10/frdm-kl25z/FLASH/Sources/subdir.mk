################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/main.c" \
"../Sources/sa_mtb.c" \
"../Sources/ucp_app.c" \
"../Sources/ucp_hal.c" \

C_SRCS += \
../Sources/main.c \
../Sources/sa_mtb.c \
../Sources/ucp_app.c \
../Sources/ucp_hal.c \

OBJS += \
./Sources/main.o \
./Sources/sa_mtb.o \
./Sources/ucp_app.o \
./Sources/ucp_hal.o \

C_DEPS += \
./Sources/main.d \
./Sources/sa_mtb.d \
./Sources/ucp_app.d \
./Sources/ucp_hal.d \

OBJS_QUOTED += \
"./Sources/main.o" \
"./Sources/sa_mtb.o" \
"./Sources/ucp_app.o" \
"./Sources/ucp_hal.o" \

C_DEPS_QUOTED += \
"./Sources/main.d" \
"./Sources/sa_mtb.d" \
"./Sources/ucp_app.d" \
"./Sources/ucp_hal.d" \

OBJS_OS_FORMAT += \
./Sources/main.o \
./Sources/sa_mtb.o \
./Sources/ucp_app.o \
./Sources/ucp_hal.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/main.o: ../Sources/main.c
	@echo 'Building file: $<'
	@echo 'Executing target #1 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/main.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/main.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/sa_mtb.o: ../Sources/sa_mtb.c
	@echo 'Building file: $<'
	@echo 'Executing target #2 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/sa_mtb.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/sa_mtb.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/ucp_app.o: ../Sources/ucp_app.c
	@echo 'Building file: $<'
	@echo 'Executing target #3 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/ucp_app.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/ucp_app.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/ucp_hal.o: ../Sources/ucp_hal.c
	@echo 'Building file: $<'
	@echo 'Executing target #4 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/ucp_hal.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/ucp_hal.o"
	@echo 'Finished building: $<'
	@echo ' '


