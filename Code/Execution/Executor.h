#pragma once

#include <cstdlib>
#include <map>
#include <thread>
#include <vector>

#include "IExecutable.h"


using namespace std;

/* Executor class. Has three queues for sequential execution (pre-, normal and 
 * post execution) and an additional list for thread-based executables. */
class Executor {

  private:
    vector<IExecutable*> _preExec;       // Pre-tick execution list.
    vector<IExecutable*> _exec;          // Normal execution list, may be shuffled.
    vector<IExecutable*> _postExec;      // Post-tick execution list.
    map<IExecutable*, thread> _threads;  // Thread mapping structure.
    long _idCounter = 0;                 // ID distribution counter.
    long _cycle = 0;                     // Counter for execution cycle.
    unsigned int _ex0, _ex1, _ex2;       // Iteration counters (+/- for list changes). 

  public:
    bool RandomExecution = false;   // On true, _exec is shuffled each round.

    // Constructor and destructor.
    Executor();
    ~Executor();

    // Enumeration of sequential execution queues.
    enum SeqQueue {PRE, NORMAL, POST};

    // Addition and removal of tasks to the sequential execution.
    void SequentialAdd(IExecutable* exec, SeqQueue queue = NORMAL);
    void SequentialRemove(IExecutable* exec);

    // Addition and removal of tasks to the parallel execution.
    void ThreadStart(IExecutable* exec, int eps = 0); // "Executions per second." 0: once.
    void ThreadStop(IExecutable* exec);

    // Advances the sequential execution by one step.
    void Advance();
};
