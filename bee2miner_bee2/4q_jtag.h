/*************************************************************************
 ** From Perforce:
 **
 ** $Id: //Smart_design/Smash_board/sm_fpga/sm_software/sm_jtag.h#7 $ID$
 ** $DateTime: 2009/06/03 13:47:18 $
 ** $Change: 7780 $
 ** $Author: zasgar $
 *************************************************************************/


#ifndef __SM_JTAG__H_
#define __SM_JTAG__H_

#include "4q_bee2lib.h"

//----------------------------------------JTAG----------------------------------
// JTAG TAP instructions
//---- (taken from $SMASH/rtl/QuadStatDebug/QuadTAP.v)
#define JTAG_INST_LEN    5
#define JTAG_DEVICE_ID   0x53534D01 /*{8'd83, 8'd83, 8'd77, 4'h0, 4'h1}*/

// Encodings for required Instructions
#define JTAG_BYPASS      15 //5'b11111
#define JTAG_EXTEST      1  //5'b00001       
#define JTAG_SAMPLE      2  //5'b00010
#define JTAG_INTEST      4  //5'b00100
#define JTAG_IDCODE      3  //5'b00011

// User Defined JTAG instructions
#define JTAG_CFG_INST    0x10 // 5'b10000
#define JTAG_CFG_ADDR    0x11 // 5'b10001
#define JTAG_CFG_DATA    0x12 // 5'b10010
#define JTAG_JRA_INST    0x13 // 5'b10011
#define JTAG_JRA_ADDR    0x14 // 5'b10100
#define JTAG_JRA_DATA    0x15 // 5'b10101

  

// JTAG Config interface definitions (instructions to the config interface)
//---- (taken from $SMASH/rtl/QuadStatDebug/QuadStatDebug.vh)
#define JTAG_CFG_WRITE 0x0      // 5'b00000
#define JTAG_CFG_READ 0x1       // 5'b00001
#define JTAG_CFG_META_WRITE 0x2 // 5'b00010
#define JTAG_CFG_META_READ 0x3  // 5'b00011

// Jtag Register Array (JRA)
#define JRA_ADDR_WIDTH 8
#define TOTAL_NUM_OF_JRA 2
 
// Possible config instructions
#define JTAG_JRA_WRITE 0x0 // 5'b00000
#define JTAG_JRA_READ 0x1  //5'b00001
#define QUAD_TAP_ID 0
#define PROC_0_TAP_ID 1
#define PROC_1_TAP_ID 2
#define PROC_2_TAP_ID 3
#define PROC_3_TAP_ID 4
#define PROC_4_TAP_ID 5
#define PROC_5_TAP_ID 6
#define PROC_6_TAP_ID 7
#define PROC_7_TAP_ID 8
#define NUM_TAPS 9


/* *************************************
 * Definitions for EXTEST and SAMPLE
 * ************************************* */

/* ***
 * The following definitions are based on a BSR testbench run that 
 * ran on the smash_rel RTL. The file processed was: 
 * /home/shacham/smart_memories/design/Smash_rel/rtl/smash_pad_wrapper.v
 * and the complete BSR testbench is at:
 * /home/shacham/smart_memories/design/Smash_rel/QuadTest/BSR/
 * ***/

//
// Define length of boundary scan registers
//
#define BSR_CHAIN_LENGTH 194

