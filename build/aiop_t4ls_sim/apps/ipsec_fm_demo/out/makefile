################################################################################
# Automatically-generated file. Do not edit!
################################################################################

#Progress monitor hint: 2
first : all
-include ../makefile.init

# This file contains definitions of environment variables used in the makefiles and .args files if exist.
-include makefile.local

RM := "$(GNU_Make_Install_DirEnv)/rm" -f

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
STATICLIB += \
aiop_app.elf \

STATICLIB_QUOTED += \
"aiop_app.elf" \

STATICLIB_OS_FORMAT += \
aiop_app.elf \

EXECUTABLES += \
aiop_app.elf \

EXECUTABLES_QUOTED += \
"aiop_app.elf" \

EXECUTABLES_OS_FORMAT += \
aiop_app.elf \


# All Target
all: warning-messages explicit-dependencies aiop_app.elf

dependents:
	-cd "$(AIOPSL)/out" && $(MAKE) all

# Tool invocations
aiop_app.elf: $(OBJS) $(USER_OBJS) $(OUT)/aiopsl.a $(CW_FILES)/aiop_link.lcf
	@echo 'Building target: $@'
	@echo 'Executing target #3 $@'
	@echo 'Invoking: PowerPC Linker'
	"$(PAToolsDirEnv)/mwldeppc" -o "aiop_app.elf" @@"aiop_app.args"  $(USER_OBJS_QUOTED)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(S_DEPS_QUOTED) $(STATICLIB_QUOTED) "./*/*.obj"  $(CPP_DEPS_QUOTED) $(CC_DEPS_QUOTED) $(S_UPPER_DEPS_QUOTED) $(CXX_DEPS_QUOTED) $(ASM_DEPS_QUOTED) $(ASM_UPPER_DEPS_QUOTED) $(C_UPPER_DEPS_QUOTED) $(C++_DEPS_QUOTED) $(EXECUTABLES_QUOTED) "./*/*.d"  $(ProjDirPath)/out/aiop_app.elf
	-@echo ' '

.PHONY: all clean dependents explicit-dependencies warning-messages
.SECONDARY:
$(OUT)/aiopsl.a:

-include ../makefile.targets

explicit-dependencies: $(CW_FILES)/aiop_link.lcf 

warning-messages: 

