#include "World.h"

#include <Agents/Agent.h>
#include <Agents/TestAgent.h>
#include <Data/StatsInfo.h>
#include <Environment/Environment.h>
#include <Physics/PhysicsEngine.h>
#include <Physics/Modules/GravityModule.h>
#include <Visualization/IDrawable.h>

#include <algorithm>
#include <stdio.h>
#include <time.h>



void MakeCubeModel() {
  Model3D mdl = Model3D();
  Geoset* geoset = new Geoset();
  mdl.Geosets.push_back(geoset);
  geoset->enabled = true;

  geoset->nrN = 6;
  geoset->normals = new Float3[6];
  geoset->normals[0] = Float3( 0.0f, 1.0f, 0.0f); // top face
  geoset->normals[1] = Float3( 0.0f, 0.0f, 1.0f); // front face
  geoset->normals[2] = Float3( 1.0f, 0.0f, 0.0f); // right face
  geoset->normals[3] = Float3(-1.0f, 0.0f, 0.0f); // left face
  geoset->normals[4] = Float3(0.0f, -1.0f, 0.0f); // bottom face
  geoset->normals[5] = Float3(0.0f, 0.0f, -1.0f); // back face

  geoset->nrV = 24;
  geoset->vertices = new Float3[24];
	geoset->vertices[0] = Float3(0.5f, 0.5f, 0.5f); 
	geoset->vertices[1] = Float3(0.5f, 0.5f, -0.5f);
	geoset->vertices[2] = Float3(-0.5f, 0.5f, -0.5f);
	geoset->vertices[3] = Float3(-0.5f, 0.5f, 0.5f);

	geoset->vertices[4] = Float3(0.5f, 0.5f, 0.5f); 
	geoset->vertices[5] = Float3(-0.5f, 0.5f, 0.5f);
	geoset->vertices[6] = Float3(-0.5f, -0.5f, 0.5f);
	geoset->vertices[7] = Float3(0.5f, -0.5f, 0.5f);

	geoset->vertices[8] = Float3(0.5f, 0.5f, 0.5f);
	geoset->vertices[9] = Float3(0.5f, -0.5f, 0.5f);
	geoset->vertices[10] = Float3(0.5f, -0.5f, -0.5f);
	geoset->vertices[11] = Float3(0.5f, 0.5f, -0.5f);

	geoset->vertices[12] = Float3(-0.5f, 0.5f, 0.5f);
	geoset->vertices[13] = Float3(-0.5f, 0.5f, -0.5f);
	geoset->vertices[14] = Float3(-0.5f, -0.5f, -0.5f);
	geoset->vertices[15] = Float3(-0.5f, -0.5f, 0.5f);

	geoset->vertices[16] = Float3(0.5f, -0.5f, 0.5f);
	geoset->vertices[17] = Float3(0.5f, -0.5f, -0.5f);
	geoset->vertices[18] = Float3(-0.5f, -0.5f, -0.5f);
	geoset->vertices[19] = Float3(-0.5f, -0.5f, 0.5f);

	geoset->vertices[20] = Float3(0.5f, 0.5f, -0.5f); 
	geoset->vertices[21] = Float3(-0.5f, 0.5f, -0.5f);
	geoset->vertices[22] = Float3(-0.5f, -0.5f, -0.5f); 
  geoset->vertices[23] = Float3(0.5f, -0.5f, -0.5f);


  geoset->nrT = 1;
  geoset->textures = new Float2[1];
  geoset->textures[0] = Float2(0.0f, 0.0f);


  geoset->nrG = 12;
  geoset->geometries = new Geometry[geoset->nrG];
  for (int i = 0; i < geoset->nrG; i ++) {
    geoset->geometries[i] = Geometry();
    geoset->geometries[i].symIndices = false;
    for (int j = 0; j < 3; j ++) {    
      if (i%2 == 0) geoset->geometries[i].vIdx[j] = i*2 + j;
      else {
        if (j == 0) geoset->geometries[i].vIdx[j] = (i-1)*2;
        if (j == 1) geoset->geometries[i].vIdx[j] = i*2;
        if (j == 2) geoset->geometries[i].vIdx[j] = i*2 + 1;
      }
      geoset->geometries[i].nIdx[j] = (i/2);
      geoset->geometries[i].tIdx[j] = 0;      
    }
  }


  mdl.Echo();
  mdl.WriteFile("Cube.m4");
}



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
