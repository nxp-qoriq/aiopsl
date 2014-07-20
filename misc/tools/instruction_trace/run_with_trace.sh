#!/bin/bash
#run simulator

SIM_PORT=${1:-42828}
export SIMAPI_VIPR_STARTUP_PYTHON_SCRIPTS=trace.py && ./start_ccssim2 -port $SIM_PORT -imodel "ls_sim_init_file=ls2085a_sim_init_params.cfg" -smodel "ls_sim_config_file=ls2085a_sys_test.cfg"         
