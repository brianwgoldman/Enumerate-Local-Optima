################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Enumeration.cpp \
../src/GraphUtilities.cpp \
../src/MKLandscape.cpp \
../src/main.cpp 

OBJS += \
./src/Enumeration.o \
./src/GraphUtilities.o \
./src/MKLandscape.o \
./src/main.o 

CPP_DEPS += \
./src/Enumeration.d \
./src/GraphUtilities.d \
./src/MKLandscape.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -O3 -pedantic -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


