#include "common.h"
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
static void usage(const char *a){fprintf(stderr,"Usage: %s <pid>\n",a); exit(1);}
static int isnum(const char*s){for(;*s;s++) if(!isdigit(*s)) return 0; return 1;}

static void procinfo(__PID_T_TYPE pid)
{
    char path[256];
    FILE *fp;

    snprintf(path, sizeof(path), "/proc/%d/stat", pid);
    fp = fopen(path, "r");
    if (!fp) {
        perror("stat");
        return;
    }

    char state;
    int ppid;
    unsigned long userTime, sysTime;

    fscanf(fp,
           "%*d " //skip process id        
           "%*[^)]"     //skip the cmd within parenthesis 
           ") %c %d " //reads state and parent pid
           "%*s %*s %*s %*s %*s %*s %*s %*s " //skip
           "%lu %lu", //reads times
           &state, &ppid, &userTime, &sysTime);//locates the necessary information provided by proc

    fclose(fp);

    long clk = sysconf(_SC_CLK_TCK);
    double cpu_time = (double)(userTime + sysTime) / clk; //add up the clk values

    snprintf(path, sizeof(path), "/proc/%d/cmdline", pid); 
    fp = fopen(path, "r");
    if (!fp) {
        perror("cmdline");
        return;
    }

    char cmdline[4096];
    size_t n = fread(cmdline, 1, sizeof(cmdline) - 1, fp); //retrieve entire cmdline
    fclose(fp);

    if (n == 0) {
        strcpy(cmdline, "[empty]");
    } else {
        cmdline[n] = '\0';
        for (size_t i = 0; i < n - 1; i++)
            if (cmdline[i] == '\0') {
                cmdline[i] = ' '; //remove null character
            }
    }

    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    fp = fopen(path, "r");
    if (!fp) {
        perror("status");
        return;
    }

    char line[256];
    char vmrss[64] = "0 kB";

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "VmRSS:", 6) == 0) { //locate the VmRSS data
            sscanf(line, "VmRSS: %63[^\n]", vmrss); //Copy data
            break;
        }
    }

    fclose(fp);
    printf("PID:       %d\n", pid);
    printf("State:     %c\n", state);
    printf("PPID:      %d\n", ppid);
    printf("Cmd:       %s\n", cmdline);
    printf("CPU time:  %.3f seconds\n", cpu_time);
    printf("VmRSS:     %s\n", vmrss);
}


int main(int c,char**v){
 if(c!=2||!isnum(v[1])) usage(v[0]);
 procinfo(atoi(v[1]));
 return 0;
}
