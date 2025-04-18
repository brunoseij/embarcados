################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/bsp.c \
../src/main.c \
../src/pisca.c 

C_DEPS += \
./src/bsp.d \
./src/main.d \
./src/pisca.d 

OBJS += \
./src/bsp.o \
./src/main.o \
./src/pisca.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"C:\qp\qpc\include" -I"C:\Users\bruno\OneDrive\Documentos\Codigos\Projeto Elevador\Pisca8\Pisca8\src" -I"C:\qp\qpc\ports\win32-qv" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/bsp.d ./src/bsp.o ./src/main.d ./src/main.o ./src/pisca.d ./src/pisca.o

.PHONY: clean-src

