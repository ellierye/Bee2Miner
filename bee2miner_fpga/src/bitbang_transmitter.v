module bitbang_transmitter(clk, TxD_start, TxR, TxC, TxD_data, TxD, TxD_busy);
   
  input clk, TxD_start;
   
   input [7:0] TxD_data;
   output      TxD, TxD_busy;
   input       TxC, TxR;
   
   // Transmitter state machine
   reg [3:0]   current_bit;
   reg 	       data_available;
   
   
   wire        TxD_ready = (current_bit==0 && !data_available);
   assign TxD_busy = ~TxD_ready;

   //update on positive edge of clock...
   reg 	       TxC_last;
   reg 	       TxC_last_last;
   
   
   always@(posedge clk) begin
      if (TxR) begin 
	 TxC_last <= 1'b0;
	 TxC_last_last <= 1'b0;
	 
      end else begin
	 TxC_last <= TxC;
	 TxC_last_last <= TxC_last;
      end
   end
   
   reg next_bit;
   always@(posedge clk) begin
      if (TxR) begin
	 next_bit <= 1'b0;
      end
      else
	next_bit <= (TxC && ~TxC_last && data_available);
   end
   
   always @(posedge clk)
     if (TxR)
       current_bit <= 4'b0000;
     else
       if (next_bit) current_bit <= (current_bit < 4'b1000) ? current_bit + 1 : 4'b0;
   
   always @(posedge clk) 
     if (TxR)
       data_available <= 1'b0;
     else if (!data_available && TxD_start) 
       data_available <= 1'b1;
     else if (data_available && next_bit &&  current_bit == 4'b1000)
       data_available <= 1'b0;
   
   reg [7:0]   TxD_dataReg;
   always @(posedge clk) 
     if(TxD_ready & TxD_start) TxD_dataReg <= TxD_data;
   
   wire [7:0]  TxD_dataD = TxD_dataReg;
   
   // Put together the start, data and stop bits
   reg 	       TxD;
   always @(posedge clk)
     if (TxR)
       TxD <= 1'b0;
     else if (current_bit == 4'b0 && TxC && ~TxC_last)
       TxD <= data_available;
     else if (next_bit && current_bit != 4'b0)
       TxD <= TxD_dataD[current_bit-1];
   
   
endmodule