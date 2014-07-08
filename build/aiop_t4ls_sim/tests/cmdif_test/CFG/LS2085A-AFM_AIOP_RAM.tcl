set startAddress [evaluate __start]

# chain position for the first AIOP core
set firstCore		 		27
# chain position for the last  AIOP core
set lastCore			  	42
# ccs index for PC
set pcReg					2032
# ccs index for SP
set spReg					2001
# Invalid value for the SP register
set spInitialValue 			0xf

# set PC reg for all AIOP cores, less the first core 
for {set core [expr $firstCore + 1]} {$core<=$lastCore} {incr core} {
   protocol ccs::write_reg $core $pcReg 1 4 $startAddress
}

# invalidate SP register for all AIOP cores
for {set core $firstCore} {$core<=$lastCore} {incr core} {
   protocol ccs::write_reg $core $spReg 1 4 $spInitialValue
}
