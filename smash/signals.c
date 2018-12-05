// signals.c
// contains signal handler functions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
#include "signals.h"


void CatchSIGCHLD(int sigNum)
{
	int pid = waitpid(-1, NULL, WNOHANG);
	if(0 == pid)
		return;
	JobRemovePid(pid);
}


// Ctrl+Z
void CatchSIGTSTP(int sigNum)
{
	if(NO_PROCESS == runningPid)
	{
		printf("\n");
	}
	else
	{
		JobInsert(L_Fg_Cmd, runningPid, false);
		L_Fg_Cmd[0] = '\0';

		if (!kill(runningPid, SIGTSTP))
			printf("smash > signal SIGTSTP was sent to pid %d\n",runningPid);
		else
			perror("CTRL Z failed \n");

		runningPid = NO_PROCESS;
	}
}

// Ctrl+C
void CatchSIGINT(int sigNum)
{
	if(NO_PROCESS == runningPid)
	{
		printf("\n");
	}
	else
	{
		if (!kill(runningPid, SIGINT))
			printf("smash > signal SIGINT was sent to pid %d\n",runningPid);
		else
			perror("CTRL C failed \n");

		runningPid = NO_PROCESS;
	}
}
