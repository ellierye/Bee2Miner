#include "4q_bee2lib.h"
#include "4q_ppc_user_logic.h"

int DEBUG = 0;

// CoreDrv utility functions
int open_coredev (char *name, int base, int length, int width, int *fd)
{

  int dev_fd;

  /* open device file for instr memory range */ 
  if (length/(1024*1024) > 256){
    printf("Unable to open %s: length (0x%x) is too big", name, length);
    return 1;
  }
  if ((dev_fd = open(name, O_RDWR)) < 0) { 
    printf("Unable to open %s: %s\n", name, strerror(errno)); 
    return 1; 
  } 

  /* setup the device */ 
  if (ioctl(dev_fd, IOC_BASE_WR, &base) < 0) { 
    printf("Unable to set base address 0x%08x: %s\n", 
	   base, strerror(errno)); 
    return 2; 
  } 
  if (ioctl(dev_fd, IOC_LENGTH_WR, &length) < 0) { 
    printf("Unable to set the address range length 0x%08x: %s\n", 
	   length, strerror(errno)); 
    return 2; 
  } 
  if (ioctl(dev_fd, IOC_WIDTH_WR, &width) < 0) { 
    printf("Unable to set the access width %d: %s\n", 
	   width, strerror(errno)); 
    return 2; 
  }

  *fd = dev_fd;
  return 0;
}

// Access PPC registers
void write_reg (int index, unsigned long data, int fd)
{
  lseek(fd, index * 4, SEEK_SET);
  write(fd, &data, 4);  
}

void read_reg (int index, unsigned long *data, int fd)
{
  lseek(fd, index * 4, SEEK_SET);
  read(fd, data, 4);  
}

