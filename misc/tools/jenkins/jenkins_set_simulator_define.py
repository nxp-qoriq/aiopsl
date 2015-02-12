#! /usr/bin/python

import os
from shutil import move
from os import remove, close
import fnmatch
import re
import sys


def check_if_file_exists(filename):
	return os.path.isfile(filename)


#define() is used to add defines before the compilation of stack estimation and the aiopsl
def defines(build_flags_path):
	orig_file = build_flags_path + "/build_flags.h"
	new_file = build_flags_path + "/build_flags_new.h"
	if check_if_file_exists(orig_file) == False:
		print "build_flags.h not found in: " + build_flags_path
		exit(1)

	ins = open(orig_file, "r")
	temp_file = open(new_file, "w")

	for line in ins:
		if '#endif /* __BUILD_FLAGS_H */' not in line:
			temp_file.write(line)
		else:
			temp_file.write("#ifndef SIMULATOR\n")
			temp_file.write("#define SIMULATOR\n")
			temp_file.write("#endif\n")
			
			print "Added SIMULATOR macro."
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
		print "Please provide build_flags.h path."
		exit(1)

	arg1 = str(sys.argv[1:])
	l = re.compile("'").split(arg1)
	defines(l[1]) #jump to add defines
	