//
// Define the bsr order number, first BSR at MSB, last BSR at LSB
#define BSR_chain_to_sm_ios(bsr,sm_io)				\
  sm_io.PreventReset = bsr[193];					\
  sm_io.QuadBreakIn = bsr[192];						\
  sm_io.CreditEn_in = bsr[191];						\
  sm_io.CreditVC_in[0] = bsr[190];					\
  sm_io.CreditVC_in[1] = bsr[189];					\
  sm_io.CreditVC_in[2] = bsr[188];					\
  sm_io.FlitData_in[0] = bsr[187];					\
  sm_io.FlitData_in[1] = bsr[186];					\
  sm_io.FlitData_in[2] = bsr[185];					\
  sm_io.FlitData_in[3] = bsr[184];					\
  sm_io.FlitData_in[4] = bsr[183];					\
  sm_io.FlitData_in[5] = bsr[182];					\
  sm_io.FlitData_in[6] = bsr[181];					\
  sm_io.FlitData_in[7] = bsr[180];					\
  sm_io.FlitData_in[8] = bsr[179];					\
  sm_io.FlitData_in[9] = bsr[178];					\
  sm_io.FlitData_in[10] = bsr[177];					\
  sm_io.FlitData_in[11] = bsr[176];					\
  sm_io.FlitData_in[12] = bsr[175];					\
  sm_io.FlitData_in[13] = bsr[174];					\
  sm_io.FlitData_in[14] = bsr[173];					\
  sm_io.FlitData_in[15] = bsr[172];					\
  sm_io.FlitData_in[16] = bsr[171];					\
  sm_io.FlitData_in[17] = bsr[170];					\
  sm_io.FlitData_in[18] = bsr[169];					\
  sm_io.FlitData_in[19] = bsr[168];					\
  sm_io.QuadBreakOut = bsr[167];					\
  sm_io.FlitData_in[20] = bsr[166];					\
  sm_io.FlitData_in[21] = bsr[165];					\
  sm_io.FlitData_in[22] = bsr[164];					\
  sm_io.FlitData_in[23] = bsr[163];					\
  sm_io.FlitData_in[24] = bsr[162];					\
  sm_io.FlitData_in[25] = bsr[161];					\
  sm_io.FlitData_in[26] = bsr[160];					\
  sm_io.FlitData_in[27] = bsr[159];					\
  sm_io.FlitData_in[28] = bsr[158];					\
  sm_io.FlitData_in[29] = bsr[157];					\
  sm_io.FlitData_in[30] = bsr[156];					\
  sm_io.FlitData_in[31] = bsr[155];					\
  sm_io.FlitData_in[32] = bsr[154];					\
  sm_io.FlitData_in[33] = bsr[153];					\
  sm_io.FlitData_in[34] = bsr[152];					\
  sm_io.FlitData_in[35] = bsr[151];					\
  sm_io.FlitData_in[36] = bsr[150];					\
  sm_io.FlitData_in[37] = bsr[149];					\
  sm_io.FlitData_in[38] = bsr[148];					\
  sm_io.FlitData_in[39] = bsr[147];					\
  sm_io.FlitData_in[40] = bsr[146];					\
  sm_io.FlitData_in[41] = bsr[145];					\
  sm_io.FlitData_in[42] = bsr[144];					\
  sm_io.FlitData_in[43] = bsr[143];					\
  sm_io.FlitData_in[44] = bsr[142];					\
  sm_io.FlitData_in[45] = bsr[141];					\
  sm_io.FlitData_in[46] = bsr[140];					\
  sm_io.FlitData_in[47] = bsr[139];					\
  sm_io.FlitData_in[48] = bsr[138];					\
  sm_io.FlitData_in[49] = bsr[137];					\
  sm_io.FlitData_in[50] = bsr[136];					\
  sm_io.FlitData_in[51] = bsr[135];					\
  sm_io.FlitData_in[52] = bsr[134];					\
  sm_io.FlitData_in[53] = bsr[133];					\
  sm_io.FlitData_in[54] = bsr[132];					\
  sm_io.FlitData_in[55] = bsr[131];					\
  sm_io.FlitData_in[56] = bsr[130];					\
  sm_io.FlitData_in[57] = bsr[129];					\
  sm_io.FlitData_in[58] = bsr[128];					\
  sm_io.FlitData_in[59] = bsr[127];					\
  sm_io.FlitData_in[60] = bsr[126];					\
  sm_io.FlitData_in[61] = bsr[125];					\
  sm_io.FlitData_in[62] = bsr[124];					\
  sm_io.FlitData_in[63] = bsr[123];					\
  sm_io.FlitData_in[64] = bsr[122];					\
  sm_io.FlitData_in[65] = bsr[121];					\
  sm_io.FlitData_in[66] = bsr[120];					\
  sm_io.FlitData_in[67] = bsr[119];					\
  sm_io.FlitData_in[68] = bsr[118];					\
  sm_io.FlitData_in[69] = bsr[117];					\
  sm_io.FlitData_in[70] = bsr[116];					\
  sm_io.FlitData_in[71] = bsr[115];					\
  sm_io.FlitType_in[0] = bsr[114];					\
  sm_io.FlitType_in[1] = bsr[113];					\
  sm_io.FlitType_in[2] = bsr[112];					\
  sm_io.FlitVC_in[0] = bsr[111];					\
  sm_io.FlitVC_in[1] = bsr[110];					\
  sm_io.FlitVC_in[2] = bsr[109];					\
  sm_io.QuadId[0] = bsr[108];						\
  sm_io.QuadId[1] = bsr[107];						\
  sm_io.QuadId[2] = bsr[106];						\
  sm_io.QuadId[3] = bsr[105];						\
  sm_io.QuadId[4] = bsr[104];						\
  sm_io.host_interrupt = bsr[103];					\
  sm_io.CreditEn_out = bsr[102];					\
  sm_io.CreditVC_out[0] = bsr[101];					\
  sm_io.CreditVC_out[1] = bsr[100];					\
  sm_io.CreditVC_out[2] = bsr[99];					\
  sm_io.FlitData_out[0] = bsr[98];					\
  sm_io.FlitData_out[1] = bsr[97];					\
  sm_io.FlitData_out[2] = bsr[96];					\
  sm_io.FlitData_out[3] = bsr[95];					\
  sm_io.FlitData_out[4] = bsr[94];					\
  sm_io.FlitData_out[5] = bsr[93];					\
  sm_io.FlitData_out[6] = bsr[92];					\
  sm_io.FlitData_out[7] = bsr[91];					\
  sm_io.FlitData_out[8] = bsr[90];					\
  sm_io.FlitData_out[9] = bsr[89];					\
  sm_io.FlitData_out[10] = bsr[88];					\
  sm_io.FlitData_out[11] = bsr[87];					\
  sm_io.FlitData_out[12] = bsr[86];					\
  sm_io.FlitData_out[13] = bsr[85];					\
  sm_io.FlitData_out[14] = bsr[84];					\
  sm_io.FlitData_out[15] = bsr[83];					\
  sm_io.FlitData_out[16] = bsr[82];					\
  sm_io.FlitData_out[17] = bsr[81];					\
  sm_io.FlitData_out[18] = bsr[80];					\
  sm_io.FlitData_out[19] = bsr[79];					\
  sm_io.FlitData_out[20] = bsr[78];					\
  sm_io.FlitData_out[21] = bsr[77];					\
  sm_io.FlitData_out[22] = bsr[76];					\
  sm_io.FlitData_out[23] = bsr[75];					\
  sm_io.FlitData_out[24] = bsr[74];					\
  sm_io.FlitData_out[25] = bsr[73];					\
  sm_io.FlitData_out[26] = bsr[72];					\
  sm_io.FlitData_out[27] = bsr[71];					\
  sm_io.FlitData_out[28] = bsr[70];					\
  sm_io.FlitData_out[29] = bsr[69];					\
  sm_io.FlitData_out[30] = bsr[68];					\
  sm_io.FlitData_out[31] = bsr[67];					\
  sm_io.FlitData_out[32] = bsr[66];					\
  sm_io.FlitData_out[33] = bsr[65];					\
  sm_io.FlitData_out[34] = bsr[64];					\
  sm_io.FlitData_out[35] = bsr[63];					\
  sm_io.FlitData_out[36] = bsr[62];					\
  sm_io.FlitData_out[37] = bsr[61];					\
  sm_io.FlitData_out[38] = bsr[60];					\
  sm_io.FlitData_out[39] = bsr[59];					\
  sm_io.FlitData_out[40] = bsr[58];					\
  sm_io.FlitData_out[41] = bsr[57];					\
  sm_io.FlitData_out[42] = bsr[56];					\
  sm_io.FlitData_out[43] = bsr[55];					\
  sm_io.FlitData_out[44] = bsr[54];					\
  sm_io.FlitData_out[45] = bsr[53];					\
  sm_io.FlitData_out[46] = bsr[52];					\
  sm_io.FlitData_out[47] = bsr[51];					\
  sm_io.FlitData_out[48] = bsr[50];					\
  sm_io.FlitData_out[49] = bsr[49];					\
  sm_io.FlitData_out[50] = bsr[48];					\
  sm_io.FlitData_out[51] = bsr[47];					\
  sm_io.FlitData_out[52] = bsr[46];					\
  sm_io.FlitData_out[53] = bsr[45];					\
  sm_io.FlitData_out[54] = bsr[44];					\
  sm_io.FlitData_out[55] = bsr[43];					\
  sm_io.FlitData_out[56] = bsr[42];					\
  sm_io.FlitData_out[57] = bsr[41];					\
  sm_io.FlitData_out[58] = bsr[40];					\
  sm_io.FlitData_out[59] = bsr[39];					\
  sm_io.FlitData_out[60] = bsr[38];					\
  sm_io.FlitData_out[61] = bsr[37];					\
  sm_io.FlitData_out[62] = bsr[36];					\
  sm_io.FlitData_out[63] = bsr[35];					\
  sm_io.FlitData_out[64] = bsr[34];					\
  sm_io.FlitData_out[65] = bsr[33];					\
  sm_io.FlitData_out[66] = bsr[32];					\
  sm_io.FlitData_out[67] = bsr[31];					\
  sm_io.FlitData_out[68] = bsr[30];					\
  sm_io.FlitData_out[69] = bsr[29];					\
  sm_io.FlitData_out[70] = bsr[28];					\
  sm_io.FlitData_out[71] = bsr[27];					\
  sm_io.FlitType_out[0] = bsr[26];					\
  sm_io.FlitType_out[1] = bsr[25];					\
  sm_io.FlitType_out[2] = bsr[24];					\
  sm_io.FlitVC_out[0] = bsr[23];					\
  sm_io.FlitVC_out[1] = bsr[22];					\
  sm_io.FlitVC_out[2] = bsr[21];					\
  sm_io.IO_Div_in[0] = bsr[20];						\
  sm_io.IO_Div_in[1] = bsr[19];						\
  sm_io.Reset = bsr[18];						\
  sm_io.IOClk = bsr[17];						\
  sm_io.Debug_vec_out[0] = bsr[16];					\
  sm_io.Debug_vec_out[1] = bsr[15];					\
  sm_io.Debug_vec_out[2] = bsr[14];					\
  sm_io.Debug_vec_out[3] = bsr[13];					\
  sm_io.Debug_vec_out[4] = bsr[12];					\
  sm_io.Debug_vec_out[5] = bsr[11];					\
  sm_io.Debug_vec_out[6] = bsr[10];					\
  sm_io.Debug_vec_out[7] = bsr[9];					\
  sm_io.Debug_vec_out[8] = bsr[8];					\
  sm_io.Debug_vec_out[9] = bsr[7];					\
  sm_io.Debug_vec_out[10] = bsr[6];					\
  sm_io.Debug_vec_out[11] = bsr[5];					\
  sm_io.DebugMuxSel[0] = bsr[4];					\
  sm_io.DebugMuxSel[1] = bsr[3];					\
  sm_io.DebugMuxSel[2] = bsr[2];					\
  sm_io.DebugMuxSel[3] = bsr[1];					\
  sm_io.DebugMuxSel[4] = bsr[0]; // END OF MACRO "BSR_chain_to_sm_outputs"

//
// Define a variable BSR_PORT_DIRECTION which specifies I/O direction
// note 1 is input, 0 is output
//
#define BSR_PORT_DIRECTION {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1}





//JTAG basic functions
void set_JTCK(int val, int fd);
void set_JTRST(int val, int fd);
void set_JTDI(int val, int fd);
void set_JTMS(int val, int fd);
int get_JTDO(int fd);

// jtag mid layer functions
void pulse_JTCK(int fd);
void reset_jtag(int quad, int fd);
void shift_jtag_inst(int quad, char* tap_inst_in, char* tap_inst_out, int tap_idx, int fd);
void shift_jtag_data(int quad, int len, char* data_in, char* data_out, int tap_idx, int fd);

// auxiliary functions
void IntToBytes(char* target, int len, int src);
int BytesToInt(char* src, int len);


// Top level function to read device ID
int jtagGetDeviceID(int quad, int fd);

// Top level functions for config writes and reads
void jtagWrite(int fd, int quad, int* addr, int isMeta, int data_in);
int jtagRead(int fd, int quad, int* addr, int isMeta);




#endif /* __SM_JTAG__H_ */
