#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

void xargs(int argc, char* argv[])
{
    char arg[512];
    int k = 0;
    char ch;
    char* cmd = argv[0];
    char* pure_argv[20];

    for (int i = 0; i < argc; i++) {
        pure_argv[i] = argv[i];
    }

    while (read(0, &ch, 1) > 0) {
        if (ch == '\n') {
            arg[k] = '\0';
            pure_argv[argc] = arg;
            pure_argv[argc + 1] = 0;

            if (fork() == 0) {
                exec(cmd, pure_argv);
            } else {
                wait(0);
            }

            k = 0;
        } else {
            arg[k++] = ch;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(2, "Usage: xargs <command> [arg1 ...]\n");
        exit(1);
    }

    char** real_argv = argv;
    for (int i = 0; i < argc-1; ++i)
        real_argv[i] = argv[i+1];

    xargs(argc-1, real_argv);
    exit(0);
}