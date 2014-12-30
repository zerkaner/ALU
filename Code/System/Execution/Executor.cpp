#include "Executor.h"

#include <stdio.h>
#include <algorithm>


// Executor constructor.
Executor::Executor() {
}


// Executor destructor.
Executor::~Executor() {
  //TODO Listen freigeben!
}


// Addition of tasks to the sequential execution.
void Executor::SequentialAdd(IExecutable* exec, SeqQueue queue) {
  switch (queue) {
    case PRE:    _preExec.push_back(exec);  break; 
    case NORMAL: _exec.push_back(exec);     break; 
    case POST:   _postExec.push_back(exec); break; 
  }
}


template<typename T>

/** Checks, if an element is present in a vector.
 * @param vec The vector to iterate.
 * @param elem The element to search for.
 * @return The element's index or -1, if not contained. */
int GetIndexOfElement(vector<T*> vec, T* elem) {
  if (vec.empty()) return -1;            // Return on empty list.
  auto iterator = find(vec.begin(), vec.end(), elem);
  if (iterator == vec.end()) return -1;  // Element not in vector.
  return iterator - vec.begin();         // Return offset (= index).
}


template<typename T>

/** Remove an element from a vector.
 * @param vec The vector to delete from.
 * @param elem The element to remove.
 * @return True on success, false, if element not found. */
bool RemoveElementFromVector(vector<T*> vec, T* elem) {
  int index = GetIndexOfElement(vec, elem);
  if (index == -1) return false;
  vec.erase(remove(begin(vec), end(vec), elem), end(vec));
  return true;
}



// Removal of sequential executed tasks.
void Executor::SequentialRemove(IExecutable* exec) {
  int index;

  if (index = GetIndexOfElement(_exec, exec) != -1) {
    if(index <= _ex1) _ex1 --;

  }




  if(find(_exec.begin(), _exec.end(), exec) != _exec.end()) {


  } 
  else if (find(_preExec.begin(), _preExec.end(), exec) != _preExec.end()) {
    
    _preExec.erase(remove(begin(_preExec), end(_preExec), exec), end(_preExec)); 
    
  
  } 
  else if (find(_postExec.begin(), _postExec.end(), exec) != _postExec.end()) {
  
  
  }

      if (_execList.IndexOf(agent) <= _execCounter) _execCounter --;
      _agents.Remove(agent);
}


// Addition of parallel executed tasks.
void Executor::ThreadStart(IExecutable* exec, int eps) {
}


// Removes / stops a threaded task.
void Executor::ThreadStop(IExecutable* exec) {
}





// Advances the sequential execution by one step.
void Executor::Advance() {
  
  // Execute all objects of first priority queue.
  for (_ex0 = 0; _ex0 < _preExec.size(); _ex0++) _preExec[_ex0]->Execute();
  
  // Main queue: If a random execution is desired, we shuffle the agent list.
  if (RandomExecution) {
    for (int i = 0; i < _exec.size(); i++) {
      int j = rand() % (_exec.size() - i) + i;  // Interval (i -> count).
      IExecutable* temp = _exec[i];
      _exec[i] = _exec[j];
      _exec[j] = temp;
    }
  }
  for (_ex1 = 0; _ex1 < _exec.size(); _ex1++) _exec[_ex1]->Execute();

  // Post-queue execution and tick counter increase.
  for (_ex2 = 0; _ex2 < _postExec.size(); _ex2++) _postExec[_ex2]->Execute();
  _cycle ++;
}
