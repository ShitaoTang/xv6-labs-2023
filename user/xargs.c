#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

void xargs(int argc, char* argv[])
{
    char buf[512], arg[512], ch;
    int index = 0;
    char *cmd = argv[0];
    char** pure_argv = argv;

    while (read(0, (char*)&ch, 1) > 0) {
        buf[index++] = ch;
    }

    int k = 0;
    for (int i = 0; i < index; ++i) {
        if (buf[i] == '\n') {
            arg[k] = '\0';
            k = 0;
            pure_argv[argc] = arg;

            if (fork() == 0) {
                exec(cmd, pure_argv);
            }
            else {
                wait(0);
            }
        }
        else {
            arg[k++] = buf[i];
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