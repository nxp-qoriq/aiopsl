#! /usr/bin/python

import os
from shutil import move
from os import remove, close
import fnmatch
import re
import sys


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

def stack():
	global error_stack_too_big
	error_stack_too_big = False
	print "Start script to check stack estimation in AIOP"

	if check_if_file_exists("build/ls2085a/rev1/tests/aiopsl_stack_estimation/out/aiop_app.MAP") == False:
		exit(1)


	ins = open( "build/ls2085a/rev1/tests/aiopsl_stack_estimation/out/aiop_app.MAP", "r" )

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

#define() is used to add defines before the compilation of stack estimation and the aiopsl
def defines():
	orig_file = "build/build_flags/build_flags.h"
	new_file = "build/build_flags/build_flags_new.h"
	if check_if_file_exists(orig_file) == False:
		exit(1)

	ins = open(orig_file, "r")
	temp_file = open(new_file, "w")

	for line in ins:
		if '#endif /* __BUILD_FLAGS_H */' not in line:
			temp_file.write(line)
		else:
			temp_file.write("#define STACK_CHECK\n")
			print "Added: #define STACK_CHECK"
			temp_file.write("#define DISABLE_ASSERTIONS\n")
			print "Added: #define DISABLE_ASSERTIONS"
			temp_file.write(line)
			ins.close()
			temp_file.close()
			remove(orig_file)
			move(new_file,orig_file)
			print "file was updated with new defines SUCCESSFULLY"
			exit(0)


if __name__ == "__main__":

	if len(sys.argv) < 1:
		print "Incorrect number of arguments"
		exit(1)

	arg1 = str(sys.argv[1:])

	if arg1 == "['stack_check']":
		stack() #jump to stack checking part
	elif arg1 == "['add_defines']":
		defines() #jump to add defines
	elif arg1 == "['-h']":
		print "python jenkins_aiop_stack_estimation.py [stack_check|add_defines]"
		exit(2)
	else:
		print "Wrong parameter " + arg1
		print "python jenkins_aiop_stack_estimation.py [-h|stack_check|add_defines]"
		exit(1)
