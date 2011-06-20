#include "4q_bee2lib.h"
#include "4q_libsim_for_rtl.h"
#include "4q_ppc_user_logic.h"

int main(int argc, char *argv[]) 
{ 
  int ppc_fd = 0; 
  int magic_fd = 0; 
  int io_fd = 0; 

  int i = 0, k = 0; 
  unsigned long buf = 0; 
  unsigned long data = 0, meta = 0; 

  int cpuVector;

  int err;

  err = open_coredev ("/dev/coredrv0", 0xe0002700, 64, 4, &ppc_fd);

  if (err != 0) {
    printf ("Error setting up core devices\n");
    exit (1);
  }
  
  //Testing the interface.
  unsigned long read_data;
  unsigned long snoop_ctrl;
  
  unsigned int fifo;
  for (fifo = 0; fifo < 8; fifo++){
    reset_fifo(fifo, ppc_fd);
  }
  
  int q;
  for (q = 0; q < 4; q++){
    for (fifo = 0; fifo < 8; fifo++){
      reset_offchip_fifo(q, fifo, ppc_fd);
    }
  }
  

  return 0;

}
