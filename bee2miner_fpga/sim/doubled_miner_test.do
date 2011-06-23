vlib work
vlog  +acc  "../src/sha-256-functions.v"
vlog  +acc  "../src/sha256_transform.v"
vlog  +acc  "../src/bitbang_transmitter.v"
vlog  +acc  "../src/bitbang_receiver.v"
vlog  +acc  "../src/serial.v"
vlog  +acc  "../src/fpgaminer_doubled_top.v"
vlog  +acc  "doubled_miner_test.v"
vlog  +acc  "/hd/cad/xilinx/ise9.2i/verilog/src/glbl.v"
vsim -t 1ps   -L xilinxcorelib_ver -L unisims_ver -lib work doubled_miner_test_v glbl
do {wave_doubled.do}
view structure
view signals
run 1000ns
