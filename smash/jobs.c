#include "jobs.h"

int runningPid = NO_PROCESS;

struct _job_t
{
	pid_t pid;
	char name[MAX_NAME_SIZE];
	time_t creatTime;
	bool isRunning;
	struct _job_t *next;
};

job_t* jobsList = NULL;
char* L_Fg_Cmd = NULL;
bool isBgcmd = false;
int numOfJobs = 0;

char* JobGetName(int jobNum)
{
	return JobGet(jobNum)->name;
}

int JobGetPid(int jobNum)
{
	return JobGet(jobNum)->pid;
}

void JobSetRun(int jobNum)
{
	JobGet(jobNum)->isRunning = true;
}

void JobSendKill(int sigNum, int jobNum)
{
	job_t* job = JobGet(jobNum);
	if(kill(job->pid, sigNum))
		printf("smash error: > kill %d - cannot send signal\n",job->pid);
	else
	{
		printf("smash > signal %d was sent to pid %d\n", sigNum, job->pid);
		if (sigNum == SIGCONT)
			job->isRunning = true;
		else if (sigNum == SIGTSTP)
			job->isRunning = false;
		else if (sigNum == SIGINT)
			JobRemovePid(job->pid);
	}
}


int JobGetLastStopped()
{
	int i = 1;
	int stopInx = 0;
	job_t* curJob = jobsList;
	job_t* stopJob = jobsList;

	if (NULL == jobsList)
	{
		printf("jobsList is EMPLTY!!!!\n");
		return 0;
	}

	while (NULL != curJob->next)
	{
		if(false == curJob->isRunning)
		{
			stopJob = curJob;
			stopInx = i;
		}
		curJob = curJob->next;
		i++;
	}

	if (0 == stopInx)
	{
		curJob->isRunning = true;
		return i;
	}
	else
	{
		stopJob->isRunning = true;
		return stopInx;
	}
}


job_t* JobGet(int jobNum)
{
	int curJobNum = 1;
	job_t* curJob = jobsList;

	if (curJob == NULL)
		return NULL;

	if (jobNum > numOfJobs)
		return NULL;

	if(jobNum != curJobNum)
	{
		while (curJob != NULL)
		{
			curJobNum++;
			curJob = curJob->next;
			if (jobNum == curJobNum)
				break;
		}

		if (jobNum == curJobNum)
		{
			return curJob;
		}
		else
		{
			printf("Got EOL & NO cmdNum = %d  Found! NOT GOOD :(\n",jobNum);
			return NULL;
		}
	}
	else
	{
		return curJob;
	}

}


int JobRemovePid(pid_t pid)
{
	job_t* curJob = jobsList;
	job_t* prev = NULL;

	if (curJob == NULL)
		return 1;

	while ((curJob->next != NULL) && (curJob->pid != pid))
	{
		prev = curJob;
		curJob = curJob->next;
	}

	if (curJob->pid == pid)
	{
		if (prev == NULL)
			jobsList = curJob->next;
		else
			prev->next = curJob->next;

		free(curJob);

		numOfJobs--;
		if(numOfJobs < 0)
		{
			printf("JobRemovePid:: numOfJobs = %d (< 0) NOT GOOD :(\n",numOfJobs);
			return 1;
		}
	}

	return 0;
}

int JobRemove(int jobNum)
{
	job_t* curJob = jobsList;
	job_t* prev = NULL;
	int curJobNum = 1;
	if (jobsList == NULL)
		return 1;

	if(curJobNum == jobNum)
	{
		if(NULL != curJob->next)
			jobsList = curJob->next;
		else
			jobsList = NULL;

		free(curJob);
	}
	else
	{
		prev = curJob;
		curJob = curJob->next;

		do{
			curJobNum++;
			if (jobNum == curJobNum)
				break;
			prev = curJob;
			curJob = curJob->next;
		}while (curJob != NULL);

		if (jobNum == curJobNum)
		{
			prev->next = curJob->next;
			free(curJob);
		}
		else
		{
			printf("JobRemove:: Got EOL & NO cmdNum = %d  Found! NOT GOOD :(\n",jobNum);
			return 1;
		}
	}

	numOfJobs--;
	if(numOfJobs < 0)
	{
		printf("JobRemove:: numOfJobs = %d (< 0) NOT GOOD :(\n",numOfJobs);
		return 1;
	}
	return 0;
}

int JobInsert(char* name, pid_t pid, bool isRunning)
{
	job_t* tmpJob = jobsList;
	job_t* newJob = (job_t*)malloc(sizeof(job_t));
	if (NULL == newJob)
	{
		printf("malloc fail\n");
		exit(-1);
	}

	newJob->creatTime = time(NULL);
	strcpy(newJob->name, name);
	newJob->isRunning = isRunning;
	newJob->pid = pid;
	newJob->next = NULL;
	
	if (jobsList == NULL)
	{
		jobsList = newJob;
	}
	else
	{
		while (NULL != tmpJob->next)
		{
			tmpJob = tmpJob->next;
		}
		
		tmpJob->next = newJob;
	}
	numOfJobs++;
	return 0;
}

void JobsPrint()
{
	int i = 1;
	job_t* tmpJob = jobsList;
	while (NULL != tmpJob)
	{
		if(true == tmpJob->isRunning)
			printf("[%d] %s : %d %ld secs\n", i, tmpJob->name, tmpJob->pid,	time(NULL) - tmpJob->creatTime);
		else
			printf("[%d] %s : %d %ld secs (Stopped)\n", i, tmpJob->name, tmpJob->pid, time(NULL) - tmpJob->creatTime);
		
		tmpJob = tmpJob->next;
		i++;
	}
}











