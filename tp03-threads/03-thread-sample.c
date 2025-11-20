#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>

#define SIZE (int)1e8
int tab[SIZE];
#define THREAD 4
int threadresult[THREAD];
static pthread_mutex_t mutex_threadresult = PTHREAD_MUTEX_INITIALIZER;

typedef struct
{
  int start;
  int end;
  int threadId;
} ThreadData;

void *min(void *data)
{
  ThreadData *threadData = (ThreadData *)data;

  int minVal = INT32_MAX;
  for (int i = threadData->start; i < threadData->end; i++)
  {
    if (tab[i] < minVal)
    {
      minVal = tab[i];
    }
  }
  pthread_mutex_lock(&mutex_threadresult);
  threadresult[threadData->threadId] = minVal;
  pthread_mutex_unlock(&mutex_threadresult);
  return NULL;
}

void *max(void *data)
{
  ThreadData *threadData = (ThreadData *)data;

  int maxVal = INT32_MIN;
  for (int i = threadData->start; i < threadData->end; i++)
  {
    if (tab[i] > maxVal)
    {
      maxVal = tab[i];
    }
  }
  pthread_mutex_lock(&mutex_threadresult);
  threadresult[threadData->threadId] = maxVal;
  pthread_mutex_unlock(&mutex_threadresult);
  return NULL;
}

int main(int argc, char **argv)
{
  for (int i = 0; i < SIZE; i++)
  {
    int random = rand();
    tab[i] = random;
  }

  pthread_t threads[THREAD];
  ThreadData threadsData[THREAD];
  int chunksize = SIZE / THREAD;

  struct timeval start_min, end_min;
  gettimeofday(&start_min, NULL);

  for (int i = 0; i < THREAD; i++)
  {
    threadsData[i].start = chunksize * i;
    threadsData[i].end = (i == THREAD - 1) ? SIZE - chunksize * (i + 1) : chunksize * (i + 1);
    threadsData[i].threadId = i;

    pthread_create(&threads[i], NULL, min, &threadsData[i]);
  }

  int bestMinResult = INT32_MAX;
  for (int i = 0; i < THREAD; i++)
  {
    pthread_join(threads[i], NULL);
    pthread_mutex_lock(&mutex_threadresult);
    if (threadresult[i] < bestMinResult)
      bestMinResult = threadresult[i];
    pthread_mutex_unlock(&mutex_threadresult);
  }

  gettimeofday(&end_min, NULL);
  double time_min = (end_min.tv_sec - start_min.tv_sec) + (end_min.tv_usec - start_min.tv_usec) / 1000000.0;
  printf("Temps d'execution MIN: %.6f secondes (resultat: %d)\n", time_min, bestMinResult);

  struct timeval start_max, end_max;
  gettimeofday(&start_max, NULL);

  for (int i = 0; i < THREAD; i++)
  {
    pthread_create(&threads[i], NULL, max, &threadsData[i]);
  }

  int bestMaxResult = INT32_MIN;
  for (int i = 0; i < THREAD; i++)
  {
    pthread_join(threads[i], NULL);
    pthread_mutex_lock(&mutex_threadresult);
    if (threadresult[i] < bestMaxResult)
      bestMaxResult = threadresult[i];
    pthread_mutex_unlock(&mutex_threadresult);
  }

  gettimeofday(&end_max, NULL);
  double time_max = (end_max.tv_sec - start_max.tv_sec) + (end_max.tv_usec - start_max.tv_usec) / 1000000.0;
  printf("Temps d'execution MAX: %.6f secondes (resultat: %d)\n", time_max, bestMaxResult);

  // Testez ce programme avec 2, 4, 8 threads et en augmentant la taille du tableau. Que constatez - vous ? -> On remarque que c'est de plus en plus rapide

  return 0;
}
