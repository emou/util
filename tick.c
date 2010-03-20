#include <unistd.h>
#include <time.h>
#include <signal.h>

/* A simple program that I use when I want to see how much time a specific program spends on specific tasks.
 * USAGE: $0 <prog> <progargs>
 * I wrote this to analyze where is the bottleneck in my KDE4 startup.
 * The idea is pretty simple: this program shows the current time in a specified interval (currently 1 second).
 * The external program <prog> that is being is supposed to output some stuff, so that the ticking is mangled with
 * this output.
 * TODO: add command line options for specifying a ticking interval and date/time format. 
 * */

inline void usage()
{
  write(1, "Usage: tick command [args]\n", 28);
}

void die(int sig)
{
    (void) sig;
    _exit(0);
}

int main(int argc, char **argv)
{
  char buff[35];
  struct tm* timeinfo;
  time_t t;
  size_t len;
  pid_t pid;
  signal(SIGCHLD, die);

  
  if(argc<2) {
    usage();
    return 0;
  }
  pid = fork();

  if(pid==-1)
    return 21;

  if(pid==0)
  {
        /* Child */
        execvp(argv[1], argv+1);
  } else {

        /* Parent */
        while(1)
        {
          t=time(NULL);
          timeinfo=localtime(&t);
          len=strftime(buff, 35, "TICK: %d/%m/%y %H:%M:%S\n", timeinfo);
          write(1, buff, len);
          usleep(1e6);
        }
  }

  _exit(0);
}
