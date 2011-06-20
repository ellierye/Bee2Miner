/*************************************************************************
 ** From Perforce:
 **
 ** $Id: //Smart_design/Smash_board/sm_fpga/sm_software/sm_jtag.c#6 $ID$
 ** $DateTime: 2009/06/03 13:47:18 $
 ** $Change: 7780 $
 ** $Author: zasgar $
 *************************************************************************/

#include  "4q_bee2lib.h"
#include "4q_jtag.h"
#include "4q_ppc_user_logic.h"

// -------------------------------------------JTAG------------------------------
//JTAG basic functions ... these assume MODE is already set correctly.
void set_JTCK(int val, int fd){
  write_reg(Q4_W_ADDR_JTCK, val, fd);
}

void set_JTRST(int val, int fd){
  write_reg(Q4_W_ADDR_JTRST, val, fd);
}

void set_JTDI(int val, int fd){
  write_reg (Q4_W_ADDR_JTDI, val, fd);
}

void set_JTMS(int val, int fd){
  write_reg(Q4_W_ADDR_JTMS, val, fd);
}

int get_JTDO(int fd){
  unsigned long read_data;
  read_reg(Q4_R_ADDR_CTRL_SIGNALS, &read_data, fd);
  return ((read_data >> 10) &0x1);
}


// function to reset the jtag state machine
void pulse_JTCK(int fd){
  set_JTCK(1, fd);
  set_JTCK(0, fd);  
}

// function to pulse the jtck clk
void reset_jtag(int quad, int fd){
  int i=0;
  unsigned long mode = Q4_MODE_SM_CTRL;
  mode |= (1 << (quad + 3));
  mode |= (1 << (WRITE_ACTIVE_QUADS_BIT));
  write_reg(Q4_W_ADDR_MODE, mode, fd);
  // drive all values async to zero
  set_JTMS(1, fd);
  set_JTDI(0, fd);
  set_JTRST(0,fd);
  
  // Now start the clock
  for (i=0; i<100; i++){
    pulse_JTCK(fd);
  }
  
  // now un-reset the jtag interface
  set_JTRST(1,fd);
  for (i=0; i<100; i++){
    pulse_JTCK(fd);
  }

  // Move to run test idle state
  set_JTMS(0, fd);
  for (i=0; i<100; i++){
    pulse_JTCK(fd);
  }
}


// write the TAP instruction in
// This function assume that tap_inst_in and tap_inst_out are of size JTAG_INST_LEN
void shift_jtag_inst(int quad, char* tap_inst_in, char* tap_inst_out, int tap_idx, int fd){
  int i = 0, j = 0;
  char all_TAPs_inst[NUM_TAPS*JTAG_INST_LEN];
  char all_TAPs_inst_out[NUM_TAPS*JTAG_INST_LEN];

  unsigned long mode = Q4_MODE_SM_CTRL;
  mode |= (1 << (quad + 3));
  mode |= (1 << (WRITE_ACTIVE_QUADS_BIT));
  write_reg(Q4_W_ADDR_MODE, mode, fd);
    
  // reset all taps to bypass
  for (i=0; i<NUM_TAPS; i++){
    for (j=0; j<JTAG_INST_LEN; j++){
      all_TAPs_inst[ i*JTAG_INST_LEN + j ] = (JTAG_BYPASS >> j) & 0x1;
    }
  }

  // put the relevant instruction in place
  for (j=0; j<JTAG_INST_LEN; j++){
    all_TAPs_inst[(NUM_TAPS-(tap_idx+1))*JTAG_INST_LEN + j] = tap_inst_in[j];
  }
  // Move to Run-Test/Idle state (if not already there)
  pulse_JTCK(fd);
  set_JTMS(0, fd);
  pulse_JTCK(fd);
  pulse_JTCK(fd);
  pulse_JTCK(fd);

  /***** IR Path *****/
  
  // Move to Select-DR-Scan state
  set_JTMS(1, fd);
  pulse_JTCK(fd);
  
  // Move to Select-IR-Scane state
  set_JTMS(1, fd);
  pulse_JTCK(fd);
  
  // Move to Capture IR state
  set_JTMS(0, fd);
  pulse_JTCK(fd);
  
  // Move to Shift-IR state
  set_JTMS(0, fd);
  pulse_JTCK(fd);
  
  // Remain in Shift-IR state and shift in the instruction
  for (i = 0; i < (NUM_TAPS*JTAG_INST_LEN-1); i = i + 1)
  {
    set_JTDI((int)all_TAPs_inst[i], fd);
    all_TAPs_inst_out[i] =  (char)get_JTDO(fd); //os
    pulse_JTCK(fd);
    //os    all_TAPs_inst_out[i] =  (char)get_JTDO(fd); 
  }
  
  // Shift in the last bit and switch to Exit1-IR state
  set_JTMS(1, fd);
  set_JTDI((int)all_TAPs_inst[(NUM_TAPS*JTAG_INST_LEN-1)], fd);
  all_TAPs_inst_out[(NUM_TAPS*JTAG_INST_LEN-1)] = (char)get_JTDO(fd); //os
  pulse_JTCK(fd);
  //os  all_TAPs_inst_out[(NUM_TAPS*JTAG_INST_LEN-1)] = (char)get_JTDO(fd); 

  // Move to Update-IR state
  set_JTMS(1, fd);
  pulse_JTCK(fd);

  // Move to Run-Test/Idle state
  set_JTMS(0, fd);
  pulse_JTCK(fd);
  
  // the output is only part of the padded data out
  for (j=0; j<JTAG_INST_LEN; j++){
    tap_inst_out[j] = all_TAPs_inst_out[ (NUM_TAPS-(tap_idx+1))*JTAG_INST_LEN + j];
  }
  pulse_JTCK(fd); 

}




