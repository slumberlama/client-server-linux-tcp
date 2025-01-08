#ifndef SEND_H
#define SEND_H

#include <sys/types.h>
#include <sys/socket.h>

#define MAX_EVENTS 1024 /*Максимальное кличество событий для обработки за один раз*/
#define LEN_NAME 32 /*Длина имени файла не превышает 32 символов*/
#define EVENT_SIZE  ( sizeof (struct inotify_event) ) /*размер структуры события*/
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + LEN_NAME )) /*буфер  для хранения данных о событиях*/
#define UNPACKED_BUFFER_SIZE sizeof("create") + LEN_NAME + 32 + 1

typedef struct msg {
  char create_or_delete[8];
  char file_name[32];
  char sha256[32];
} msg_t;

extern msg_t log_inf;

void *send_change(void *);

void unpack(char const* buf, size_t len);

#endif

