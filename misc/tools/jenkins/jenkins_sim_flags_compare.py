#! /usr/bin/python

import os
import sys, getopt
import re
from tabulate import tabulate
import operator

def check_if_file_exists(filename):
	return os.path.isfile(filename)

def main(argv):
	global g_inputfile1
	global g_inputfile2
	g_inputfile1 = ''
	g_inputfile2 = ''

	try:
		opts, args = getopt.getopt(argv,"hs:l:",["i_sim=, i_lin="])
	except getopt.GetoptError:
		print 'jenkins_files_compare.py -i_sim <input_sim_file> -i_lin <input_lin_file>'
		sys.exit(2)
	for opt, arg in opts:
		if opt == '-h':
			print 'jenkins_sim_flags_compare.py -s <input_sim_file> -l <input_lin_file>'
			sys.exit()
		elif opt in ("-s", "--i_sim"):
			g_inputfile1 = arg
		elif opt in ("-l", "--i_lin"):
			g_inputfile2 = arg

	if g_inputfile1 == '':
		print 'input sim file missing, run -h for help'
		sys.exit(2)

	if g_inputfile2 == '':
		print 'input lin file missing, run -h for help'
		sys.exit(2)

def getKey(item):
	return item[1]
def  compare_sim_flags(sim_flags, lin_flags):
	flags_equal = False
	table = []
	print "\nStart flags comparison:\n"
	for flag in sim_flags:
		if flag not in lin_flags:
			#print flag + "\t\tV\tX"
			table.append([flag,"1","0"])
			flags_equal = False
		else:
			#print flag + "\t\tV\tV"
			table.append([flag,"1","1"])
	
	for flag in lin_flags:
		if flag not in sim_flags:
			#print flag + "\t\tX\tV"
			table.append([flag,"0","1"])
			flags_equal = False
	
	table.sort(key=operator.itemgetter(0))

	#table = sorted(table,key=itemgetter(0))

	print tabulate(table, headers=["Flag","Sim","Lin"],tablefmt="grid")
	
	return flags_equal


if __name__ == "__main__":
	global g_inputfile1
	global g_inputfile2
	ignore_sim_flags_file = 'ignore_simulator_flags.txt'
	sim_flags = []
	lin_flags = []
	ignore_sim_flags = []


	main(sys.argv[1:])
	print 'Input files are "' + g_inputfile1 + ',"' + " " + g_inputfile2 + "."

	if check_if_file_exists(g_inputfile1) == False or check_if_file_exists(g_inputfile1) == False:
		exit(1)

	if check_if_file_exists(ignore_sim_flags_file) == False:
		exit(1)

	#Read the flags from ignored ligs to avoid during comparison.
	input_read = open(ignore_sim_flags_file, "r")
	print 'Reading simulator flags to ignore during comparison from: ' + ignore_sim_flags_file
	for line in input_read:	
		if not line.startswith('#') and line.strip():
			temp = line.split()[0]
			print "Ignored Flag: " + temp
			ignore_sim_flags.append(temp)
	input_read.close()

	#read the flags from simulator file (*.cfg).
	input_read = open(g_inputfile1, "r" )
	print 'Reading simulator flags from: ' + g_inputfile1
	for line in input_read:	
		if not line.startswith('#') and line.strip():
			temp = line.split()[0]
			if temp not in ignore_sim_flags:
				print "Flag: " + temp
				sim_flags.append(temp)	
	input_read.close()

	
	#read the flags from linux environment
	input_read = open(g_inputfile2, "r" )
	print 'Reading Linux flags from: ' + g_inputfile2	
	LS2_SIM_SUPPORT_S_FLAGS = False

	for line in input_read:
		if "LS2_SIM_SUPPORT_S_FLAGS" not in line and LS2_SIM_SUPPORT_S_FLAGS == False:
			continue
		elif "LS2_SIM_SUPPORT_S_FLAGS" in line:
			LS2_SIM_SUPPORT_S_FLAGS = True
			continue
		else:
			temp = line.strip()
			if temp.startswith('-s'):
				temp = temp.split()[1]
				lin_flags.append(temp)
				print "Lin Flag: " + temp
	input_read.close()

	if compare_sim_flags(sim_flags, lin_flags):
		sys.exit(0)
	else:
		sys.exit(1)

	