// write the jtag data in
// This function assumes that data_in and data_out are of size len
void shift_jtag_data(int quad, int len, char* data_in, char* data_out, int tap_idx, int fd){
  int i = 0;
  int total_length = len+NUM_TAPS-1;
  char paded_data_in[total_length];
  char paded_data_out[total_length];
  
  unsigned long mode = Q4_MODE_SM_CTRL;
  mode |= (1 << (quad + 3));
  mode |= (1 << (WRITE_ACTIVE_QUADS_BIT));
  write_reg(Q4_W_ADDR_MODE, mode, fd);
  
  // zero the vector
  for (i=0; i<total_length; i++){
    paded_data_in[i] = 0;
  }

  // assign the input data at the right place
  for (i=0; i<len; i++){
    paded_data_in[NUM_TAPS-(tap_idx+1) + i] = data_in[i];
  }

  // Move to Run-Test/Idle state (if not already there)
  pulse_JTCK(fd);
  set_JTMS(0, fd);
  pulse_JTCK(fd);
  pulse_JTCK(fd);
  pulse_JTCK(fd);


  /***** DR PATH *****/
  // Move to Select-DR-Scan state
  set_JTMS(1, fd);
  pulse_JTCK(fd);
  
  // Move to Capture DR state
  set_JTMS(0, fd);
  pulse_JTCK(fd);
  
  // Move to Shift-DR state
  set_JTMS(0, fd);
  pulse_JTCK(fd);

  // Remain in Shift-DR state and shift in paded_data_in. Observe the TDO signal to read the paded_data_out
  for (i = 0; i < total_length-1; i = i + 1){
    set_JTDI((int)paded_data_in[i], fd);
    paded_data_out[i] = (char)get_JTDO(fd); //os
    pulse_JTCK(fd);
    //os    paded_data_out[i] = (char)get_JTDO(fd); 
  }
  
  // Shift in the last bit and switch to Exit1-DR state
  set_JTDI((int)paded_data_in[(total_length-1)], fd);
  set_JTMS(1, fd);
  paded_data_out[(total_length-1)] = (char)get_JTDO(fd); //os
  pulse_JTCK(fd);
  //os  paded_data_out[(total_length-1)] = (char)get_JTDO(fd); 
  
  
  // Move to Update-DR state
  set_JTMS(1, fd);
  pulse_JTCK(fd);
  
  // Move to Run-Test/Idle state 
  set_JTMS(0, fd);
  pulse_JTCK(fd); 

  // the output is only part of the padded data out
  for (i=0; i<len; i++){
    data_out[i] = paded_data_out[NUM_TAPS-(tap_idx+1)+i];
  }
  pulse_JTCK(fd); 


}


// An auxiliary function to generate a byte stream from an int
void IntToBytes(char* target, int len, int src){
  int idx;
  for (idx=0; idx<len; idx++){
    target[idx] = (src >> idx) & 0x1;
  }
}

// An auxiliary function to generate an int from a byte stream 
int BytesToInt(char* src, int len){
  int idx;
  int target=0;
  for (idx=0; idx<len; idx++){
    target = (target << 1) + src[len -1 - idx];
  }
  return target;
}

// Top level function to read device ID
int jtagGetDeviceID(int quad, int fd){
  char tap_inst_in[JTAG_INST_LEN];
  char tap_inst_out[JTAG_INST_LEN];
  char data_in[32];
  char data_out[32];
  int idcode=0;

  unsigned long mode = Q4_MODE_SM_CTRL;
  mode |= (1 << (quad + 3));
  mode |= (1 << (WRITE_ACTIVE_QUADS_BIT));
  write_reg(Q4_W_ADDR_MODE, mode, fd);
  // set the instruction
  IntToBytes(tap_inst_in, JTAG_INST_LEN, JTAG_IDCODE);

  // shift in/out the instruction
  shift_jtag_inst(quad, tap_inst_in, tap_inst_out, QUAD_TAP_ID, fd);

  // reset the data in
  IntToBytes(data_in, 32, 0);

  // shift in/out the data
  shift_jtag_data(quad, 32, data_in, data_out, QUAD_TAP_ID, fd);
  idcode = BytesToInt(data_out, 32);
  return idcode;
}


