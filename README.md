# producer_consumer_problem
_Producer consumer problem using parallel programming._

This code is a modification of the pc.c file by Andrae Muys and it's a solution to the producer consumer problem using pthreads.

**Goal**: examine how threads are changing the time needed to put an item to a fifo and take it. 

**Description:** the producer threads add arguments in the fifo in order to be used from the consumer to run a function that is calculating 10 times the sine of the argument. 

To achieve the goal of the assignment we use gettimeofday(), which gives the current time in secs and usecs, placed after adding an item to the fifo and after taking the item in order to find the time between these events by subtracting them.

## Files
**first.c:** the code that it was modified based on pc.c for the producer consumer problem

**data.csv:** the file that first.c creates to store the time results 

the format of the file is:
|# of producer threads|# of consumer threads  |  time |
|---|---|---|

**plot.m**: the matlab file for plotting the results
