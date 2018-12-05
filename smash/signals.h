#ifndef _SIGS_H
#define _SIGS_H
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "jobs.h"

// Ctrl+Z
void CatchSIGTSTP(int sigNum);

// Ctrl+C
void CatchSIGINT(int sigNum);

// SIGCHLD
void CatchSIGCHLD(int sigNum);

#endif

