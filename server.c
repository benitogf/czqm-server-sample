#include <signal.h>
#include <czmq.h>

static bool keepRunning = true;
static zsock_t *serverPULL;
static zsock_t *serverPUSH;
static const char ECHO[] = " from server";

void intHandler(int dummy) {
    puts("bye bye.");
    zctx_interrupted = 1;
    zsys_interrupted = 1;
    zsock_destroy(&serverPULL);
    zsock_destroy(&serverPUSH);
    keepRunning = 0;
}

char* newReply(char *msg) {
    char *reply = malloc(sizeof(char) * strlen(msg) + sizeof(char) * strlen(ECHO) + 1);
    if (reply == NULL) {
        puts ("failed to allocate.");
        return NULL;
    }
    return reply;
}

int main (void) {
    // https://github.com/zeromq/goczmq/issues/46#issuecomment-59521223
    // prevent the sighandler override
    setenv ("ZSYS_SIGHANDLER", "false", 1);
    signal (SIGINT, intHandler);
    //  Create and bind server sockets
    serverPULL = zsock_new (ZMQ_PULL);
    zsock_bind (serverPULL, "tcp://127.0.0.1:9000");
    serverPUSH = zsock_new (ZMQ_PUSH);
    zsock_bind (serverPUSH, "tcp://127.0.0.1:9001");

    
    puts ("listening.");
    while (keepRunning) {
        char *msg = zstr_recv(serverPULL);
        if (msg == NULL) {
            continue;
        }
        puts (msg);
        char *reply = newReply(msg);
        if (reply == NULL) {
            puts ("failed to allocate.");
            continue;
        }
        strcpy(reply, msg);
        strcat(reply, ECHO);
        zstr_send(serverPUSH, reply);
        free(reply);
        zstr_free(&msg);
    }

    return 0;
}