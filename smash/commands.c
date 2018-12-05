//		commands.c
//********************************************
#include "commands.h"

char lastDir[MAX_LINE_SIZE] = "";
char currentDir[MAX_LINE_SIZE] = "";
char* cmdsHistory[MAX_HISTORY_SIZE] = {0};

//********************************************
// function name: cmdsHistoryInsert
// Description: saves the last 50 commands
// Parameters: command string
// Returns: 0 - success,1 - failure
//**********************************************************
int CmdsHistoryInsert(char *cmd)
{
	static int nextIdx = 0;
	int prevIdx;

	if (0 == nextIdx)
		prevIdx = MAX_HISTORY_SIZE - 1;
	else
		prevIdx = nextIdx - 1;

	if (NULL != cmdsHistory[prevIdx])
		 if(!strcmp(cmd, cmdsHistory[prevIdx]))
			 return 0;

	if(!strcmp(cmd, ""))
		return 0;

	if(NULL != cmdsHistory[nextIdx])
		free(cmdsHistory[nextIdx]);

	cmdsHistory[nextIdx] = (char*)malloc(strlen(cmd) + 1);

	if(NULL == cmdsHistory[nextIdx])
	{
		printf("cmdsHistoryInsert :: NULL == cmdsHistory[%d]", nextIdx);
		return 1;
	}

	strcpy(cmdsHistory[nextIdx], cmd);
	nextIdx = (nextIdx + 1) % MAX_HISTORY_SIZE;
	return 0;
}

