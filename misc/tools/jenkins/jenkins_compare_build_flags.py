#! /usr/bin/python

import os
import fnmatch
import re
import sys, getopt

global DEBUG
global BUILD
DEBUG = False
BUILD = False
DIFFERENT_FLAGS_FILE = "different_flags_found.properties"
TOTAL_CPROJETCTS_FILE = "cprojects_found.properties"

def check_if_file_exists(filename):
	return os.path.isfile(filename)


def compile_projects(matches):
	global g_arch
	global g_cw
	flags = ''
	cw_path = 'c:\\Freescale\\' + str(g_cw) + '\\eclipse\\ecd.exe'
	workspace = os.getcwd()

	if BUILD:
		flags = ' -build -cleanAll -cleanBuild'
	else:
		flags = ' -generateMakefiles '
	
	if DEBUG:
		flags = flags + ' -verbose '
	
	for line in matches:
		command = cw_path + flags + '-data \"'+ workspace + '\"' + ' -project ' + line
		if DEBUG:
			print "compilation command:\n" + command
		error = os.system(command)

		if error != 0:
			if DEBUG:
				print "compilation failed for: " + line
				print "[ERROR]: " + str(error)
			exit(error)

def parse_flags(filename):
	flags_array = []

	if check_if_file_exists(filename) == False:
		if DEBUG:
			print 'The following file is missing:\n' + filename
		exit(1)

	with open(filename, 'r+') as f:
		for line in f:
			line = line.rstrip('\r\n')
			if DEBUG:
				print line
			if '-i' == line:
				if DEBUG:
					print 'this line and the one after should be removed'
				next(f)
				continue
			if line.startswith( '-I"$' ):
				continue
			if '-I-' == line:
				continue
			flags_array.append(line)
		f.close()

	return flags_array


def compare_flags(matches):
	global g_ignore
	global DEBUG
	aiopsl = []
	app_process_packet = []
	temp = []
	compare_error = 0
	different_projects_found = 0
	filename = ''
	ignore_flag = False
	ignore_flags_array = []

	if g_ignore != '':
		ignore_flag = True
		ignore_flags_array = parse_flags(g_ignore)

		if DEBUG == False:
			print "Ignore flags"
			for line in ignore_flags_array:
				print line

	for line in matches:
		if "aiopsl" in line:
			filename = line + '\\out\\cw_files\\start.args'
			aiopsl = parse_flags(filename)
			continue
		if "app_process_packet" in line:
			filename = line + '\\out\\src\\config.args'
			app_process_packet = parse_flags(filename)
			continue

	print "==============================================================================================="
	print "=*********************************************************************************************="
	print "==============================================================================================="
	print "Compare aiopsl and app_process_packet:"
	if DEBUG:
		print "aiopsl flags:"
		for line in aiopsl:
			print line

		print "app_process_packet flags:"
		for line in app_process_packet:
			print line

	if app_process_packet == aiopsl:
		print "[INFO]: Flags are equal"
	else:
		for line in app_process_packet:
			same_flag = False
			for line2 in aiopsl:
				if line == line2:
					same_flag = True

			if same_flag == False:			
				print "[ERROR]: Flag: \"" + line + "\" not found in aiopsl." 
				compare_error |= 1

		for line in aiopsl:
			same_flag = False
			for line2 in app_process_packet:
				if line == line2:
					same_flag = True

			if same_flag == False:			
				print "[ERROR]: Wrong flag \"" + line + "\" in aiopsl." 
				compare_error |= 1
		if compare_error != 0:
			different_projects_found += 1
		else: #flags order maybe different and failed first compare
			print "[INFO]: Flags are equal"



	for line in matches:
		if "aiopsl" not in line and "app_process_packet" not in line:
			for root, dirnames, filenames in os.walk(line):
				for filename in fnmatch.filter(filenames, 'config.args'):
					filename = os.path.join(root, filename)
					break
			print filename
			temp = parse_flags(filename)
			project = line.split('\\')[-1]
			error = 0
			print "==============================================================================================="
			print "=*********************************************************************************************="
			print "==============================================================================================="
			print "Compare app_process_packet and " + project + ":"
			if app_process_packet == temp:
				print "[INFO]: Flags are equal"
			else:
				for line in app_process_packet:
					same_flag = False
					for line2 in temp:
						if line == line2:
							same_flag = True

					if same_flag == False:
						if ignore_flag:
							if line in ignore_flags_array:
								print "[INFO]: Ignore flag: " + line
								continue
						print "[ERROR]: Flag: \"" + line + "\" not found in " + project + "." 
						compare_error |= 1
						error = 1

				for line in temp:
					same_flag = False
					for line2 in app_process_packet:
						if line == line2:
							same_flag = True

					if same_flag == False:
						if ignore_flag:
							if line in ignore_flags_array:
								print "[INFO]: Ignore flag: " + line
								continue
						print "[ERROR]: Wrong flag \"" + line + "\" in " + project + "." 
						compare_error |= 1
						error = 1
				if error == 1:
					different_projects_found += 1
				else: #flags order maybe different and failed first compare
					print "[INFO]: Flags are equal"


	if compare_error == 0:
		print "Finished SUCCESSFULLY"
		exit(0)
	else:
		print "[INFO]: " + str(different_projects_found) + " different projects found during the test."
		f = open(DIFFERENT_FLAGS_FILE,'w')
		f.write('YVALUE=' + str(different_projects_found) + '\n')
		f.close()
		exit(1)
	


