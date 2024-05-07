# Relatório - Trabalho 1

**Nome: Lucca Vieira Rocha | Matrícula: 2011342
Nome: Thalita Rangel Bullos | Matrícula: 2020273**
Nota fina: 10/10
---

# Programas

## `interpretador.c`

Este programa é responsável por ler um arquivo de entrada, `exec.txt`, que contém as informações dos processos que serão escalonados. As informações lidas incluem o nome do processo no formato ‘PX’, em que ‘X’ representa um valor numérico. Além disso, é fornecido o tipo de processo, que pode ser classificado como Real Time, Prioridade ou Round Robin. No caso do processo ser do tipo Real Time, é especificado o tempo de início e de duração. Para processos do tipo Prioridade, há o valor que representa o seu nível de prioridade. Eis um exemplo de um arquivo utilizado para os testes dos programas:

```
Run <P1> I=<0> D=<5>
Run <P2> I=<2> D=<5>
Run <P3> P=<3>
Run <P4> I=<20> D=<5>
Run <P5> P=<0>
Run <P6> I=<40> D=<10>
Run <P7> I=<50> D=<5>
Run <P8> P=<5>
Run <P9> I=<9> D=<5>
Run <P10>
```

O processo `P1` é do tipo Real Time, em que `I=<0>` determina o início de sua execução e `D=<5>` especifica a sua duração.

O processo `P3` é do tipo Prioridade, em que `P=<3>` indica que a sua prioridade é de nível 3.

O processo `P10` é do tipo Round Robin e, por isso, não possui informações adicionais.

Com base nas informações obtidas da leitura do arquivo `exec.txt`, o programa cria os processos e os envia para o escalonador através da memória compartilhada.

## `escalonador.c`

O objetivo deste programa é receber os processos, como mencionado anteriormente no programa `interpretador.c`, e organizá-los em filas de acordo com os seus tipos de escalonamento (Real Time, Prioridade ou Round Robin). Dessa forma, o programa gerencia a execução dos processos em tempo real, interrompendo-os quando necessário para resolver conflitos de duração com outros processos.

## `queue.c`

Este programa é responsável por implementar a estrutura de fila necessária para o funcionamento do escalonador. As filas são utilizadas para armazenar os processos de acordo com o seu tipo de escalonamento. Com isso, este programa fornece funções para inicialização, inserção, remoção e exibição de elementos na fila. Além disso, inclui uma função para ordenar a fila com base no tipo de escalonamento.

# Instruções para Compilação

Foi criado um arquivo Makefile que contém todos os comandos necessários para compilar os arquivos .c, simplificando o processo de compilação dos programas. Para compilar cada processo localizado na pasta 'Processos', utiliza-se o seguinte comando:

```makefile
gcc -o Processos/P1 Processos/P1.c
```

Para compilar o interpretador juntamente com a fila (queue), utiliza-se o seguinte comando:

```makefile
gcc -Wall -g -o interpretador interpretador.c queue.c
```

Da mesma forma, para compilar o escalonador junto com a fila, utiliza-se o seguinte comando:

```makefile
gcc -Wall -g -o escalonador queue.c escalonador.c
```

A execução do programa é realizada com o seguinte comando:

```makefile
./interpretador
```

Para executar as linhas de comando no arquivo Makefile, basta escrever o seguinte comando:

```bash
$ make
```

Eis o código completo do arquivo Makefile:

```makefile
all:
	
	clear
	gcc -o Processos/P1 Processos/P1.c
	gcc -o Processos/P2 Processos/P2.c
	gcc -o Processos/P3 Processos/P3.c
	gcc -o Processos/P4 Processos/P4.c
	gcc -o Processos/P5 Processos/P5.c
	gcc -o Processos/P6 Processos/P6.c
	gcc -o Processos/P7 Processos/P7.c
	gcc -o Processos/P8 Processos/P8.c
	gcc -o Processos/P9 Processos/P9.c
	gcc -o Processos/P10 Processos/P10.c
	
	gcc -Wall -g -o interpretador interpretador.c queue.c
	gcc -Wall -g -o escalonador queue.c escalonador.c 
	./interpretador
```

