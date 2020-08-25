# Summary 
Search an array of integers more efficiently by using multiple threads to search different sections of the array.
Semaphores are used to signal the threads to start searching. While the threads are searching if a target number is found in a thread, the thread gains mutually exclusive access (mutex) to a results array which records which index the target number exists at. Mutually exclusive access prevents threads overwriting results in the result array.
## How to build
"make mt_search"  
"make run"
