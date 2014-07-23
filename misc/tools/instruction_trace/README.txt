instruction trace to file
=========================
1) Copy to simulator folder:
	run_with_trace.sh (edit 
	trace.py
2) Add two dummy functions to your code in place you want to start the trace:

__HOT_CODE static int start_trace(void){
	return 0;
	}
__HOT_CODE static int end_trace(void){
	return 0;
	}
3) Add call to start_trace before the place you want to start tracing, and a call to stop tracing after.
	for example:
	start = start_trace();
	*time = my_div(time_us,1000);
	end = end_trace();
4) Build the project.
5) Open the MAP file located in "out" folder of your project, and find the virtual addresses of the start_trace and
   end_trace functions.
   Replace those addresses in trace.py.
6) Run simulator using the script you copied. (./run_with_trace <port>)
7) Run the application. ("Enable instruction trace" should appear in simulator log when the trace is activated
   and "Disable instruction trace" right after the trace is finished.)
8) "lsdpaa2sim_tarmacTraceFile.log" log should be created in your simulator folder.


Trace.py works on AIOP.
for MC trace, replace the script with the following:

def my_observer_start(o):
    o.remove()
    print "\nEnable instruction trace\n"
    top.mc.core0.thread0.exts.setInstrTrace(True)

def my_observer_end(o):
    o.remove()
    print "\nDisable instruction trace\n"
	top.mc.core0.thread0.exts.setInstrTrace(False)

top.mc.core0.thread0.events.instrcomp.data.ea.observe.on_occurrence(my_observer_start, 0x00fe0304)
top.mc.core0.thread0.events.instrcomp.data.ea.observe.on_occurrence(my_observer_end, 0x00fe0308)


Good Luck  

    
	
  