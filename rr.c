
#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#define DIRSEP '\\'
#else
#define DIRSEP '/'
#endif

typedef struct {
  int size;
  char * data;
} rrbuf;

const char* get_home_dir() {
#ifdef _WIN32
  return getenv("USERPROFILE");
#else
  return getenv("HOME");
#endif
}

const int get_config_file(rrbuf *buf) {
  char * home = get_home_dir();
  char * len = strlen(home);
  strcpy(rrbuf.data ""
}

int main() {
  printf(get_home_dir());
  return 0;
}
