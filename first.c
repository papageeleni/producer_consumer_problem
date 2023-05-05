/*
 *	File	: pc.c
 *
 *	Title	: Demo Producer/Consumer.
 *
 *	Short	: A solution to the producer consumer problem using
 *		pthreads.
 *
 *	Long 	:
 *
 *	Author	: Andrae Muys
 *
 *	Date	: 18 September 1997
 *
 *	Revised	:
 */

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <signal.h>

#define QUEUESIZE 10
#define LOOP 10000
#define PRO_MAX 1
#define CON_MAX 4

void *producer(void *args); // starting routine of pro pthread
void *consumer(void *args); // starting routine of con p`thread

typedef struct
{ // work functoin must replace usleep
  void *(*work)(void *);
  void *arg;
} workFunction;

typedef struct
{
  int buf[QUEUESIZE];
  long head, tail;
  int full, empty;
  pthread_mutex_t *mut;               // multiple access
  pthread_cond_t *notFull, *notEmpty; // signal and wait
} queue;

queue *queueInit(void);
void queueDelete(queue *q);
void queueAdd(queue *q, int in);
void queueDel(queue *q, int *out);
struct timeval start[LOOP*PRO_MAX], end[LOOP*PRO_MAX];
void signal_handler(int signum);
FILE *fp;
int delete_count = -1;
int count = -1;
int time_interval[LOOP*PRO_MAX];

int main()
{
  queue *fifo;
  char file_name[50];
  sprintf(file_name,"producer_%dconsumer_%d.csv",PRO_MAX,CON_MAX);
  fp = fopen("file_name.csv", "a");

  signal(SIGINT, signal_handler); 
  pthread_t pro[PRO_MAX], con[CON_MAX];

  fifo = queueInit();

  if (fifo == NULL)
  {
    fprintf(stderr, "main: Queue Init failed.\n");
    exit(1);
  }

  for (int i = 0; i < PRO_MAX; i++)
    pthread_create(&pro[i], NULL, producer, fifo);

  for (int i = 0; i < CON_MAX; i++)
    pthread_create(&con[i], NULL, consumer, fifo);

  for (int i = 0; i < PRO_MAX; i++)
  {
    pthread_join(pro[i], NULL);

  }
  for (int i = 0; i < CON_MAX; i++){
    pthread_join(con[i], NULL);
  }
    
  queueDelete(fifo);

  return 0;
}

void signal_handler(int signum) 
{
  double avg;
  unsigned long long int sum = 0;

  printf("Received signal %d, closing file\n", signum);
  for (int i=0; i<LOOP*PRO_MAX; i++)
  {
    sum += time_interval[i];
  }
  avg = sum/(LOOP*PRO_MAX);
  printf("sum: %lld, avg:%f \n",sum, avg);
  fprintf(fp, "%d,%d,%f,", PRO_MAX, CON_MAX, avg);  //    |Producer number|consumer number|average time        
  fprintf(fp, "\n");          


  fclose(fp);
  exit(signum);
}

void *task(void *arg)
{ // calulates 10 times a sine function
  double x;
  x = sin(*(int *)(arg)*M_PI / 180);
  return (NULL);
}

void *producer(void *q)
{
  queue *fifo;
  int i;
  fifo = (queue *)q;
  workFunction in;

  for (i = 0; i < LOOP; i++)
  {
    pthread_mutex_lock(fifo->mut);
    while (fifo->full)
    {
      printf("producer: queue FULL.\n");
      pthread_cond_wait(fifo->notFull, fifo->mut);
    }
    in.work = task;
    in.arg = &i;
    queueAdd(fifo, *(int *)in.arg);
    count++;
    gettimeofday(&start[count], NULL); // start times
    pthread_mutex_unlock(fifo->mut);
    pthread_cond_signal(fifo->notEmpty);
  }
  return (NULL);
}

void *consumer(void *q)
{
  queue *fifo;
  int arguement;
  workFunction wf;
  fifo = (queue *)q;
  

  while (1)
  {
    pthread_mutex_lock(fifo->mut);
    while (fifo->empty)
    {
      printf("consumer: queue EMPTY.\n");
      pthread_cond_wait(fifo->notEmpty, fifo->mut);
    }
    queueDel(fifo, &arguement);
    delete_count++;
    wf.work = task;
    wf.work(&arguement);
    gettimeofday(&end[delete_count], NULL); // end time
    time_interval[delete_count] = (end[delete_count].tv_usec - start[delete_count].tv_usec);
//    printf("%d,", time_interval[delete_count]);
    pthread_mutex_unlock(fifo->mut);
    pthread_cond_signal(fifo->notFull);    
  }

  return (NULL);
}

queue *queueInit(void)
{
  queue *q;

  q = (queue *)malloc(sizeof(queue));
  if (q == NULL)
    return (NULL);

  q->empty = 1;
  q->full = 0;
  q->head = 0;
  q->tail = 0;
  q->mut = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(q->mut, NULL);
  q->notFull = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
  pthread_cond_init(q->notFull, NULL);
  q->notEmpty = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
  pthread_cond_init(q->notEmpty, NULL);

  return (q);
}

void queueDelete(queue *q)
{
  pthread_mutex_destroy(q->mut);
  free(q->mut);
  pthread_cond_destroy(q->notFull);
  free(q->notFull);
  pthread_cond_destroy(q->notEmpty);
  free(q->notEmpty);
  free(q);
}

void queueAdd(queue *q, int in)
{
  q->buf[q->tail] = in;
  q->tail++;
  if (q->tail == QUEUESIZE)
    q->tail = 0;
  if (q->tail == q->head)
    q->full = 1;
  q->empty = 0;

  return;
}

void queueDel(queue *q, int *out)
{
  *out = q->buf[q->head];

  q->head++;
  if (q->head == QUEUESIZE)
    q->head = 0;
  if (q->head == q->tail)
    q->empty = 1;
  q->full = 0;

  return;
}
