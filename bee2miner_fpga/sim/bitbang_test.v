`timescale 1ns / 1ps

////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:
//
// Create Date:   17:54:41 06/18/2011
// Design Name:   interface_tester_top
// Module Name:   /home/wachs/bee2miner/bee2miner/bitbang_test.v
// Project Name:  bee2miner
// Target Device:  
// Tool versions:  
// Description: 
//
// Verilog Test Fixture created by ISE for module: interface_tester_top
//
// Dependencies:
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////

module bitbang_test_v;

   // Inputs
   reg sysclk_m;
   wire sysclk_p;
   reg 	RxD;
   reg 	RxC;
   reg 	RxTxR;
   reg 	TxC;
   reg 	data_available;

   reg [7:0] read_data;
   
   
   // Outputs
   wire      TxD;
   wire [5:0] leds;
   
   // Instantiate the Unit Under Test (UUT)
   interface_tester_top uut (
			     .sysclk_m(sysclk_m),
			     .sysclk_p(sysclk_p),
			     .RxD(RxD), 
			     .RxC(RxC), 
			     .RxTxR(RxTxR), 
			     .TxD(TxD), 
			     .TxC(TxC), 
			     .leds(leds)
			     );
   
   
   always 
     sysclk_m = #5 ~sysclk_m;	
   
   assign sysclk_p = ~sysclk_m;
   
   initial begin
      // Initialize Inputs
      sysclk_m = 0;
      RxD = 0;
      RxC = 0;
      RxTxR = 0;
      TxC = 0;
      
      data_available = 0;
      
      // Wait 100 ns for global reset to finish
      #100;
      
      RxTxR = #2 1;
      repeat (10) @(posedge sysclk_p);
      RxTxR = #2 0;
      repeat (10) @(posedge sysclk_p);
      
      
      //write
      repeat (512) begin
	 RxD = #2 $random();
	 repeat (10) @ (posedge sysclk_p);
	 RxC = #2 1;
	 repeat (10) @ (posedge sysclk_p);
	 RxC = #2 0;
	 repeat (10) @ (posedge sysclk_p);
      end
      
	   
      RxTxR = #2 1;
      repeat (10) @(posedge sysclk_p);
      RxTxR = #2 0;
      repeat (10) @(posedge sysclk_p);
      
      
      //read
      repeat (8) begin
	 data_available = #2 0;
	 
	 while (data_available == 0) begin
	    repeat (10) @ (posedge sysclk_p);
	    TxC = #2 1;
	    repeat (10) @ (posedge sysclk_p);
	    TxC = #2 0;
	    repeat (10) @ (posedge sysclk_p);
	    data_available = #2 TxD;
	 end
	 read_data = 8'b0;
	 repeat(8) begin
	    repeat (10) @ (posedge sysclk_p);
	    TxC = #2 1;
	    repeat (10) @ (posedge sysclk_p);
	    TxC = #2 0;
	    repeat (10) @ (posedge sysclk_p);
	    read_data = {TxD, read_data[7:1]};
	 end
      end
			
		
		$stop;
		$finish;
		
	end //initial
	
   
   
endmodule

