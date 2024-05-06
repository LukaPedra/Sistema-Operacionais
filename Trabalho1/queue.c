#include <stdio.h>
#include <stdlib.h>

#include "info.h"

void initQueue(Queue *q, int Type)
{
	q->front = NULL;
	q->rear = NULL;
	q->Type = Type;
}

int isEmpty(Queue *q)
{
	if (q->front == NULL)
	{
		return TRUE;
	}
	return FALSE;
}

void enqueue(Queue *q, Process p)
{
	Node *newNode = (Node *)malloc(sizeof(Node));
	newNode->process = p;
	newNode->next = NULL;

	if (isEmpty(q))
	{
		q->front = newNode;
		q->rear = newNode;
	}
	else
	{
		q->rear->next = newNode;
		q->rear = newNode;
	}
}

void dequeue(Queue *q)
{
	if (isEmpty(q))
	{
		printf("A fila está vazia. Nenhum elemento para remover.\n");
		return;
	}

	Node *temp = q->front;
	q->front = q->front->next; // Segundo da fila

	// Caso em que a fila só tinha um elemento
	if (q->front == NULL)
	{					
		q->rear = NULL; // Fila se torna vazia
	}

	free(temp); // Libera o antigo primeiro da fila
}

void displayQueue(Queue *q)
{
	if (isEmpty(q))
	{
		printf("A fila está vazia.\n");
		return;
	}

	Node *temp = q->front;
	
	if (q->Type == REAL_TIME)
	{
		printf("Real Time -> ");
	}
	else if (q->Type == PRIORIDADE)
	{
		printf("Prioridade -> ");
	}
	else if (q->Type == ROUND_ROBIN)
	{
		printf("Round Robin -> ");
	}
	else if (q->Type == PRONTOS)
	{
		printf("Processos Prontos: ");
	}
	
	while (temp != NULL)
	{
		if (q->Type == PRONTOS) {
			printf("%s ", temp->process.name);	
		} else {
			printf("%s -> ", temp->process.name);
		}
		temp = temp->next;
	}

	if (q->Type == PRONTOS) {
		printf("\n");
	} else {
		printf("FINAL DA FILA\n*******************\n");
	}
}

void queueSort(Queue *q)
{
	if (isEmpty(q) || q->front->next == NULL)
	{
		return;
	}

	Node *currNode = q->front;
	int swapped;
	Process temp;

	do
	{
		swapped = 0;
		currNode = q->front;
		if (q->Type == REAL_TIME)
		{
			while (currNode->next != NULL)
			{
				if (currNode->process.init > currNode->next->process.init)
				{
					temp = currNode->process;
					currNode->process = currNode->next->process;
					currNode->next->process = temp;
					swapped = 1;
				}
				currNode = currNode->next;
			}
		}

		// Organização da fila de prioridade por prioridade
		else if (q->Type == PRIORIDADE)
		{
			while (currNode->next != NULL)
			{
				if (currNode->process.priority > currNode->next->process.priority)
				{
					temp = currNode->process;
					currNode->process = currNode->next->process;
					currNode->next->process = temp;
					swapped = 1;
				}
				currNode = currNode->next;
			}
		}
		
	} while (swapped);
}