// Top level functions for config writes and reads
void jtagWrite(int fd, int quad, int* addr, int isMeta, int data_in){
  char tap_inst_in[JTAG_INST_LEN];
  char tap_inst_out[JTAG_INST_LEN];
  char data_as_bytes_in[32];
  char data_as_bytes_out[32];
  
  int cfg_inst = (isMeta)? JTAG_CFG_META_WRITE:JTAG_CFG_WRITE;

  unsigned long mode = Q4_MODE_SM_CTRL;
  mode |= (1 << (quad + 3));
  mode |= (1 << (WRITE_ACTIVE_QUADS_BIT));
  write_reg(Q4_W_ADDR_MODE, mode, fd);
  
  // DoJtagCfgWriteAddr();
  IntToBytes(tap_inst_in, JTAG_INST_LEN, JTAG_CFG_ADDR);
  shift_jtag_inst(quad, tap_inst_in, tap_inst_out, QUAD_TAP_ID, fd);
  IntToBytes(data_as_bytes_in, 32, (int)addr);
  shift_jtag_data(quad, 32, data_as_bytes_in, data_as_bytes_out, QUAD_TAP_ID, fd);

  // DoJtagCfgWriteData();
  IntToBytes(tap_inst_in, JTAG_INST_LEN, JTAG_CFG_DATA);
  shift_jtag_inst(quad, tap_inst_in, tap_inst_out, QUAD_TAP_ID, fd);
  IntToBytes(data_as_bytes_in, 32, data_in);
  shift_jtag_data(quad, 32, data_as_bytes_in, data_as_bytes_out, QUAD_TAP_ID, fd);

  // DoJtagCfgWriteInstr();
  IntToBytes(tap_inst_in, JTAG_INST_LEN, JTAG_CFG_INST);
  shift_jtag_inst(quad, tap_inst_in, tap_inst_out, QUAD_TAP_ID, fd);
  IntToBytes(data_as_bytes_in, 5, cfg_inst);
  shift_jtag_data(quad, 5, data_as_bytes_in, data_as_bytes_out, QUAD_TAP_ID, fd);
}

int jtagRead(int fd, int quad, int* addr, int isMeta){
  unsigned int data_out;

  char tap_inst_in[JTAG_INST_LEN];
  char tap_inst_out[JTAG_INST_LEN];
  char data_as_bytes_in[32];
  char data_as_bytes_out[32];
  
  int cfg_inst = (isMeta)? JTAG_CFG_META_READ:JTAG_CFG_READ;
  unsigned long mode = Q4_MODE_SM_CTRL;
  mode |= (1 << (quad + 3));
  mode |= (1 << (WRITE_ACTIVE_QUADS_BIT));
  
  write_reg(Q4_W_ADDR_MODE, mode, fd);
  
  // DoJtagCfgWriteAddr();
  IntToBytes(tap_inst_in, JTAG_INST_LEN, JTAG_CFG_ADDR);
  shift_jtag_inst(quad, tap_inst_in, tap_inst_out, QUAD_TAP_ID, fd);
  IntToBytes(data_as_bytes_in, 32, (int)addr);
  shift_jtag_data(quad, 32, data_as_bytes_in, data_as_bytes_out, QUAD_TAP_ID, fd);

  // DoJtagCfgWriteInstr();
  IntToBytes(tap_inst_in, JTAG_INST_LEN, JTAG_CFG_INST);
  shift_jtag_inst(quad, tap_inst_in, tap_inst_out, QUAD_TAP_ID, fd);
  IntToBytes(data_as_bytes_in, 5, cfg_inst);
  shift_jtag_data(quad, 5, data_as_bytes_in, data_as_bytes_out, QUAD_TAP_ID, fd);

  // data_out = DoJtagCfgReadData();
  IntToBytes(tap_inst_in, JTAG_INST_LEN, JTAG_CFG_DATA);
  shift_jtag_inst(quad, tap_inst_in, tap_inst_out, QUAD_TAP_ID, fd);
  IntToBytes(data_as_bytes_in, 32, 0); // send zeros in
  shift_jtag_data(quad, 32, data_as_bytes_in, data_as_bytes_out, QUAD_TAP_ID, fd);

  data_out = BytesToInt(data_as_bytes_out, 32);
  return data_out;
}
