
module bitbang_receiver(clk, RxD, RxC, RxR, RxD_data_ready, RxD_data, RxD_idle);
   input clk, RxD, RxC, RxR;
   output RxD_data_ready;  // onc clock pulse when RxD_data is valid
   output [7:0] RxD_data;
   
   output 	RxD_idle;  // no data is being received
   
   ////////////////////////////
      
   reg [2:0] current_bit;

   //sample on positive edge of clock...
   reg 	     RxC_last;
   always@(posedge clk) begin
      if (RxR) 
	 RxC_last <= 1'b0;
      else
	RxC_last <= RxC;
   end
   
   reg next_bit;
   always@(posedge clk) begin
      if (RxR) begin
	 next_bit <= 1'b0;
      end
      else
	next_bit <= RxC && !RxC_last;
   end
      
   always @(posedge clk)
     if (RxR)
       current_bit <= 3'b000;
     else 
       if (next_bit) current_bit <= current_bit + 1;
   
   reg [7:0] RxD_data;
   always @(posedge clk)
     if (next_bit) 
       RxD_data <= {RxD, RxD_data[7:1]};
   
   reg 	     RxD_data_ready, RxD_data_error;
   
   always @(posedge clk)
     RxD_data_ready <= (next_bit && current_bit==3'b111);  

   
   assign RxD_idle = current_bit == 3'b000;
   
endmodule