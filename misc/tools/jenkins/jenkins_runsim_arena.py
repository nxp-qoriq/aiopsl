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

def inject_packets(pcap):
	global g_capture
	if 'arena_test_40.pcap' in pcap:
		print 'injecting 40 packets'
		g_capture = 39
		Popen(["./fm_tio_inject","-hub","localhost:42975","-ser","w0_m1","-file","arena_test_40.pcap"])
	elif 'eth_ipv4_udp.pcap' in pcap:
		g_capture = 3
		print 'injecting 3 packets (one broadcast packet to second NI)'
		Popen(["./fm_tio_inject","-hub","localhost:42975","-ser","w0_m1","-file","eth_ipv4_udp.pcap"])
		Popen(["./fm_tio_inject","-hub","localhost:42975","-ser","w0_m1","-file","eth_ipv4_udp.pcap"])
		Popen(["./fm_tio_inject","-hub","localhost:42975","-ser","w0_m3","-file","broadcast.pcap"])
	elif 'reassembled_frame.pcap' in pcap:
		g_capture = 4
		print 'injecting 1 packet'
		Popen(["./fm_tio_inject","-hub","localhost:42975","-ser","w0_m1","-file","reassembled_frame.pcap"])
	elif 'frag.pcap' in pcap:
		g_capture = 1
		print 'injecting 1 packet'
		Popen(["./fm_tio_inject","-hub","localhost:42975","-ser","w0_m1","-file","frag1.pcap"])
		Popen(["./fm_tio_inject","-hub","localhost:42975","-ser","w0_m1","-file","frag2.pcap"])
		Popen(["./fm_tio_inject","-hub","localhost:42975","-ser","w0_m1","-file","frag3.pcap"])
		Popen(["./fm_tio_inject","-hub","localhost:42975","-ser","w0_m1","-file","frag4.pcap"])
	else:
		print 'name of recognized pcap not found'

if __name__ == "__main__":
	global g_capture 
	g_capture = 0
	finished_successfully = False
	pcap = 'NULL'
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
		if i == 300:
			print 'Simulator is not responding 5 minutes'
			if g_capture > 0:
				print str(g_capture) + 'packet/s missing'
			exit_script(runsim,1)
		while True:
			try:  line = q.get_nowait()
			except Empty:
				break
			else: # got line
				i = 0
				print line
				if 'AIOP boot finished' in line:
					if pcap != 'NULL':
						inject_packets(pcap)
				if 'inject packets:' in line:
					tio_capture = Popen(["./fm_tio_capture","-hub","localhost:42975","-ser","w0_m1","w0_m3","-verbose_level","2"], stdout=PIPE, bufsize=1, close_fds=ON_POSIX)
					c = Thread(target=enqueue_output, args=(tio_capture.stdout, q))
					c.daemon = True # thread dies with the program
					c.start()
					pcap = line
				elif 'Got a packet' in line:
					g_capture -= 1
				elif 'Finished SUCCESSFULLY' in line or finished_successfully:
					finished_successfully = True
					if g_capture == 0:
						exit_script(runsim,0)
				elif 'Finished with ERRORS' in line:
					exit_script(runsim,1)