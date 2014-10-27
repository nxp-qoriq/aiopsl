#! /usr/bin/python

import os
import fnmatch
import re



def check_if_file_exists(filename):
	return os.path.isfile(filename)


if __name__ == "__main__":
	
	print "Start script to check stack estimation in AIPO"

	if check_if_file_exists("build/aiop_sim/test/aiopsl_stack_estimation/out/aiop_app.MAP") == False:
		exit(1)


	ins = open( "build/aiop_sim/test/aiopsl_stack_estimation/out/aiop_app.MAP", "r" )

	append_line = False
	counter = 0
	aiposl_stack_estimation = []
	for line in ins:
		if "ESTIMATED STACK USAGE" in line:
			append_line = True
		
		if append_line:
			if "---------------------" in line:
				counter = counter + 1
			aiposl_stack_estimation.append( line )
		if counter == 2:
			break
	ins.close()

	print "[INFO]: Stack estimation data:"
	for line in aiposl_stack_estimation:
		print "[INFO]:" + line


	if counter != 2:
		print "Stack estimation was unsuccessful"
		exit(1)
	else:
		print "Finished SUCCESSFULLY"
		exit(0)


