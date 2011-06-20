Bee2 Miner
-------------------
This is a port of fpgaminer's

https://github.com/progranism/Open-Source-FPGA-Bitcoin-Miner

to the Berkeley BEE2-Board, using a bit-banging interface
to communicate to the User FPGAs running the hashing,
 (the bit-banging is driven by the BEE2's Ctrl FPGA).


Implementation details
----------------------

The BEE2 Board uses Virtex-2 FPGAs, which can handle
LOOPLOG2 of 2 using 75% of the slices. (lower values
crash my implementation of ISE when synthesizing). 
The LEDs are used to indicate bits [25:20] of the
current nonce being used, so lots of nice blinky lights.

The User FPGAs have a 100MHz system clock, but this
timing was too agressive to meet so I have divided it
down to 50MHz. With one hash every 4 cycles,
this gives a rate of 125MHash/sec (for one user FPGA).

The bitbanging interface is simple and builds off a custom
Control FPGA image used for a different project.
Sending data is simply putting it onto RxD line and clocking RxC.
Receiving Data involves searching for a "start bit" by clocking
TxC and waiting for TxD to be high, then 8 bits of data 
can be clocked out before checking for a start bit again.
RxTxR can be used to reset both interfaces (only at teh byte
level, does not reset the higher level deserializer).

Usage
-----

1. Install the control FPGA image (not provided).

2. Build the design -- I do this through the ISE GUI.

3. Scp the bitfile to the bee2 board's /cfg/ directory.

4. As root on the bee2, use user_program to program the 4 user fpgas.

5. Use 'make upload' to upload the Bee2 Control FPGA software.

6. on the Bee2, run the Control FPGA software with 4q_resetnio

7. To Be Continued...
