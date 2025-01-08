#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <msgpack.h>
#include "erproc.h"

volatile sig_atomic_t done = 0;

void term(int signum)
{
   printf("Caught!\n");
   done = 1;
}

int main(int argc, char *argv[]) {
  char * path = NULL;
  int opt = 0;
  char * sec = NULL;
  int fd = 0;
  pid_t childpid;
  pid_t pid = 0;
  char buf[256];
  
  struct sigaction action;
  memset(&action, 0, sizeof(action));
  action.sa_handler = term;
  sigaction(SIGTERM, &action, NULL);
  
  pid = getpid();
  int parentID = getppid();
  printf("PID is %d  and Parent is %d \n",pid, parentID);

  while ((opt = getopt(argc, argv, ":t:d:")) != -1) {
    switch (opt) {
      case 't':
        printf("time sec: %s\n", sec = optarg);
        break;
      case 'd':
        path = optarg;
        printf("dirname: %s\n", path = optarg);
        break;
      default: /* '?' */
        fprintf(stderr, "Usage: %s [-t sec] [-d dirname]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }
  
  if(optind >= argc+1) {
    fprintf(stderr, "Expected argument after options\n");
    exit(EXIT_FAILURE);
    //close(fd);
  }
  
  int server = Socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in adr = {0};
  adr.sin_family = AF_INET;
  adr.sin_port = htons(5555);
  adr.sin_addr.s_addr = inet_addr("127.0.0.1");
  Bind(server, (struct sockaddr *) &adr, sizeof(adr));
  Listen(server, 5);
  socklen_t adrlen = sizeof adr;
  strcat(sec, "_");
  strcat(sec, path);
  while(1) {  
    fd = Accept(server, (struct sockaddr*)&adr, &adrlen);
    printf("Connection accepted from %s:%d\n", inet_ntoa(adr.sin_addr), ntohs(adr.sin_port));
    if((childpid = fork()) == 0) {
      printf("Client's PID is %d, %d, %d\n", childpid, getpid(), getppid());
      close(server);
      while(1){
        recv(fd, buf, 256, 0);
        strcpy(buf+5, sec);
        if(done) {
          printf("Disconnected from %s:%d\n", inet_ntoa(adr.sin_addr), ntohs(adr.sin_port));
          break;
        } else {
            printf("Client: %s\n", buf);
            send(fd, buf, strlen(buf), 0);
            memset(buf, 0, sizeof(buf));
        }
      }
    }
  }
  close(fd);
  close(server);
  return 0;
}
