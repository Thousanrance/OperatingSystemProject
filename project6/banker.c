#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4
#define MAX_BUFFER_SIZE 100

/* the available amount of each resource */
int available[NUMBER_OF_RESOURCES];

/*the maximum demand of each customer */
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the amount currently allocated to each customer */
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the remaining need of each customer */
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

int request[NUMBER_OF_RESOURCES];
int release[NUMBER_OF_RESOURCES];

/* return 0 if successful and –1 if unsuccessful */
int request_resources(int customer_num, int request[])
{
	int new_available[NUMBER_OF_RESOURCES];
	int new_allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
	int new_need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
	
	/*calculate the new state*/
	for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
	{
		for(int j = 0; j < NUMBER_OF_RESOURCES; j++)
		{
			new_allocation[i][j] = allocation[i][j];
			new_need[i][j] = need[i][j];
		}
	}
	for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
	{
		new_available[i] = available[i] - request[i];
		if(new_available[i] < 0)
		{
			return -1;
		}
		new_allocation[customer_num][i] += request[i];
		if(new_allocation[customer_num][i] > maximum[customer_num][i])
		{
			return -1;
		}
		new_need[customer_num][i] -= request[i];
	}
	
	/* whether the new state is safety*/
	/*
	1. Let Work and Finish be vectors of length m and n, respectively. Initialize
	Work = Available and Finish[i] = false for i = 0, 1, ..., n − 1.
	
	2. Find an index i such that both
		a. Finish[i] == false
		b. Needi ≤ Work
	   If no such i exists, go to step 4.
	
	3. Work = Work + Allocationi
	   Finish[i] = true
	   Go to step 2.
	
	4. If Finish[i] == true for all i, then the system is in a safe state.
	*/
	/* step 1 */
	int work[NUMBER_OF_RESOURCES];
	for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
	{
		work[i] = new_available[i]; 
	}
	
	int choose; //which process to be the next in safety sequence
	
	int finish[NUMBER_OF_CUSTOMERS]; //which process is already finished
	for(int i = 0; i<NUMBER_OF_CUSTOMERS; i++)
	{
		finish[i] = 0;
	}
	
	int lessthan; //need <= available
	
	while(1)
	{
		/* step 2 */
		choose = -1;
		for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
		{
			if(finish[i] == 1)
			{
				continue;
			}
			lessthan = 0;
			for(int j = 0; j < NUMBER_OF_RESOURCES; j++)
			{
				if(new_need[i][j] > work[j])
				{
					break;
				}
				lessthan++;
			}
			if(lessthan == NUMBER_OF_RESOURCES) //need <= work
			{
				choose = i;
				break;
			}
		}
		
		/* step 4 */
		if(choose == -1)
		{
			for(int i = 0; i<NUMBER_OF_CUSTOMERS; i++)
			{
				if(finish[i] == 0) //the new state is unsafe
				{
					printf("The other processes can't finish!\n");
					return -1;
				}
			}
			break; //the new state is safe
		}
		
		/* step 3 */
		for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
		{
			work[i] += new_allocation[choose][i];
		}
		finish[choose] = 1;
		printf("T[%d] can finish!\n", choose);
	}
	/* renew the state */
	for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
	{
		available[i] = new_available[i];
		allocation[customer_num][i] = new_allocation[customer_num][i];
		need[customer_num][i] = new_need[customer_num][i];
	}
	return 0;
}

void release_resources(int customer_num, int release[])
{
	for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
	{
		if(allocation[customer_num][i] < release[i])
		{
			printf("Release failed!\n");
			return;
		} 
	}
	for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
	{
		allocation[customer_num][i] -= release[i]; 
		available[i] += release[i]; 
		need[customer_num][i] += release[i];
	}
}

void show_current_state()
{
	printf("Allocation[][]:\n");
	for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
	{
		printf("T[%d] ", i);
		for(int j = 0; j < NUMBER_OF_RESOURCES; j++)
		{
			printf("allocation[%d][%d] = %d ", i, j, allocation[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	
	printf("Max[][]:\n");
	for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
	{
		printf("T[%d] ", i);
		for(int j = 0; j < NUMBER_OF_RESOURCES; j++)
		{
			printf("maximum[%d][%d] = %d ", i, j, maximum[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	
	printf("Available[]:\n");
	for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
	{
		printf("available[%d] = %d ", i, available[i]);
	}
	printf("\n");
	printf("\n");
	
	printf("Need[][]:\n");
	for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
	{
		printf("T[%d] ", i);
		for(int j = 0; j < NUMBER_OF_RESOURCES; j++)
		{
			printf("need[%d][%d] = %d ", i, j, need[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	
	return;
}

int main(int argc, char *argv[])
{
	/* read the initial available resource */
	for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
	{
		available[i] = atoi(argv[i+1]);
	}
	
	/* read the maximum demand from maximum.txt */
	/* initialize need and allocation */
	FILE *fp = fopen("maximum.txt","r");
	char *tmp;
	char T[MAX_BUFFER_SIZE];
	for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
	{
		fgets(T, MAX_BUFFER_SIZE, fp);
		tmp = strdup(T);
		for(int j = 0; j < NUMBER_OF_RESOURCES; j++)
		{
			maximum[i][j] = atoi(strsep(&tmp, ","));
			need[i][j] = maximum[i][j];
			allocation[i][j] = 0;
		}
	}
	fclose(fp);
	
	//show_current_state();
	
	/* banker> */
	int should_run = 1;
	char input[MAX_BUFFER_SIZE];
	while(should_run)
	{
		printf("banker>");
		fflush(stdout);
		
		fgets(input, MAX_BUFFER_SIZE, stdin);
		
		/* exit */
		if(strcmp(input, "exit\n") == 0)
		{
			should_run = 0;
			continue;
		}
		
		/* RQ & RL */
		int customer_num;
		if(input[0] == 'R')
		{
			if(input[1] == 'Q')
			{
				tmp = strdup(input);
				strsep(&tmp, " "); //remove "RQ"
				
				customer_num = atoi(strsep(&tmp, " ")); //which customer
				for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
				{
					request[i] = atoi(strsep(&tmp, " "));
				}
				
				if(request_resources(customer_num, request) == -1)
				{
					printf("Request unsuccessfully!\n");
				}
				else
				{
					printf("Request successfully!\n");
				}
				continue;
			}
			else if(input[1] == 'L')
			{
				tmp = strdup(input);
				strsep(&tmp, " "); //remove "RL"
				
				customer_num = atoi(strsep(&tmp, " ")); //which customer
				for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
				{
					release[i] = atoi(strsep(&tmp, " "));
				}
				release_resources(customer_num, release);
				continue;
			}
		}
		/* output current state */
		else if(input[0] == '*')
		{
			show_current_state();
		}
	}
	
	return 0;
}