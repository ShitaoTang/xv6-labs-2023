#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void pipe_next(int);

int main(int argc, char *argv[])
{
    if (argc != 1) {
        fprintf(2, "Usage: primes...");
        exit(1);
    }
    
    int p[2];
    pipe(p);

    for (int i = 2; i <= 35; ++i) {
        write(p[1], (int *)&i, 4);
    }
    close(p[1]);

    pipe_next(p[0]);
    close(p[0]);
    exit(0);
}

void pipe_next(int fd) {
    int prime_num, num = -1;

    read(fd, (int*)&prime_num, 4);
    printf("prime %d\n", prime_num);

    int p[2];
    pipe(p);

    while (read(fd, (int*)&num, 4) > 0) {
        if (num % prime_num != 0) {
            write(p[1], (int*)&num, 4);
        }
    }
    close(fd);
    close(p[1]);
    if (num == -1) {    // no data is available
        close(p[0]);
        return; 
    }

    if (fork() == 0){   // child process
        pipe_next(p[0]);
        close(p[0]);
    }
    else {              // parent process
        close(p[0]);
        wait(0);
    }
}