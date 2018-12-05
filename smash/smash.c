/*	smash.c
main file. This file contains the main function of smash
*******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "commands.h"
#include "signals.h"
#include "jobs.h"

#define MAX_LINE_SIZE 80
#define MAXARGS 20

char lineSize[MAX_LINE_SIZE];

//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv[])
{
    char cmdString[MAX_LINE_SIZE];

	//	signal declarations
	//	NOTE: The signal handlers and the function/s that sets the handler should be found in siganls.c
	
    sigset_t sa_mask;
    sigfillset(&sa_mask);

    //Ctrl+Z
	struct sigaction act_SIGTSTP;
	act_SIGTSTP.sa_mask = sa_mask;
	act_SIGTSTP.sa_flags = 0;
	act_SIGTSTP.sa_handler = &CatchSIGTSTP;
	sigaction(SIGTSTP, &act_SIGTSTP, NULL);

	//Ctrl+C
	struct sigaction act_SIGINT;
	act_SIGINT.sa_mask = sa_mask;
	act_SIGINT.sa_flags = 0;
	act_SIGINT.sa_handler = &CatchSIGINT;
	sigaction(SIGINT, &act_SIGINT, NULL);

	struct sigaction act_SIGCHLD;
	act_SIGCHLD.sa_mask = sa_mask;
	act_SIGCHLD.sa_flags = 0;
	act_SIGCHLD.sa_handler = &CatchSIGCHLD;
	sigaction(SIGCHLD, &act_SIGCHLD, NULL);
	/************************************/
	// Init globals
	if (NULL == getcwd(currentDir, sizeof(char)*MAX_LINE_SIZE))
	{
		printf("getcwd() FAILED!!\n");
		perror("getcwd() error");
		return 1;
	}
	strcpy(lastDir, currentDir);

	
	L_Fg_Cmd =(char*)malloc(MAX_LINE_SIZE+1);
	if (L_Fg_Cmd == NULL)
			exit (-1);
	L_Fg_Cmd[0] = '\0';
	

	while (1)
	{
	 	printf("smash > ");

		/* initialize for next line read*/
	 	runningPid = NO_PROCESS;
	 	lineSize[0]='\0';
		cmdString[0]='\0';

		fgets(lineSize, MAX_LINE_SIZE, stdin);
		strcpy(cmdString, lineSize);
		cmdString[strlen(cmdString)-1]='\0';


		if (1 == CmdsHistoryInsert(cmdString))
			exit(-1);

		// Perform a complicated Command
		if(!ExeComp(lineSize))
			continue;


		// Background command
		if(!BgCmd(lineSize))
			continue;

	 	// built in commands
		ExeCmd(lineSize);
	}

    return 0;
}

