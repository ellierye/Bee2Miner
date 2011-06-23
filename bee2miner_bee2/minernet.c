
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "quadrwlib.h"
#include "simplenet.h"

void PrintHexBuffer(unsigned char* buffer, int length) {
  int ii;
  for (ii = 0; ii < length; ii++){
    printf("%02hhx", buffer[ii]);
  }
}

unsigned int BufferToNonce(unsigned char *buffer) {
  unsigned int nonce;
  nonce =  (buffer[0] << 0  & 0x000000FF);
  nonce |= (buffer[1] << 8  & 0x0000FF00);
  nonce |= (buffer[2] << 16 & 0x00FF0000);
  nonce |= (buffer[3] << 24 & 0xFF000000);
  return nonce;
}

typedef struct miner_s {
  connection_t connection;
  int state;
} miner_t;

void print_usage(char** argv){
  printf("USAGE: %s [numQuads]\n", argv[0]);
}
  

#define STATE_IDLE 0
#define STATE_WORKING 1

// TODO: add input buffer
int main(int argc, char *argv[]) { 
  int q;
  int ppc_fd = 0; 
  int  num_q = 4;
  miner_t miner[4];
  int start_port = 8087;

  unsigned int nonce;
  unsigned char bufferin[64];  // 512 bits
  unsigned char bufferout[4];  // 32 bits

  // Initialize HW interface and reset
  ppc_fd = InitQuadInterface();

  if (argc == 2){
    num_q = atoi(argv[1]);
    if (num_q <= 0 || num_q > 4){
      printf("Num Quads must be 1-4\n");
      print_usage(argv);
      exit(1);
    }
  }
  
  for (q = 0; q < num_q; q++){
    SetQuad(q, ppc_fd);
    ResetRW(ppc_fd);
  }

  // Initialize miners: numQ consecutive ports
  for (q = 0; q < num_q; ++q) {
    NetGetConnection(start_port + q,
		     &miner[q].connection);
    miner[q].state = STATE_IDLE;
  }

  while (1) {
    // feed / consume FPGA data
    // Note: several potential sources of lockups here.
    for (q = 0; q < num_q; ++q) {
      SetQuad(q, ppc_fd);
      if (miner[q].state == STATE_WORKING) {
	// Check for available data and read if available.
	if (ReadByteNB(&bufferout[0], ppc_fd)) {
	  ReadByte(&bufferout[1], ppc_fd);
	  ReadByte(&bufferout[2], ppc_fd);
	  ReadByte(&bufferout[3], ppc_fd);
	  // Send
	  NetWrite(&miner[q].connection, bufferout, 4);
	  miner[q].state = STATE_IDLE;
	  // Debug
	  nonce = BufferToNonce(bufferout);
	  printf("Miner(%d) TX nonce: %d  (0x%08x)\n", q, nonce, nonce); 
	}
      } 
      //else if (miner[q].state == STATE_READY) {
      // Just always do this.
      // RX net data and forward to FPGA
      if (NetRead(&miner[q].connection, bufferin, 64) == 64) {
	WriteBuffer(bufferin, 64, ppc_fd);
	miner[q].state = STATE_WORKING;
	printf("Miner(%d) RX data\n", q); 
      }
    } // quad iterator
    
    // TODO: Check sideband for commands.
    
  } // while (1)
  
  // Close connections
  for (q = 0; q < num_q; ++q) {
    NetClose(&miner[q].connection);
  }

  return 0;
} 
