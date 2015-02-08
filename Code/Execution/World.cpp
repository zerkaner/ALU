#include "World.h"

#include <Agents/Agent.h>
#include <Agents/TestAgent.h>
#include <Data/StatsInfo.h>
#include <Environment/Cube.h>
#include <Environment/Environment.h>
#include <Physics/PhysicsEngine.h>
#include <Physics/Modules/GravityModule.h>
#include <Visualization/IDrawable.h>

#include <algorithm>
#include <stdio.h>
#include <time.h>



World::World() {
  _environment = new Environment();
  _physics = new PhysicsEngine(&_environment->Objects);

  _randomExec = true;
  srand((unsigned int) time(NULL));
  _ticks = 0;
  _idCounter = 0;

  new TestAgent(this, _environment);
  //_physics->AddModule(new GravityModule());
}


void World::AdvanceOneTick() {
  _environment->AdvanceEnvironment(); // Advance environment first.

  // Main queue: If a random execution is desired, we shuffle the agent list.
  if (_randomExec) {        
    _rndAgts = std::vector<Agent*> (_agents);
    _execSize = _rndAgts.size();
    for (int i = 0; i < _execSize; i ++) {
      int j = rand() % (_execSize - i) + i;  // Interval (i -> count).
      Agent* temp = _rndAgts[i];
      _rndAgts[i] = _rndAgts[j];
      _rndAgts[j] = temp;
    }                           
    for (_execIndex = 0; _execIndex < _execSize; _execIndex ++) {
      _rndAgts[_execIndex]->Tick();  // Execute agent.
    }
  }

  // Otherwise a sequential execution is performed.
  else {
    _execSize = _agents.size();
    for (_execIndex = 0; _execIndex < _execSize; _execIndex ++) {
      _agents[_execIndex]->Tick();
    }
  }

  _physics->Execute();  // Agents are done. Do physics calculation now!
      
  // Post tick: Increase tick counter and write it to blackboard.
  _ticks ++;
  StatsInfo::Ticks = _ticks;
}



void World::AddAgent(Agent* agent) {
  _agents.push_back(agent);
}



/** Checks, if an element is present in a vector.
 * @param vec The vector to iterate.
 * @param elem The element to search for.
 * @return The element's index or -1, if not contained. */
int GetIndexOfElement(std::vector<Agent*> vec, Agent* elem) {
  if (vec.empty()) return -1;            // Return on empty list.
  std::vector<Agent*>::iterator iterator = find(vec.begin(), vec.end(), elem);
  if (iterator == vec.end()) return -1;  // Element not in vector.
  return iterator - vec.begin();         // Return offset (= index).
  return 0;
}



void World::RemoveAgent(Agent* agent) {    

  // Remove from agent list. Note: This has to be done regardless of random or 
  // sequential execution (because random list is derived from sequential list).
  int aIndex = GetIndexOfElement(_agents, agent);
  if (aIndex != -1) _agents.erase(_agents.begin() + aIndex);
  if (!_randomExec && (aIndex <= _execIndex)) _execIndex --; // Index check for seq.
      
  // If we use the shuffled agent execution, remove also from that list. 
  if (_randomExec) {
    int rIndex = GetIndexOfElement(_rndAgts, agent);
    if (rIndex != -1) _rndAgts.erase(_rndAgts.begin() + rIndex);
    if (rIndex <= _execIndex) _execIndex --;  // Index check for random exec.
  }

  _execSize --; // Reduce list size variable (necessary when in exec loop).
}



long World::GetID() {
  return _idCounter ++;
}  



IDrawable* World::GetDrawableWorld() {
  return _environment;
}
