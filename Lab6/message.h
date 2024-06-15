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

#define QTD_MEN 128
#define TAM_BUFFER 8
struct message {
    long msg_type;
    int msg_data;
};

void processo1_sinc(int msgid);
void processo2_sinc(int msgid);
void processo1_asssinc(int msgid);
void processo2_asssinc(int msgid);
