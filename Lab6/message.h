#ifndef MESSAGE_H
#define MESSAGE_H

#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/msg.h>

#define QTD_MEN 8
#define TAM_BUFFER 8
struct message {
    long msg_type;
    int msg_data;
};

void process1_sync(int msgid);
void process2_sync(int msgid);
void process1_async(int msgid);
void process2_async(int msgid);


#endif
