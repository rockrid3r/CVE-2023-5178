#define FUSE_USE_VERSION 30

#include <fuse3/fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <sys/uio.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/xattr.h>

#include <sys/param.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define EXIT_ON_ERR(cond) \
    if (cond) { \
        perror(#cond); \
        exit(1); \
    }

struct msgbuf {
    long mtype;
    char mtext[1];
};

char file_buffer[4096];
unsigned int file_size = sizeof(file_buffer);
static int xpl_msqid[1000];
static int fuse_msqid[1000];

static int FUSE_getattr(const char *path, struct stat *stbuf, struct fuse_file_info* _fi){
    int res = 0;
    memset(stbuf, 0, sizeof(struct stat));
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    } else {
        stbuf->st_mode = S_IFREG | 0666;
        stbuf->st_nlink = 1;
        stbuf->st_size = file_size;
        stbuf->st_blocks = 0;
    }
    return res;
}

static int FUSE_readdir(
            const char *path, 
            void *buf, 
            fuse_fill_dir_t filler, 
            off_t offset, 
            struct fuse_file_info *fi,
            enum fuse_readdir_flags _frdf) {
    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);
    return 0;
}

static int FUSE_open(const char *path, struct fuse_file_info *fi) {
#if FUSE_LOG_LEVEL >= 4
    char log[50];
    snprintf(log, sizeof(log), "[fuse] Opened file %s", path);
    printf("%s\n", log);
#endif
    return 0;
}

static int FUSE_read(const char *path, char *dst, size_t size, off_t offset, struct fuse_file_info *fi){
#if FUSE_LOG_LEVEL >= 4
    char log[50];
    snprintf(log, sizeof(log), "[fuse] Reading file %s, size: %lu", path, size);
    printf("%s\n", log);
#endif

    int i = atoi(path + 1); // without leading '/'

    size_t read_size = MIN(file_size, size);

    memcpy(dst, file_buffer, read_size);

    char buf[0x100];
    struct msgbuf* msgp = (void*)buf;
    msgp->mtype = 1337;

#if FUSE_LOG_LEVEL >= 1
    printf("[fuse] Signaling thread %d was blocked\n", i);
#endif
    EXIT_ON_ERR(msgsnd(fuse_msqid[i], msgp, 0, 0) < 0); // signal that thread was blocked

#if FUSE_LOG_LEVEL >= 1
    printf("[fuse] Waiting for signal to unblock thread %d\n", i);
#endif
    EXIT_ON_ERR(msgrcv(xpl_msqid[i], msgp, 0, 0, 0) < 0); // wait until signal to unblock
#if FUSE_LOG_LEVEL >= 1
    printf("[fuse] Unblocking thread %d\n", i);
#endif
    
    return read_size;
}


static int FUSE_write(const char *path, const char *src, size_t size, off_t offset, struct fuse_file_info *fi ){
	return size;
}

static struct fuse_operations FUSE_ops = {
    .open       = FUSE_open,
    .read       = FUSE_read,
    .write 	    = FUSE_write,
    .readdir    = FUSE_readdir,
    .getattr    = FUSE_getattr,
};

int main(int argc, char** argv) {
    for (int i = 0; i < 1000; ++i) {
        EXIT_ON_ERR((xpl_msqid[i] = msgget(1337 + i, 0666 | IPC_CREAT)) < 0);
        EXIT_ON_ERR((fuse_msqid[i] = msgget(1336 - i, 0666 | IPC_CREAT)) < 0);
    }
    return fuse_main(argc, argv, &FUSE_ops, NULL);
}
