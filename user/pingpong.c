#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    if (argc != 1) {
        fprintf(2, "Usage: pingpong...");
        exit(1);
    }

    int pipe1[2], pipe2[2];
    char *byte1 = "ping", *byte2 = "pong";

    // data wriiten to pipe[1] can be read from pipe[0]
    pipe(pipe1);
    pipe(pipe2);

    // child process receives "ping"
    if(fork() == 0) {
        close(pipe1[1]);
        close(pipe2[0]);

        read(pipe1[0], (char*)byte1, strlen(byte1));
        close(pipe1[0]);
        fprintf(1, "%d: received %s\n", getpid(), byte1);

        write(pipe2[1], (char*)byte2, strlen(byte2));
        close(pipe2[1]);

        exit(0);
    }
    else {  // parent process receives "pong"
        close(pipe1[0]);
        close(pipe2[1]);

        write(pipe1[1], (char*)byte1, strlen(byte1));
        close(pipe1[1]);

        read(pipe2[0], (char*)byte2, strlen(byte2));
        close(pipe2[0]);
        fprintf(1, "%d: received %s\n", getpid(), byte2);

        wait(0);
    }

    exit(0);
}