#! /usr/bin/python

import os
import fnmatch
import re

DEBUG = False
DIFFERENT_FLAGS_FILE = "different_flags_found.properties"
TOTAL_CPROJETCTS_FILE = "cprojects_found.properties"


def check_if_file_exists(filename):
	return os.path.isfile(filename)
#the input of thuis script is two different .cproject to compare
def check_if_flags_different(app_process_packet, cproject_file):
	global ignore_flags
	ignore_flags_exist = False

	#flag to identify if we are checking the first target in .cproject - only first is supported
	target_1_flag = 0
	temp_file = []
	ins = open( cproject_file, "r" )
	for line in ins:
		if "cconfiguration" in line:
			target_1_flag += 1 
		if "/cconfiguration" in line:
			target_1_flag += 1

		if "superClass=" in line and target_1_flag == 1:
			l = re.compile("superClass=").split(line)
			if len(l) == 2:
				temp_file.append( "<" + l[1] )
	ins.close()

	# the comparison for .cprojects starts from here:
	different_flag = False

	if 'Null' not in ignore_flags and 'apps' not in cproject_file:
		ignore_flags_exist = True

	print "Checking project: " + cproject_file + " for identical flags with app process packet."

	if str(len(app_process_packet)) != str(len(temp_file)) and ignore_flags_exist == False:
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

		if ignore_flags_exist:
			if l[1] in ignore_flags:
				print "[INFO]: Flag ignored (exist in app_process_packet):" + line
				continue


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

	#search for flags that are not in app process packet		
	for line in temp_file:
		same_flag = False
		l = re.compile("\"").split(line)
		for line2 in app_process_packet:
			if "\"" + l[1] +"\"" in line2:
				same_flag = True

		if ignore_flags_exist:
			if l[1] in ignore_flags:
				print "[INFO]: Flag ignored (exist in aiopsl):" + line
				continue

		if same_flag == False:
			print "[ERROR]: Wrong flag found (missing in app_process_packet): " + line
			different_flag = True


	if different_flag:
		print "[INFO]: Project flags are different (app process packet, " + cproject_file + ")"
	else:
		print "[INFO]: Project flags are identical for app process packet and " + cproject_file + "."
	return different_flag










if __name__ == "__main__":
	global ignore_flags
	ignore_flags = []
	different_flags_in_some_projects = False
	print "Start script to compare all .cproject files for applications and tests in AIOP"

	if check_if_file_exists("build/ls2085a/rev1/apps/app_process_packet/.cproject") == False:
		exit(1)

	#Check if there flags to ignore for compare with aiopsl
	if check_if_file_exists("misc/tools/jenkins/ignore_cproject_flags.txt") == True:
		if os.stat("misc/tools/jenkins/ignore_cproject_flags.txt")[6] == 0:
			ignore_flags = 'Null'
		else:			
			ins = open( "misc/tools/jenkins/ignore_cproject_flags.txt", "r" )
			for line in ins:
				l = re.compile("\"").split(line)
				if len(l) > 1:
					ignore_flags.append(l[1])
				else:
					ignore_flags.append(line)
			ins.close()
			print "Ignore flags for aiopsl:"
			print ignore_flags
	else:
		ignore_flags = 'Null'

	ins = open( "build/ls2085a/rev1/apps/app_process_packet/.cproject", "r" )


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

	#matched - an array to all .cproject paths found in aiopsl.
	matches = []

	total_cprojects_checked = 0
	different_cprojects_found = 0

	for root, dirnames, filenames in os.walk('build/ls2085a/rev1'):
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
	
	if DEBUG:
		for line in matches:
			print line
		for line in app_process_packet:
			print line

	if different_flags_in_some_projects:
		print "[INFO]: " + str(different_cprojects_found) + " different cprojects found during the test."
		f = open(DIFFERENT_FLAGS_FILE,'w')
		f.write('YVALUE=' + str(different_cprojects_found) + '\n')
		f.close()
		exit(1)
	else:
		print "Finished SUCCESSFULLY"
		exit(0)


