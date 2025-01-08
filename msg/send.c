#include <msgpack.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "send.h"

pthread_mutex_t mutex_s;
msg_t log_inf;

void *prepare(void* sbuf) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, sbuf, msgpack_sbuffer_write);
    msgpack_pack_array(&pk, 6);
    msgpack_pack_str(&pk, 14);
    msgpack_pack_str_body(&pk, "type_of_event:", 14);
    msgpack_pack_str(&pk, 8);
    msgpack_pack_str_body(&pk, log_inf.create_or_delete, 8);
    msgpack_pack_str(&pk, 10);
    msgpack_pack_str_body(&pk, "file_name:", 10);
    msgpack_pack_str(&pk, sizeof(log_inf.file_name));
    msgpack_pack_str_body(&pk, log_inf.file_name, sizeof(log_inf.file_name));
    msgpack_pack_str(&pk, 7);
    msgpack_pack_str_body(&pk, "sha256:", 7);
    msgpack_pack_str(&pk, 7);
    msgpack_pack_str_body(&pk, "sha256:", 7);
}

void unpack(char const* buf, size_t len) {
  msgpack_unpacked result;
  size_t off = 0;
  msgpack_unpack_return ret;
  char unpacked_buffer[UNPACKED_BUFFER_SIZE];
  msgpack_unpacked_init(&result);
  buf = calloc(UNPACKED_BUFFER_SIZE, UNPACKED_BUFFER_SIZE);
  ret = msgpack_unpack_next(&result, buf, len, &off);
  while (ret == MSGPACK_UNPACK_SUCCESS && !(buf[0])) {
    msgpack_object obj = result.data;
    msgpack_object_print(stdout, obj);
    printf("\n");
    msgpack_object_print_buffer(unpacked_buffer, UNPACKED_BUFFER_SIZE, obj);
    printf("%s\n", unpacked_buffer);
    ret = msgpack_unpack_next(&result, buf, len, &off);
  }
  msgpack_unpacked_destroy(&result);

  if (ret == MSGPACK_UNPACK_CONTINUE) {
    printf("All msgpack_object in the buffer is consumed.\n");
  }
  else if (ret == MSGPACK_UNPACK_PARSE_ERROR) {
    printf("The data in the buf is invalid format.\n");
  }
}

void *send_change(void *) {
  pthread_mutex_init(&mutex_s, NULL);
  msgpack_sbuffer sbuf;
  msgpack_sbuffer_init(&sbuf);
 
  pthread_mutex_lock(&mutex_s);
  prepare(&sbuf);
  //unpack(sbuf.data, sbuf.size);
  pthread_mutex_unlock(&mutex_s);

  pthread_mutex_destroy(&mutex_s);
  msgpack_sbuffer_destroy(&sbuf);
  return 0;
}

void print(char const* buf,size_t len) {
  size_t i = 0;
  for(; i < len ; ++i) {printf("%02x ", 0xff & buf[i]);}
  printf("\n");
}
