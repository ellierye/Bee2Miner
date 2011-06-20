#ifndef __BEE2LIB__H_
#define __BEE2LIB__H_

#include <stdio.h> 
#include <fcntl.h> 
#include <errno.h> 
#include <unistd.h> 
#include <string.h> 

#include <sys/ioctl.h> 
#include <sys/types.h> 

#include "coredrv.h" 


// Global declaration of the stdout.log file handle
int sout_handle;

// Global declaration of the main loop variable
int keep_running;

// Not sure if this is the best place for these globals but I'll leave them here anyway (shacham)
int cpuArr[32];
int cpuExitStatus[32];
int numExits;
int numCPUs;



// CoreDrv utility functions
int open_coredev (char *name, int base, int length, int width, int *fd);

// Access PPC registers
void write_reg (int index, unsigned long data, int fd);
void read_reg (int index, unsigned long *data, int fd);
//int writeTileAddr (unsigned long addr, unsigned long writeData, int TileId, int meta, int ppc_fd);
//void reset_fifo(unsigned int fifo_num, int fd);
//void read_fifo(unsigned int fifo_num, int fd);

// DDR memory access routines
//void write_mem_data (unsigned long addr, unsigned long data, int fd, int ppc_fd);
//void write_mem_meta (unsigned long addr, unsigned long meta, int fd, int ppc_fd);
//void write_mem (unsigned long addr, unsigned long data, unsigned long meta, int fd, int ppc_fd);
//void read_mem_data (unsigned long addr, unsigned long *data, int fd, int ppc_fd);
//void read_mem_meta (unsigned long addr, unsigned long *meta, int fd, int ppc_fd);
//void read_mem (unsigned long addr, unsigned long *data, unsigned long *meta, int fd, int ppc_fd);

//unsigned long copyStrTo_SM_Mem (char *str, unsigned long addr, int fd, int ppc_fd);

//enum addr_type {DATA_ADDR, META_ADDR}; 
//unsigned long map_address (unsigned long addr, enum addr_type type, int* mcNum);

///*base address of the segment*/
//extern unsigned long seg_base[16];
///*mask to prevent overflow of the segment (see below)*/
//extern unsigned long seg_mask[16];
///*size of the segment in MB*/
//extern unsigned long seg_size[16];
//
///*Information necessary for computing
//* actual memory locations.*/
//extern int numMCs;
//extern int numQuads;
//extern int interleavingFactor;

extern int DEBUG;

//void configure_memory_map(int ppc_fd);
//void read_memory_map(int ppc_fd);
//void read_and_save_memory_map(int ppc_fd);

// General Utility functions
unsigned long htoi(char *str); 
inline int isHex(char *ptr);
unsigned long reverse_bytes (unsigned long in);
 


////-----------------SM IO Testing --------------------
//typedef struct sm_io{
//  // outputs:
//  char Debug_vec_out[12];
//  char QuadBreakOut;
//  char host_interrupt;
////os  char JTDO;
//  char IOClk;
//  char CreditEn_out;
//  char FlitType_out[3];
//  char FlitVC_out[3];
//  char CreditVC_out[3];
//  char FlitData_out[72];
//
//  // inputs:
//  char PreventReset;
//  char QuadBreakIn;
//  char CreditEn_in;
//  char CreditVC_in[3];
//  char FlitData_in[72];
//  char FlitType_in[3];
//  char FlitVC_in[3];
//  char QuadId[5];
//  char IO_Div_in[2];
//  char Reset; 
//  char DebugMuxSel[5];
//}sm_io_t;
//
//
//
//void zero_sim_io(sm_io_t * ios_ptr); 
//void read_sm_outputs(sm_io_t * ios_ptr, int fd);  
//void mc_drive_sm_inputs(int ppc_fd);
//void write_sm_inputs(sm_io_t * ios_ptr, int ppc_fd);
//int compare_sm_outputs(sm_io_t * fpga_side_ios_ptr, sm_io_t * jtag_side_ios_ptr);
//int compare_sm_inputs(sm_io_t * fpga_side_ios_ptr, sm_io_t * jtag_side_ios_ptr);

  


#endif
