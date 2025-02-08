#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <mqueue.h>
#include <sys/syscall.h>

static mqd_t mqd;                   /* 消息队列描述符 */
static char *mq_name = "/.test_mq"; /* 消息队列名称  /代表 /dev/mqueue */

void *pt1_send(void *argv) 
{
    printf("pt1_send start, tid = %d\n", gettid());

    sleep(1);

    char *msg = "hello, world";
    int res = mq_send(mqd, msg, strlen(msg) + 1, 1);
    if (res == -1) {
        perror("mq_send");
        return NULL;
    } else {
        printf("send msg: %s\n", msg);
    }

    return NULL;
}

void *pt2_recv(void *argv) 
{
    printf("pt2_recv start, tid = %d\n", gettid());

    char buf[50];
    ssize_t size = mq_receive(mqd, buf, sizeof(buf), NULL);
    if (size > 0) {
        printf("recv msg: %s\n", buf);
    } else {
        perror("mq_receive");
    }

    return NULL;
}

int main(int argc, char **argv) 
{
    struct mq_attr attr;
    attr.mq_maxmsg = 10; /* 队列中最大消息数 */
    attr.mq_msgsize = 50; /* 消息最大字节数 */

    /* 创建消息队列 */
    mqd = mq_open(mq_name, O_CREAT | O_RDWR, 0644, &attr);
    if (mqd == (mqd_t)-1) {
        perror("mq_open");
        return -1;
    }

    /* 创建两个线程 一个用于发送 一个用于接收 */
    pthread_t pt1, pt2;
    pthread_create(&pt1, NULL, pt1_send, NULL);
    pthread_create(&pt2, NULL, pt2_recv, NULL);

    pthread_join(pt1, NULL);
    pthread_join(pt2, NULL);

    mq_close(mqd);
    mq_unlink(mq_name);
    return 0;
}