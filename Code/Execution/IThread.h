#pragma once

#include <stdio.h>
#include <thread>
using namespace std;


/** Abstract thread class. Defines thread loop, start and stop methods. */
class IThread {

  private:

    bool _run;          // Loop execution flag. Is set to 'false' to terminate thread.
    unsigned int _eps;  // Desired executions per second. Used to calculate sleep times.

    /** Thread execution loop. Runs 'ThreadExecute()' until a kill signal is sent. */
    void Run() {
      while (_run) {
        // Timer stuff.
        //ThreadExecute();
        // Sleep
      }
    }


  protected:
    
    /** This function is filled with the code to execute. */
    virtual void ThreadExecute() = 0;


  public:

    /** Virtual class destructor. */
    virtual ~IThread() {}


    /** Start a thread for a single execution. */
    void StartThread() {
      thread thr (&IThread::ThreadExecute, this);
      thr.join();  //TODO Problem??
    }


    /** Start a thread that runs in a loop. 
     * @param eps Executions per second. */
    void StartThread(unsigned int eps) {
      _eps = eps;
      _run = true;
      // Thread stuff.
      Run();
    }


    /** Stop a looped thread. */
    void StopThread() {
      _run = false;
      // Thread join here?
    }
};
