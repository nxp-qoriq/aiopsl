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
	global g_outputfile
	g_inputfile = ''
	g_outputfile = ''

	try:
		opts, args = getopt.getopt(argv,"hi:o:",["ifile=,ofile="])
	except getopt.GetoptError:
		print 'jenkins_files_compare.py -i <inputfile> -o <outputfile>'
		sys.exit(2)
	for opt, arg in opts:
		if opt == '-h':
			print 'jenkins_files_compare_<mc/arena>_aiop.py -i <inputfile> -o <outputfile>'
			sys.exit()
		elif opt in ("-i", "--ifile"):
			g_inputfile = arg
		elif opt in ("-o", "--ofile"):
			g_outputfile = arg

	if g_inputfile == '':
		print 'input file missing, run -h for help'
		sys.exit(2)

	if g_outputfile == '':
		print 'output file missing, use default: email.txt'
		g_outputfile = 'email.txt'

def  compare_files(file1, file2):
	copyright_exist = False
	copy_index = 0
	orig_file1 = file1
	orig_file2 = file2
	
	f = open(file1,"r")
	lines = f.readlines()
	f.close()
	for i in range(0, len(lines)):
		line = lines[i]
		if 'copyright' in line.lower() and not copyright_exist:
			copyright_exist = True
			print 'copyrights exist (not used for compare) in file: ' + file1
		if copyright_exist and '*/' in line:
			#save index
			copy_index = i + 1
			line = lines[copy_index]
			#move to first not empty line after copyrights
			while line == '\n' and copy_index < len(lines):
				copy_index = copy_index + 1
				line = lines[copy_index]
			break


	if copyright_exist:
		file1 = file1 + '.temp'
		fw = open(file1,"wb")
		for i in range(copy_index, len(lines)):
			line = lines[i]
			fw.write(line)
		fw.close()
		copyright_exist = False
		copy_index = 0


	f = open(file2,"r")
	lines = f.readlines()
	f.close()
	for i in range(0, len(lines)):
		line = lines[i]
		if 'copyright' in line.lower() and not copyright_exist:
			copyright_exist = True
			print 'copyrights exist (not used for compare) in file: ' + file2
		if copyright_exist and '*/' in line:
			#save index
			copy_index = i + 1
			line = lines[copy_index]
			#move to first not empty line after copyrights
			while line == '\n' and copy_index < len(lines):
				copy_index = copy_index + 1
				line = lines[copy_index]
			break


	if copyright_exist:
		file2 = file2 + '.temp'
		fw = open(file2,"wb")
		for i in range(copy_index, len(lines)):
			line = lines[i]
			fw.write(line)
		fw.close()
		copyright_exist = False
		copy_index = 0




	file1_md5 = md5(file1)
	file2_md5 = md5(file2)
	if file1_md5 != file2_md5:
		print "files not equal: \n\t" + orig_file1 + ",\n\t" + orig_file2
		return False
	else:
		print "files equal: \n\t" + orig_file1 + ",\n\t" + orig_file2
		return True 



if __name__ == "__main__":
	global g_inputfile
	files_equal = True
	first_enter = True

	main(sys.argv[1:])
	print 'Input file is "' + g_inputfile + '"'

	if check_if_file_exists(g_inputfile) == False:
		exit(1)
	Email_recipients = 'Email = '

	input_read = open(g_inputfile, "r" )
	

	for line in input_read:
		print '****************************************************************************************************'
		if ':' in line:
			line = line.rstrip('\n')
			l = re.compile(':').split(line)
			if len(l) == 3:
				if check_if_file_exists(l[0]) == False:
					print l[0] + ' file is missing'
				elif check_if_file_exists(l[1]) == False:
					print l[1] + ' file is missing'
				else:
					if compare_files(l[0], l[1]) == False:
						files_equal = False
						if first_enter:
							Email_recipients = Email_recipients + l[2]
							first_enter = False	
						else:
							if l[2] not in Email_recipients:
								Email_recipients = Email_recipients + ',' + l[2]



			else:
				files_equal = False
				print 'wrong format of input file'
		print '\n'
	input_read.close()
	print 'Emails will be sent to the following recipients'
	print Email_recipients
	output_write = open(g_outputfile, "w")
	output_write.write(Email_recipients)
	output_write.close

	if files_equal == False:
		print "Non equal files found in the list"
		print "Test Failed"
		sys.exit(1)

	else:
		sys.exit(0)

