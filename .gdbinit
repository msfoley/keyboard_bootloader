target remote localhost:3333
#monitor reset halt
b startup.c:97

source ../PyCortexMDebug/scripts/gdb.py
source device/max32650/.gdbinit