# Ordem de Entrada

Uma vez recebido os processos do interpretador, o programa encaminha cada processo para uma fila correspondente ao seu tipo (Real Time, Prioridade ou Round Robin). A execução dos processos segue a ordem de prioridade dos tipos, começando pelos processos Real Time, seguidos pelos de Prioridade e, por último, os de Round Robin.

```c
...
// Recebe o processo do interpretador
if (processInfo->escalonado == FALSE)
{ 
	Queue *filaAux;

	// Alocação para fila de acordo com o seu tipo
	/* Fila REAL TIME */
	if (processInfo->p.policy == REAL_TIME)
	{
		filaAux = &filaRT;
	}

	/* Fila PRIORIDADE */
	else if (processInfo->p.policy == PRIORIDADE)
	{
		filaAux = &filaPR;
	}

	/* Fila ROUND ROBIN */
	else if (processInfo->p.policy == ROUND_ROBIN)
	{
		filaAux = &filaRR;
	}

	processInfo->escalonado = alocateProcess(filaAux, processInfo->p);
	if (processInfo->escalonado == TRUE)
	{
		printf("Processo %s escalonado\n", processInfo->p.name);
	}
}
...
```

Após a alocação na fila, esta pode ser reorganizada, se necessário, para garantir a execução adequada dos processos de acordo com seu tipo. Por exemplo, as filas de Prioridade e Real Time podem ser ajustadas para garantir a execução dos processos com base em suas prioridades ou tempos de início estabelecidos.

```c
int alocateProcess(Queue *q, Process p)
{
	enqueue(q, p);
	// Organização da fila de acordo com o tipo do processo
	if (q->Type != ROUND_ROBIN)
	{
		queueSort(q); // Prioridade e Real Time precisam ser organizado as filas
	}
	return TRUE;
}
```

# Ordem de Execução

Primeiro, o escalonador verifica se há algum processo atualmente em execução. Se não houver, o programa seleciona o próximo processo a ser executado, seguindo as regras de prioridade dos tipos de processos e a fila de processos prontos. Ao escolher um processo para execução, o escalonador verifica se é a primeira vez que o processo será executado. Se for, o programa inicia a sua execução. Caso contrário, continua a execução do processo a partir do ponto em que foi interrompido.

```c
// Inicia ou continua a execução dos processos
if (executing == FALSE)
{
	/* Execução do REAL TIME */
	//Encontra se existe um processo para executar neste segundo
	Process* aux = achaProcessoRealTime(&filaRT, sec);
	if (aux != NULL)
	{ // Primeiro da fila entra em execução
		secIni = sec;
		p = *aux;

		if (!p.started)
		{
			execProcess(&p); // Executa processo pela primeira vez
			p.started = TRUE; // diz que o processo começou
		}
		else
		{
			printf("Continuando o processo %s %d\n", p.name,p.pid);
			kill(p.pid, SIGCONT); // Continua o processo já executado uma vez
		}

		executing = TRUE;
	}

	/* Execução do PRIORIDADE */
	else if (!isEmpty(&filaPR))
	{
		p = filaPR.front->process;
		if (!p.started)
		{
			execProcess(&p); // Executa processo pela primeira vez
			p.started = TRUE; // diz que o processo começou
			p.init = sec;
		}
		else
		{
			printf("Continuando o processo %s %d\n", p.name,p.pid);
			kill(p.pid, SIGCONT); // Continua o processo já executado uma vez
		}

		executing = TRUE;
	}

	/* Execução do ROUND ROBIN */
	else if (!isEmpty(&filaRR))
	{
		p = filaRR.front->process;
		if(!p.started){
			execProcess(&p); // Executa processo pela primeira vez
			p.started = TRUE; // diz que o processo começou
		}
		else{
			printf("Continuando o processo %s %d\n", p.name,p.pid);
			kill(p.pid, SIGCONT); // Continua o processo já executado uma vez
		}
		executing = TRUE;
	}
}
if (executing == TRUE){
	printf("Processo %s em execução\n", p.name);
}
```

