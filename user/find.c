#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char* fmtname(char *path)
{
    static char buf[DIRSIZ+1];
    char *p;

    for (p=path+strlen(path); p>=path && *p!='/'; p--)
        ;
    p++;

    if (strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));
    memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
    return buf;
}

void find(char *path, char *pattern)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type) {
    case T_DEVICE:
    case T_FILE:
        fprintf(2, "find: %s is not a valid path\n", path);
        break;

    case T_DIR:
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
            fprintf(2, "find: path is too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';     // form a sub-path
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (de.inum == 0)
                continue;
            if (strcmp(de.name, ".")==0 || strcmp(de.name, "..")==0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if (stat(buf, &st) < 0) {
                fprintf(2, "find: cannot stat %s\n", buf);
                continue;
            }
            switch (st.type)
            {
            case T_DEVICE:
            case T_FILE:
                if (strcmp(de.name, pattern) == 0)
                    printf("%s\n", buf);
                break;
            case T_DIR:
                find(buf, pattern);
                break;
            }
        }
    }
}

int main(int argc, char *argv[]) 
{
    if (argc != 3) {
        fprintf(2, "Usage: find directory pattern...\n");
        exit(1);
    }

    find(argv[1], argv[2]);
    exit(0);
}