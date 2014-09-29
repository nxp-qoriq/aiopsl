#! /usr/bin/python

import os
import fnmatch
import re

DEBUG = False
DIFFERENT_FLAGS_FILE = "different_flags_found.properties"
TOTAL_CPROJETCTS_FILE = "cprojects_found.properties"

def check_if_file_exists(filename):
	return os.path.isfile(filename)

def check_if_flags_different(app_process_packet, cproject_file):
	temp_file = []
	ins = open( cproject_file, "r" )
	for line in ins:
		if "superClass=" in line:
			l = re.compile("superClass=").split(line)
			if len(l) == 2:
				temp_file.append( "<" + l[1] )
	ins.close()

	different_flag = False 

	print "Checking project: " + cproject_file + " for identical flags with app process packet."

	if str(len(app_process_packet)) != str(len(temp_file)):
		print "app process packet and project:" + cproject_file + " have different number of flags"
		different_flag = True

	same_flag = False
	found_wrong_flag = False
	for line in app_process_packet:
		same_flag = False
		l = re.compile("\"").split(line)
		for line2 in temp_file:
			if line == line2:
				same_flag = True

		if same_flag == False:			
			for line3 in temp_file:
				if "\"" + l[1] +"\"" in line3:
					found_wrong_flag = True
					break
			if found_wrong_flag:
				print "[ERROR]: Wrong flag found: " + line3 + "\nShould be:\n" + line 
			else:
				print "[ERROR]: Flag not found: " + line 

			different_flag = True
			found_wrong_flag = False

	#search for dlags that are not in app process packet		
	for line in temp_file:
		same_flag = False
		l = re.compile("\"").split(line)
		for line2 in app_process_packet:
			if "\"" + l[1] +"\"" in line2:
				same_flag = True

		if same_flag == False:
			print "[ERROR]: Wrong flag found (missing in app_process_packet): " + line
			different_flag = True


	if different_flag:
		print "[INFO]: Project flags are different (app process packet, " + cproject_file + ")"
	else:
		print "[INFO]: Project flags are identical for app process packet and " + cproject_file + "."
	return different_flag










if __name__ == "__main__":

	different_flags_in_some_projects = False
	print "Start script to compare all .cproject files for applications and tests in AIOP"

	if check_if_file_exists("build/aiop_sim/apps/app_process_packet/.cproject") == False:
		exit(1)


	ins = open( "build/aiop_sim/apps/app_process_packet/.cproject", "r" )
	app_process_packet = []
	for line in ins:
		if "superClass=" in line:
			l = re.compile("superClass=").split(line)
			if len(l) == 2:
				app_process_packet.append( "<" + l[1] )
	ins.close()

	print "[INFO]: Flags found in app_process_packet:"
	for line in app_process_packet:
		print "[INFO]:" + line


	print "[INFO]: Number of flags found in app process packet -\".cproject\": " + str(len(app_process_packet)) + "\n\n"
	matches = []

	total_cprojects_checked = 0
	different_cprojects_found = 0

	for root, dirnames, filenames in os.walk('build/aiop_sim'):
		for filename in fnmatch.filter(filenames, '*.cproject'):
			matches.append(os.path.join(root, filename))


	for line in matches:
		if "app_process_packet" not in line:
			total_cprojects_checked += 1
			print "==============================================================================================="
			print "=*********************************************************************************************="
			print "==============================================================================================="
			if check_if_flags_different(app_process_packet, line):
				different_flags_in_some_projects = True
				different_cprojects_found += 1

	print "[INFO]: Total cprojects checked " + str(total_cprojects_checked)
	f = open(TOTAL_CPROJETCTS_FILE,'w')
	f.write('YVALUE=' + str(total_cprojects_checked) + '\n')
	f.close()

	if different_flags_in_some_projects:
		print "[INFO]: " + str(different_cprojects_found) + " different cprojects found during the test."
		f = open(DIFFERENT_FLAGS_FILE,'w')
		f.write('YVALUE=' + str(different_cprojects_found) + '\n')
		f.close()
		exit(1)
	else:

		exit(0)



	if DEBUG:
		for line in matches:
			print line
		for line in app_process_packet:
			print line

