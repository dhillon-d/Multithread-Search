#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

// ****************************
// This code is provided as an *optional* framework to use to 
// write your multi-threaded search code for Assignment 6. 
// 
// Feel free to change anything you want to in this. 
// ****************************


// ===================================================
// Global Variables
// ===================================================

#define MAX_NUM_RESULTS 100 
int num_data_points;
int section_size;

// Number of threads
int num_threads;

// Array of numbers to search
#define MAX_NUM_DATA 5000
int data[MAX_NUM_DATA]; 

// Number to be found
int target;

// Array of indices of target number, initialized with -1 to indicate EOF 
int results[MAX_NUM_RESULTS];

// Number of results we have so far
int num_results = 0; 

// Used for synchronization. 
// I encourage you to rename this something more meaningful. 
pthread_mutex_t lock;
sem_t semaphore;

// All the threads (an array)
#define MAX_NUM_THREADS 1000
pthread_t threads[MAX_NUM_THREADS];

// Values to help threads search disjoint sections of data array 
int lines_per_thread;
int lines_remainder;


// ===================================================
// Functions
// ===================================================

// The function that each thread will run 
// to look for the target value in the array. 
// The argument provided is a pointer to the array
// where this thread should start looking for the target. 
// (This can be either the index OR the address. Your choice.)
// Each thread will look at a max of section_size elements. 
void* DoSearch(void* which_thread) {
  
  sem_wait(&semaphore);

  // Use input thread number to scale the indexes of the data array
  int thread = (int)(intptr_t) which_thread;
  int start = thread * lines_per_thread;
  int stop = (thread + 1) * lines_per_thread;
  int j;
  for (j = start; j < stop; ++j) {
    if (j == num_data_points) {
      break;
    } 
    // Check if element at index matches target
    else if  (data[j] == target) {
      // Lock threads -> write to results array -> increment result counter -> unlock
      pthread_mutex_lock(&lock);
      results[num_results] = j;
      ++num_results;
      pthread_mutex_unlock(&lock);
    }
  }

  sem_post(&semaphore);
  return NULL;

}

// Opens the file called filename. 
// Reads line by line, adding each number to the 
// global data array. 
void ReadFile(char* filename) {

  FILE *myFile;
  myFile = fopen(filename, "r");
  int i;
  for (i = 0; i < num_data_points; ++i) {
    fscanf(myFile, "%d", &data[i]);
      }
  fclose(myFile);

}

// Prints out the results, stored in the 
// global "results" array. 
void PrintResults() {

  int i;
  for (i = 0; i < num_results; ++i) {
    printf("Target found line: %d\n", results[i] + 1);
  }

}


int main(int num_args, char* args[]) {

  // Read in the arguments and populate global variables. 
  target = atoi(args[2]);
  num_threads = atoi(args[3]);
  num_data_points = atoi(args[4]);
  lines_per_thread = num_data_points / num_threads;
  lines_remainder = num_data_points % num_threads;
  if (lines_remainder != 0) {
    ++lines_per_thread;
  }

  // Initialize semaphore and decrement to 0, initialize mutex
  sem_init(&semaphore, 0, 1);
  sem_wait(&semaphore);
  pthread_mutex_init(&lock, NULL);

  // Create the threads
  int i;
  for (i = 0; i < num_threads; ++i) {
    pthread_create(&threads[i], NULL, DoSearch, (void *) (intptr_t) i);
  }
  
  // Read the file into the data array
  ReadFile(args[1]);

  // Create hall passes for threads, analogous to signaling threads
  for (i = 0; i < num_threads; ++i) {
    sem_post(&semaphore);
  }

  // Wait for threads to finish doing their job
  for (i = 0; i < num_threads; ++i) {
    pthread_join(threads[i], NULL);
  }

  // Print out the results
  PrintResults();

  // Clean up everything! 
  // Free everything you malloc'd. 
  // Don't forget to destroy the semaphore and mutex, which 
  // get malloc'd when you create them. 
  
  sem_destroy(&semaphore);
  pthread_mutex_destroy(&lock);
  exit(0);

}
