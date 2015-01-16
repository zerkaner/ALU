#pragma once
#include <Execution/Stopwatch.h>
#include <Execution/World.h>
#include <Input/InputListener.h>
#include <Input/InputController.h>
#include <Physics/PhysicsEngine.h>
#include <UserInterface/UserInterface.h>
#include <Visualization/Camera.h>
#include <Visualization/Engine.h>


/** Runtime system main class. */
class ALU {

  private:
    Stopwatch _stopwatch;        // Stopwatch to measure execution time.
    World _world;                // 'The World' with agents, environment, etc.
    InputListener _listener;     // Listener for user input.
    InputController _controller; // Input controller (called by listener).
    Engine _3dEngine;            // OpenGL 3D engine to render the environment.
    Camera _camera;              // 3D user perspective.
    UserInterface _interface;    // 2D user interface overlay. 
    bool _run;                   // Loop execution flag. 
    int _targetFPS = 25;         // Targeted frames per second.


  public:
    
    /** Create a new runtime. */
    ALU();


    /** Enters execution loop. */
    void Start();


    /** Stops the execution. */
    void Stop();
};


//TODO ALU-Constr.: Initialization parameter object.
//TODO Kameragrenzen (bei Verschiebung)? [Anhand der Umweltdimensionen festlegen]
