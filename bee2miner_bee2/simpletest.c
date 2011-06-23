
#include <stdio.h>
#include <string.h>

#include "quadrwlib.h"

int main(int argc, char *argv[]) { 
  int ppc_fd = 0; 
  int numQ = 4; //Number of Quads, should probably be command line arg.

  unsigned int nonce;
 
  unsigned char bufferin[64];
  unsigned char bufferout[4];

  unsigned char midstate_buf[32];
  unsigned char data_buf[32];
 
  // Test vectors
  char *midstate_hex = "2b3f81261b3cfd001db436cfd4c8f3f9c7450c9a0d049bee71cba0ea2619c0b5"; // 256 bits
  char *data_hex = "000000000000000000000000800000000000000039f3001b6b7b8d4dc14bfc31"; // 256 bits
  // Expected nonce <= 30411740;  
  
  int ii;
  for (ii = 0; ii < 32; ii++){
    sscanf(&midstate_hex[ii*2], "%02hhx", &midstate_buf[ii]);
    sscanf(&data_hex[ii*2], "%02hhx", &data_buf[ii]);
    //    printf("%02x %02x\n", midstate_buf[ii], data_buf[ii]);
  }
 
#ifndef NOXC
  ppc_fd = InitQuadInterface();

  //SetQuad(0, ppc_fd);
  for (q = 0; q < numQ; q++){
    SetQuad(q, ppc_fd);
    ResetRW(ppc_fd);
  }
#endif

  // Single byte read/write test
  //  WriteByte('z', ppc_fd);
  //  ReadByte(&data, ppc_fd);
  //  printf("\nRead : %c\n", data);

  // Buffer read/write test
  memset(bufferin, 0, 64);
  memcpy(bufferin, midstate_buf, 32);
  memcpy(bufferin+32, data_buf, 32);

#ifndef NOXC
  //WriteBuffer(bufferin, 64, ppc_fd);
  //ReadBuffer(bufferout, 4, ppc_fd);
  WriteBufferToAll(bufferin, 64, numQ, ppc_fd);
  ReadBufferFromAny(bufferout, 4, numQ, ppc_fd);
#endif

  nonce =  (bufferout[0] << 0  & 0x000000FF);
  nonce |= (bufferout[1] << 8  & 0x0000FF00);
  nonce |= (bufferout[2] << 16 & 0x00FF0000);
  nonce |= (bufferout[3] << 24 & 0xFF000000);
  
  printf("\nRead : %d  (0x%08x)\n", nonce, nonce); 
  
  return 0;
} 
