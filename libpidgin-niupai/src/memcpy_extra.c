#include <string.h>

#include "memcpy_extra.h"

void memcpy_offset (char *dst, char *src, int doff, int soff, int len)
{
  memcpy(dst + doff, src + soff, len);
}

void memmove_offset (char *dst, char *src, int doff, int soff, int len)
{
  memmove(dst + doff, src + soff, len);
}
