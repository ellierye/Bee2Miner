These files are designed to run on the BEE2 to provide the functionality 
for uploading and running diags. They currently must be compiled on neva 
because it has the correct version of GCC for the PowerPC which is on
the Bee2's FPGAs.

To compile and upload to the BEE2 (bee2-sm@stanford.edu), simply run 

make upload

and enter the password when prompted. 
The tools will already be placed on the board at 
/home/$USER/4q_software/bin,
which should be in the path.

Description of files: 

bee2lib.c/h : a collection of utility functions called by many 
programs, such as reading and 
writing configuration registers on the FPGA

coredrv.h : Header file which allows accessing coredrv.o module, to 
essentially memory-map address ranges

Makefile : the makefile. Run "make upload" to compile and upload everything. 
"make clean" to clean.

ppc_user_logic.h: Taken from the verilog and turned into a C header, this describes many 
values for the PPC interface block

README.txt: this file

4q_jtag.c/h : this library of functions handles what is necessary to interface 
to SM from PPC over JTAG.  This is the interface we used to talk to the
miners running on the User FPGAs.

minernet.c : This main program will send the data/midstate to the user
FPGA(s) and read off nonces. It will also handle asking for more work
via a socket interface.
