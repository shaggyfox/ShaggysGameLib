#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "helper.h"
char *load_file(char *name, size_t *out_len)
{
  int fd = open(name, O_RDONLY);
  if (-1 == fd) {
    return NULL;
  }
  size_t len = 0;
  struct stat st;
  fstat(fd, &st);
  char *ret = malloc(st.st_size);
  while (len < st.st_size) {
    int c = read(fd, ret + len, st.st_size - len);
    if (c < 0) {
      free(ret);
      return NULL;
    } else {
      len += c;
    }
  }
  if (out_len) {
    *out_len = len;
  }
  return ret;
}

