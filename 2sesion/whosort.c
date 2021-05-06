#include <unistd.h>
#include <stdio.h>
#include <wait.h>


int main(void){
    int fds[2];
    pipe(fds);

    if(fork() == 0){
        dup2(fds[1],1);
        close(fds[0]);
        execlp("who","who",NULL);
    } else if ( fork() == 0){
        dup2(fds[0],0);
        close(fds[1]);
        execlp("sort","sort", NULL);
    } else{
        close(fds[0]);
        close(fds[1]);
        wait(0); wait(0);
    }
    return 1;





}