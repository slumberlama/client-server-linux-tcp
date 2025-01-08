#include <sys/types.h>
#include <sys/socket.h>
#include <sys/inotify.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <msgpack.h>
#include "erproc.h"
#include "send.h"

pthread_t track;
pthread_t send_ch;
pthread_mutex_t mutex;
unsigned int sleep_sec = 0;

void displayInotifyEvent(struct inotify_event *i)
{
  printf("    wd =%2d; ", i->wd);
  printf("mask = ");
  if (i->mask & IN_CREATE) {
    printf("create ");
    memcpy(log_inf.create_or_delete, " create ", 8);
  }
  if (i->mask & IN_DELETE) {
    printf("delete ");
    memcpy(log_inf.create_or_delete, " delete ", 8);
  }
  printf("\n");

  if (i->len > 0) {
    printf("name = %s\n", i->name);
    strcpy(log_inf.file_name, i->name);
  }
}

void *tracking_change(void* path_to_dir) {
  int length = 0;
  char *p;
  struct inotify_event *event;
  char buffer[BUF_LEN] __attribute__ ((aligned(8)));
  
  int inotify_fd = inotify_init();
  if(inotify_fd < 0) {perror( "Couldn't initialize inotify");}
  int wd = inotify_add_watch(inotify_fd, (char*)path_to_dir, IN_CREATE | IN_DELETE);
  if(wd == -1) {
    printf("Couldn't add watch to %s\n", (char*)path_to_dir);
  }
  else {
    printf("Watching:: %s\n", (char*)path_to_dir);
  }  
  while(1) {
    //sleep(sleep_sec);
    pthread_mutex_lock(&mutex);
    length = read(inotify_fd, buffer, BUF_LEN);
    if (length <= 0) {perror("read");}
    for(p = buffer; p < buffer + length; ) {
      event = (struct inotify_event *) p;
      displayInotifyEvent(event);
      p += sizeof(struct inotify_event) + event->len;
    }
    pthread_mutex_unlock(&mutex);
  }
  inotify_rm_watch(inotify_fd, wd);
  close(inotify_fd);
  return 0;
}

int main() {
  int fd = Socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in adr = {0};
  char path[4096];
       
  adr.sin_family = AF_INET;  //TCP
  adr.sin_port = htons(5555);
    
  Inet_pton(AF_INET, "127.0.0.1", &adr.sin_addr);
  Connect(fd, (struct sockaddr *) &adr, sizeof(adr));
  
  char buf[256];
  memset(buf, 0, 256);
  while(1) {
    strcpy(buf, "HELLO");
    printf("Client: \t%s\n", buf);
    send(fd, buf, strlen(buf), 0);
    if(strcmp(buf, ":exit") == 0){
      close(fd);
      printf("[-]Disconnected from server.\n");
      exit(1);
    }
    if(recv(fd, buf, 256, 0) < 0){
      printf("[-]Error in receiving data.\n");
    } else {
      printf("Server: \t%s\n", buf);
      
      for(int i = 5; i < 256; i++) {
        if(buf[i] == '_') {
          char t[i-5];
          memcpy(t, buf+5, i-5);
          sleep_sec = atoi(t);
          strcpy(path, buf+i+1);
          break;
        }
      }
      break;
    }
  }
  //strcpy(path, "/home/nik");
  //printf("%d\n", sleep_sec);
  printf("%s\n", path);
  pthread_mutex_init(&mutex, NULL);
  if(pthread_create(&track, NULL, &tracking_change, (char*)path)) {
    printf("pthread_create failed");
    close(fd);
    exit(EXIT_FAILURE);
  }
  
  if(pthread_create(&send_ch, NULL, &send_change, NULL) ) {
    printf("pthread_create failed");
    close(fd);
    exit(EXIT_FAILURE);
  }
  
  pthread_join(track, NULL);
  pthread_join(send_ch, NULL);
  
  pthread_mutex_destroy(&mutex);
  
  close(fd);
  return 0;
}
