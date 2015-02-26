#include <Agents/TestAgent.h>
#include <Data/StatsInfo.h>
#include <Environment/HeightmapTerrain.h>
#include <Execution/ALU.h>
#include <Input/IControllerModule.h>
#include <Input/ControllerModules/CameraControllerFPS.h>
#include <Input/ControllerModules/CameraControllerRTS.h>
#include <Input/ControllerModules/EngineController.h>
#include <Input/ControllerModules/ExecutionController.h>


/** Create a new runtime. */
ALU::ALU() : 
  _stopwatch(Stopwatch()),
  _world(World()),
  _camera(Camera()),
  _interface(UserInterface()),
  _3dEngine(Engine("ALU-Testlauf", 640, 480, false, &_camera, &_interface, _world.GetEnvironment())),
  _listener(InputListener(_3dEngine.GetWindowHandle())) {

  // Add controller modules.
  _listener.AddControllerModule(new ExecutionController(this));
  _listener.AddControllerModule(new CameraControllerRTS(&_camera, &_listener));  
  _listener.AddControllerModule(new EngineController(&_3dEngine));
  
  // Initial camera position.
  _camera.SetPosition(6, 6, 4, 211, -30);

  // Create some test agents.
  new TestAgent(&_world, _world.GetEnvironment(), Vector(2.0f, 2.0f, 0.5f));
  new TestAgent(&_world, _world.GetEnvironment(), Vector(4.0f, 1.0f, 0.5f));
  _world.GetEnvironment()->AddObject(new HeightmapTerrain("formats/heightfield2.raw"));
}


/** Enters execution loop. */
void ALU::Start() {
  _run = true;         // Enable loop execution.
  long delay;          // Stores sleep time (in ms).
      
  while (_run) {
    _stopwatch.GetInterval();     // Reset stopwatch counter.
    _world.AdvanceOneTick();      // 1) Execute the world.
    _listener.EvaluateEvents();   // 2) Read and evaluate user input.
    _3dEngine.Render();           // 3) Render the 3D environment.

    // Information pass along and sleep calculation.
    long execTime = _stopwatch.GetInterval();
    StatsInfo::ExecTime = execTime;
    StatsInfo::FPS = _targetFPS;
    delay = (long)((1.0f/_targetFPS)*1000) - execTime;
    if (delay > 0) Sleep(delay);
  }
}


/** Stops the execution. */
void ALU::Stop() {
  _run = false;
}
