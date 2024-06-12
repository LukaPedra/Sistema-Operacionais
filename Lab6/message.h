#ifndef MESSAGE_H
#define MESSAGE_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QTD_MEN 128
#define TAM_BUFFER 8
struct message {
    long msg_type;
    int msg_data;
};

void process1_sync(int msgid);
void process2_sync(int msgid);

#endif