def main(argv):
	global DEBUG
	global BUILD
	global g_arch
	global g_cw
	global g_rev
	global g_ignore
	print "Start script to compare all compilation flags for applications and tests in AIOP"

	if DEBUG:
		print 'Run script in debug mode'

	if len(sys.argv) <= 1:
		print "Incorrect number of arguments"
		exit(1)

	g_arch = ''
	g_cw = ''
	g_rev =''
	g_ignore = ''

	try:
		opts, args = getopt.getopt(argv,"hvbd:c:r:i:",["device=, codewarrior=, revision=, ignore=, verbose=, build="])
	except getopt.GetoptError:
		print 'jenkins_flags_compare.py -d [device] -c [codewarrior version] -r [revision]'
		exit(2)
	for opt, arg in opts:
		if opt == '-h':
			print 'jenkins_flags_compare.py -d [device] -c [codewarrior version] -r [revision] -i [ignore flags file] -v <debug enabled> -b <build> '
			exit()
		elif opt in ("-d", "--device"):
			g_arch = arg
		elif opt in ("-c", "--codewarrior"):
			g_cw = arg
		elif opt in ("-r", "--revision"):
			g_rev = arg
		elif opt in ("-i", "--ignore"):
			g_ignore = arg
		elif opt in ("-v", "--verbose"):
			DEBUG = True
			print "Debug mode enabled"
		elif opt in ("-b", " --build"):
			BUILD = True
			print "Build projects"


	if g_arch == '':
		print 'device input missing, run -h for help'
		exit(2)
	if g_cw == '':
		print 'codewarrior input missing, run -h for help'
		exit(2)
	if g_rev == '':
		print 'revision input missing, run -h for help'
		exit(2)
	if g_ignore != '':
		if check_if_file_exists(g_ignore) == False: #check if ignore flags file exists
			print "[ERROR]: File not found: " + str(g_ignore)
			exit(1)
		else:
			print "Ignore flags file: " + str(g_ignore) + " exists."

	if BUILD == False and DEBUG:
		print "Only make files will be generated."

	if check_if_file_exists("build\\"+str(g_arch)+"\\"+str(g_rev)+"\\aiopsl\\.cproject") == False:
		exit(1)

	#find number of projects
	#matched - an array to all .cproject paths found in aiopsl.
	matches = []
	total_cprojects_found = 0

	for root, dirnames, filenames in os.walk('build\\'+str(g_arch)+'\\'+str(g_rev)+'\\aiopsl'):
		for filename in fnmatch.filter(filenames, '*.cproject'):
			matches.append(os.path.dirname(os.path.join(root, filename)))
			total_cprojects_found += 1
	for root, dirnames, filenames in os.walk('build\\'+str(g_arch)+'\\'+str(g_rev)+'\\apps'):
		for filename in fnmatch.filter(filenames, '*.cproject'):
			matches.append(os.path.dirname(os.path.join(root, filename)))
			total_cprojects_found += 1
	for root, dirnames, filenames in os.walk('build\\'+str(g_arch)+'\\'+str(g_rev)+'\\tests'):
		for filename in fnmatch.filter(filenames, '*.cproject'):
			matches.append(os.path.dirname(os.path.join(root, filename)))
			total_cprojects_found += 1

	if DEBUG:
		print "Found " + str(total_cprojects_found) + " .cprojects for " + str(g_arch)
		print "Build AIOP projects"


	compile_projects(matches)
	compare_flags(matches)



if __name__ == "__main__":
	main(sys.argv[1:])
