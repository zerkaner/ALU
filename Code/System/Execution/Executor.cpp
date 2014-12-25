#include "Executor.h"

#include <stdio.h>


// Executor constructor.
Executor::Executor() {
}


// Executor destructor.
Executor::~Executor() {
  //TODO Listen freigeben!
}


// Addition and removal of tasks to the sequential execution.
void Executor::SequentialAdd(IExecutable* exec, SeqQueue queue) {
}


// Removal of sequential executed tasks.
void Executor::SequentialRemove(IExecutable* exec) {
}


// Addition of parallel executed tasks.
void Executor::ThreadStart(IExecutable* exec, int eps) {
}


// Removes / stops a threaded task.
void Executor::ThreadStop(IExecutable* exec) {
}


// Advances the sequential execution by one step.
void Executor::Advance() {
  
  int v2 = rand() % 100 + 1;     // v2 in the range 1 to 100
}
