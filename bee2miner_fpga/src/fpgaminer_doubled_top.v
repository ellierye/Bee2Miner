/*
*
* Copyright (c) 2011 fpgaminer@bitcoin-mining.com
*
*
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* 
*/

//`include "main_pll.v"
//`include "main_pll_2x.v"

//`include "sha-256-functions.v"
//`include "sha256_transform.v"
//`include "main_pll.v"
//`include "main_pll_2x.v"

//`include "sha-256-functions.v"
//`include "sha256_transform.v"
//`include "serial.v" // replaces virtual_wire

//`include "display7seg.v"

`timescale 1ns/1ps

//module fpgaminer_top (osc_clk, RxD, TxD);
module fpgaminer_doubled_top (sysclk_p, sysclk_m, RxD, RxC, RxTxR, TxD, TxC, leds);
//module fpgaminer_top (osc_clk, RxD, TxD, anode, segment, led);

   // The LOOP_LOG2 parameter determines how unrolled the SHA-256
   // calculations are. For example, a setting of 1 will completely
   // unroll the calculations, resulting in 128 rounds and a large, fast
   // design.
   //
   // A setting of 2 will result in 64 rounds, with half the size and
   // half the speed. 3 will be 32 rounds, with 1/4th the size and speed.
   // And so on.
   //
   // Valid range: [0, 5]
   
   parameter LOOP_LOG2 = 2;
   
   // No need to adjust these parameters
   localparam [5:0] LOOP = (6'd1 << LOOP_LOG2);
   // The nonce will always be larger at the time we discover a valid
   // hash. This is its offset from the nonce that gave rise to the valid
   // hash (except when LOOP_LOG2 == 0 or 1, where the offset is 131 or
   // 66 respectively).
   localparam [31:0] GOLDEN_NONCE_OFFSET = (32'd1 << (7 - LOOP_LOG2)) + 32'd1;
   
   input sysclk_p, sysclk_m;
   
   //// 
   reg [255:0] state = 0;
	
   reg [511:0] data_0 = 0;
   reg [31:0]  nonce_0 = 32'h00000000;
   
   reg [511:0] data_1 = 0;
   reg [31:0]  nonce_1 = 32'h80000000;
	
   //// PLL
   wire        hash_clk;
   //`ifndef SIM
   //		main_pll pll_blk (.CLKIN_IN(osc_clk), .CLK0_OUT(hash_clk));
   //     		main_pll pll_blk (.CLKIN_IN(osc_clk), .CLK2X_OUT(hash_clk));
   //`else
