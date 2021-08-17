#include <signal.h>
#include <czmq.h>

static bool keepRunning = true;
zsock_t *clientPULL;
zsock_t *clientPUSH;

void intHandler(int dummy) {
    puts ("bye bye.");
    zctx_interrupted = 1;
    zsys_interrupted = 1;
    zsock_destroy  (&clientPUSH);
    zsock_destroy  (&clientPULL);
    keepRunning = 0;
}

int main (void)
{
    // https://github.com/zeromq/goczmq/issues/46#issuecomment-59521223
    // prevent the sighandler override
    setenv ("ZSYS_SIGHANDLER", "false", 1);
    signal (SIGINT, intHandler);
    //  Create and bind clients sockets
    clientPULL = zsock_new (ZMQ_PULL);
    zsock_connect (clientPULL, "tcp://127.0.0.1:9001");
    clientPUSH = zsock_new (ZMQ_PUSH);
    zsock_connect (clientPUSH, "tcp://127.0.0.1:9000");
    
    zstr_send (clientPUSH, "Hello");
    puts ("pushed message.");

    puts ("listening.");
    while (keepRunning) {
        char *msg = zstr_recv (clientPULL);
        if (msg != NULL) {
            puts (msg);
            zstr_send (clientPUSH, msg);
            sleep(1);
        }
        zstr_free (&msg);
    }

    return 0;
}