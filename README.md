# icfp2006
Task: http://boundvariable.org/task.shtml

##### um.c
Here, I use a pointer to pointers to create a 2d array-like structure with consecutive array ids.
The initial version did not reuse existing ids, so the speed and memory consumption were _really_ bad. The bottleneck was the constant reallocation of the memory up.
The improved version with a list of free ids runs much faster and uses little memory.

##### um32.c
Instead of handling the array ids myself, I just use 32-bit pointers returned by standart malloc/free functions. So this implementation is simpler and faster.