////ASSUMES THAT YOU HAVE ALREADY SET OFFCHIP access mode and the correct user flags!
//void write_offchip_reg(int index, unsigned long data, int fd){
//  write_reg(Q4_OFFCHIP_W_ADDR_WDATA, data, fd);
//  write_reg(Q4_OFFCHIP_W_ADDR_WCE, (0x1 << index), fd);
//  
//  //!!! TODO: poll busy
//  int busy = 1;
//  while (busy){
//    read_reg(Q4_OFFCHIP_R_ADDR_BUSY, &busy, fd);
//  }
//}
//
//void read_offchip_reg(int index, unsigned long * data, int fd){
//  write_reg(Q4_OFFCHIP_W_ADDR_RCE, (0x1 << index), fd);
//  int busy = 1;
//  while (busy){
//    read_reg(Q4_OFFCHIP_R_ADDR_BUSY, &busy, fd);
//  }
//  read_reg(Q4_OFFCHIP_R_ADDR_RDATA, data, fd);
//}
//
//// DDR memory access routines
//void write_mem_data (unsigned long addr, unsigned long data, int fd, int ppc_fd)
//{
// 
//  int addr64 = addr & 0xfffffff8;
//
//  int upper = addr & 0x4;
//  int dmask = upper ? 0xf0 : 0x0f;
//  int mmask = 0;
//  
//  int iaddr;
//  int mcNum;
//
//  // ASSUMES INTERLEAVING FACTOR IS ALWAYS 1 (32-byte)
//  switch(numMCs){
//  case(1): 
//    mcNum = 0;
//    iaddr = addr64;
//    break;
//  case(2): 
//    mcNum = (addr64 >> (1 + 4)) & 0x1;
//    iaddr = ((addr64 & 0xFFFFFFC0) >> 1) | (addr64 & 0x1F);
//    break;
//  case(4): 
//    mcNum = (addr >> (1 + 4)) & 0x3;
//    iaddr = ((addr64 & 0xFFFFFF80) >> 2) | (addr64 & 0x1F);
//    break;
//  default : 
//    printf (" Invalid # of MC's (%d)\n", numMCs);
//    exit(1);
//  }
//  // TO ME: ia = {ra[31:(cfg_ilv_fctr+4+cfg_num_MCs)], ra[cfg_ilv_fctr+4-1:0]}
//  //real_addr = (cfg_num_MCs)? /* only if there is more then one MC */
//  //{addr[31:(cfg_ilv_fctr+4)], mc_number[cfg_num_MCs-1:0], addr[cfg_ilv_fctr+4-1:0]}:
//  //addr;
//  
//  int read_data = 0;
//  //addr
//  unsigned long mode = Q4_MODE_OFFCHIP;
//  mode |= (1 << (mcNum + 3));
//  mode |= (1 << (WRITE_ACTIVE_QUADS_BIT));
//  write_reg(Q4_W_ADDR_MODE, mode, ppc_fd);
//  write_offchip_reg( Q4_W_ADDR_MODE, Q4_MODE_MEM_BD, ppc_fd);
//  write_offchip_reg( Q4_MEM_W_ADDR_ADDR, iaddr,  ppc_fd);
//  //data
//  read_offchip_reg(Q4_MEM_R_ADDR_ADDR, &read_data, ppc_fd);
//  if (upper) {
//    write_offchip_reg( Q4_MEM_W_ADDR_WDATA_H, data, ppc_fd);
//  } else{
//    write_offchip_reg( Q4_MEM_W_ADDR_WDATA_L, data, ppc_fd);
//  }
//  //mask
//  write_offchip_reg( Q4_MEM_W_ADDR_DMASK, dmask, ppc_fd);
//  // ensure no meta data is written.
//  write_offchip_reg( Q4_MEM_W_ADDR_MMASK, 0, ppc_fd);
//  //do a write
//  write_offchip_reg( Q4_MEM_W_ADDR_RW, 1, ppc_fd);
//  // go!
//  write_offchip_reg( Q4_MEM_W_ADDR_GO, 1, ppc_fd);
//  // poll on busy
//  int busy = 1;
//
//  while (busy) {
//    read_offchip_reg(Q4_MEM_R_ADDR_BUSY, &busy, ppc_fd);
//  }
//
//    
//}
//
//void write_mem_meta (unsigned long addr, unsigned long meta, int fd, int ppc_fd)
//{
//  unsigned long mapped_addr;
//  
//  int addr64 = addr & 0xfffffff8;
//  int upper = addr & 0x4;
//  int dmask = 0;
//  int mmask = upper? 0x2:0x1;
//  int mdata = upper? (meta << 6) : meta;
//  int read_data = 0;
//  
//  int mcNum;
//  int iaddr;
//
//  // ASSUMES INTERLEAVING FACTOR IS ALWAYS 1 (32-byte)
//  switch(numMCs){
//  case(1): 
//    mcNum = 0;
//    iaddr = addr64;
//    break;
//  case(2): 
//    mcNum = (addr64 >> (1 + 4)) & 0x1;
//    iaddr = ((addr64 & 0xFFFFFFC0) >> 1) | (addr64 & 0x1F);
//    break;
//  case(4): 
//    mcNum = (addr >> (1 + 4)) & 0x3;
//    iaddr = ((addr64 & 0xFFFFFF80) >> 2) | (addr64 & 0x1F);
//    break;
//  default : 
//    printf (" Invalid # of MC's\n");
//    exit(1);
//  }
//
//  unsigned long mode = Q4_MODE_OFFCHIP;
//  mode |= (1 << (mcNum + 3));
//  mode |= (1 << (WRITE_ACTIVE_QUADS_BIT));
//  write_reg(Q4_W_ADDR_MODE, mode, ppc_fd);
//  //addr
//  write_offchip_reg( Q4_W_ADDR_MODE, Q4_MODE_MEM_BD, ppc_fd);
//  write_offchip_reg( Q4_MEM_W_ADDR_ADDR, iaddr, ppc_fd);
//  //data - don't write it
//  //mask
//  write_offchip_reg( Q4_MEM_W_ADDR_DMASK, dmask, ppc_fd);
//  //meta
//  write_offchip_reg( Q4_MEM_W_ADDR_WMETA, mdata, ppc_fd);
//  // meta mask
//  write_offchip_reg( Q4_MEM_W_ADDR_MMASK, mmask, ppc_fd);
//  //do a write
//  write_offchip_reg( Q4_MEM_W_ADDR_RW, 1, ppc_fd);
//  // go!
//  write_offchip_reg( Q4_MEM_W_ADDR_GO, 1, ppc_fd);
//  // poll on busy
//  int busy = 1;
//  while (busy) {
//    read_offchip_reg(Q4_MEM_R_ADDR_BUSY, &busy, ppc_fd);
//  }
//  
//}
//
//// DDR memory access routines
//void write_mem (unsigned long addr, unsigned long data, unsigned long meta, int fd, int ppc_fd)
//{
//  
//  write_mem_data (addr, data, fd, ppc_fd);
//  write_mem_meta (addr, meta, fd , ppc_fd);
//  
//
//}
//
//void read_mem_data (unsigned long addr, unsigned long *data, int fd, int ppc_fd)
//{
//   int addr64 = addr & 0xfffffff8;
//   int upper = addr & 0x4;
//   
//   int iaddr;
//   int mcNum;
//   
//   // ASSUMES INTERLEAVING FACTOR IS ALWAYS 1 (32-byte)
//   switch(numMCs){
//   case(1): 
//     mcNum = 0;
//     iaddr = addr64;
//     break;
//   case(2): 
//     mcNum = (addr64 >> (1 + 4)) & 0x1;
//     iaddr = ((addr64 & 0xFFFFFFC0) >> 1) | (addr64 & 0x1F);
//     break;
//   case(4): 
//     mcNum = (addr >> (1 + 4)) & 0x3;
//     iaddr = ((addr64 & 0xFFFFFF80) >> 2) | (addr64 & 0x1F);
//     break;
//   default : 
//     printf (" Invalid # of MC's\n");
//     exit(1);
//   }
//
//   unsigned long mode = Q4_MODE_OFFCHIP;
//   mode |= (1 << (mcNum + 3));
//   mode |= (1 << (WRITE_ACTIVE_QUADS_BIT));
//   write_reg(Q4_W_ADDR_MODE, mode, ppc_fd);
//   //addr
//   write_offchip_reg( Q4_W_ADDR_MODE, Q4_MODE_MEM_BD, ppc_fd);
//   write_offchip_reg( Q4_MEM_W_ADDR_ADDR, iaddr, ppc_fd);
//   //do a read
//   write_offchip_reg( Q4_MEM_W_ADDR_RW, 0, ppc_fd);
//   // go!
//
//   write_offchip_reg( Q4_MEM_W_ADDR_GO, 1, ppc_fd);
//   // poll on busy
//   int busy = 1;
//   while (busy) {
//     read_offchip_reg(Q4_MEM_R_ADDR_BUSY, &busy, ppc_fd);
//    
//   }
//
//   // read the result
//   if (upper){
//     read_offchip_reg(Q4_MEM_R_ADDR_RDATA_H, data, ppc_fd);
//   } else{
//     read_offchip_reg(Q4_MEM_R_ADDR_RDATA_L, data, ppc_fd);
//   }
//
//}
//
//void read_mem_meta (unsigned long addr, unsigned long *meta, int fd, int ppc_fd)
//{
//
//  int addr64 = addr & 0xfffffff8;
//  int upper = addr & 0x4;
//  
//  int iaddr;
//  int mcNum;
//  
//  // ASSUMES INTERLEAVING FACTOR IS ALWAYS 1 (32-byte)
//  switch(numMCs){
//  case(1): 
//    mcNum = 0;
//    iaddr = addr64;
//    break;
//  case(2): 
//    mcNum = (addr64 >> (1 + 4)) & 0x1;
//    iaddr = ((addr64 & 0xFFFFFFC0) >> 1) | (addr64 & 0x1F);
//    break;
//  case(4): 
//    mcNum = (addr >> (1 + 4)) & 0x3;
//    iaddr = ((addr64 & 0xFFFFFF80) >> 2) | (addr64 & 0x1F);
//    break;
//  default : 
//    printf (" Invalid # of MC's\n");
//    exit(1);
//  }
//  
//  unsigned long mode = Q4_MODE_OFFCHIP;
//  mode |= (1 << (mcNum + 3));
//  mode |= (1 << (WRITE_ACTIVE_QUADS_BIT));
//  
//  write_offchip_reg( Q4_W_ADDR_MODE, mode, ppc_fd);
//  //addr
//  write_offchip_reg( Q4_W_ADDR_MODE, Q4_MODE_MEM_BD, ppc_fd);
//  write_offchip_reg( Q4_MEM_W_ADDR_ADDR, iaddr, ppc_fd);
//  //do a read
//  write_offchip_reg( Q4_MEM_W_ADDR_RW, 0, ppc_fd);
//  // go!
//  write_offchip_reg( Q4_MEM_W_ADDR_GO, 1, ppc_fd);
//  // poll on busy
//  int busy = 1;
//  while (busy) {
//    read_offchip_reg(Q4_MEM_R_ADDR_BUSY, &busy, ppc_fd);
//  }
//  // read the result
//  int rmeta;
//  read_offchip_reg(Q4_MEM_R_ADDR_RMETA, &rmeta, ppc_fd);
//  if (upper){
//    *meta = rmeta >> 6;
//  }
//  else{
//    *meta = rmeta & 0x3f;
//  } 
//    
//}
//
//void read_mem (unsigned long addr, unsigned long *data, unsigned long *meta, int fd, int ppc_fd)
//{
//  int addr64 = addr & 0xfffffff8;
//  int upper = addr & 0x4;
//  /*
//  //addr
//  write_reg(Q4_W_ADDR_MODE, Q4_MODE_MEM_BD, ppc_fd);
//  write_reg(Q4_MEM_W_ADDR_ADDR, addr64,  ppc_fd);
//  //do a read
//  write_reg(Q4_MEM_W_ADDR_RW, 0, ppc_fd);
//  // go!
//  write_reg(Q4_MEM_W_ADDR_GO, 1, ppc_fd);
//  // poll on busy
//  int busy = 1;
//  while (busy) {
//    read_reg(Q4_MEM_R_ADDR_BUSY, &busy, ppc_fd);
//  }
//  // read the result
//  if (upper){
//    read_reg(Q4_MEM_R_ADDR_RDATA_H, data, ppc_fd);
//  } else{
//    read_reg(Q4_MEM_R_ADDR_RDATA_L, data, ppc_fd);
//  }
//   // read the result
//  int rmeta;
//  read_reg(Q4_MEM_R_ADDR_RMETA, &rmeta, ppc_fd);
//  if (upper){
//    *meta = rmeta >> 6;
//  }
//  else{
//    *meta = rmeta & 0x3f;
//  }
//  */
//
//  read_mem_data (addr, data, fd, ppc_fd);
//  read_mem_meta (addr, meta, fd, ppc_fd);
//
//}
//
//unsigned long copyStrTo_SM_Mem (char *str, unsigned long addr, int fd, int ppc_fd) {
//
//  int length = strlen(str);
//  int int_str_copy[64] = {0};
//  char * str_copy = (char*) int_str_copy;
//  if (length > 255){
//    printf("ERROR: string is too long to copy to SM (%s)\n", 
//	   str);
//    exit(1);
//  }
//  strcpy(str_copy, str);
//
//  int *str_ptr = (int *) str_copy;
//  int i;
//  
//  for (i=0; (i*4)<length; i++)
//  {
//    write_mem_data(addr, reverse_bytes(str_ptr[i]), fd, ppc_fd);
//    addr += 4;
//  }
//  //Ensure that there is a null terminating character.
//  write_mem_data(addr, 0, fd, ppc_fd);
//  addr += 4;  
//  return addr;
//}
//
///*INTIAL VALUES CORRESPOND TO THE HW*/
//unsigned long seg_base[4][16] ={
//
//  0x3c000000, 0x3c000000, 0x3c000000, 0x3c000000,
//  0x20000000, 0x3c000000, 0x3c000000, 0x3c000000,
//  0x28000000, 0x3c000000, 0x3c000000, 0x30000000,
//  0x30000000, 0x3c000000, 0x3c000000, 0x34000000,
//
//  0x3c000000, 0x3c000000, 0x3c000000, 0x3c000000,
//  0x20000000, 0x3c000000, 0x3c000000, 0x3c000000,
//  0x28000000, 0x3c000000, 0x3c000000, 0x30000000,
//  0x30000000, 0x3c000000, 0x3c000000, 0x34000000,
//
//  0x3c000000, 0x3c000000, 0x3c000000, 0x3c000000,
//  0x20000000, 0x3c000000, 0x3c000000, 0x3c000000,
//  0x28000000, 0x3c000000, 0x3c000000, 0x30000000,
//  0x30000000, 0x3c000000, 0x3c000000, 0x34000000,
//  
//  0x3c000000, 0x3c000000, 0x3c000000, 0x3c000000,
//  0x20000000, 0x3c000000, 0x3c000000, 0x3c000000,
//  0x28000000, 0x3c000000, 0x3c000000, 0x30000000,
//  0x30000000, 0x3c000000, 0x3c000000, 0x34000000
//};
//
//
//
//unsigned long seg_mask[16] = {
//  0x01fffff8, 0x01fffff8, 0x01fffff8, 0x01fffff8,
//  0x03fffff8, 0x01fffff8, 0x01fffff8, 0x01fffff8,
//  0x03fffff8, 0x01fffff8, 0x01fffff8, 0x01fffff8,
//  0x01fffff8, 0x01fffff8, 0x01fffff8, 0x01fffff8};
//
//
//
//unsigned long seg_size[16] = {
//  64, 64, 64, 64,
//  128, 64, 64, 64,
//  128, 64, 64, 64,
//  64, 64, 64, 64};
//
//int numMCs = 0; 
//int numQuads = 0;
//int interleavingFactor = 0x1; // 32 bytes
//
//
////void configure_memory_map(int mc_fd){
////  
////  unsigned long long seg_map = 0;
////  int seg;
////  for (seg = 4; seg <= 9; seg++){
////    unsigned long long seg_data = 0;
////    seg_data |= (seg_size[seg]/64 -1);
////    seg_data |= (((seg_base[seg] & 0xfc000000)>> 26)<<3);
////    seg_map |= (seg_data << 9*(seg-4));
////  }
////  unsigned long write_data;
////  write_data = (seg_map >> 32);
////  write_reg(0, write_data, mc_fd);
////  write_data = seg_map & (unsigned long long )0xffffffff;
////  write_reg(1, write_data, mc_fd);
////  
////  seg_map = 0;
////
////  for (seg = 0xa; seg <= 0xf; seg++){
////    unsigned long long seg_data = 0;
////    seg_data |= (seg_size[seg]/64 -1);
////    seg_data |= (((seg_base[seg] & 0xfc000000)>> 26)<<3);
////    seg_map |= (seg_data << 9*(seg-0xa));
////  }
////  
////  write_data = (seg_map >> 32);
////  write_reg(2, write_data, mc_fd);
////  write_data = seg_map & (unsigned long long )0xffffffff;
////  write_reg(3, write_data, mc_fd);
////  
////
////  read_memory_map(mc_fd);
////}
//
//void configure_memory_map (int ppc_fd){
//
//  /*Need to do several levels of indirection for this one.*/
//  int mc;
//  unsigned long rdata;
//  for (mc = 0; mc < 4; mc++){
//    unsigned long mode;
//    mode = Q4_MODE_OFFCHIP;
//    mode |= (0x1 << (mc + 3)); 
//    mode |= (1 << (WRITE_ACTIVE_QUADS_BIT));
//    write_reg(Q4_W_ADDR_MODE, mode, ppc_fd);
//    
//    write_offchip_reg(Q4_W_ADDR_MODE, Q4_MODE_MEM_CFG, ppc_fd);
//    
//    write_offchip_reg(Q4_MEMCFG_W_ADDR_RESET, 1, ppc_fd);
//    write_offchip_reg(Q4_MEMCFG_W_ADDR_RESET, 0, ppc_fd);
//    unsigned long segment_selector;
//    //Write the segment selector. based on the # of MC's.
//    switch(numMCs){
//    case(1): 
//      segment_selector = 0; 
//      break;
//    case(2):
//      segment_selector = 1;
//      break;
//    case(4):
//      segment_selector = 2;
//      break;
//    default:
//      printf("Invalid # of MC's (%d)\n", numMCs);
//      exit(1);
//    }
//    
//    write_offchip_reg(Q4_MEMCFG_W_ADDR_WDATA, segment_selector, ppc_fd);
//    write_offchip_reg(Q4_MEMCFG_W_ADDR_WCE, 0x10, ppc_fd);
//   
//    
//    
//    unsigned long long seg_map = 0;
//    int seg;
//    for (seg = 4; seg <= 9; seg++){
//      unsigned long long seg_data = 0;
//      seg_data |= (seg_size[seg]/64 -1);
//      seg_data |= (((seg_base[mc][seg] & 0xfc000000)>> 26)<<3);
//      seg_map |= (seg_data << 9*(seg-4));
//    }
//    unsigned long write_data;
//    write_data = (seg_map >> 32);
//    //write_reg(0, write_data, mc_fd);
//    write_offchip_reg(Q4_MEMCFG_W_ADDR_WDATA, write_data, ppc_fd);
//    write_offchip_reg(Q4_MEMCFG_W_ADDR_WCE, 0x2, ppc_fd);
//
//
//    write_data = seg_map & (unsigned long long )0xffffffff;
//    //    write_reg(1, write_data, mc_fd);
//    write_offchip_reg(Q4_MEMCFG_W_ADDR_WDATA, write_data, ppc_fd);
//    write_offchip_reg(Q4_MEMCFG_W_ADDR_WCE, 0x1, ppc_fd);
//
//
//  
//    
//    seg_map = 0;
//  
//    for (seg = 0xa; seg <= 0xf; seg++){
//      unsigned long long seg_data = 0;
//      seg_data |= (seg_size[seg]/64 -1);
//      seg_data |= (((seg_base[mc][seg] & 0xfc000000)>> 26)<<3);
//      seg_map |= (seg_data << 9*(seg-0xa));
//    }
//    
//    write_data = (seg_map >> 32);
//    //  write_reg(2, write_data, mc_fd);
//    write_offchip_reg(Q4_MEMCFG_W_ADDR_WDATA, write_data, ppc_fd);
//    write_offchip_reg(Q4_MEMCFG_W_ADDR_WCE, 0x8, ppc_fd);
//
//   
//
//
//    write_data = seg_map & (unsigned long long )0xffffffff;
//    //write_reg(3, write_data, mc_fd);
//    write_offchip_reg(Q4_MEMCFG_W_ADDR_WDATA, write_data, ppc_fd);
//    write_offchip_reg(Q4_MEMCFG_W_ADDR_WCE, 0x4, ppc_fd);
//
//      
//  }
//
//
//
//  read_memory_map(ppc_fd);
//}
//
//
////void read_memory_map(int mc_fd){
////  
////  
////  unsigned long long seg_map;
////  int seg;
////  unsigned long read_data0, read_data1;
////  
////  read_reg(0,&read_data0, mc_fd);
////  read_reg(1, &read_data1, mc_fd);
////  seg_map = ((unsigned long long) read_data0) << 32;
////  seg_map |= read_data1;
////  
////  for (seg = 4; seg <= 9; seg++){
////    unsigned long long seg_data = seg_map & (0x1ff);
////    int int_seg = (int) seg_data;
////    seg_map = seg_map >> 9;
////    printf("Seg #%d : size %d at 0x%x\n", seg, (int_seg & 0x7), ((int_seg & 0x1f8)>>3)<<26);
////  }
////
//// 
////  read_reg(2,&read_data0, mc_fd);
////  read_reg(3, &read_data1, mc_fd);
////  seg_map = ((unsigned long long) read_data0) << 32;
////  seg_map |= read_data1;
////
////  
////  for (seg = 0xA; seg <= 0xF; seg++){
////    unsigned long long seg_data = seg_map & (0x1ff);
////    int int_seg = (int) seg_data;
////    seg_map = seg_map >> 9;
////    printf("Seg #%d : size %d at 0x%x\n", seg, (int_seg & 0x7), ((int_seg & 0x1f8)>>3)<<26);
////  }
////  
////
////}
//
//
//void read_memory_map(int ppc_fd){
//  
//  int mc; 
//  for (mc = 0; mc < 4; mc++){
//    unsigned long mode;
//    mode = Q4_MODE_OFFCHIP;
//    mode |= (0x1 << (mc + 3)); 
//    mode |= (1 << (WRITE_ACTIVE_QUADS_BIT));
//    write_reg(Q4_W_ADDR_MODE, mode, ppc_fd);
//    
//    write_offchip_reg(Q4_W_ADDR_MODE, Q4_MODE_MEM_CFG, ppc_fd);
//    
//    unsigned long long seg_map;
//    int seg;
//    unsigned long read_data0, read_data1;
//  
//    //read_reg(0,&read_data0, mc_fd);
//    write_offchip_reg(Q4_MEMCFG_W_ADDR_RCE, 0x1, ppc_fd);
//    read_offchip_reg(Q4_MEMCFG_R_ADDR_RDATA, &read_data0, ppc_fd);
//    
//    //read_offchip_reg(1, &read_data1, mc_fd);
//    write_offchip_reg(Q4_MEMCFG_W_ADDR_RCE, 0x2, ppc_fd);
//    read_offchip_reg(Q4_MEMCFG_R_ADDR_RDATA, &read_data1, ppc_fd);
//    
//    seg_map = ((unsigned long long) read_data1) << 32;
//    seg_map |= read_data0;
//  
//    for (seg = 4; seg <= 9; seg++){
//      unsigned long long seg_data = seg_map & (0x1ff);
//      int int_seg = (int) seg_data;
//      seg_map = seg_map >> 9;
//      printf("Seg #%d : size %d at 0x%x\n", seg, (int_seg & 0x7), ((int_seg & 0x1f8)>>3)<<26);
//    }
//
//    
//    //read_reg(2,&read_data0, mc_fd);
//    write_offchip_reg(Q4_MEMCFG_W_ADDR_RCE, 0x4, ppc_fd);
//    read_offchip_reg(Q4_MEMCFG_R_ADDR_RDATA, &read_data0, ppc_fd);
//    //read_reg(3, &read_data1, mc_fd);
//    write_offchip_reg(Q4_MEMCFG_W_ADDR_RCE, 0x8, ppc_fd);
//    read_offchip_reg(Q4_MEMCFG_R_ADDR_RDATA, &read_data1, ppc_fd);
//    
//    seg_map = ((unsigned long long) read_data1) << 32;
//    seg_map |= read_data0;
//    
//    
//    for (seg = 0xA; seg <= 0xF; seg++){
//      unsigned long long seg_data = seg_map & (0x1ff);
//      int int_seg = (int) seg_data;
//      seg_map = seg_map >> 9;
//      printf("Seg #%d : size %d at 0x%x\n", seg, (int_seg & 0x7), ((int_seg & 0x1f8)>>3)<<26);
//    }
//    
//    write_offchip_reg(Q4_MEMCFG_W_ADDR_RCE, 0x10, ppc_fd);
//    read_offchip_reg(Q4_MEMCFG_R_ADDR_RDATA, &read_data0, ppc_fd);
//    printf("Segment Selector = %d\n", read_data0);
//  }
//}
//
//
//void read_and_save_memory_map(int ppc_fd){
//  
//  int mc; 
//  for (mc = 0; mc < 4; mc++){
//    unsigned long mode;
//    mode = Q4_MODE_OFFCHIP;
//    mode |= (0x1 << (mc + 3)); 
//    mode |= (1 << (WRITE_ACTIVE_QUADS_BIT));
//    write_reg(Q4_W_ADDR_MODE, mode, ppc_fd);
//    
//    write_offchip_reg(Q4_W_ADDR_MODE, Q4_MODE_MEM_CFG, ppc_fd);
//    
//    unsigned long long seg_map;
//    int seg;
//    unsigned long read_data0, read_data1;
//  
//    //read_reg(0,&read_data0, mc_fd);
//    write_offchip_reg(Q4_MEMCFG_W_ADDR_RCE, 0x1, ppc_fd);
//    read_offchip_reg(Q4_MEMCFG_R_ADDR_RDATA, &read_data0, ppc_fd);
//    
//    //read_offchip_reg(1, &read_data1, mc_fd);
//    write_offchip_reg(Q4_MEMCFG_W_ADDR_RCE, 0x2, ppc_fd);
//    read_offchip_reg(Q4_MEMCFG_R_ADDR_RDATA, &read_data1, ppc_fd);
//    
//    seg_map = ((unsigned long long) read_data1) << 32;
//    seg_map |= read_data0;
//  
//    for (seg = 4; seg <= 9; seg++){
//      unsigned long long seg_data = seg_map & (0x1ff);
//      int int_seg = (int) seg_data;
//      seg_map = seg_map >> 9;
//      printf("Seg #%d : size %d at 0x%x\n", seg, ((int_seg & 0x7)+1)*64, ((int_seg & 0x1f8)>>3)<<26);
//      seg_size[seg] = ((int_seg & 0x7) + 1)*64;
//      seg_base[mc][seg] = (((int_seg & 0x1f8) >> 3) << 26);
//    }
//
//    
//    //read_reg(2,&read_data0, mc_fd);
//    write_offchip_reg(Q4_MEMCFG_W_ADDR_RCE, 0x4, ppc_fd);
//    read_offchip_reg(Q4_MEMCFG_R_ADDR_RDATA, &read_data0, ppc_fd);
//    //read_reg(3, &read_data1, mc_fd);
//    write_offchip_reg(Q4_MEMCFG_W_ADDR_RCE, 0x8, ppc_fd);
//    read_offchip_reg(Q4_MEMCFG_R_ADDR_RDATA, &read_data1, ppc_fd);
//    
//    seg_map = ((unsigned long long) read_data1) << 32;
//    seg_map |= read_data0;
//    
//    
//    for (seg = 0xA; seg <= 0xF; seg++){
//      unsigned long long seg_data = seg_map & (0x1ff);
//      int int_seg = (int) seg_data;
//      seg_map = seg_map >> 9;
//      printf("Seg #%d : size %d at 0x%x\n", seg, ((int_seg & 0x7)+1)*64, ((int_seg & 0x1f8)>>3)<<26);
//      seg_size[seg] = ((int_seg & 0x7) +1)*64;
//      seg_base[mc][seg] = (((int_seg & 0x1f8) >> 3) << 26);
//    }
//    
//    write_offchip_reg(Q4_MEMCFG_W_ADDR_RCE, 0x10, ppc_fd);
//    read_offchip_reg(Q4_MEMCFG_R_ADDR_RDATA, &read_data0, ppc_fd);
//    printf("Segment Selector = %d\n", read_data0);
//  }
//}
//
//
//unsigned long map_address (unsigned long addr, enum addr_type type, int * mcNum)
//{
//  int msb;
//  unsigned long mask;
//  unsigned long offset;
//  unsigned long mapped_addr;
//  unsigned long iaddr;
//
//  //first need to determine which MC (memory) this corresponds to
//  // Calculate the real address (ra) out of the interleaved address (ia):
//  // TO ME: ia = {ra[31:(cfg_ilv_fctr+4+cfg_num_MCs)], ra[cfg_ilv_fctr+4-1:0]}
//  //real_addr = (cfg_num_MCs)? /* only if there is more then one MC */
//  //{addr[31:(cfg_ilv_fctr+4)], mc_number[cfg_num_MCs-1:0], addr[cfg_ilv_fctr+4-1:0]}:
//  //addr;
//  // So, to go the other way:
//  // if (numMCs ==1 )
//  //     *mcNum = 0;
//  // else if (numMCs == 4)
//  //     *mcNum = (addr >> (ilv_factor+4)) && 0x3
//  // else 
//  
//  if (!mcNum){
//    printf("ERROR: null mcNum Pointer\n");
//    exit(1);
//  }
//  switch(numMCs){
//  case(1): 
//    *mcNum = 0;
//    iaddr = addr;
//    msb = (iaddr >> 28) & 0xf;
//    break;
//  case(2):
//    *mcNum = ((addr >> (/*interleavingFactor*/1+4)) & 0x1);
//    iaddr =  ((addr  & 0xFFFFFFc) >> 1)| (addr & 0x1F);
//    msb = (iaddr >> 27) & 0xf;
//    break;
//  case(4):
//    *mcNum = ((addr >> (/*interleavingFactor*/1+4)) & 0x3);
//    iaddr =  ((addr & 0xFFFFFF8) >> 2)|(addr & 0x1F);//assumes interleavingFactor ==1;
//    msb = (iaddr >> 26) & 0xf;
//    break;
//  default:
//    printf("ERROR: Invalid # of MCs (%d)\n", numMCs);
//    exit(1);
//  }
//  //msb = (addr >> 28) & 0xf;
//  mask = seg_mask[msb];
//  
//  if (type == DATA_ADDR) {
//    if (addr % 8 == 0) {
//      offset = ((iaddr & mask) << 1) + 4;
//    }
//    else {
//      offset = ((iaddr & mask) << 1);
//    }
//  } else {
//    if (addr % 8 == 0) {
//      offset = ((iaddr & mask) << 1) + 12;
//    }
//    else {
//      offset = ((iaddr & mask) << 1) + 8;
//    }     
//  }
//
//  mapped_addr = offset;
//
//  return mapped_addr;
//}



