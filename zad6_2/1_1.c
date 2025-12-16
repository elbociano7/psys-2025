#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    printf("Usage: %s <pipename>\n", argv[0]);
    return 1;
  }

  char *pipename = argv[1];

  signal(SIGPIPE, SIG_IGN);

  if (mkfifo(pipename, 0666) == -1)
  {
    if(errno != EEXIST) {
      printf("Error creating named pipe: %s\n", pipename);
      return 2;
    }
  }

  int fd = open(pipename, O_WRONLY);
  if (fd == -1)
  {
    printf("Error opening named pipe: %s\n", pipename);
    unlink(pipename);
    return 3;
  }

  int message = 0;

  while (1)
  {
    char text_buffer[32];
    int length = snprintf(text_buffer, sizeof(text_buffer), "%d\n", message);

    printf("Message write attempt: %d\n", message);

    int res = write(fd, text_buffer, length);
    
    printf("Writing(%d) message: %d\n", res, message);

    if (res == -1)
    {
      printf("Error writing to named pipe: %s\n", pipename);
      unlink(pipename);
      return 4;
    }

    message++;
  }

  return 0;
}
