#define _POSIX_C_SOURCE 200809L
#include "common.h"
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
static void usage(const char *a){fprintf(stderr,"Usage: %s <cmd> [args]\n",a); exit(1);}
static double d(struct timespec a, struct timespec b){
 return (b.tv_sec-a.tv_sec)+(b.tv_nsec-a.tv_nsec)/1e9;}
int main(int c,char**v){
/* TODO : ADD CODE HERE
*/
    if (c < 2){
        usage(v[0]);
    }

    struct timespec start, end;
    pid_t pid;
    int status;

    clock_gettime(CLOCK_MONOTONIC, &start);

    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        execvp(v[1], &v[1]);
        perror("execvp");
        exit(127);
    }

    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid");
        exit(1);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    if (WIFEXITED(status)) {
        printf("pid=%d elapsed=%.3f exit=%d\n", pid, d(start, end), WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("pid=%d elapsed=%.3f exit=%d\n", pid, d(start, end),WTERMSIG(status));
    }



    return 0;
}
