onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate -radix hexadecimal /doubled_miner_test_v/sysclk_m
add wave -noupdate -radix hexadecimal /doubled_miner_test_v/sysclk_p
add wave -noupdate -radix hexadecimal /doubled_miner_test_v/uut/is_golden_ticket_0
add wave -noupdate -radix hexadecimal /doubled_miner_test_v/uut/is_golden_ticket_1
add wave -noupdate -radix hexadecimal /doubled_miner_test_v/uut/hash2_0
add wave -noupdate -radix hexadecimal /doubled_miner_test_v/uut/hash2_1
add wave -noupdate -radix hexadecimal /doubled_miner_test_v/uut/hash_clk
add wave -noupdate -radix hexadecimal /doubled_miner_test_v/uut/golden_nonce
add wave -noupdate -radix hexadecimal /doubled_miner_test_v/uut/nonce_0
add wave -noupdate -radix hexadecimal /doubled_miner_test_v/uut/nonce_1
add wave -noupdate -radix hexadecimal /doubled_miner_test_v/uut/midstate_buf
add wave -noupdate -radix hexadecimal /doubled_miner_test_v/uut/data_buf
add wave -noupdate -radix hexadecimal /doubled_miner_test_v/midstate_buf
add wave -noupdate -radix hexadecimal /doubled_miner_test_v/data_buf
add wave -noupdate -radix hexadecimal /doubled_miner_test_v/uut/midstate_vw
add wave -noupdate -radix hexadecimal /doubled_miner_test_v/uut/data2_vw
add wave -noupdate -expand -group Rx -radix hexadecimal /doubled_miner_test_v/uut/serrx/RxD_data_ready
add wave -noupdate -expand -group Rx -radix hexadecimal /doubled_miner_test_v/uut/serrx/RxD_data
add wave -noupdate -expand -group Rx -radix hexadecimal /doubled_miner_test_v/uut/serrx/demux_state
add wave -noupdate -expand -group Rx -radix hexadecimal /doubled_miner_test_v/uut/serrx/input_buffer
add wave -noupdate -expand -group Rx -radix hexadecimal /doubled_miner_test_v/uut/serrx/input_copy
add wave -noupdate -expand -group Rx /doubled_miner_test_v/RxD
add wave -noupdate -expand -group Rx /doubled_miner_test_v/RxC
add wave -noupdate -expand -group Tx -radix hexadecimal /doubled_miner_test_v/read_data
add wave -noupdate -expand -group Tx -radix hexadecimal /doubled_miner_test_v/TxD
add wave -noupdate -expand -group Tx -radix hexadecimal /doubled_miner_test_v/TxC
add wave -noupdate -expand -group Tx -radix hexadecimal /doubled_miner_test_v/read_nonce
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {154337167 ps} 0}
configure wave -namecolwidth 344
configure wave -valuecolwidth 98
configure wave -justifyvalue left
configure wave -signalnamewidth 0
configure wave -snapdistance 10
configure wave -datasetprefix 0
configure wave -rowmargin 4
configure wave -childrowmargin 2
configure wave -gridoffset 0
configure wave -gridperiod 1
configure wave -griddelta 40
configure wave -timeline 0
configure wave -timelineunits ps
update
WaveRestoreZoom {0 ps} {3138462 ps}