//   assign hash_clk = osc_clk;
   //`endif
   
	wire clk_100MHz;
	reg clk_50MHz = 0;
	
   IBUFDS_LVDS_25 ib (.I(sysclk_p), .IB(sysclk_m), .O(clk_100MHz));
      
	always@(posedge clk_100MHz) 
		clk_50MHz <= ~clk_50MHz;
	
	assign hash_clk = clk_50MHz;

   
   //// Hashers (0)
   wire [255:0] hash_0;
   wire [255:0] hash2_0;
   reg [5:0] 	cnt = 6'd0;
   reg 		feedback = 1'b0;
   
   sha256_transform #(.LOOP(LOOP)) hash00 (
					.clk(hash_clk),
					.feedback(feedback),
					.cnt(cnt),
					.rx_state(state),
					.rx_input(data_0),
					.tx_hash(hash_0)
					);
   sha256_transform #(.LOOP(LOOP)) hash10 (
					 .clk(hash_clk),
					 .feedback(feedback),
					 .cnt(cnt),
					 .rx_state(256'h5be0cd191f83d9ab9b05688c510e527fa54ff53a3c6ef372bb67ae856a09e667),
					 .rx_input({256'h0000010000000000000000000000000000000000000000000000000080000000, hash_0}),
					 .tx_hash(hash2_0)
					 );
   
   //// Hashers (1)
   wire [255:0] hash_1;
   wire [255:0] hash2_1;
   
   sha256_transform #(.LOOP(LOOP)) hash01 (
					.clk(hash_clk),
					.feedback(feedback),
					.cnt(cnt),
					.rx_state(state),
					.rx_input(data_1),
					.tx_hash(hash_1)
					);
   sha256_transform #(.LOOP(LOOP)) hash11 (
					 .clk(hash_clk),
					 .feedback(feedback),
					 .cnt(cnt),
					 .rx_state(256'h5be0cd191f83d9ab9b05688c510e527fa54ff53a3c6ef372bb67ae856a09e667),
					 .rx_input({256'h0000010000000000000000000000000000000000000000000000000080000000, hash_1}),
					 .tx_hash(hash2_1)
					 );
	
	
	
	
   //// Virtual Wire Control
   reg [255:0] 	midstate_buf = 0, data_buf = 0;
   wire [255:0] midstate_vw, data2_vw;
   
   input 	RxD, RxC, RxTxR;
   
   serial_receive serrx (.clk(hash_clk), .RxD(RxD), .RxC(RxC), .RxR(RxTxR) , .midstate(midstate_vw), .data2(data2_vw));
   
   //// Virtual Wire Output
   reg [31:0] 	golden_nonce = 0;
   reg 		serial_send;
   wire 	serial_busy;
   input 	TxC;
   output 	TxD;
   
   serial_transmit sertx (.clk(hash_clk), .TxD(TxD), .TxC(TxC), .TxR(RxTxR), .send(serial_send), .busy(serial_busy), .word(golden_nonce));
   
   
   //// Control Unit
   reg 		is_golden_ticket_0 = 1'b0;
   reg 		is_golden_ticket_1 = 1'b0;
   
	reg 		feedback_d1 = 1'b1;
   wire [5:0] 	cnt_next;
   wire [31:0] 	nonce_0_next;
	wire [31:0]    nonce_1_next;
	
   wire 	feedback_next;
`ifndef SIM
   wire 	reset;
   assign reset = 1'b0;
`else
   reg 		reset = 1'b0;	// NOTE: Reset is not currently used in the actual FPGA; for simulation only.
`endif
   
   assign cnt_next =  reset ? 6'd0 : (LOOP == 1) ? 6'd0 : (cnt + 6'd1) & (LOOP-1);
   // On the first count (cnt==0), load data from previous stage (no feedback)
   // on 1..LOOP-1, take feedback from current stage
   // This reduces the throughput by a factor of (LOOP), but also reduces the design size by the same amount
   assign feedback_next = (LOOP == 1) ? 1'b0 : (cnt_next != {(LOOP_LOG2){1'b0}});
   assign nonce_0_next =
		      reset ? 32'd0 :
		      feedback_next ? nonce_0 : (nonce_0 + 32'd1);
   assign nonce_1_next = nonce_0_next + 32'h80000000;
	
   always @ (posedge hash_clk)
     begin
`ifdef SIM
	//midstate_buf <= 256'h2b3f81261b3cfd001db436cfd4c8f3f9c7450c9a0d049bee71cba0ea2619c0b5;
	//data_buf <= 256'h00000000000000000000000080000000_00000000_39f3001b6b7b8d4dc14bfc31;
	//nonce <= 30411740;
`else
	midstate_buf <= midstate_vw;
	data_buf <= data2_vw;
`endif
	
	cnt <= cnt_next;
	feedback <= feedback_next;
	feedback_d1 <= feedback;
	
	// Give new data to the hasher
	state <= midstate_buf;
	data_0 <= {384'h000002800000000000000000000000000000000000000000000000000000000000000000000000000000000080000000, nonce_0_next, data_buf[95:0]};
	data_1 <= {384'h000002800000000000000000000000000000000000000000000000000000000000000000000000000000000080000000, nonce_1_next, data_buf[95:0]};
	
	nonce_0 <= nonce_0_next;
	nonce_1 <= nonce_1_next;
		
	// Check to see if the last hash generated is valid.
	is_golden_ticket_0 <= (hash2_0[255:224] == 32'h00000000) && !feedback_d1;
	is_golden_ticket_1 <= (hash2_1[255:224] == 32'h00000000) && !feedback_d1;
	
	if(is_golden_ticket_0)
	  begin
	     // TODO: Find a more compact calculation for this
	     //if (LOOP == 1)
	     //  golden_nonce <= nonce_0 - 32'd131;
	     //else if (LOOP == 2)
	     //  golden_nonce <= nonce_0 - 32'd66;
	     //else
	       golden_nonce <= nonce_0 - GOLDEN_NONCE_OFFSET;
	     
	     if (!serial_busy) serial_send <= 1;
	  end // if (is_golden_ticket)
	else if (is_golden_ticket_1)
	 begin
	     // TODO: Find a more compact calculation for this
	     //if (LOOP == 1)
	     //  golden_nonce <= nonce_0 - 32'd131;
	     //else if (LOOP == 2)
	     //  golden_nonce <= nonce_0 - 32'd66;
	     //else
	       golden_nonce <= nonce_1 - GOLDEN_NONCE_OFFSET;
	     
	     if (!serial_busy) serial_send <= 1;
	  end // if (is_golden_ticket)
	else
	  serial_send <= 0;
	
`ifdef SIM
	if (!feedback_d1)
	  $display ("nonce: %8x\nhash2: %64x\n", nonce, hash2);
`endif
     end
   
   // die debuggenlichten
   
   //output [7:0] leds;
   output [5:0] leds;
   //output [3:0] anode;
   //   display7seg disp(.clk(hash_clk), .leds(leds), .anode(anode), .word({midstate_vw[15:0], data2_vw[15:0]}));
   //display7seg disp(.clk(hash_clk), .leds(leds), .anode(anode), .word(golden_nonce));
   //assign leds[5:0] = golden_nonce[5:0];
   assign leds[5:0] = nonce_0[25:20];
   
endmodule

