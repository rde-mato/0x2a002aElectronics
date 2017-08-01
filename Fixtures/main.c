#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>

#define PARENT_WRITE_END fd[0][1]
#define PARENT_READ_END fd[1][0]
#define CHILD_WRITE_END fd[1][1]
#define CHILD_READ_END fd[0][0]
#define BUF_SIZE 1

pid_t child_pid = 0;

void send_buf(char *buf, size_t len)
{
  if (write(PARENT_WRITE_END, buf, len) == -1)
  {
    perror("write");
    exit(1);
  }
  if (kill(child_pid, SIGUSR1) == -1)
  {
    perror("kill");
    exit(1);
  }
}

void loop(void)
{
  char buf[BUF_SIZE];
  ssize_t read_ret;
  
  while ((read_ret = read(PARENT_READ_END, buf, BUF_SIZE)) != 0)
  {
    if (read_ret == -1 && errno != EAGAIN)
    {
      perror("read");
      exit(1);
    }
    else if (read_ret > 0)
    {
      
    }
  }
}

int main(int ac, char **av)
{
  int   fd[2][2];

  if (ac != 2)
  {
    dprintf(stderr, "usage: %s <test_program>\n", av[0]);
    exit(1);
  }

  for (int i = 0; i < 2; i++) {
    if (pipe(fd[i]) == -1) {
      perror("pipe");
      exit(1);
    }
  }

  child_pid = fork();
  if (child_pid == -1)
  {
    perror("fork");
    exit(1);
  }
  else if (child_pid == 0)
  {
    close(PARENT_WRITE_END);
    close(PARENT_READ_END);
    if (dup2(CHILD_READ_END, 0) == -1 || dup2(CHILD_WRITE_END, 1) == -1)
    {
      perror("child dup");
      exit(1);
    }
    if (execv(av[1], av) == -1)
    {
      perror("child execv");
      exit(1);
    }
  }
  else
  {
    close(CHILD_WRITE_END);
    close(CHILD_READ_END);
    loop();
  }
}
