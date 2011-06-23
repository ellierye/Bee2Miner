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

module miner_test_v;

   // Inputs
   reg sysclk_m;
   wire sysclk_p;
   reg 	RxD;
   reg 	RxC;
   reg 	RxTxR;
   reg 	TxC;
   reg 	data_available;
   
   reg [255:0] midstate_buf;
   reg [255:0] data_buf;
   
   reg [7:0]   send_byte;
   
   
   reg [7:0] read_data;
   reg [31:0] read_nonce;
   
   
   
   // Outputs
   wire      TxD;
   wire [5:0] leds;
   
   // Instantiate the Unit Under Test (UUT)
   fpgaminer_top uut (
			     .sysclk_m(sysclk_m),
			     .sysclk_p(sysclk_p),
			     .RxD(RxD), 
			     .RxC(RxC), 
			     .RxTxR(RxTxR), 
			     .TxD(TxD), 
			     .TxC(TxC), 
			     .leds(leds)
			     );
   
   
   always #5 sysclk_m = ~sysclk_m;	
   
   assign sysclk_p = ~sysclk_m;
   
   initial begin
       // Initialize Inputs
       sysclk_m = 0;
       RxD = 0;
       RxC = 0;
       RxTxR = 0;
       TxC = 0;
      
      //midstate_buf = 256'h2b3f81261b3cfd001db436cfd4c8f3f9c7450c9a0d049bee71cba0ea2619c0b5;
      //data_buf =     256'h00000000000000000000000080000000_00000000_39f3001b6b7b8d4dc14bfc31;
      //nonce <= 30411740;

      midstate_buf = 256'h228ea4732a3c9ba860c009cda7252b9161a5e75ec8c582a5f106abb3af41f790;
      data_buf =     256'h00000000000000000000000080000000000000002194261a9395e64dbed17115;
      //nonce = 32'h0e33337a;
		     
		     
  		
      data_available = 0;
      
      // Wait 100 ns for global reset to finish
      #100;
      
      #2 RxTxR =  1;
      repeat (10) @(posedge sysclk_p);
      #2 RxTxR =  0;
      repeat (10) @(posedge sysclk_p);
      
      
      //write
      repeat (32) begin
	 send_byte = midstate_buf[255:248];
	 repeat(8) begin
	    #2 RxD =  send_byte[0];
    	    send_byte = send_byte >> 1;
	    repeat (10) @ (posedge sysclk_p);
    	    #2 RxC =  1;
    	    repeat (10) @ (posedge sysclk_p);
    	    #2 RxC =  0;
    	    repeat (10) @ (posedge sysclk_p);
	 end
	 midstate_buf = midstate_buf << 8;
      end
      
      repeat (32) begin
	 send_byte = data_buf[255:248];
	 repeat(8) begin
	    #2 RxD =  send_byte[0];
    	    send_byte = send_byte >> 1;
    	    repeat (10) @ (posedge sysclk_p);
    	    #2 RxC =  1;
    	    repeat (10) @ (posedge sysclk_p);
    	    #2 RxC =  0;
    	    repeat (10) @ (posedge sysclk_p);
	 end
	 data_buf = data_buf << 8;
	 
      end
      
      #2 RxTxR =  1;
      repeat (10) @(posedge sysclk_p);
      #2 RxTxR =  0;
      repeat (10) @(posedge sysclk_p);

      repeat (100) begin
	 @(posedge sysclk_p);
	 uut.nonce = 0;
      end
      @(posedge sysclk_p);
      //uut.nonce = 30411740 - 10;
      //uut.nonce = 32'h2966e1b9 - 10;
      uut.nonce = 32'h0e33337a - 10;
            
      //read
      read_nonce = 0;
      repeat (4) begin
    	 #2 data_available =  0;
    	 
    	 while (data_available == 0) begin
    	    repeat (10) @ (posedge sysclk_p);
    	    #2 TxC =  1;
    	    repeat (10) @ (posedge sysclk_p);
    	    #2 TxC =  0;
    	    repeat (10) @ (posedge sysclk_p);
    	    #2 data_available =  TxD;
    	 end
    	 read_data = 8'b0;
	 repeat(8) begin
    	    repeat (10) @ (posedge sysclk_p);
    	    #2 TxC =  1;
    	    repeat (10) @ (posedge sysclk_p);
    	    #2 TxC =  0;
    	    repeat (10) @ (posedge sysclk_p);
    	    read_data = {TxD, read_data[7:1]};
    	 end
	 read_nonce = {read_nonce[24:0], read_data};
      end
      
      
      $stop;
      
   end //initial
   
   
   
endmodule

