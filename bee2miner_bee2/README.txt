/*************************************************************************
 ** From Perforce:
 **
 ** $Id: //Smart_design/Smash_board/sm_fpga/sm_software/README.txt#2 $
 ** $DateTime: 2009/06/09 11:13:13 $
 ** $Change: 7810 $
 ** $Author: wachs $
 *************************************************************************/



These files are designed to run on the BEE2 to provide the functionality 
for uploading and running diags.

To compile and upload to the BEE2 (bee2-sm@stanford.edu), simply run 

make upload

and enter the
password when prompted. The tools will already be placed on the board at /root/sm_software/bin,
which should be in the path.

Description of files: 

bee2lib.c/h : a collection of utility functions called by many programs, such as reading and 
writing configuration registers on the FPGA, reading and writing memory mapped to SM's 
address space (complicated due to the use of meta-data bits).

cc_definitions.h : Taken from the verilog and turned into a C header, this describes many 
values for the CC.

coredrv.h : Header file which allows accessing coredrv.o module, to essentially memory-map
address ranges

example_memfile.mem : This is an example mem file to show the sort of input needed to 
sm_test_by_jtag.  

libsim_for_rtl.c/h : This is a translation of the functions in the Vera environment to 
enable I/Os between SM and the host.


load_program.c : This should be called in order to load a .dat.iss file into the DRAM on the
BEE2 prior to starting up SM. When SM starts up, it will begin fetching instructions, so
the dat.iss puts them in the right place. Load_program also handles configuring the address
mapping from MC addresses to physical DDR addresses (inside the translator block). For
that reason, it needs to be told whether to use TCC, CACHED, etc memory mapping.

Makefile : the makefile. Run "make upload" to compile and upload everything. 
"make clean" to clean.

mc_proc.h:   Taken from the verilog and turned into a C header, this describes many 
values for the MC

ppc_rw_reg.c/h: This creates a command line program which allows reading and writing PPC registers
directly. For example, one can call "ppc_rw_reg W RESET_N 0" to assert reset to SM.

ppc_user_logic.h: Taken from the verilog and turned into a C header, this describes many 
values for the PPC interface block

Quad_def.h : Taken from the verilog and turned into a C header, this describes many 
values for the entire Quad

qualify_chip.sh : This script is a tool for quickly running basic tests to make sure the chip and
system are "good". Currently it just runs the BSR test and does a random test on all the
block rams on the chip.

README.txt: this file

run_test.sh : This is the main tool for running a diag on the chip. It handles resetting the system,
loading a program, configuring the MC and MC translator, taking the appropriate chip out of
reset, then monitoring for IOs and exit code.

sim_io_def.h :  Taken from the verilog/Vera and turned into a C header, this describes many 
values used by the SIM IO interface (simple printfs)

sm_cfg_mc.c/h : This program configures the Memory Controller on the FPGA with the specified 
configuration, by writing specific registers on the MC.

sm_jtag.c/h : this library of functions handles what is necessary to interface to SM from PPC
over JTAG. 

sm_jtag_cf_rd_and_wr.c/h: This simple test tests basic JTAG functionality by writing and reading
back a few config registers on SM.

sm_jtag_extest_and_sample.c/h : This test does a boundary scan on SM to test the connectivity
of the whole system for I/Os. It runs several patterns and reports any errors.

sm_jtag_mem_test.c/h: This does reads and writes to all memories on the chip through JTAG 

sm_jtag_read_id.c/h : This simple test reads the IDcode on the chip

sm_mc.c/h : This library of functions handles different memory controller configurations.

sm_read_fifos.c/h: This program reads out the snooping fifos which are on the FPGA for debugging
purposes. Once this is called the fifos are drained so they will show up as empty (unless something
is still happnening on the chip!)

sm_reset_fifos.c/h : this program resets the snooping fifos on the FPGA. They will show up as empty
after this command.

sm_resetnio.c/h : This main program will bring the specified processor out of reset and monitor 
the memory for IO transactions and exit codes. Once a program is loaded into the memory, this is 
the function to use to actually get SM to run it.

sm_TEMPLATE.c : if you want to write a new program, start here.

sm_test_by_jtag.c/h : This program will take in a .mem file, parse it, and issue the
specified JTAG commands to SM

vh_to_h.pl : this utility tool will convert most verilog header files to c header files.
