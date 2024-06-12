#ifndef MESSAGE_H
#define MESSAGE_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct message {
    long msg_type;
    int msg_data;
};

#endif
