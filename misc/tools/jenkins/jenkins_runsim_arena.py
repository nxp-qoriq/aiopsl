#! /usr/bin/python
import time
import shutil
import sys
import re
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
	global g_order_packets_flag
	if 'arena_test_40.pcap' in pcap:
		print 'injecting 40 packets'
		g_capture = 39
		Popen(["./fm_tio_inject","-hub","localhost:42975","-ser","w0_m1","-file","arena_test_40.pcap"])
	elif 'app_process_packet.pcap' in pcap:
		g_capture = 4
		#g_order_packets_flag used to test the order of received packet from aiop w0_m1
		g_order_packets_flag = True
		print 'injecting 4 packets (one broadcast packet to second NI)'
		Popen(["./fm_tio_inject","-hub","localhost:42975","-ser","w0_m3","-file","broadcast.pcap"])
		Popen(["./fm_tio_inject","-hub","localhost:42975","-ser","w0_m1","-file","app_process_packet.pcap"])
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
	elif 'eth_ipv4_udp.pcap' in pcap:
		g_capture = 1
		print 'injecting 1 packet'
		Popen(["./fm_tio_inject","-hub","localhost:42975","-ser","w0_m1","-file","eth_ipv4_udp.pcap"])
	else:
		print 'name of recognized pcap not found'

if __name__ == "__main__":
	global g_capture 
	global g_order_packets_flag
	global g_no_AIOP
	g_capture = 0
	g_no_AIOP = False
	last_packet_size = 0
	g_order_packets_flag = False
	finished_successfully = False
	pcap = 'NULL'
	device =''
	sim_init_params = ''
	sys_test = ''

	if len(sys.argv) < 2:
		print "Using old configurations for ls2085a"
		sim_init_params = "jenkins_ls2085a_sim_init_params.cfg"
		sys_test = "jenkins_ls2085a_sys_test.cfg"
	else:
		sim_init_params = str(sys.argv[1])
		sys_test = str(sys.argv[2])

	if "ls2085a" in str(sim_init_params) or "ls2088a" in str(sim_init_params):
		device = "ls2085aiss"
	elif "ls1088a" in str(sim_init_params):
		device = "ls1088aiss"
	elif "ls2080a" in str(sim_init_params):
		device = "ls2085aiss"
		g_no_AIOP = True
	else:
		print "device is not supported"
		exit_script(runsim,1)

	print "Run sim with the following cfg files:"
	print sys_test
	print sim_init_params

	runsim = Popen(["./runsim","-t","-d",str(device),"-nc","0","-pnc","00000000_00000000_00000000__10__00000000_00000000","-imodel","ls_sim_init_file="+str(sim_init_params), "-smodel","ls_sim_config_file="+str(sys_test),"-noprog"], stdout=PIPE, bufsize=1, close_fds=ON_POSIX)
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
					#if we test the app process packet test, to check that the order is correct I will check the packet size (injected packets come with raced size 64, 128, 1480)
					if g_order_packets_flag and 'w0_m1' in line:
						search_str = 'pkt_size =\s(\d+)'
						m = re.search(search_str.decode('utf-8'), line.decode('utf-8'), re.I | re.U) 
						packet_size = int(m.group(1))
						if packet_size < last_packet_size:
							exit_script(runsim,2)
						last_packet_size = packet_size
				elif 'Finished SUCCESSFULLY' in line or finished_successfully:
					finished_successfully = True
					if g_capture <= 0:
						exit_script(runsim,0)
				elif 'Finished with ERRORS' in line:
					exit_script(runsim,1)