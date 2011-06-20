
#include "4q_bee2lib.h"
#include "4q_jtag.h"
#include "4q_ppc_user_logic.h"
#include <fcntl.h>
#include <stdlib.h> // for getenv()

void SetQuad(int quad, int fd) {
  if (quad >=0 && quad <= 3) {
    unsigned long mode = Q4_MODE_SM_CTRL;
    mode |= (1 << (quad + 3));
    mode |= (1 << (WRITE_ACTIVE_QUADS_BIT));
    write_reg(Q4_W_ADDR_MODE, mode, fd);
  } else {
    printf("Invalid quad selected: %i\n", quad);
  }
}

void ResetRW(int fd) {
  set_JTRST(0, fd);
  //usleep(1);
  set_JTCK(0, fd);
  //usleep(1);
  set_JTMS(0, fd);
  //usleep(1);
  set_JTRST(1, fd);
  //usleep(1);
  set_JTRST(0, fd);
  //usleep(1);
}

void WriteByte(unsigned char c, int fd) {
  int i;
  printf("Write Byte %02x\n", c);
  for (i = 0; i < 8; ++i) {
   
    set_JTDI(c & 0x01, fd); // set data
    //usleep(1);
    set_JTCK(1, fd);
    //usleep(1);
    set_JTCK(0, fd);        // zero clock
    //usleep(1);
    c = c >> 1;
  }  
}

int ReadDataAvailable(int fd) {
  set_JTMS(1, fd); //Tx Clock
  //usleep(1);
  set_JTMS(0, fd);
  //usleep(1);
  if (get_JTDO(fd)) {
    return 1;
  } else {
    return 0;
  }
}

void ReadByte(unsigned char *c, int fd) {
  int i;
  int j = 0;

  while(!ReadDataAvailable(fd)) {
    j = j + 1;
  };

  printf("J was %d\n", j);
  *c = 0;
  for (i = 0; i < 8; ++i) {
    *c = *c >> 1;
    set_JTMS(1, fd);
    //usleep(1);
    set_JTMS(0, fd);
    //usleep(1);
    if (get_JTDO(fd)) {
      *c = *c | 0x80;
    }
  }
  printf("Read Byte %02x\n", *c);
}

void WriteBuffer(unsigned char *buffer, int size, int fd) {
  int i;
  for (i = 0; i < size; ++i) {
    WriteByte(*buffer, fd);
    buffer++;
  }
}

void ReadBuffer(unsigned char *buffer, int size, int fd) {
  int i;
  for (i = 0; i < size; ++i) {
    ReadByte(buffer, fd);
    buffer++;
  }
}
 
int main(int argc, char *argv[]) 
{ 
  int ppc_fd = 0; 
  int err;
  unsigned int nonce;
   
  unsigned char bufferin[64];
  unsigned char bufferout[32];

  unsigned char midstate_buf[32];
  unsigned char data_buf[32];
  
  //midstate_buf <= 256'h2b3f81261b3cfd001db436cfd4c8f3f9c7450c9a0d049bee71cba0ea2619c0b5;
  //data_buf <= 256'h00000000000000000000000080000000_00000000_39f3001b6b7b8d4dc14bfc31;
  //nonce <= 30411740;
  
  unsigned char midstate_buf_rev[32] = {0x2b, 
					0x3f, 
					0x81, 
					0x26, 
					0x1b, 
					0x3c, 
					0xfd, 
					0x00, 
					0x1d, 
					0xb4, 
					0x36, 
					0xcf, 
					0xd4, 
					0xc8, 
					0xf3, 
					0xf9, 
					0xc7, 
					0x45, 
					0x0c, 
					0x9a, 
					0x0d, 
					0x04, 
					0x9b, 
					0xee, 
					0x71, 
					0xcb, 
					0xa0, 
					0xea, 
					0x26, 
					0x19, 
					0xc0, 
					0xb5};

  unsigned char data_buf_rev[32] = {0x00, 
				    0x00, 
				    0x00, 
				    0x00, 
				    0x00, 
				    0x00,
				    0x00,
				    0x00,
				    0x00,
				    0x00,
				    0x00,
				    0x00,
				    0x80,
				    0x00,
				    0x00,
				    0x00,
				    0x00,
				    0x00,
				    0x00,
				    0x00,
				    0x39,
				    0xf3,
				    0x00,
				    0x1b,
				    0x6b,
				    0x7b,
				    0x8d,
				    0x4d,
				    0xc1,
				    0x4b,
				    0xfc,
				    0x31};
  
  
  int ii;
  for (ii = 0; ii < 32; ii++){
    //midstate_buf[ii] = midstate_buf_rev[31-ii];
    //data_buf[ii] = data_buf_rev[31-ii];
    midstate_buf[ii] = midstate_buf_rev[ii];
    data_buf[ii] = data_buf_rev[ii];
  }
  
  printf("\nOpen and init devices\n"); fflush(stdout);
  err = open_coredev ("/dev/coredrv0", 0xe0002700, 64, 4, &ppc_fd);
  if (err != 0) {
    printf ("ERROR setting up core devices\n");
    exit (1);
  }

  SetQuad(0, ppc_fd);
  ResetRW(ppc_fd);

  // Single byte read/write test
  //  WriteByte('z', ppc_fd);
  //  ReadByte(&data, ppc_fd);
  //  printf("\nRead : %c\n", data);

  // Buffer read/write test
  memset(bufferin, 0, 64);
  memcpy(bufferin, midstate_buf, 32);
  memcpy(bufferin+32, data_buf, 32);
  
  WriteBuffer(bufferin, 64, ppc_fd);
  ReadBuffer(bufferout, 4, ppc_fd);
  
  nonce =  (bufferout[0] << 0  & 0x000000FF);
  nonce |= (bufferout[1] << 8  & 0x0000FF00);
  nonce |= (bufferout[2] << 16 & 0x00FF0000);
  nonce |= (bufferout[3] << 24 & 0xFF000000);
  
  printf("\nRead : %d  (0x%08x)\n", nonce); 
   
  //  fflush(stdout);
  //usleep(1);
  // END OF METHODIC BREAK 
      
  return 0;
} 