// General Utility functions
unsigned long htoi(char *str) 
{ 
  unsigned long value = 0; 
  size_t len = 8; 
  char *ptr = NULL; 
  int i; 

  for (i=0; i < len; i++) 
    { 
      ptr = str + len - 1 - i; 

      if ((unsigned char)*ptr >= '0' && (unsigned char)*ptr <= '9') 
	value += ((unsigned char)*ptr - '0') << (4 * i); 
      else if ((unsigned char)*ptr >= 'A' && (unsigned char)*ptr <= 'F') 
	value += ((unsigned char)*ptr - 'A' + 10) << (4 * i); 
      else if ((unsigned char)*ptr >= 'a' && (unsigned char)*ptr <= 'f') 
	value += ((unsigned char)*ptr - 'a' + 10) << (4 * i); 
      else 
	return -1; 
    } 
  return (unsigned long)value; 
} 

inline int isHex(char *ptr) 
{ 
  return (ptr[0] == '0' && (ptr[1] == 'x' || ptr[1] == 'X')); 
} 

unsigned long reverse_bytes (unsigned long in) {
  unsigned long out;

  char *inPtr = (char *) &in;
  char *outPtr = (char *) &out;

  outPtr[3] = inPtr[0];
  outPtr[2] = inPtr[1];
  outPtr[1] = inPtr[2];
  outPtr[0] = inPtr[3];

  return out;
}

