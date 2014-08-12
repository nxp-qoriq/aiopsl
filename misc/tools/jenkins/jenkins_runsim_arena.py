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
def exit_script(runsim,exit_status):
	runsim.terminate() #close simulator
	exit(exit_status)
if __name__ == "__main__":
	capture = 0
	runsim = Popen(["./runsim","-t","-d","ls2085aiss","-nc","0","-pnc","00000000_00000000_00000000__10__00000000_00000000","-imodel","ls_sim_init_file=jenkins_ls2085a_sim_init_params.cfg", "-smodel","ls_sim_config_file=jenkins_ls2085a_sys_test.cfg","-noprog"], stdout=PIPE, bufsize=1, close_fds=ON_POSIX)
	q = Queue()
	t = Thread(target=enqueue_output, args=(runsim.stdout, q))
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
			exit_script(runsim,1)
		while True:
			try:  line = q.get_nowait()
			except Empty:
				break
			else: # got line
				i = 0
				print line
				if 'inject packets:' in line:
					tio_capture = Popen(["./fm_tio_capture","-hub","localhost:42975","-ser","w0_m1","-verbose_level","2"], stdout=PIPE, bufsize=1, close_fds=ON_POSIX)
					c = Thread(target=enqueue_output, args=(tio_capture.stdout, q))
					c.daemon = True # thread dies with the program
					c.start()
					if 'arena_test_40.pcap' in line:
						print 'injecting 40 packets'
						capture = 39
						Popen(["./fm_tio_inject","-hub","localhost:42975","-ser","w0_m1","-file","arena_test_40.pcap"])
					elif 'eth_ipv4_udp.pcap' in line:
						capture = 3
						print 'injecting 3 packets'
						Popen(["./fm_tio_inject","-hub","localhost:42975","-ser","w0_m1","-file","eth_ipv4_udp.pcap"])
						Popen(["./fm_tio_inject","-hub","localhost:42975","-ser","w0_m1","-file","eth_ipv4_udp.pcap"])
						Popen(["./fm_tio_inject","-hub","localhost:42975","-ser","w0_m1","-file","eth_ipv4_udp.pcap"])
				elif 'Got a packet' in line:
					capture -= 1 
				elif 'Test Finished SUCCESSFULLY' in line:
					if capture == 0:
						exit_script(runsim,0)
				elif 'ERROR found during ARENA test' in line:
					exit_script(runsim,1)
	
