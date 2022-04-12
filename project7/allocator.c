#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 100

typedef struct Hole
{
	int id;
	int start;
	int end;
	int size;

	struct Hole* next;
} Hole;

/* memory head */
Hole* memory = NULL;
/* whole memory size */
int MAX;

void first_fit(int id, long size)
{
	Hole* p = memory;

	while (p != NULL)
	{
		if (p->id == -1)
		{
			if (p->size == size) //change the hole to process
			{
				p->id = id;

				return;
			}
			else if (p->size > size) //divide the hole
			{
				Hole* tmp = (Hole*)malloc(sizeof(Hole));
				tmp->id = -1;
				tmp->start = p->start + size;
				tmp->end = p->end;
				tmp->size = p->size - size;
				tmp->next = p->next;

				p->id = id;
				p->size = size;
				p->end = p->start + size - 1;
				p->next = tmp;

				return;
			}
			else
			{
				p = p->next;
			}
		}
		else
		{
			p = p->next;
		}
	}
	//can't be place in
	printf("No enough space!\n");
	return;
}

void best_fit(int id, long size)
{
	Hole* p = memory;
	Hole* best = p;
	while (p != NULL) //find the min suitable hole
	{
		if (p->id == -1)
		{
			if(best->id != -1) //first hole
			{
				best = p;
			}
			else if (p->size >= size && p->size < best->size)
			{
				best = p;
			}
			else
			{
				p = p->next;
			}
		}
		else
		{
			p = p->next;
		}
	}

	if (best->id == -1)
	{
		if (best->size == size) //change the hole to process
		{
			best->id = id;

			return;
		}
		else if (best->size > size) //divide the hole
		{
			Hole* tmp = (Hole*)malloc(sizeof(Hole));
			tmp->id = -1;
			tmp->start = best->start + size;
			tmp->end = best->end;
			tmp->size = best->size - size;
			tmp->next = best->next;

			best->id = id;
			best->size = size;
			best->end = best->start + size - 1;
			best->next = tmp;

			return;
		}
	}
	//can't be place in
	printf("No enough space!\n");
	return;
}

void worst_fit(int id, long size)
{
	Hole* p = memory;
	Hole* worst = p;
	while (p != NULL) //find the biggest hole
	{
		if (p->id == -1)
		{
			if(worst->id != -1) //first hole
			{
				worst = p;
			}
			else if (p->size >= size && p->size > worst->size)
			{
				worst = p;
			}
			else
			{
				p = p->next;
			}
		}
		else
		{
			p = p->next;
		}
	}

	if (worst->id == -1)
	{
		if (worst->size == size) //change the hole to process
		{
			worst->id = id;

			return;
		}
		else if (worst->size > size) //divide the hole
		{
			Hole* tmp = (Hole*)malloc(sizeof(Hole));
			tmp->id = -1;
			tmp->start = worst->start + size;
			tmp->end = worst->end;
			tmp->size = worst->size - size;
			tmp->next = worst->next;

			worst->id = id;
			worst->size = size;
			worst->end = worst->start + size - 1;
			worst->next = tmp;

			return;
		}
	}
	//can't be place in
	printf("No enough space!\n");
	return;
}

void request(int id, long size, char type)
{
	if (size <= 0)
	{
		printf("Invalid process size!\n");
		return;
	}

	if (id < 0)
	{
		printf("Invalid process ID!\n");
		return;
	}

	switch (type)
	{
	case 'F': first_fit(id, size); break;
	case 'B': best_fit(id, size); break;
	case 'W': worst_fit(id, size); break;
	default: printf("'%c' doesn't refer to a strategy!\n", type); break;
	}

	return;
}

