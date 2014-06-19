################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../src/apps.c" \
"../src/ipsec_fm_demo.c" \

C_SRCS += \
../src/apps.c \
../src/ipsec_fm_demo.c \

OBJS += \
./src/apps_c.obj \
./src/ipsec_fm_demo_c.obj \

OBJS_QUOTED += \
"./src/apps_c.obj" \
"./src/ipsec_fm_demo_c.obj" \

C_DEPS += \
./src/apps_c.d \
./src/ipsec_fm_demo_c.d \

OBJS_OS_FORMAT += \
./src/apps_c.obj \
./src/ipsec_fm_demo_c.obj \

C_DEPS_QUOTED += \
"./src/apps_c.d" \
"./src/ipsec_fm_demo_c.d" \


# Each subdirectory must supply rules for building sources it contributes
src/apps_c.obj: ../src/apps.c
	@echo 'Building file: $<'
	@echo 'Executing target #1 $<'
	@echo 'Invoking: PowerPC Compiler'
	"$(PAToolsDirEnv)/mwcceppc" @@"src/apps.args" -o "src/apps_c.obj" "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '

src/%.d: ../src/%.c
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

src/ipsec_fm_demo_c.obj: ../src/ipsec_fm_demo.c
	@echo 'Building file: $<'
	@echo 'Executing target #2 $<'
	@echo 'Invoking: PowerPC Compiler'
	"$(PAToolsDirEnv)/mwcceppc" @@"src/ipsec_fm_demo.args" -o "src/ipsec_fm_demo_c.obj" "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '


