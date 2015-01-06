#pragma once

#include <Execution/Stopwatch.h>
#include <Execution/World.h>
#include <Input/InputController.h>
#include <Input/InputListener.h>
#include <Physics/PhysicsEngine.h>
#include <UserInterface/UserInterface.h>
#include <Visualization/Engine.h>


/** Runtime system main class. */
class ALU {

  private:
    Stopwatch _stopwatch;      // Stopwatch to measure execution time.
    World _world;              // 'The World' with agents, environment, etc.
    InputListener _listener;   // Listener for user input.
    Engine _3dEngine;          // OpenGL 3D engine to render the environment.
    UserInterface _interface;  // 2D user interface overlay. 
    bool _run;                 // Loop execution flag. 
    int _targetFPS = 25;       // Targeted frames per second.


  public:
    
    //TODO Initialization parameter object.
    /** Create a new runtime. */
    ALU() : 
      _stopwatch(Stopwatch()),
      _world(World()),
      _listener(InputListener(InputController())),
      _3dEngine(Engine("ALU-Testlauf", 640, 480, false)),
      _interface(UserInterface()) {
    }


    /** Enters execution loop. */
    void Start() {
      _run = true;               // Enable loop execution.
      long delay;                // Stores sleep time (in ms).
      
      while (_run) {
        _stopwatch.GetInterval();     // Reset stopwatch counter.
        _world.AdvanceOneTick();      // 1) Execute the world.
        _listener.EvaluateEvents();   // 2) Read and evaluate user input.
        _3dEngine.Render();           // 3) Render the 3D environment.
        _interface.Update();          // 4) Update the 2D user interface.

        // Sleep calculation.
        delay = (long)((1.0f/_targetFPS)*1000) - _stopwatch.GetInterval();
        if (delay > 0) Sleep(delay);
      }
    }


    /** Stops the execution. */
    void Stop() {
      _run = false;
    }
};
