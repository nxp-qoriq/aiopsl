#! /usr/bin/python

import os
import hashlib
import sys, getopt
import re

def md5(file_path):
	md5 = hashlib.md5()
	with open(file_path, 'rb', 0) as f:
		while True:
			chunk = f.read(128)
			if not chunk:
				# EOF reached, end loop
				break
			md5.update(chunk)
			# chunk is up to 128 characters long


	f.close()
	return md5.hexdigest()

def check_if_file_exists(filename):
	return os.path.isfile(filename)

def main(argv):
	global g_inputfile
	g_inputfile = ''

	try:
		opts, args = getopt.getopt(argv,"hi:",["ifile="])
	except getopt.GetoptError:
		print 'test.py -i <inputfile>'
		sys.exit(2)
	for opt, arg in opts:
		if opt == '-h':
			print 'jenkins_files_compare_<mc/arena>_aiop.py -i <inputfile>'
			sys.exit()
		elif opt in ("-i", "--ifile"):
			g_inputfile = arg

	if g_inputfile == '':
		print 'input file missing, run -h for help'
		sys.exit(2)

def  compare_files(file1, file2):
	file1_md5 = md5(file1)
	file2_md5 = md5(file2)
	if file1_md5 != file2_md5:
		print "files not equal: \n\t" + file1 + ",\n\t" + file2
		return False
	else:
		print "files equal: \n\t" + file1 + ",\n\t" + file2
		return True 



if __name__ == "__main__":
	global g_inputfile
	files_equal = True

	main(sys.argv[1:])
	print 'Input file is "' + g_inputfile + '"'

	if check_if_file_exists(g_inputfile) == False:
		exit(1)

	ins = open( g_inputfile, "r" )
	for line in ins:
		print '****************************************************************************************************'
		if ':' in line:
			line = line.rstrip('\n')
			l = re.compile(':').split(line)
			if len(l) == 2:
				if check_if_file_exists(l[0]) == False:
					print l[0] + ' file is missing'
				elif check_if_file_exists(l[1]) == False:
					print l[1] + ' file is missing'
				else:
					if compare_files(l[0], l[1]) == False:
						files_equal = False

			else:
				files_equal = False
				print 'wrong format'
		print '\n'
	ins.close()

	if files_equal == False:
		print "Non equal files found in the list"
		print "Test Failed"
		sys.exit(1)

	else:
		sys.exit(0)















































	
	
	
	
	
	
