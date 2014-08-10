#! /usr/bin/python
import time
import shutil
import sys
from subprocess import PIPE, Popen
from threading  import Thread
try:
	from Queue import Queue, Empty
except ImportError:
	from queue import Queue, Empty  # python 3.x


ON_POSIX = 'posix' in sys.builtin_module_names

def enqueue_output(out, queue):
	for line in iter(out.readline, 'b'):
		queue.put(line)
	out.close()
def exit_script(p,exit_status):
	p.terminate() #close simulator
	exit(exit_status)

p = Popen(["./runsim","-t","-d","ls2085aiss","-nc","0","-pnc","00000000_00000000_00000000__10__00000000_00000000","-imodel","ls_sim_init_file=jenkins_ls2085a_sim_init_params.cfg", "-smodel","ls_sim_config_file=jenkins_ls2085a_sys_test.cfg","-noprog"], stdout=PIPE, bufsize=1, close_fds=ON_POSIX)
q = Queue()
t = Thread(target=enqueue_output, args=(p.stdout, q))
t.daemon = True # thread dies with the program
t.start()
i = 0
# ... do other things here
while True:
# read line without blocking
	time.sleep(1)
	i += 1
	if i == 60:
		print 'Simulator is not responding for a minute'
		exit_script(p,1)
	while True:
		try:  line = q.get_nowait()
		except Empty:
			break
		else: # got line
			i = 0
			print line
			if 'arena_test_40.pcap' in line:
				print 'injecting packets'
				Popen(["./fm_tio_inject","-hub","localhost:42975","-ser","w0_m1","-file","arena_test_40.pcap"])
			elif 'Test Finished SUCCESSFULLY' in line:
				exit_script(p,0)
			elif 'ERROR found during ARENA test' in line:
				exit_script(p,1)
	