Durante a execução de um processo, o escalonador monitora o tempo especificado e as condições estabelecidas para cada tipo de processo. Por exemplo, no caso de processos Real Time, o escalonador deve interromper o processo quando ele atingir sua duração máxima. Além disso, o escalonador deve interromper a execução de um processo em casos de conflitos de tempo. Por exemplo, no caso de dois processos do tipo Real Time como a seguir:

```
Run <P1> I=<0> D=<5>
Run <P2> I=<2> D=<5>
```

O processo `P2` terá que ser interrompido, pois o seu tempo de início coincide com o tempo de duração do processo `P1`.

Quando um processo conclui sua execução ou quando há situações de conflito, como o caso citado anteriormente, o escalonador deve interromper o processo, remover da fila de execução e, se necessário, realocar na fila de processos prontos para uma possível reexecução.

```c
...
//Tratamento de processos em execução
if (executing == TRUE)
{
	if (p.policy == REAL_TIME)
	{
		if (sec == secIni + p.duration)
		{
			printf("Processo %s terminou\n", p.name);
			kill(p.pid, SIGSTOP);
			executing = FALSE;
		}
	}
	else if ((p.policy == PRIORIDADE)&&(sec == p.init + 5.0))
	{
		kill(p.pid, SIGSTOP);
		dequeue(&filaPR);
		displayQueue(&filaPR);
		enqueue(&filaProntos, p);
		executing = FALSE;
	}
	// Espera do ROUND ROBIN
	else if (p.policy == ROUND_ROBIN)
	{
		kill(p.pid, SIGSTOP);
		dequeue(&filaRR);

		enqueue(&filaProntos, p);
		
		executing = FALSE;
	}
}
...
```

# Conclusão

Durante os testes dos programas, identificamos um problema ao executá-los no sistema operacional MacOS. Os processos foram lidos e executados conforme o esperado neste ambiente. No entanto, ao tentar executar os programas em um sistema Linux, encontramos um erro relacionado à memória compartilhada. Para resolver esse problema, realizamos a seguinte modificação na linha de código:

```c
...
if (!shmid_process)
{
	perror("Erro ao criar a memória compartilhada do shmid_process.\n");
	exit(1);
}
...
```

Para a seguinte linha de código:

```c
...
if (shmid_process == -1)
{
	perror("Erro ao criar a memória compartilhada do shmid_process.\n");
	exit(1);
}
...
```

Após resolver esse problema, todos os programas funcionam conforme o esperado. A prioridade dos processos foi respeitada ao longo da execução do programa, e as interrupções ocorreram quando houveram conflitos entre processos. Ao executar os programas, a seguinte saída é obtida:

```
0.0'
A fila está vazia.
Processo atual: P1
Processo P1 escalonado
A fila está vazia.
P1 Iniciando pela primeira vez
Processo P1 em execução
Processo: (P2) inválido. Tempo de execução excede o limite permitido.

1.0'
A fila está vazia.
Processo atual: P1
A fila está vazia.
Processo P1 em execução

2.0'
A fila está vazia.
Processo atual: P3
Processo P3 escalonado
A fila está vazia.
Processo P1 em execução

3.0'
Prioridade -> P3 -> FINAL DA FILA
*******************
Processo atual: P4
Processo P4 escalonado
A fila está vazia.
Processo P1 em execução

4.0'
Prioridade -> P3 -> FINAL DA FILA
*******************
Processo atual: P5
Processo P5 escalonado
A fila está vazia.
Processo P1 em execução

5.0'
Prioridade -> P5 -> P3 -> FINAL DA FILA
*******************
Processo atual: P6
Processo P6 escalonado
A fila está vazia.
Processo P1 terminou
P5 Iniciando pela primeira vez
Processo P5 em execução

6.0'
Prioridade -> P5 -> P3 -> FINAL DA FILA
*******************
Processo atual: P7
Processo P7 escalonado
A fila está vazia.
Processo P5 em execução

7.0'
Prioridade -> P5 -> P3 -> FINAL DA FILA
*******************
Processo atual: P8
Processo P8 escalonado
A fila está vazia.
Processo P5 em execução

8.0'
Prioridade -> P5 -> P3 -> P8 -> FINAL DA FILA
*******************
Processo atual: P9
Processo P9 escalonado
A fila está vazia.
Processo P5 em execução

9.0'
Prioridade -> P5 -> P3 -> P8 -> FINAL DA FILA
*******************
Processo atual: P10
Processo P10 escalonado
A fila está vazia.
Processo P5 em execução

10.0'
Prioridade -> P5 -> P3 -> P8 -> FINAL DA FILA
*******************
Processo atual: P10
A fila está vazia.
Prioridade -> P3 -> P8 -> FINAL DA FILA
*******************
P3 Iniciando pela primeira vez
Processo P3 em execução

11.0'
Prioridade -> P3 -> P8 -> FINAL DA FILA
*******************
Processo atual: P10
Processos Prontos: P5 
Processo P3 em execução

12.0'
Prioridade -> P3 -> P8 -> FINAL DA FILA
*******************
Processo atual: P10
Processos Prontos: P5 
Processo P3 em execução

13.0'
Prioridade -> P3 -> P8 -> FINAL DA FILA
*******************
Processo atual: P10
Processos Prontos: P5 
Processo P3 em execução

14.0'
Prioridade -> P3 -> P8 -> FINAL DA FILA
*******************
Processo atual: P10
Processos Prontos: P5 
Processo P3 em execução

15.0'
Prioridade -> P3 -> P8 -> FINAL DA FILA
*******************
Processo atual: P10
Processos Prontos: P5 
Prioridade -> P8 -> FINAL DA FILA
*******************
P8 Iniciando pela primeira vez
Processo P8 em execução

16.0'
Prioridade -> P8 -> FINAL DA FILA
*******************
Processo atual: P10
Processos Prontos: P5 P3 
Processo P8 em execução

17.0'
Prioridade -> P8 -> FINAL DA FILA
*******************
Processo atual: P10
Processos Prontos: P5 P3 
Processo P8 em execução

18.0'
Prioridade -> P8 -> FINAL DA FILA
*******************
Processo atual: P10
Processos Prontos: P5 P3 
Processo P8 em execução

19.0'
Prioridade -> P8 -> FINAL DA FILA
*******************
Processo atual: P10
Processos Prontos: P5 P3 
Processo P8 em execução

20.0'
Prioridade -> P8 -> FINAL DA FILA
*******************
Processo atual: P10
Processos Prontos: P5 P3 
A fila está vazia.
P4 Iniciando pela primeira vez
Processo P4 em execução

21.0'
A fila está vazia.
Processo atual: P10
Processos Prontos: P5 P3 P8 
Processo P4 em execução
```

Ao rodar o programa, o processo `P1` é escalonado e inicia sua execução e, neste momento, não há outros processos em execução. O processo `P2` é interrompido, pois é considerado inválido devido ao seu tempo de execução exceder o limite permitido. Dessa forma, o processo `P1` continua sua execução pelo tempo estabelecido, 5 segundos. Os processos `P3`, `P4`, `P5`, e assim por diante, são escalonados e executados conforme a disponibilidade. Após a execução de cada processo, a fila é atualizada conforme necessário para garantir a execução correta dos próximos processos. O processo de execução continua até que todos os processos sejam concluídos ou interrompidos. Como o processo `P10` é do tipo Round Robin, o processo permanece sendo executado.