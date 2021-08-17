#include <signal.h>
#include <czmq.h>

static bool keepRunning = true;
zsock_t *serverPULL;
zsock_t *serverPUSH;

void intHandler(int dummy) {
    puts ("bye bye.");
    zctx_interrupted = 1;
    zsys_interrupted = 1;
    zsock_destroy  (&serverPULL);
    zsock_destroy  (&serverPUSH);
    keepRunning = 0;
}

int main (void)
{
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
        char *msg = zstr_recv (serverPULL);
        if (msg != NULL) {
            puts (msg);
            // this doesn't work:
            // strcpy(msg, " from server");
            // zstr_send (serverPUSH, msg);
            
            // this does:
            char *reply = malloc (sizeof(char) * strlen(msg) + sizeof(char) * 11 + 1);
            strcpy(reply, msg);
            strcat(reply, " from server");
            zstr_send (serverPUSH, reply);
            free(reply);
        }
        zstr_free (&msg);
    }

    return 0;
}