void release(int id)
{
	if (id == -1)
	{
		printf("Invalid process ID!\n");
	}

	/* handle head first
	* 3 situations:
	* head->NULL
	* head->p
	* head->h
	*/
	Hole* p = memory;
	if (p->id == id)
	{
		if(p->next == NULL) //head->NULL
		{
			p->id = -1;
			
			return ;
		}
		else
		{
			if (p->next->id != -1) //head->p
			{
				p->id = -1;

				return;
			}
			else //head->h
			{
				Hole* tmp = p->next;
				p->id = -1;
				p->end = p->next->end;
				p->size += p->next->size;
				p->next = p->next->next;
				free(tmp);

				return;
			}
		}
	}

	/* handle medium node
	* 6 situations:
	* p->target->NULL
	* p->target->p
	* p->target->h
	* h->target->NULL
	* h->target->p
	* h->target->h
	*/
	Hole* prev = p;
	p = p->next;
	while (p != NULL)
	{
		if (p->id == id)
		{
			if (prev->id != -1) //p->target
			{
				if(p->next == NULL) //p->target->NULL
				{
					p->id = -1;
					
					return;
				}
				else
				{
					if (p->next->id != -1) //p->target->p
					{
						p->id = -1;

						return;
					}
					else //p->target->h
					{
						Hole* tmp = p->next;
						p->id = -1;
						p->end = p->next->end;
						p->size += p->next->size;
						p->next = p->next->next;
						free(tmp);

						return;
					}
				}
			}
			else // h->target
			{
				if(p->next == NULL) //h->target->NULL
				{
					prev->end = p->end;
					prev->size += p->size;
					prev->next = NULL;
					free(p);

					return;
				}
				else
				{
					if (p->next->id != -1) //h->target->p
					{
						prev->end = p->end;
						prev->size += p->size;
						prev->next = p->next;
						free(p);

						return;
					}
					else // h->target->h
					{
						prev->end = p->next->end;
						prev->size += (p->size + p->next->size);
						prev->next = p->next->next;
						free(p->next);
						free(p);

						return;
					}
				}
			}
		}
		else
		{
			prev = p;
			p = p->next;
		}
	}
	//can't find P_id
	printf("Non-existent process ID!\n");
	return;
}

void compact()
{
	if(memory->next == NULL) //one hole or one process
	{
		return;
	}
	
	Hole* p = memory;
	Hole* prev = NULL;
	int unused_space = 0;
	while (p != NULL)
	{
		if (p->id == -1) // hole
		{
			unused_space += p->size;

			Hole* tmp = p;
			p = p->next;
			free(tmp);
		}
		else //process
		{
			if (prev == NULL) //first process
			{
				p->start = 0;
				p->end = p->size - 1;
				prev = p;
				p = p->next;
			}
			else //next process
			{
				p->start = prev->end + 1;
				p->end = p->start + p->size - 1;
				prev->next = p;
				prev = p;
				p = p->next;
			}
		}
	}

	//compact the unused space into one block at the end of memory
	Hole* hole = (Hole*)malloc(sizeof(Hole));
	hole->id = -1;
	hole->start = prev->end + 1;
	hole->end = MAX - 1;
	hole->size = unused_space;
	hole->next = NULL;
	prev->next = hole;

	return;
}

void stat()
{
	Hole* p = memory;
	while (p != NULL)
	{
		if (p->id == -1)
		{
			printf("Address [%d:%d] Unused\n", p->start, p->end);
		}
		else
		{
			printf("Address [%d:%d] Process P%d\n", p->start, p->end, p->id);
		}

		p = p->next;
	}

	return;
}

int main(int argc, char* argv[])
{
	/* read the size of the contiguous region of memory */
	MAX = atoi(argv[1]);

	/* initialize the memory */
	memory = (Hole*)malloc(sizeof(Hole));
	memory->id = -1; //unused
	memory->start = 0;
	memory->end = MAX - 1;
	memory->size = MAX;
	memory->next = NULL;

	/* allocator> */
	int should_run = 1;
	char input[BUFFER_SIZE];
	while (should_run)
	{
		printf("allocator>");
		fflush(stdout);

		fgets(input, BUFFER_SIZE, stdin);

		/* exit */
		if (input[0] == 'X')
		{
			should_run = 0;
			continue;
		}

		/* RQ & RL */
		char* tmp;
		char* P;
		int id;
		if (input[0] == 'R')
		{
			if (input[1] == 'Q') //"RQ P0 40000 W"
			{
				tmp = strdup(input);
				strsep(&tmp, " "); //remove "RQ"

				P = strsep(&tmp, " "); //"P0"
				P[0] = '0'; //"00"
				id = atoi(P); //0

				int size;
				size = atoi(strsep(&tmp, " ")); //40000

				char type;
				tmp = strsep(&tmp, " "); //"W/n"
				type = tmp[0]; //'W'

				request(id, size, type);

				continue;
			}
			else if (input[1] == 'L') //"RL P0"
			{
				tmp = strdup(input);
				strsep(&tmp, " "); //remove "RL"

				P = strsep(&tmp, " "); //"P0"
				P[0] = '0'; //"00"
				id = atoi(P); //0

				release(id);

				continue;
			}
		}
		/* Compact unused holes of memory into one single block */
		else if (input[0] == 'C')
		{
			compact();
		}
		/* Report the regions of free and allocated memory */
		else if (input[0] == 'S')
		{
			stat();
		}
	}
	
	return 0;
}


