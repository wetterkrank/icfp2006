# icfp2006
ICFP is an annual programming contest. This repo contains a C implementation of the virtual machine as specified in the ICFP'06 task description.
The details are here: http://boundvariable.org/task.shtml

##### um.c
In this implementation, I use a pointer to pointers to create a 2d array-like structure with consecutive array ids.
The initial version did not reuse existing ids, so the speed and memory consumption were _really_ bad. The bottleneck was the constant reallocation of the memory.
The improved version with a list of free ids runs much faster and uses little memory.

##### um32.c
Instead of managing the array ids myself, I simply use 32-bit pointers returned by standart malloc/free functions. So this implementation is simpler and faster.
