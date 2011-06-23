
#include "4q_bee2lib.h"
#include "4q_jtag.h"
#include "4q_ppc_user_logic.h"
#include <fcntl.h>
#include <stdlib.h> // for getenv()

#include "quadrwlib.h"

int InitQuadInterface() {
  int err, ppc_fd;
  printf("\nOpen and init devices\n"); fflush(stdout);
  err = open_coredev ("/dev/coredrv0", 0xe0002700, 64, 4, &ppc_fd);
  if (err != 0) {
    printf ("ERROR setting up core devices\n");
    exit (1);
  }
  return ppc_fd;
}

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
  int i, j;

  while(!ReadDataAvailable(fd)) {
    j = j + 1;
  };

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
  //  printf("Read Byte %02x (polls: %d)\n", *c, j);
}

int ReadByteNB(unsigned char* c, int fd){
  int i;
  int ready = ReadDataAvailable(fd);
  if (ready){
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
    //    printf("Read Byte (NB) %02x\n", *c);
  }
  return ready;
}

void WriteBuffer(unsigned char *buffer, int size, int fd) {
  int i;
  for (i = 0; i < size; ++i) {
    WriteByte(*buffer, fd);
    buffer++;
  }
}

void WriteBufferToAll(unsigned char * buffer, int size, int numQuads, int fd){
  int q;
  for (q = 0; q < numQuads; q++){
    SetQuad(q, fd);
    WriteBuffer(buffer, size, fd);
  }
}

void ReadBuffer(unsigned char *buffer, int size, int fd) {
  int i;
  for (i = 0; i < size; ++i) {
    ReadByte(buffer, fd);
    buffer++;
  }
}

void ReadBufferFromAny(unsigned char *buffer, int size, int numQuads, int fd){
  int done = 0;
  unsigned char * bufferPtr = buffer;
  while (!done){
    int q;
    for (q = 0; q < numQuads; q++){
      SetQuad(q, fd);
      if (ReadByteNB(bufferPtr, fd)){
	bufferPtr ++;
	ReadBuffer(bufferPtr, size-1, fd);
	done = 1;
	break;
      }
    }
  }
}
  
