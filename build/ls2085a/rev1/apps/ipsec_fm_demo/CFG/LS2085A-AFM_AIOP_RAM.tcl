set startAddress [evaluate __sys_start]

# chain position for the first AIOP core
set firstCore		 		27
# chain position for the last  AIOP core
set lastCore			  	42
# ccs index for PC
set pcReg					4106
# ccs index for SP
set spReg					8193
# Invalid value for the SP register
set spInitialValue 			0xf

# AIOP Boot Release Request Register (ABRR)
mem 0x02080090 = 0xffff0000

# set PC reg for all AIOP cores, less the first core 
for {set core [expr $firstCore + 1]} {$core<=$lastCore} {incr core} {
   protocol ccs::write_reg $core $pcReg 1 4 $startAddress
}

# invalidate SP register for all AIOP cores
for {set core $firstCore} {$core<=$lastCore} {incr core} {
   protocol ccs::write_reg $core $spReg 1 4 $spInitialValue
}
