#! /usr/bin/python

import os
import fnmatch
import re



def check_if_file_exists(filename):
	return os.path.isfile(filename)

def check_stack(aiopsl_stack_estimation):
	global error_stack_too_big
	enable = False
	func_stack_sizes = []
	func_list = []
	for line in aiopsl_stack_estimation:
		if "Initial Stack Size" in line:
			enable = True
		if enable:
			l = re.compile("<").split(line)
			if len(l) > 1:
				s = re.compile(">").split(l[2])
				func_stack_sizes.append(s[0])
				func_list.append(l[0] + " - " + s[0])
				if int(s[0]) > 300 and "stack_estimation" not in l[0]:
					print "[ERROR]: stack is bigger then 300 for function: " + l[0] + "(Stack size: " + s[0] + ")"
					error_stack_too_big = True

#	for line in func_list:
#		print line



if __name__ == "__main__":
	global error_stack_too_big
	error_stack_too_big = False
	print "Start script to check stack estimation in AIOP"

	if check_if_file_exists("build/aiop_sim/tests/aiopsl_stack_estimation/out/aiop_app.MAP") == False:
		exit(1)


	ins = open( "build/aiop_sim/tests/aiopsl_stack_estimation/out/aiop_app.MAP", "r" )

	append_line = False
	counter = 0
	aiopsl_stack_estimation = []
	for line in ins:
		if "ESTIMATED STACK USAGE" in line:
			append_line = True
		
		if append_line:
			if "---------------------" in line:
				counter = counter + 1
			aiopsl_stack_estimation.append( line )
		if counter == 2:
			break
	ins.close()

	print "[INFO]: Stack estimation data:"
	for line in aiopsl_stack_estimation:
		print "[INFO]:" + line


	if counter != 2:
		print "Stack estimation was unsuccessful"
		exit(1)
	else:
		check_stack(aiopsl_stack_estimation)
		if 	error_stack_too_big:
			print "Function with too big stack usage found during the test"
			exit(1)
		print "Finished SUCCESSFULLY"
		exit(0)


