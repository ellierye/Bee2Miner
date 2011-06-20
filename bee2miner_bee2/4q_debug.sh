#!/bin/sh

echo "4q_read_fifos..."
4q_read_fifos > fifos_ctrl.log
4q_read_fifos 0 > fifos0.log
4q_read_fifos 1 > fifos1.log
4q_read_fifos 2 > fifos2.log
4q_read_fifos 3 > fifos3.log
echo "4q_jtag_read_trace..."
4q_jtag_read_trace 0 0 14 > q0t0_trace.log
4q_jtag_read_trace 1 0 14 > q1t0_trace.log
4q_jtag_read_trace 2 0 14 > q2t0_trace.log
4q_jtag_read_trace 3 0 14 > q3t0_trace.log
4q_jtag_read_trace 0 1 14 > q0t1_trace.log
4q_jtag_read_trace 1 1 14 > q1t1_trace.log
4q_jtag_read_trace 2 1 14 > q2t1_trace.log
4q_jtag_read_trace 3 1 14 > q3t1_trace.log
4q_jtag_read_trace 0 2 14 > q0t2_trace.log
4q_jtag_read_trace 1 2 14 > q1t2_trace.log
4q_jtag_read_trace 2 2 14 > q2t2_trace.log
4q_jtag_read_trace 3 2 14 > q3t2_trace.log
4q_jtag_read_trace 0 3 14 > q0t3_trace.log
4q_jtag_read_trace 1 3 14 > q1t3_trace.log
4q_jtag_read_trace 2 3 14 > q2t3_trace.log
4q_jtag_read_trace 3 3 14 > q3t3_trace.log
