/*************************************************************************
 ** From Perforce:
 **
 ** $Id: //Smart_design/Smash_board/sm_fpga/sm_software/sm_resetnio.h#5 $
 ** $DateTime: 2009/08/05 10:46:01 $
 ** $Change: 8012 $
 ** $Author: steveri $
 *************************************************************************/


void doTileSIMPrint (int sndrID, int fd);
void handle_open (int cpuID, int fd);
void handle_close (int cpuID, int fd);
void handle_read (int cpuID, int fd);
void handle_write (int sndrID, int fd);
void handle_notify_env(int cpuID, int fd);
void doTileIO (int sndrID, int fd);
void clearIOBuffers (int fd);
void doIO (int fd, int ppc_fd);
void doSIMPrint (int fd);
void returnActiveCPUs(char *cpuVector_str, int *cpuVector, int *numCPUs);
void clearMats (int tileId, int fd);
void setCommandArgs (int argc, char *argv[], int io_fd);
int main(int argc, char *argv[]) ;
