#! /usr/bin/python

import os
import fnmatch

DEBUG = False

def check_if_file_exists(filename):
	return os.path.isfile(filename)

def check_if_flags_different(app_process_packet, cproject_file):
	temp_file = []
	ins = open( cproject_file, "r" )
	for line in ins:
		if "option id" in line:
			temp_file.append( line )
	ins.close()

	different_flag = False 

	print "Checking project: " + cproject_file + " for identical flags with app process packet."

	if str(len(app_process_packet)) != str(len(temp_file)):
		print "app process packet and project:" + cproject_file + " have different number of flags"
		different_flag = True

	same_flag = False

	for line in app_process_packet:
		same_flag = False
		for line2 in temp_file:
			if line == line2:
				same_flag = True

		if same_flag == False:
			print "Flag not match: " + line
			different_flag = True

	if different_flag:
		print "Project flags are different (app process packet, " + cproject_file + ")"
	return different_flag










if __name__ == "__main__":

	different_flags_in_some_projects = False
	print "Start script to compare all .cproject files for applications and tests in AIOP"

	if check_if_file_exists("build/aiop_sim/apps/app_process_packet/.cproject") == False:
		exit(1)


	ins = open( "build/aiop_sim/apps/app_process_packet/.cproject", "r" )
	app_process_packet = []
	for line in ins:
		if "option id" in line:
			app_process_packet.append( line )
	ins.close()


	print "Number of flags found in app process packet -\".cproject\": " + str(len(app_process_packet))

	matches = []

	for root, dirnames, filenames in os.walk('build/aiop_sim'):
		for filename in fnmatch.filter(filenames, '*.cproject'):
			matches.append(os.path.join(root, filename))


	for line in matches:
		if "aiopsl" not in line and "app_process_packet" not in line:
			print "==============================================================================================="
			print "=*********************************************************************************************="
			print "==============================================================================================="
			if check_if_flags_different(app_process_packet, line):
				different_flags_in_some_projects = True

	if different_flags_in_some_projects:
		print "Different flags found during the test."
		exit(1)
	else:
		exit(0)



	if DEBUG:
		for line in matches:
			print line
		for line in app_process_packet:
			print line

