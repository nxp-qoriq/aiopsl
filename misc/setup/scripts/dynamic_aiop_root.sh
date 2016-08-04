#!/usr/bin/env bash
#/*
# * Copyright (c) 2015-2016 Freescale Semiconductor, Inc. All rights reserved.
# *
# */

# Script for AIOP Reference Applications
# Compatible with: control_flow
#
# This scripts takes no arguments.
# This script performs the following operations:
# 1. Create an AIOP DPRC
#    AIOP DPRC contains 4 DPBPs, 2 DPNIs
# 2. Create an AIOP Tool DPRC
#    AIOP Tool DPRC contains 1 DPAIOP and 1 DPMCP
#
# DPNIs of AIOP DPRC will be connected according to:
# 1 DPNI <-----> DPMAC.1
# 1 DPNI <-----> 1 DPNI from Root Container, available in Linux
#
# NOTE: DPNI from root container will be disconnected from it's endpoint
#
# Using this script:
# 1. Run the script to create containers mentioned above
# 2. Run AIOP Tool for loading the ELF for AIOP Reference Application image on AIOP (AIOP Tool DPRC)
# which you want to start
# 3. Check AIOP log for succesfull load: cat /dev/fsl_aiop_console
#
# Assumptions:
#  - The RDB board was pre-loaded with dpl-eth.0x2A_0x41.dtb used for simple ETH scenarios
#  - This script assumes that enough resources are available to create the
#    three DPRCs
#  - no DPAIOP exists in any of the existing DPRCs
#  - 'restool' binary is installed in searchable binary path;
#    This is designed for 'restool' version compatible with MC v9.0.2 and v9.0.3

############################################################################
# Globals
############################################################################
DEBUG=0
INFO=1

ac_DPRC=
atc_DPRC=

DPMAC1="dpmac.1"
ROOTDPNI="dpni.0"

############################################################################
# Logging Routines
############################################################################
log_debug()
{
	if [ $DEBUG -eq 1 ]
	then
		echo $@
	fi
}

log_info()
{
	if [ $INFO -eq 1 ]
	then
		echo $@
	fi
}

log_error()
{
	echo $@
	exit 1
}

############################################################################
# Helper Routines
############################################################################
sleep_fn()
{
	sleep 1
}

check_error_and_exit()
{
	if [ $1 -ne 0 ]
	then
		log_error "Failure to execute last command. Unable to continue!"
	else
		sleep_fn
	fi
}

perform_vfio_mapping()
{
	# Assuming argument has DPRC to bind
	log_info "Performing vfio mapping for $1"
	if [ "$1" == "" ]
	then
		log_debug "Incorrect usage: pass DPRC to bind"
	else
		if [ -e /sys/module/vfio_iommu_type1 ];
		then
			echo 1 > /sys/module/vfio_iommu_type1/parameters/allow_unsafe_interrupts
		else
			echo "NO VFIO Support available"
			exit
		fi
		echo vfio-fsl-mc > /sys/bus/fsl-mc/devices/$1/driver_override
		if [ $? -ne 0 ]
		then
			log_debug "No such DPRC (/sys/bus/fsl-mc/devices/ \
					$1/driver_override) exists."
			return
		fi
		echo $1 > /sys/bus/fsl-mc/drivers/vfio-fsl-mc/bind
	fi
}

