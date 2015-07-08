#include <Agents/TestAgent.h>     // DBG
#include <Agents/ModelTestAgent.h>// DBG
#include <Data/Object3D.h>        // DBG
#include <Data/StatsInfo.h>
#include <Data/Textures/ImageLoader.h>      // DBG
#include <Environment/HeightmapTerrain.h>   // DBG
#include <Execution/ALU.h>
#include <Input/IControllerModule.h>
#include <Input/ControllerModules/CameraControllerFPS.h>  // DBG
#include <Input/ControllerModules/CameraControllerRTS.h>  // DBG
#include <Input/ControllerModules/EngineController.h>
#include <Input/ControllerModules/ExecutionController.h>
#include <Input/ControllerModules/RenderModeController.h>


/** Create a new runtime. */
ALU::ALU() : 
  _stopwatch(Stopwatch()),
  _world(World()),
  _camera(Camera()),
  _interface(UserInterface()),
  _3dEngine(Engine("ALU-Testlauf", 800, 600, false, &_camera, &_interface, _world.GetEnvironment())),
  _listener(InputListener(_3dEngine.GetWindowHandle())) {

  // Add controller modules.
  _listener.AddControllerModule(new ExecutionController(this));
  _listener.AddControllerModule(new CameraControllerRTS(&_camera, &_listener));  
  _listener.AddControllerModule(new EngineController(&_3dEngine));
  _listener.AddControllerModule(new RenderModeController(&(_world.GetEnvironment()->Objects.Objects)));
  
  // Initial camera position.
  _camera.SetPosition(11, 8.4f, 2, 250, -13);


  // Create some test agents.
  new TestAgent(&_world, _world.GetEnvironment(), Vector(2.0f, 2.0f, 0.5f));
  new TestAgent(&_world, _world.GetEnvironment(), Vector(4.0f, 1.0f, 0.5f));
  _world.GetEnvironment()->AddObject(new HeightmapTerrain("Other/Heightfield.raw"));

  Object3D* obj = new Object3D();
  obj->Position = Vector(8, 3, 0.2f);
  obj->Model = new Model3D("M4/Tree01.m4");
  _world.GetEnvironment()->AddObject(obj);

  
  obj = new Object3D();
  obj->Position = Vector(1, 1.5f, 0.2f);
  obj->Model = new Model3D("M4/Tree03.m4");
  obj->Heading.X = 25.0f;
  _world.GetEnvironment()->AddObject(obj);

  obj = new Object3D();
  obj->Position = Vector(0.8f, 8.8f, 0.2f);
  obj->Model = new Model3D("M4/Tree04.m4");
  _world.GetEnvironment()->AddObject(obj);


  obj = new Object3D();
  obj->Position = Vector(5, 6, 0.2f);
  obj->Heading.X = -25.0f;
  obj->Model = new Model3D("M4/Bear.m4");
  _world.GetEnvironment()->AddObject(obj);


  obj = new Object3D();
  obj->Position = Vector(1, 7.2f, 0.1f);
  obj->Model = new Model3D("M4/Timberwolf.m4");
  _world.GetEnvironment()->AddObject(obj);


  obj = new Object3D();
  obj->Position = Vector(1, 5.5f, 0.05f);
  obj->Model = new Model3D("M4/Stable.m4");
  obj->Heading.X = 80.0f;
  _world.GetEnvironment()->AddObject(obj);

  IControllerModule* agent = new ModelTestAgent(&_world, _world.GetEnvironment(), new Model3D("M4/Timberwolf.m4"), Vector(7.5f, 7.5f, 0.05f));
  _listener.AddControllerModule(agent);
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
