#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

typedef struct {
    char msg[50];
    struct timespec ts;
} msg;

typedef union {
    int msg_num;
    msg message;
}msgr;

#define CAP_BUFFER _IOR('a','a', int *)
#define MESSAGE_GET _IOWR('a','b', msgr *)


int main(){
    msgr req;
    char buf[100];
    printf("enter chardev name:\n");
    scanf("%s", buf);
    while (1){
        int number = 0;
        int fp = open(buf, O_RDWR);
        if(fp < 0) {
            printf("Cannot open device file...\n");
            return 0;
        }
        ioctl(fp, CAP_BUFFER, (int *)&number);
        printf("buffer have %d \nwrite number of message to get[0...%d)\n", number, number);
        scanf("%i", &req.msg_num);
        printf("====message %i\n", req.msg_num);
        ioctl(fp, MESSAGE_GET,(msgr *)&req);
        printf("message %ld.%li: %s\n", req.message.ts.tv_sec, req.message.ts.tv_nsec, req.message.msg);
        close(fp);
    }
}
