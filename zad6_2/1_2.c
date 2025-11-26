#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    printf("Usage: %s <pipename>\n", argv[0]);
    return 1;
  }

  char *pipename = argv[1];

  printf("Checking for pipe '%s'...\n", pipename);

  while (access(pipename, F_OK) == -1) {
      printf("Pipe not found, waiting...\n");
      sleep(1);
  }

  printf("Opening pipe for reading...\n");
  
  int fd = open(pipename, O_RDONLY);
  if (fd == -1)
  {
    perror("Error opening pipe");
    return 3;
  }

  printf("Connected! Waiting for data...\n");

  char buffer[16];
  ssize_t bytes_read;

  while (1)
  {
    bytes_read = read(fd, buffer, sizeof(buffer) - 1);

    if (bytes_read > 0)
    {
      buffer[bytes_read] = '\0';
      printf("Reader received: %s", buffer);
    }
    else if (bytes_read == 0)
    {
      printf("Writer closed the connection.\n");
      break;
    }
    else
    {
      perror("Read error");
      break;
    }

    sleep(10);
  }

  close(fd);
  return 0;
}