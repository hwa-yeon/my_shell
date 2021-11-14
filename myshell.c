#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX 1024

char* history[10];
int now_hist = 0;

void history_init(char** buf_history) {
    for(int i=0; i<10; ++i) {
        buf_history[i] = NULL;
    }
}

void history_print(char** buf_history, int cur) {
    int n = cur;
    do {
        if(buf_history[n]) {
            fprintf(stdout, "%s\n", buf_history[n]);
        }
        n = (n+1) % 10;
    }while(n != cur);
}

void history_clear(char** buf_history) {
    for(int i=0;i<10;++i) {
        free(buf_history[i]);
        buf_history[i] = NULL;
    }
}

int prompt(char* cmd) {
    char ch;
    int n = 0;

    fflush(NULL);
    fprintf(stdout, "12191659_shell$ ");

    while(ch = fgetc(stdin)) {
        if(ch == 10) {  // ENTER key
            break;
        }
        else {
            cmd[n++] = ch;
        }
    }
}

void parse(char* cmd, char** arg, int* arg_cnt) {
    int n = 0;
    char* tmp = strtok(cmd, " \n");

    while(tmp != NULL) {
        arg[n] = tmp;
        tmp = strtok(NULL, " \n");
        n++;
    }
    arg[n] = NULL;
    *arg_cnt = n;
}

bool execute(char** arg,int  arg_cnt) {
    int status;
    pid_t pid;

    bool background = (strcmp(arg[arg_cnt-1],"&")==0) ? 1 : 0;
    
    pid = fork();
    if(pid == -1) {
        fprintf(stdout, "fork error\n");
        return 0;
    }
    else if(pid == 0) {
        if(execvp(arg[0], arg) == -1) {
            fprintf(stdout, "execvp error\n");
            return 0;
        }
        exit(0);
    }
    else {
        if(background == 1) {
            return 1;
        }
        else {
        wait(&status);
        return 1;   
        }
    }
}

int main(int argc, char** argv) {
    char cmd[MAX];
    history_init(history);

    while(1) {
        char* arg[MAX];
        int arg_cnt = 0;

        cmd[0] = '\0';

        prompt(cmd);
        parse(cmd, arg, &arg_cnt);

        history[now_hist] = strdup(arg[0]);
        now_hist = (now_hist+1) % 10;

        if(arg_cnt == 0) continue;
        if(strcmp(arg[0],"quit") == 0 || strcmp(cmd,"quit\0") == 0) {
            fprintf(stdout, "myshell developed by leehwayeon(12191659)\n");
            exit(0);
        }
        else if(strcmp(arg[0],"history") == 0 || strcmp(cmd,"history\0") == 0) {
            history_print(history, now_hist);
        }
        else if(strcmp(arg[0], "help") == 0 || strcmp(arg[0], "help\0") == 0) {
            fprintf(stdout, "my shell is simple shell.\n");
            fprintf(stdout, "12191659 Lee hwa Yeon\n");
        }
        else {
           execute(arg, arg_cnt);
        }
    }
    history_clear(history);

    return 0;
}