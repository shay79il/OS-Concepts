#ifndef JOBS_H_
#define JOBS_H_

#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define NO_PROCESS -1
typedef struct _job_t job_t;

extern job_t* jobsList;
extern bool isBgcmd;
extern char* L_Fg_Cmd;
extern int numOfJobs;
extern int runningPid;

#define MAX_JOBS_NUM 100
#define MAX_NAME_SIZE 50


int JobInsert(char* name, pid_t pid, bool isRunning);
int JobRemovePid(pid_t pid);
int JobRemove(pid_t pid);
job_t* JobGet(int jobNum);
char* JobGetName(int jobNum);
int JobGetPid(int jobNum);
int JobGetLastStopped();
void JobSetRun(int jobNum);
void JobSendKill(int sigNum, int jobNum);
void JobsPrint();


#endif /* JOBS_H_ */