//********************************************
// function name: ExeCmd
// Description: interprets and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**********************************************************
int ExeCmd(char* cmdString)
{
	char* cmd;
	char* args[MAX_ARG];
	char pwd[MAX_LINE_SIZE];
	char* delimiters = " \t\n";  
	int i = 0, num_arg = 0;
	bool illegal_cmd = false; // illegal command

	cmd = strtok(cmdString, delimiters);
    if (cmd == NULL)
		return 0; 

    args[0] = cmd;
	for (i = 1 ; i < MAX_ARG ; i++)
	{
		args[i] = strtok(NULL, delimiters); 
		if (args[i] != NULL) 
			num_arg++;
	}

	strcpy(L_Fg_Cmd, cmd);

	/***************************************************
	* Built in Commands
	* PLEASE NOTE
	* NOT ALL REQUIRED ARE IN THIS CHAIN OF IF COMMANDS.
	* PLEASE ADD MORE IF STATEMENTS AS REQUIRED
	****************************************************/
	if (!strcmp(cmd, "pwd") )
	{
		if (NULL == getcwd(pwd, sizeof(pwd)))
		{
			illegal_cmd = true;
		}
		else
		{
			printf("%s\n", pwd);
			return 0;
		}
	} 
	
	/*************************************************/
	else if (!strcmp(cmd, "cd"))
	{
		if(num_arg > 1)
		{
			illegal_cmd = true;
		}
		else
		{
			if(!strcmp(args[1] ,"-"))
				args[1] = lastDir;

			if (-1 == chdir(args[1]))
			{
				illegal_cmd = true;
			}
			else
			{
				strcpy(lastDir, currentDir);
				strcpy(currentDir, args[1]);
			}
		}
	}
	
	/*************************************************/
	else if (!strcmp(cmd, "mkdir"))
	{
		if ((num_arg != 1) || (mkdir(args[1], 0x777) == -1))
			illegal_cmd = true;
	}
	/*************************************************/
	else if (!strcmp(cmd, "jobs")) 
	{
		if(num_arg > 0)
			illegal_cmd = true;
		else
			JobsPrint();
	}
	/*************************************************/
	else if (!strcmp(cmd, "showpid")) 
	{
		if(0 != num_arg)
			illegal_cmd = true;
		else
			printf("‫‪smash‬‬ ‫‪pid‬‬ ‫‪is‬‬ %d\n",getpid());
	}
	/*************************************************/
	else if (!strcmp(cmd, "history"))
	{
		int historySize = 0;
		if (num_arg > 1)
		{
			illegal_cmd = true;
		}
		else if (1 == num_arg)
		{
			historySize = atoi(args[1]);
			if (0 == historySize)
				illegal_cmd = true;
		}
		else if(0 == num_arg)
		{
			historySize = MAX_HISTORY_SIZE;
		}
		int i;
		for(i = 0 ; i < historySize ; i++)
		{
			if(NULL == cmdsHistory[i]) break;
			printf("%s\n",cmdsHistory[i]);
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "mv"))
	{
		if ((num_arg != 2) || (rename(args[1], args[2]) != 0))
			illegal_cmd = true;
		else
			printf("%s ‫‪has‬‬ ‫‪been‬‬ ‫‪renamed‬‬ to %s\n", args[1], args[2]);
	}
	/*************************************************/
	else if (!strcmp(cmd, "fg")) 
	{
		int jobNum;
		if (1 == num_arg)
			jobNum = atoi(args[1]);

		if ( (num_arg > 1) || (NULL == jobsList) ||
			( (num_arg == 1) && ((jobNum < 1) || (jobNum > numOfJobs))) )
		{
			illegal_cmd = true;
		}
		else
		{
			if (0 == num_arg)
				jobNum = numOfJobs;

			strcpy(L_Fg_Cmd, JobGetName(jobNum));
			runningPid = JobGetPid(jobNum);
			kill(runningPid, SIGCONT);
			printf("smash > signal SIGCONT was sent to pid %d\n", runningPid);
			JobRemove(jobNum);
			waitpid(runningPid, NULL, 0);
		}
	} 
	/*************************************************/
	else if (!strcmp(cmd, "bg")) 
	{
  		int jobNum;
		if (1 == num_arg)
			jobNum = atoi(args[1]);

		if ( (num_arg > 1) || (NULL == jobsList) ||
			( (num_arg == 1) && ((jobNum < 1) || (jobNum > numOfJobs))) )
		{
			illegal_cmd = true;
		}
		else
		{
			if (0 == num_arg)
				jobNum = JobGetLastStopped();

			JobSetRun(jobNum);
			printf("%s\n", JobGetName(jobNum));
			kill(JobGetPid(jobNum), SIGCONT);
			printf("smash > signal SIGCONT was sent to pid %d\n", runningPid);
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
		bool killflag = false;
		if (num_arg > 1)
		{
			illegal_cmd = true;
		}
		if (num_arg == 1)
		{
			if (!strcmp(args[1], "kill"))
			{
				killflag = true;
			}
			else
			{
				illegal_cmd = true;
			}
		}
		if (killflag)
		{
			int jobsKILL=0;
			int jobN = 1;
			while (numOfJobs > 0)
			{
				int currpid;
				currpid = JobGetPid(jobN);
				printf("[%d] %s sending SIGTERM...",(int) (jobN+jobsKILL) ,JobGetName(jobN));
				kill(currpid, SIGCONT);
				kill(currpid, SIGTERM);
				sleep(5);
				if ( JobGet(jobN) != NULL )
				{
					if (currpid == JobGetPid(jobN))
					{
						printf("(5 seconds passed) sending SIGKILL... ");
						kill(currpid, SIGKILL);
					}
				}
				printf("DONE\n");
				jobsKILL++;
			}
		}
		if (illegal_cmd == false)
			exit(0);
	} 
	/*************************************************/
	else if (!strcmp(cmd, "kill"))
	{
		int jobNum;
		int sigNum;
		if (2 == num_arg)
		{
			jobNum = atoi(args[2]);
			sigNum = atoi(args[1]) * (-1);
		}

		if ((num_arg != 2) || (NULL == jobsList) || (args[1][0] != '-'))
		{
			illegal_cmd = true;
		}
		else
		{
			if((jobNum < 1) || (jobNum > numOfJobs))
				printf("smash error: > kill %d - job does not exist\n", jobNum);
			else
				JobSendKill(sigNum, jobNum);
		}
	}
	/*************************************************/
	else // external command
	{
 		ExeExternal(args);
	}

	if (illegal_cmd == true)
	{
		printf("smash error: > \"%s\"\n", cmdString);
		return 1;
	}

	L_Fg_Cmd[0] = '\0';
    return 0;
}
//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command string, external command arguments
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG])
{
	pid_t pid;
    switch(pid = fork())
	{
		case -1:
				perror("fork() FAILED");
				exit(-1);
				break;

		case 0 :// Child Process
				setpgrp();
				execv(args[0], args);
				execvp(args[0], args);
				perror("execv & execvp FAILED");
				exit(-1);
				break;

		default://Father process
				runningPid = pid;
				waitpid(pid, NULL, 0);
	}
}
//**************************************************************************************
// function name: ExeComp
// Description: executes complicated command
// Parameters: command string
// Returns: 0- if complicated -1- if not
//**************************************************************************************
int ExeComp(char* cmdString)
{
	char* cmd = NULL;
	char* ExtCmd[5] = { "csh", "-f", "-c", cmdString, NULL };
	bool isBgCmd = cmdString[strlen(cmdString) - 2] == '&';
	char *delimiters = " \t\n";
	pid_t pid;

    if ((strstr(cmdString, "|" )) ||
    	(strstr(cmdString, "<" )) ||
    	(strstr(cmdString, ">" )) ||
		(strstr(cmdString, "*" )) ||
		(strstr(cmdString, "?" )) ||
		(strstr(cmdString, ">>")) ||
		(strstr(cmdString, "|&")) )
    {
    	cmd = strtok(cmdString, delimiters);
		if (cmd == NULL)
			return 0;

		if(isBgCmd)
			cmdString[strlen(cmdString) - 2] = '\0';

    	switch(pid = fork())
		{
			case -1:
					perror("fork() FAILED");
					exit(-1);
					break;

			case 0 :// Child Process

					setpgrp();
					execvp(ExtCmd[0], ExtCmd);
					execv(ExtCmd[0], ExtCmd);
					perror("execv & execvp FAILED");
					exit(-1);
					break;

			default://Father process
					if (true == isBgCmd)
						JobInsert(cmd, pid, false);
					else
					{
						strcpy(L_Fg_Cmd, cmd);
						runningPid = pid;
						waitpid(pid, NULL, 0);
					}
		}
		return 0;
	} 
	return -1;
}
//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char* cmdString)
{
	char* Command;
	char* delimiters = " \t\n";
	char *args[MAX_ARG];
	pid_t pid;

	if (cmdString[strlen(cmdString)-2] == '&')
	{
		cmdString[strlen(cmdString)-2] = '\0';

		Command = strtok(cmdString, delimiters);
		if (Command == NULL)
			return 0;

		args[0] = Command;
		int i;
		for (i = 1; i < MAX_ARG; i++)
			args[i] = strtok(NULL, delimiters);

		switch(pid = fork())
		{
			case -1:
					perror("fork() FAILED");
					exit(-1);
					break;

			case 0: // Child Process
					setpgrp();
					execv(Command, args);
					execvp(Command, args);
					perror("execv & execvp FAILED\n");
					exit(-1);
					break;

			default: //Father process
					runningPid = pid;
					if(-1 == JobInsert(Command, pid, true))
						exit(-1);
					break;
		}
		return 0;
	}
	return -1;
}
