def my_observer_start(o):
    o.remove()
    print "\nEnable instruction trace\n"
    top.aiop.cluster0.complex0.core0.thread0.exts.setInstrTrace(True)

def my_observer_end(o):
    o.remove()
    print "\nDisable instruction trace\n"
    top.aiop.cluster0.complex0.core0.thread0.exts.setInstrTrace(False)

top.aiop.cluster0.complex0.core0.thread0.events.instrcomp.data.ea.observe.on_occurrence(my_observer_start, 0x00fe0304)
top.aiop.cluster0.complex0.core0.thread0.events.instrcomp.data.ea.observe.on_occurrence(my_observer_end, 0x00fe0308)