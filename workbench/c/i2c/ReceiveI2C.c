/* ReceiveI2C.c:
 *   Copy bytes from the I�C bus to stdout, using i2c.library.  Usage:
 * ReceiveI2C  <bus address>  <number of bytes>
 * where <bus address> is hex, <number of bytes> is a decimal number.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <proto/exec.h>
#include <proto/i2c.h>
#include <libraries/i2c.h>

struct Library *I2C_Base;

int main(int argc, char *argv[])
{
  int c, result=0, binary=0;
  size_t i, bytes;
  UBYTE adr, *i2cdata;
  ULONG err;

  if (argc<3) {
    fprintf(stderr, "Usage: \e[2m%s <addr> <bytes> [-b]\e[0m, "
        "e.g. %s 20 5\n", argv[0], argv[0]);
    return 5;
  }
  adr = strtoul(argv[1], NULL, 16);
  bytes = atol(argv[2]);
  if (argc>3 && strncmp(argv[3], "-b", 2)==0)
    binary = 1;
  i2cdata = malloc(bytes);
  if (!i2cdata) {
    fprintf(stderr, "Out of memory (%ld bytes)\n", bytes);
    return 10;
  }
  I2C_Base = OpenLibrary("i2c.library", 39);
  if (!I2C_Base) {
    fprintf(stderr, "Can't open i2c.library V39+\n");
    free(i2cdata);
    return 10;
  }
  err = ReceiveI2C(adr, bytes, i2cdata);
  if ((err & 0xff)==0) {
    fprintf(stderr, "I2C failure: \e[1m%s\e[0m\n", I2CErrText(err));
    result = 10;
  } else for (i=0; i<bytes; i++) {
    if (binary)
      putchar(i2cdata[i]);
    else {
      printf("%02X", i2cdata[i]);
      c = ((i%4)==3) ? ' ' : '\0';
      if ((i%32)==31 || i==bytes-1)  c = '\n';
      if (c)  putchar(c);
    }
  }
  CloseLibrary(I2C_Base);
  free(i2cdata);

  return result;
}