#
# Core command to interface with restool
# has following format:
# restool_cmd <cmd line, without 'restool'> <return variable | None> <target dprc | None>
#  - cmd line should be without restool command itself. This is to make it flexible for
#    testing purpose (replace restool without 'echo', for e.g.)
#  - return variable is a pass by reference of a global which contains return value of
#    restool execution, for example, dprc.1. This can be useful if the caller needs the
#    object created for some future command
#  - target dprc, when passed, would assign the object created to that dprc
restool_cmd()
{
	if [ $# -ne 3 ]
	then
		# Wrong usage
		log_info "Wrong usage: <$@> : Missing args"
		log_error "Should be: restool_cmd <cmd line> <return | None> <target dprc | None>"
	fi

	local _var=''
	local _object=''
	local _cmdline=$1
	local _assignRes=

	log_debug "Executing: $_cmdline"
	_var=$(restool $_cmdline)
	check_error_and_exit $?

	# Assgining to passed variable
	_object=$(echo ${_var} | head -1 | cut -d ' ' -f 1)
	if [ "$2" != "None" ]
	then
		eval "$2=$(echo ${_var} | head -1 | cut -d ' ' -f 1)"
		log_debug "Created Object: $_object"
	fi

	# Assigning if target dprc is not None
	if [ "$3" != "None" ]
	then
		# Assigining to target dprc
		_assignRes=$(restool dprc assign dprc.1 --child=${!3} --object=$_object --plugged=1)
		log_info "Assigned $_object to ${!3}"
		check_error_and_exit $?
	fi
}

############################################################################
# Container creation Routines
############################################################################
create_aiop_container()
{
	log_debug "Creating AIOP Container"
	ac_DPRC=
	DPRC_OPTIONS=
	restool_cmd "dprc create dprc.1 --options=DPRC_CFG_OPT_TOPOLOGY_CHANGES_ALLOWED,DPRC_CFG_OPT_SPAWN_ALLOWED,DPRC_CFG_OPT_ALLOC_ALLOWED,DPRC_CFG_OPT_AIOP,DPRC_CFG_OPT_OBJ_CREATE_ALLOWED,DPRC_CFG_OPT_IRQ_CFG_ALLOWED" ac_DPRC None

	log_debug "Creating DPBP1: restool dpbp create"
	restool_cmd "dpbp create" None ac_DPRC

	log_debug "Creating DPBP2: restool dpbp create"
	restool_cmd "dpbp create" None ac_DPRC

	log_debug "Creating DPBP3: restool dpbp create"
	restool_cmd "dpbp create" None ac_DPRC

	log_debug "Creating DPBP4: restool dpbp create"
	restool_cmd "dpbp create" None ac_DPRC

	ACTUAL_MAC="00:00:00:00:00:06"
	MAX_SENDERS=8
	DPNI_OPTIONS="DPNI_OPT_MULTICAST_FILTER,DPNI_OPT_UNICAST_FILTER,DPNI_OPT_TX_CONF_DISABLED,DPNI_OPT_DIST_HASH"
	MAX_TCS=1
	MAX_DIST_PER_TC=8
	MAX_DIST_KEY_SIZE=4
	DPCON_PRIORITIES=8
	DPIO_PRIORITIES=8
	DPCI_PRIORITIES=2

	log_debug "Creating DPNI"
	restool_cmd "dpni create --mac-addr=$ACTUAL_MAC \
				--max-senders=$MAX_SENDERS \
				--options=$DPNI_OPTIONS \
				--max-tcs=$MAX_TCS \
				--max-dist-per-tc=$MAX_DIST_PER_TC \
				--max-dist-key-size=$MAX_DIST_KEY_SIZE \
				" atc_DPNI1 ac_DPRC

	log_info "Connecting $atc_DPNI1<------->$DPMAC1"
	restool_cmd "dprc connect dprc.1 --endpoint1=$atc_DPNI1 --endpoint2=$DPMAC1" None None

	ACTUAL_MAC="00:00:00:00:00:0A"
	MAX_SENDERS=8
	DPNI_OPTIONS="DPNI_OPT_MULTICAST_FILTER,DPNI_OPT_UNICAST_FILTER,DPNI_OPT_TX_CONF_DISABLED,DPNI_OPT_DIST_HASH"
	MAX_TCS=1
	MAX_DIST_PER_TC=8
	MAX_DIST_KEY_SIZE=4
	DPCON_PRIORITIES=8
	DPIO_PRIORITIES=8
	DPCI_PRIORITIES=2

	log_debug "Creating DPNI"
	restool_cmd "dpni create --mac-addr=$ACTUAL_MAC \
	 			--max-senders=$MAX_SENDERS \
	 			--options=$DPNI_OPTIONS \
	 			--max-tcs=$MAX_TCS \
	 			--max-dist-per-tc=$MAX_DIST_PER_TC \
	 			--max-dist-key-size=$MAX_DIST_KEY_SIZE \
	 			" atc_DPNI2 ac_DPRC
	log_info "Connecting $atc_DPNI2<------->$ROOTDPNI"
	restool_cmd "dprc connect dprc.1 --endpoint1=$atc_DPNI2 --endpoint2=$ROOTDPNI" None None

	echo "AIOP Container $ac_DPRC created"
} # AIOP Container

create_aiopt_container()
{
	log_debug "Creating AIOP Tool Container"
	atc_DPRC=
	restool_cmd "dprc create dprc.1 --options=DPRC_CFG_OPT_SPAWN_ALLOWED,DPRC_CFG_OPT_ALLOC_ALLOWED,DPRC_CFG_OPT_IRQ_CFG_ALLOWED" atc_DPRC None

	# Check if the AIOP Tool Container exists or not
	if [ x"$ac_DPRC" == x"" ]
	then
		log_error "AIOP Container doesn't exist"
	fi
	log_debug "Creating DPAIOP Object"
	restool_cmd "dpaiop create --aiop-id=0 --aiop-container=$ac_DPRC" None atc_DPRC

	log_debug "Creating DPMCP Object"
	restool_cmd "dpmcp create" None atc_DPRC

	echo "AIOP Tool Container $atc_DPRC created"
}

main()
{
	restool_cmd "dprc disconnect dprc.1 --endpoint=$ROOTDPNI" None None

	log_info "Creating AIOP Container"
	create_aiop_container
	log_info "----- Contents of AIOP Container: $ac_DPRC -----"
	restool dprc show $ac_DPRC
	log_info "-----"
	echo "===================================================="

	log_info "Creating AIOP Tool Container"
	create_aiopt_container
	log_info "----- Contents of AIOP Tool Container: $atc_DPRC -----"
	restool dprc show $atc_DPRC
	log_info "-----"
	echo "===================================================="

	log_info "Performing VFIO mapping for AIOP Tool Container ($atc_DPRC)"
	sleep_fn
	perform_vfio_mapping $atc_DPRC

	echo "========== Summary ================================="
	echo " AIOP Container: $ac_DPRC"
	echo " AIOP Tool Container: $atc_DPRC"
	echo "===================================================="
}

main
