################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/glad.c \
../src/myPng.c \
../src/offscreen.c 

OBJS += \
./src/glad.o \
./src/myPng.o \
./src/offscreen.o 

C_DEPS += \
./src/glad.d \
./src/myPng.d \
./src/offscreen.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/home/mj/majun/android-ndk-r9d/platforms/android-9/arch-arm/usr/include -I/home/mj/disk/source/githup/glfw/deps -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


