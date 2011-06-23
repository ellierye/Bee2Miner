#ifndef QUADRWLIB_H_
#define QUADRWLIB_H_

int InitQuadInterface();

void SetQuad(int quad, int fd);
void ResetRW(int fd);

void WriteByte(unsigned char c, int fd);
int ReadDataAvailable(int fd);

void ReadByte(unsigned char *c, int fd);
int ReadByteNB(unsigned char* c, int fd);  // non blocking version

void WriteBuffer(unsigned char *buffer, int size, int fd);
void WriteBufferToAll(unsigned char * buffer, int size,
		      int numQuads, int fd);  // broadcast

void ReadBuffer(unsigned char *buffer, int size, int fd);
void ReadBufferFromAny(unsigned char *buffer, int size,
		       int numQuads, int fd);  // returns first available
#endif // QUADRWLIB_H_
