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
module interface_tester_top (sysclk_p, sysclk_m, RxD, RxC, RxTxR, TxD, TxC, leds);
//module fpgaminer_top (osc_clk, RxD, TxD, anode, segment, led);

   //input osc_clk;
   input sysclk_p, sysclk_m;
   
   //// 
   reg [255:0] state = 0;
   reg [511:0] data = 0;
   reg [31:0]  nonce = 32'h00000000;
   
   //// PLL
   wire        hash_clk;
   //`ifndef SIM
   //		main_pll pll_blk (.CLKIN_IN(osc_clk), .CLK0_OUT(hash_clk));
   //     		main_pll pll_blk (.CLKIN_IN(osc_clk), .CLK2X_OUT(hash_clk));
   //`else
   //assign hash_clk = osc_clk;
   //`endif
   
   IBUFDS_LVDS_25 ib (.I(sysclk_p), .IB(sysclk_m), .O(hash_clk));

   //// Virtual Wire Control
   reg [255:0] 	midstate_buf = 0, data_buf = 0;
   wire [255:0] midstate_vw, data2_vw;
   
   input 	RxD, RxC, RxTxR;
   
   serial_receive serrx (.clk(hash_clk), .RxD(RxD), .RxC(RxC), .RxR(RxTxR) , .midstate(midstate_vw), .data2(data2_vw));
   
   //// Virtual Wire Output
   wire [31:0] 	golden_nonce;
   assign golden_nonce = data2_vw[31:0];
   reg 		serial_send;
   wire 	serial_busy;
   input 	TxC;
   output 	TxD;

      
   serial_transmit sertx (.clk(hash_clk), 
			  .TxD(TxD), 
			  .TxC(TxC), 
			  .TxR(RxTxR), 
			  .send(serial_send), 
			  .busy(serial_busy), 
			  .word(golden_nonce));
   
   
   //// Control Unit
   reg 		is_golden_ticket = 1'b0;
   reg 		feedback_d1 = 1'b1;
   wire [5:0] 	cnt_next;
   wire [31:0] 	nonce_next;
   wire 	feedback_next;
`ifndef SIM
   wire 	reset;
   assign reset = 1'b0;
`else
   reg 		reset = 1'b0;	// NOTE: Reset is not currently used in the actual FPGA; for simulation only.
`endif
   
   reg [8:0] 	cnt;
   always@(posedge hash_clk)
     if (RxTxR)
       cnt <= 9'b0;
     else
       cnt <= cnt + 1;
   
   
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
	
	// Check to see if the last hash generated is valid.
	if (!serial_busy && cnt > 9'd500) 
	  serial_send <= 1;
     	else 
	  serial_send <= 0;
	
     end
   
   // die debuggenlichten
   
   //output [7:0] leds;
   output [5:0] leds;
   //output [3:0] anode;
   //   display7seg disp(.clk(hash_clk), .leds(leds), .anode(anode), .word({midstate_vw[15:0], data2_vw[15:0]}));
   //display7seg disp(.clk(hash_clk), .leds(leds), .anode(anode), .word(golden_nonce));
   assign leds[5:0] = data_buf[5:0];
   
   
endmodule

