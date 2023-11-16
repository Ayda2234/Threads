#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_THREADS 6
#define RANGE 100

// Structure pour passer des arguments aux threads
typedef struct {
    int start;
    int end;
    int* sum;
    sem_t* sem;
} ThreadArgs;

// Fonction exécutée par chaque thread
void* threadFunction(void* args) {
    ThreadArgs* threadArgs = (ThreadArgs*)args;
    int start = threadArgs->start;
    int end = threadArgs->end;
    int* sum = threadArgs->sum;
    sem_t* sem = threadArgs->sem;

    for (int i = start; i <= end; i++) {
        *sum += i;
    }

    // Entrée dans la section critique
    sem_wait(sem);
    printf("Somme du thread %lu : %d\n", pthread_self(), *sum);
    sem_post(sem);
    // Sortie de la section critique

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    ThreadArgs threadArgs[NUM_THREADS];
    int threadSums[NUM_THREADS - 1] = {0};
    int totalSum = 0;

    // la creation du semaphore pour proteger la section critique
    sem_t mutex;
    sem_init(&mutex, 0, 1);

    // lancement des 5 thread
    for (int i = 0; i < NUM_THREADS - 1; i++) {
        threadArgs[i].start = i * RANGE + 1;
        threadArgs[i].end = (i + 1) * RANGE;
        threadArgs[i].sum = &threadSums[i];
        threadArgs[i].sem = &mutex;
        pthread_create(&threads[i], NULL, threadFunction, &threadArgs[i]);
    }

    // Attendre que les 5 premiers threads se terminent
    for (int i = 0; i < NUM_THREADS - 1; i++) {
        pthread_join(threads[i], NULL);
    }

    // Afficher la somme totale
    for (int i = 0; i < NUM_THREADS - 1; i++) {
        totalSum += threadSums[i];
    }
    printf("Somme totale : %d\n", totalSum);

    // Détruire le sémaphore
    sem_destroy(&mutex);

    return 0;
}
