# This configuration script is compatible with T4240LS_MC devices and
# performs minimal intialization like ECC SRAM, IVORs.
#
# MMU: single 4GB addrss space, VLE
# Init SRAM ECC
# Disable Watchdog Timer
# Init IPVR and IVORx

######################################
# Initialize target variables
######################################
# SRAM memory sizes for supported devices.
# MPC564xL, where x  = 3
# Supported memory LSM/DPM configuration(s):
# (128K/64K) 0x20000/0x10000
set mem_size(3) 0x20000
set current_cfg 3

######################################
## Register group definitions
######################################

# GPR registrer group
set GPR_GROUP "General Purpose Registers/"
# special purpose register group
set SPR_GROUP "e200z4 Special Purpose Registers/"
#TLB1 registers group
set TLB1_GROUP "regPPCTLB1/"

###########################################################
## init core z4 registers for LockStep mode required
## to run SRAM initialization program and general debugging
###########################################################
proc init_lsm_regs {} {
	global GPR_GROUP
	global SPR_GROUP

	puts "initialize core registers for LockStep mode"	
	# init r0-r31
	reg ${GPR_GROUP}GPR0 = 0x0
	# init group r2-r31
	reg ${GPR_GROUP}GPR2..${GPR_GROUP}GPR31 = 0x0
	# reset watchdog timer
	reg ${SPR_GROUP}SPRG0..${SPR_GROUP}SPRG9 = 0x0    
	reg ${SPR_GROUP}USPRG0 = 0x0
	reg ${SPR_GROUP}SRR0 = 0x0
	reg ${SPR_GROUP}SRR1 = 0x0
	reg ${SPR_GROUP}CSRR0 = 0x0
	reg ${SPR_GROUP}CSRR1 = 0x0
	reg ${SPR_GROUP}MCSRR0 = 0x0
	reg ${SPR_GROUP}MCSRR1 = 0x0
	reg ${SPR_GROUP}DSRR0 = 0x0
	reg ${SPR_GROUP}DSRR1 = 0x0
	reg ${SPR_GROUP}ESR = 0x0
	reg ${SPR_GROUP}SPEFSCR = 0x0
	reg ${GPR_GROUP}XER = 0x0
	reg ${SPR_GROUP}MCAR = 0x0
	reg ${SPR_GROUP}TBL = 0x0
	reg ${SPR_GROUP}TBU = 0x0
	reg ${SPR_GROUP}DVC1 = 0x0
	reg ${SPR_GROUP}DVC2 = 0x0
	reg ${SPR_GROUP}DBCNT = 0x0	    
}

################################################
# Initialize the core registers: reset timers
# set SPE support and set IVORs.
################################################
proc init_z4 {ivpr} {
	global GPR_GROUP
	global SPR_GROUP

   	# reset watch dog timer
	reg ${SPR_GROUP}TCR = 0x0

	# Set up all interrupt vectors
	reg ${SPR_GROUP}IVPR = $ivpr
	
	for {set i 0} {$i < 15} {incr i} {
		reg ${SPR_GROUP}IVOR${i} %d = [expr $i * 0x100]
	}
	reg ${SPR_GROUP}IVOR32 = 0x1000
	reg ${SPR_GROUP}IVOR33 = 0x1100
	reg ${SPR_GROUP}IVOR34 = 0x1200
	
    # set SP to an unaligned (4bytes) to avoid creating
	# invalid stack frames
    reg ${GPR_GROUP}SP = 0x3

    # enable SPE and Debug interrupts
	reg ${SPR_GROUP}MSR = 0x2002000
}

################################################
## Configures MMU for z4 core.
################################################
proc init_z4_MMU {} {
	global TLB1_GROUP
	# Setup MMU for entire 4GB address space
	# Base address = 0x0000_0000
	# TLB0, 4 GByte Memory Space, Not Guarded, Cache inhibited, All Access
	# VLE page
	reg ${TLB1_GROUP}MMU_CAM0 = 0xB0000008FE0800000000000000000001
}
#################################################
# Initialize a RAM 'range' from 'start' address,
# downloading the init program at 0x4000_0000.
#################################################
proc init_ram {start range} {
  	global GPR_GROUP

    puts "init ECC SRAM $start:$range"
  	# default SRAM address
  	set pstart 0x40000000
  	# the offset from the given start address
  	# at which the init prgram starts to init SRAM
	set offset 0x0
    # stmw write page size = 128 = 4bytes * 32 GPRS
    set psize 0x80

	if {$start == $pstart} {
		# init first 4 bytes (mem access) x 128 = 512
		# bytes to avoid reading the memory around PC
		# after stopping the core
    	mem $start 256 = 0x0
		# base init address
		set offset 0x80
	}
	
	# address to start initialization
    set start [expr {$start + $offset}]
    
	# load add into GPR
    reg ${GPR_GROUP}GPR11 %d = $start

    # compute actual times stmw is called
    # and set counter
    set c [expr ((($range - $offset)/$psize))]
    reg ${GPR_GROUP}GPR12 %d = $c

    # execute init ram code
    #mtctr r12
    mem $pstart = 0x7D8903A6
    #stmw r0,0(r11)
    mem [format %x [expr $pstart + 0x4]] = 0x180B0900
    #addi r11,r11,128
    mem [format %x [expr $pstart + 0x8]] = 0x1D6B0080
    #bdnz -8
    mem [format %x [expr $pstart + 0xc]] = 0x7A20FFF8
    # infinte loop
    #se_b *+0
    mem [format %x [expr $pstart + 0x10]] = 0xE8000000
    
    # set PC to the first init instruction
    reg ${GPR_GROUP}PC = $pstart
    # execute init ram code
    # timeout 1 second to allow the code to execute
    go 1
    stop
}


proc T4240LS_init {} {
  	global GPR_GROUP
  	global current_cfg
  	global mem_size
  	global lsm_dpm
  	
  	puts "Start initializing the device ..."
	reset hard
			
	# Explicitly stop Core0
	stop
	
	puts "initialize Core0"
	init_z4 0x40000000
	puts "initialize Core0 MMU"
	init_z4_MMU
	
	# Disable Watchdog Timer
	mem 0xFFF38010 = 0x0000C520
	mem 0xFFF38010 = 0x0000D928
	mem 0xFFF38000 = 0x8000010A

	# initialize ECC SRAM for DPM Core0/1 
	set mrange  [expr $mem_size($current_cfg)/2]

	init_ram 0x40000000 $mrange

	puts "Finish initializing the device."
}

########################################
## Environment Setup: 32bit meme access
## or numbers start with hex notation.
########################################
proc envsetup {} {
	radix x 
	config hexprefix 0x
	config MemIdentifier v 
	config MemWidth 32 
	config MemAccess 32 
	config MemSwap off
}
 
# env setup
envsetup

# main entry
T4240LS_init
