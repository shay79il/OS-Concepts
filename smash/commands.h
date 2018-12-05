#ifndef _COMMANDS_H
#define _COMMANDS_H
#include <unistd.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>
#include "signals.h"
#include "jobs.h"

#define MAX_HISTORY_SIZE 50
#define MAX_LINE_SIZE 80
#define MAX_ARG 20


char lastDir[MAX_LINE_SIZE];
char currentDir[MAX_LINE_SIZE];
char* cmdsHistory[MAX_HISTORY_SIZE];

int CmdsHistoryInsert(char *cmd);

int ExeComp(char* cmdString);
int BgCmd(char* cmdString);
int ExeCmd(char* cmdString);
void ExeExternal(char *args[MAX_ARG]);
#endif