//void reset_fifo(unsigned int fifo_num, int ppc_fd){
//  unsigned long snoop_ctrl;
//  write_reg(Q4_W_ADDR_MODE, Q4_MODE_SNOOP, ppc_fd);
//  snoop_ctrl = 0;
//  snoop_ctrl |= fifo_num & 0x7;
//  snoop_ctrl |= (1<<4); //Reset
//  write_reg(Q4_W_ADDR_SNOOPER_CTRL, snoop_ctrl, ppc_fd);
//  return 0;
//}
//  
//void reset_offchip_fifo(unsigned int quad, unsigned int fifo_num, int ppc_fd){
//  
//  unsigned long mode;
//  mode = Q4_MODE_OFFCHIP;
//  mode |= (1 << (quad + 3));
//  mode |= (1 << (WRITE_ACTIVE_QUADS_BIT));
//  write_reg(Q4_W_ADDR_MODE, mode, ppc_fd);
//  unsigned long snoop_ctrl;
//  write_offchip_reg(Q4_W_ADDR_MODE, Q4_MODE_SNOOP, ppc_fd);
//  snoop_ctrl = 0;
//  snoop_ctrl |= fifo_num & 0x7;
//  snoop_ctrl |= (1<<4); //Reset
//  write_offchip_reg(Q4_W_ADDR_SNOOPER_CTRL, snoop_ctrl, ppc_fd);
//  return 0;
//}
//  
//void read_fifo(unsigned int  fifo_num, int ppc_fd) {
// unsigned long read_data0, read_data1, read_data2, read_status;
// unsigned long snoop_ctrl;
//
// snoop_ctrl = 0;
// snoop_ctrl |= (fifo_num & 0x7);
// snoop_ctrl |= (1<<5); //Read status
// write_reg(Q4_W_ADDR_MODE, Q4_MODE_SNOOP, ppc_fd); 
// //  printf("Writing Snooper Control with 0x%08x\n", snoop_ctrl);
// write_reg(Q4_W_ADDR_SNOOPER_CTRL, snoop_ctrl, ppc_fd);
// read_reg (Q4_R_ADDR_SNOOPER_STATUS, &read_status, ppc_fd);
// printf("Status [ 0x%08x] of Fifo# %d: %s %s Count = 0x%x\n", read_status, 
//	read_status & 0x7, 
//	read_status&(1<<3)?"F":"", read_status&(1<<4)?"E":"",read_status >> 5);
// //if ((fifo_num == SNOOPER_NUM_LAST_FLIT) || (fifo_num == SNOOPER_NUM_LAST_LAST_FLIT)){
// //   snoop_ctrl |= (1<<3); //Read
// //  write_reg(Q4_W_ADDR_SNOOPER_CTRL, snoop_ctrl, ppc_fd);
// //  read_reg (Q4_R_ADDR_SNOOPER_2, &read_data2, ppc_fd);
// //  read_reg (Q4_R_ADDR_SNOOPER_1, &read_data1, ppc_fd);
// //  read_reg (Q4_R_ADDR_SNOOPER_0, &read_data0, ppc_fd);
// //  printf("%s (src = %02x)\n", fifo_num == SNOOPER_NUM_LAST_FLIT? "LAST FLIT: ": " LAST LAST FLIT",
// //         (read_data2 >> 18) & 0xFF);
// //  printf("%08x.",read_data2);
// //  printf("%08x.",read_data1);
// //  printf("%08x\n",read_data0);
// //  printf("FT: %01x FVC: %01x FD: %03x%08x%07x CEN: %01x CVC: %01x\n",
// //         (read_data2 >> 15) & 0x7 , //flit_type = data[81:79]
// //         (read_data2 >> 12) & 0x7, //flit_vc = data[78:76]
// //         (read_data2) & 0xfff, //flit_data[71:60 = data[75:64]
// //         (read_data1), //flit_data[59:28] = data[63:32]
// //         (read_data0 >> 4) & (0xfffffff), //flit_data[27:0] = data[31:4]
// //         (read_data0 >> 3) & (0x1) , // credit_en = data[3]
// //         read_data0 & (0x7) // credit_vc[2:0] = data[2:0]
// //         );
// //  return;
// //} 
//
//
//
//
// int count = read_status >> 5;
// while (count > 0){ 
//   count --;
//   snoop_ctrl = 0;
//   snoop_ctrl |= (fifo_num & 0x7);
//   snoop_ctrl |= (1<<3); //Read
//   write_reg(Q4_W_ADDR_SNOOPER_CTRL, snoop_ctrl, ppc_fd);
//   printf("COUNT %d : ", count);//FIFO Read data is :0x", count);
//   read_reg (Q4_R_ADDR_SNOOPER_2, &read_data2, ppc_fd);
//   //printf("%08x.",read_data2);
//   read_reg (Q4_R_ADDR_SNOOPER_1, &read_data1, ppc_fd);
//   //printf("%08x.",read_data1);
//   read_reg (Q4_R_ADDR_SNOOPER_0, &read_data0, ppc_fd);
//   //printf("%08x\n",read_data0);
//   switch(fifo_num){
//   case(SNOOPER_NUM_SM_TO_NS0):
//   case(SNOOPER_NUM_NS_TO_SM0):
//   case(SNOOPER_NUM_SM_TO_NS1):
//   case(SNOOPER_NUM_SM_TO_NS2):
//   case(SNOOPER_NUM_SM_TO_NS3):
//   case(SNOOPER_NUM_NS_TO_SM1):
//   case(SNOOPER_NUM_MC_TO_NS0):
//   case(SNOOPER_NUM_NS_TO_MC0):
//     
//     printf("TS: %02x: FT: %01x FVC: %01x FD: %03x%08x%07x CEN: %01x CVC: %01x\n",         
//            (read_data2 >> 18) & 0xFF, // time stamp = data[82:89]
//	    (read_data2 >> 15) & 0x7 , //flit_type = data[81:79]
//	    (read_data2 >> 12) & 0x7, //flit_vc = data[78:76]
//	    (read_data2) & 0xfff, //flit_data[71:60 = data[75:64]
//	     (read_data1), //flit_data[59:28] = data[63:32]
//	     (read_data0 >> 4) & (0xfffffff), //flit_data[27:0] = data[31:4]
//	     (read_data0 >> 3) & (0x1) , // credit_en = data[3]
//	     read_data0 & (0x7) // credit_vc[2:0] = data[2:0]
//	   );
//      break;
//  //   case(SNOOPER_NUM_MC_OUT):
//  //    printf("ADDR: %08x DATA[31:0]: %08x MD: %03x DM: %01x MM: %01x, RW: %01x\n",
//  //           read_data0, // address[31:0] = data[31:0]
//  //           read_data1, //write_data[31:0] = data [63:0] (not wide enough for all data) 
//  //           (read_data2) & (0xfff), //write_meta_data[11:0] = read_data2[11:0]
//  //           (read_data2 >> 12 & 0xff), //data_byte_mask[7:0]
//  //           (read_data2 >> 20) & 0x3, //meta_mask[1:0]
//  //           (read_data2 >> 22) & 0x1 //rw
//  //           );
//  //    break;
//  //  case(SNOOPER_NUM_MC_IN):
//  //    printf("DATA: 0x%08x%08x  MD: 0x%03x\n", 
//  //           read_data1, 
//  //           read_data0, 
//  //           read_data2 & 0xfff);
//  //    break;
//    default: 
//      printf ("Unknown FIFO Num %d\n", fifo_num);
//    }
//    
//  }
//}
//
//void read_offchip_fifo(unsigned int quad, unsigned int  fifo_num, int ppc_fd) {
//  unsigned long read_data0, read_data1, read_data2, read_status;
//  unsigned long snoop_ctrl;
//  //printf("Reading offchip fifo %d\n", quad);
//  unsigned long mode;
//  mode = Q4_MODE_OFFCHIP;
//  mode |= (1 << (quad + 3));
//  mode |= (1 << (WRITE_ACTIVE_QUADS_BIT));
//  write_reg(Q4_W_ADDR_MODE, mode, ppc_fd);
//  //printf("Wrote mode : %08x\n", mode);
//
//  snoop_ctrl = 0;
//  snoop_ctrl |= (fifo_num & 0x7);
//  snoop_ctrl |= (1<<5); //Read status
//  write_offchip_reg(Q4_W_ADDR_MODE, Q4_MODE_SNOOP, ppc_fd); 
//  //printf("Wrote offchip mode: %08x\n", mode);
//  //  printf("Writing Snooper Control with 0x%08x\n", snoop_ctrl);
//  write_offchip_reg(Q4_W_ADDR_SNOOPER_CTRL, snoop_ctrl, ppc_fd);
//  //printf("wrote offchip snooper: %08x\n", snoop_ctrl);
//  read_offchip_reg (Q4_R_ADDR_SNOOPER_STATUS, &read_status, ppc_fd);
//  printf("Status [ 0x%08x] of Fifo# %d.%d: %s %s Count = 0x%x\n", read_status, 
//	 quad,
//	 read_status & 0x7, 
//	 read_status&(1<<3)?"F":"", read_status&(1<<4)?"E":"",read_status >> 5);
//  
//  int count = read_status >> 5;
//  while (count > 0){ 
//    count --;
//    snoop_ctrl = 0;
//    snoop_ctrl |= (fifo_num & 0x7);
//    snoop_ctrl |= (1<<3); //Read
//    write_offchip_reg(Q4_W_ADDR_SNOOPER_CTRL, snoop_ctrl, ppc_fd);
//    printf("COUNT %d : ", count);// FIFO Read data is :0x", count);
//    read_offchip_reg (Q4_R_ADDR_SNOOPER_2, &read_data2, ppc_fd);
//    //printf("%08x.",read_data2);
//    read_offchip_reg (Q4_R_ADDR_SNOOPER_1, &read_data1, ppc_fd);
//    //printf("%08x.",read_data1);
//    read_offchip_reg (Q4_R_ADDR_SNOOPER_0, &read_data0, ppc_fd);
//    //printf("%08x\n",read_data0);
//    switch(fifo_num){
//    case(SNOOPER_NUM_SM_TO_MC):
//    case(SNOOPER_NUM_MC_TO_SM):
//    case(SNOOPER_NUM_NS_TO_MC):
//    case(SNOOPER_NUM_MC_TO_NS):
//      printf("FT: %01x FVC: %01x FD: %03x%08x%07x CEN: %01x CVC: %01x\n",
//	     (read_data2 >> 15) & 0x7 , //flit_tye = data[81:79]
//	     (read_data2 >> 12) & 0x7, //flit_vc = data[78:76]
//	     (read_data2) & 0xfff, //flit_data[71:60 = data[75:64]
//	     (read_data1), //flit_data[59:28] = data[63:32]
//	     (read_data0 >> 4) & (0xfffffff), //flit_data[27:0] = data[31:4]
//	     (read_data0 >> 3) & (0x1) , // credit_en = data[3]
//	     read_data0 & (0x7) // credit_vc[2:0] = data[2:0]
//	   );
//      break;
//    case(SNOOPER_NUM_MC_OUT):
//      printf("ADDR: %08x DATA[31:0]: %08x MD: %03x DM: %01x MM: %01x, RW: %01x\n",
//	     read_data0, // address[31:0] = data[31:0]
//	     read_data1, //write_data[31:0] = data [63:0] (not wide enough for all data) 
//	     (read_data2) & (0xfff), //write_meta_data[11:0] = read_data2[11:0]
//	     (read_data2 >> 12 & 0xff), //data_byte_mask[7:0]
//	     (read_data2 >> 20) & 0x3, //meta_mask[1:0]
//	     (read_data2 >> 22) & 0x1 //rw
//             );
//      break;
//    case(SNOOPER_NUM_MC_IN):
//      printf("DATA: 0x%08x%08x  MD: 0x%03x\n", 
//             read_data1, 
//             read_data0, 
//             read_data2 & 0xfff);
//      break;
//    default: 
//      printf ("Unknown FIFO Num %d\n", fifo_num);
//    }
//    
//  }
//}
//
////---------------SM IO FUNCTIONS
// 
//
////!!! Does not currently work/
//void zero_sim_io(sm_io_t * ios_ptr){
//  int i;
//  // outputs:
//  for (i=0;i<12;i++)     ios_ptr->Debug_vec_out[i] = 0;
//  ios_ptr->QuadBreakOut = 0;
//  ios_ptr->host_interrupt = 0;
//  ios_ptr->IOClk = 0;
//  ios_ptr->CreditEn_out = 0;
//  for (i=0;i<3;i++)     ios_ptr->FlitType_out[i] = 0;
//  for (i=0;i<3;i++)     ios_ptr->FlitVC_out[i] = 0;
//  for (i=0;i<3;i++)     ios_ptr->CreditVC_out[i] = 0;
//  for (i=0;i<72;i++)     ios_ptr->FlitData_out[i] = 0;
//  
//  // inputs:
//  ios_ptr->PreventReset = 0;
//  ios_ptr->QuadBreakIn = 0;
//  ios_ptr->CreditEn_in = 0;
//  for (i=0;i<3;i++)     ios_ptr->CreditVC_in[i] = 0;
//  for (i=0;i<72;i++)    ios_ptr->FlitData_in[i] = 0;
//  for (i=0;i<3;i++)     ios_ptr->FlitType_in[i] = 0;
//  for (i=0;i<3;i++)     ios_ptr->FlitVC_in[i] = 0;
//  for (i=0;i<5;i++)     ios_ptr->QuadId[i] = 0;
//  for (i=0;i<2;i++)     ios_ptr->IO_Div_in[i] = 0;
//  ios_ptr->Reset = 0;
//  for (i=0;i<5;i++)     ios_ptr->DebugMuxSel[i] = 0;
//}
//
//
//
////!!! Does not currently work/
//void read_sm_outputs(sm_io_t * ios_ptr, int ppc_fd){
////  int bit;
////  unsigned long read_data;
////
////  zero_sim_io(ios_ptr);
////  write_reg(Q4_W_ADDR_MODE, Q4_MODE_SNOOP, ppc_fd);
////  read_reg(Q4_R_ADDR_CTRL_SIGNALS, &read_data, ppc_fd);
////  
////  //os  ios_ptr->JTDO = (read_data >> 11) & 0x1;
////  ios_ptr->host_interrupt = (read_data >> 18) &0x1; 
////  ios_ptr->QuadBreakOut = (read_data >> 19) & 0x1;
////  for (bit = 0; bit < 12; bit++){
////    ios_ptr->Debug_vec_out[bit] = (read_data >> (20+bit))& 0x1;
////  }
////  
////  unsigned long snoop_ctrl;
////  snoop_ctrl = 0;
////  snoop_ctrl |= SNOOPER_NUM_SM_OUTPUTS;
////  snoop_ctrl |= (1<<3); // read;
////  write_reg(Q4_W_ADDR_SNOOPER_CTRL, snoop_ctrl, ppc_fd);
////  read_reg(Q4_R_ADDR_SNOOPER_0, &read_data, ppc_fd);
////  for (bit = 0; bit < 32; bit++){
////    ios_ptr->FlitData_out[bit] = (read_data >> bit) & 0x1;
////  }
////  read_reg (Q4_R_ADDR_SNOOPER_1, &read_data, ppc_fd);
////  for (bit = 0; bit < 32; bit++){
////    ios_ptr->FlitData_out[bit+32] = (read_data >> bit) & 0x1;
////  }
////  read_reg (Q4_R_ADDR_SNOOPER_2, & read_data, ppc_fd);
////  for (bit = 0; bit < 8; bit++){
////    ios_ptr->FlitData_out[bit+64] = (read_data >> bit) & 0x1;
////  }
////  for (bit = 0; bit < 3; bit++){
////    ios_ptr->CreditVC_out[bit] = (read_data >> (bit + 8))&0x1;
////  }
////  for (bit = 0; bit < 3; bit++){
////    ios_ptr->FlitVC_out[bit] = (read_data >> (bit + 11))&0x1;
////  }
////  for (bit = 0; bit < 3; bit++){
////    ios_ptr->FlitType_out[bit] = (read_data >> (bit + 14))&0x1;
////  }
////  ios_ptr->CreditEn_out = (read_data >> 17) & 0x1;
////  ios_ptr->IOClk = (read_data >> 18) &0x1;
//}
//
//
////!!! Does not currently work/
//void write_sm_inputs(sm_io_t * ios_ptr, int ppc_fd){
////  int bit;
////  unsigned long write_data;
////
////  write_reg (Q4_W_ADDR_MODE, Q4_MODE_SM_CTRL, ppc_fd);
////  char PreventResetN = ios_ptr->PreventReset?0:1;
////  write_data = PreventResetN;
////  write_reg(Q4_W_ADDR_PREVENTRESET_N, write_data, ppc_fd);
////  
////  char ResetN = ios_ptr->Reset?0:1;
////  write_data = ResetN;
////  write_reg(Q4_W_ADDR_RESET_N, write_data, ppc_fd);
////
////  write_data = ios_ptr->QuadBreakIn;
////  write_reg(Q4_W_ADDR_QUADBREAKIN, write_data, ppc_fd);
////  
////  write_reg (Q4_W_ADDR_MODE, Q4_MODE_SNOOP, ppc_fd);
////  write_data = 0;
////  for (bit = 0; bit < 32; bit++){
////    write_data |= (ios_ptr->FlitData_in[bit] << bit);
////  }
////  write_reg(Q4_W_ADDR_INPUTS_0, write_data, ppc_fd);
////  
////  write_data = 0;
////  for (bit = 0; bit < 32; bit++){
////    write_data |= ios_ptr->FlitData_in[bit+32] << bit;
////  }
////  write_reg(Q4_W_ADDR_INPUTS_1, write_data, ppc_fd);
////  
////  write_data = 0;
////  for (bit = 0; bit < 8; bit++){
////    write_data |= ios_ptr->FlitData_in[bit+64] << bit;
////  }
////  for (bit = 0; bit < 3; bit++){
////    write_data |= ios_ptr->FlitVC_in[bit] << (bit + 8);
////  }
////  for (bit = 0; bit < 3; bit++){
////    write_data |= ios_ptr->FlitType_in[bit] << (bit + 11);
////  }
////  for (bit = 0; bit < 3; bit++){
////    write_data |= ios_ptr->CreditVC_in[bit] << (bit + 14);
////  }
////  write_data |= (ios_ptr->CreditEn_in << 17);
////  write_reg(Q4_W_ADDR_INPUTS_2, write_data, ppc_fd);
////    
////  /*Sorry, can't write the last through FPGA*/
////  //char QuadId[5];
////  //char IO_Div_in[2];
////  //char DebugMuxSel[5];
////
////  //enable forcing of the inputs. need to disable this before
////  // MC will work again. 
////  write_reg(Q4_MC_W_ADDR_SM_INPUT_PATTERN, 1, ppc_fd);
//  
//}
//
//
////!!! Does not currently work/
//void mc_drive_sm_inputs(int ppc_fd){
//  //  write_reg (Q4_W_ADDR_MODE, Q4_MODE_SNOOP, ppc_fd);
//  //write_reg(Q4_MC_W_ADDR_SM_INPUT_PATTERN, 0, ppc_fd);
//}
//
//
//
////!!! Does not currently work/
//int compare_sm_outputs(sm_io_t *fpga_side_ios_ptr, sm_io_t * jtag_side_ios_ptr){
//  int bit;
//  int err=0;
//
//  for (bit=0; bit<12; bit++){
//    if (fpga_side_ios_ptr->Debug_vec_out[bit] != jtag_side_ios_ptr->Debug_vec_out[bit]){
//      printf("ERROR: output bit Debug_vec_out[%0d] don't match (fpga_io=%0d != jtag_io=%0d)\n", 
//	     bit,fpga_side_ios_ptr->Debug_vec_out[bit], jtag_side_ios_ptr->Debug_vec_out[bit]);
//      err++;
//    }
//  }
//  
//  if (fpga_side_ios_ptr->QuadBreakOut != jtag_side_ios_ptr->QuadBreakOut){
//    printf("ERROR: output bit QuadBreakOut don't match (fpga_io=%0d != jtag_io=%0d)\n", 
//	   fpga_side_ios_ptr->QuadBreakOut, jtag_side_ios_ptr->QuadBreakOut);
//    err++;
//  }
//  
//  if (fpga_side_ios_ptr->host_interrupt != jtag_side_ios_ptr->host_interrupt){
//    printf("ERROR: output bit host_interrupt don't match (fpga_io=%0d != jtag_io=%0d)\n", 
//	   fpga_side_ios_ptr->host_interrupt, jtag_side_ios_ptr->host_interrupt);
//    err++;
//  }
//
//  if (fpga_side_ios_ptr->IOClk != jtag_side_ios_ptr->IOClk){
//    printf("ERROR: output bit IOClk don't match (fpga_io=%0d != jtag_io=%0d)\n", 
//	   fpga_side_ios_ptr->IOClk, jtag_side_ios_ptr->IOClk);
//    err++;
//  }
//
//  if (fpga_side_ios_ptr->CreditEn_out != jtag_side_ios_ptr->CreditEn_out){
//    printf("ERROR: output bit CreditEn_out don't match (fpga_io=%0d != jtag_io=%0d)\n", 
//	   fpga_side_ios_ptr->CreditEn_out, jtag_side_ios_ptr->CreditEn_out);
//    err++;
//  }
//
//  for (bit=0; bit<3; bit++){
//    if (fpga_side_ios_ptr->FlitType_out[bit] != jtag_side_ios_ptr->FlitType_out[bit]){
//      printf("ERROR: output bit FlitType_out[%0d] don't match (fpga_io=%0d != jtag_io=%0d)\n", 
//	     bit,fpga_side_ios_ptr->FlitType_out[bit], jtag_side_ios_ptr->FlitType_out[bit]);
//      err++;
//    }
//  }
//
//  for (bit=0; bit<3; bit++){
//    if (fpga_side_ios_ptr->FlitVC_out[bit] != jtag_side_ios_ptr->FlitVC_out[bit]){
//      printf("ERROR: output bit FlitVC_out[%0d] don't match (fpga_io=%0d != jtag_io=%0d)\n", 
//	     bit,fpga_side_ios_ptr->FlitVC_out[bit], jtag_side_ios_ptr->FlitVC_out[bit]);
//      err++;
//    }
//  }
//
//  for (bit=0; bit<3; bit++){
//    if (fpga_side_ios_ptr->CreditVC_out[bit] != jtag_side_ios_ptr->CreditVC_out[bit]){
//      printf("ERROR: output bit CreditVC_out[%0d] don't match (fpga_io=%0d != jtag_io=%0d)\n", 
//	     bit,fpga_side_ios_ptr->CreditVC_out[bit], jtag_side_ios_ptr->CreditVC_out[bit]);
//      err++;
//    }
//  }
//
//  for (bit=0; bit<72; bit++){
//    if (fpga_side_ios_ptr->FlitData_out[bit] != jtag_side_ios_ptr->FlitData_out[bit]){
//      printf("ERROR: output bit FlitData_out[%0d] don't match (fpga_io=%0d != jtag_io=%0d)\n", 
//	     bit,fpga_side_ios_ptr->FlitData_out[bit], jtag_side_ios_ptr->FlitData_out[bit]);
//      err++;
//    }
//  }
//  return err;
//
//}
//
//
////!!! Does not currently work/
//int compare_sm_inputs(sm_io_t * fpga_side_ios_ptr, sm_io_t * jtag_side_ios_ptr){
//  int bit;
//  int err=0;
//
//  if (fpga_side_ios_ptr->PreventReset != jtag_side_ios_ptr->PreventReset){
//    printf("ERROR: input bit PreventReset don't match (fpga_io=%0d != jtag_io=%0d)\n", 
//	   fpga_side_ios_ptr->PreventReset, jtag_side_ios_ptr->PreventReset);
//    err++;
//  }
//
//  if (fpga_side_ios_ptr->QuadBreakIn != jtag_side_ios_ptr->QuadBreakIn){
//    printf("ERROR: input bit QuadBreakIn don't match (fpga_io=%0d != jtag_io=%0d)\n", 
//	   fpga_side_ios_ptr->QuadBreakIn, jtag_side_ios_ptr->QuadBreakIn);
//    err++;
//  }
//
//  if (fpga_side_ios_ptr->CreditEn_in != jtag_side_ios_ptr->CreditEn_in){
//    printf("ERROR: input bit CreditEn_in don't match (fpga_io=%0d != jtag_io=%0d)\n", 
//	   fpga_side_ios_ptr->CreditEn_in, jtag_side_ios_ptr->CreditEn_in);
//    err++;
//  }
//
//  for (bit=0; bit<3; bit++){
//    if (fpga_side_ios_ptr->CreditVC_in[bit] != jtag_side_ios_ptr->CreditVC_in[bit]){
//      printf("ERROR: input bit CreditVC_in[%0d] don't match (fpga_io=%0d != jtag_io=%0d)\n", 
//	     bit,fpga_side_ios_ptr->CreditVC_in[bit], jtag_side_ios_ptr->CreditVC_in[bit]);
//      err++;
//    }
//  }
//
//  for (bit=0; bit<72; bit++){
//    if (fpga_side_ios_ptr->FlitData_in[bit] != jtag_side_ios_ptr->FlitData_in[bit]){
//      printf("ERROR: input bit FlitData_in[%0d] don't match (fpga_io=%0d != jtag_io=%0d)\n", 
//	     bit,fpga_side_ios_ptr->FlitData_in[bit], jtag_side_ios_ptr->FlitData_in[bit]);
//      err++;
//    }
//  }
//
//  for (bit=0; bit<3; bit++){
//    if (fpga_side_ios_ptr->FlitType_in[bit] != jtag_side_ios_ptr->FlitType_in[bit]){
//      printf("ERROR: input bit FlitType_in[%0d] don't match (fpga_io=%0d != jtag_io=%0d)\n", 
//	     bit,fpga_side_ios_ptr->FlitType_in[bit], jtag_side_ios_ptr->FlitType_in[bit]);
//      err++;
//    }
//  }
//
//  for (bit=0; bit<3; bit++){
//    if (fpga_side_ios_ptr->FlitVC_in[bit] != jtag_side_ios_ptr->FlitVC_in[bit]){
//      printf("ERROR: input bit FlitVC_in[%0d] don't match (fpga_io=%0d != jtag_io=%0d)\n", 
//	     bit,fpga_side_ios_ptr->FlitVC_in[bit], jtag_side_ios_ptr->FlitVC_in[bit]);
//      err++;
//    }
//  }
//
//  if (fpga_side_ios_ptr->Reset != jtag_side_ios_ptr->Reset){
//    printf("ERROR: input bit Reset don't match (fpga_io=%0d != jtag_io=%0d)\n", 
//	   fpga_side_ios_ptr->Reset, jtag_side_ios_ptr->Reset);
//    err++;
//  }
//
//  // skip: QuadId, IO_Div_in, DebugMuxSel
//  return err;
//}
////-------------------------MC Configuration
//void mc_config_write(int mcNum, unsigned long address, unsigned long value, 
//		     int ppc_fd){
//  int waiter;
//  unsigned long mode = Q4_MODE_OFFCHIP;
//  mode |= (1 << (mcNum + 3));
//  mode |= (1 << (WRITE_ACTIVE_QUADS_BIT));
//  write_reg(Q4_W_ADDR_MODE, mode, ppc_fd);
//  write_offchip_reg( Q4_W_ADDR_MODE, Q4_MODE_MC_CFG, ppc_fd);
//  write_offchip_reg( Q4_MC_W_ADDR_CONFIG_ADDR, address, ppc_fd);
//  write_offchip_reg( Q4_MC_W_ADDR_CONFIG_DATA, value, ppc_fd);
//  unsigned long ctrl = 0x2; // write
//  write_offchip_reg( Q4_MC_W_ADDR_CONFIG_CTRL, ctrl, ppc_fd);
//  ctrl = 0;
//  for (waiter = 0; waiter < 100; waiter++);
//  write_offchip_reg( Q4_MC_W_ADDR_CONFIG_CTRL, ctrl, ppc_fd);
//}
//  
//void mc_config_read(int mcNum, unsigned long address, unsigned long * value,
//		    int ppc_fd){
//  int waiter;
// unsigned long mode = Q4_MODE_OFFCHIP;
//  mode |= (1 << (mcNum + 3));
//  mode |= (1 << (WRITE_ACTIVE_QUADS_BIT));
//  write_reg(Q4_W_ADDR_MODE, mode, ppc_fd);
//  write_offchip_reg( Q4_W_ADDR_MODE, Q4_MODE_MC_CFG, ppc_fd);
//  write_offchip_reg( Q4_MC_W_ADDR_CONFIG_ADDR, address, ppc_fd);
//  unsigned long ctrl = 0x1; // read
//  write_offchip_reg( Q4_MC_W_ADDR_CONFIG_CTRL, ctrl, ppc_fd);
//  for (waiter = 0; waiter < 100; waiter++);
//  read_offchip_reg(Q4_MC_R_ADDR_CONFIG_WR_DATA, value, ppc_fd);
//  ctrl = 0;
//  write_offchip_reg( Q4_MC_W_ADDR_CONFIG_CTRL, ctrl, ppc_fd);
//}
//
//void mc_reset(int mcNum, int ppc_fd){
//  int waiter;
//  unsigned long mode = Q4_MODE_OFFCHIP;
//  mode |= (1 << (mcNum + 3));
//  mode |= (1 << (WRITE_ACTIVE_QUADS_BIT));
//  unsigned long ctrl = 0x4; // reset
//  write_reg(Q4_W_ADDR_MODE, mode, ppc_fd);
//  write_offchip_reg( Q4_W_ADDR_MODE, Q4_MODE_MC_CFG, ppc_fd);
//  write_offchip_reg( Q4_MC_W_ADDR_CONFIG_CTRL, ctrl, ppc_fd);
//  for (waiter = 0; waiter < 100; waiter++);
//  ctrl = 0;
//  write_offchip_reg( Q4_MC_W_ADDR_CONFIG_CTRL, ctrl, ppc_fd);
//}
