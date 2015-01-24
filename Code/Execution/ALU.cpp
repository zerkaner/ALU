#include <Data/StatsInfo.h>
#include <Execution/ALU.h>


/** Create a new runtime. */
ALU::ALU() : 
  _stopwatch(Stopwatch()),
  _world(World()),
  _camera(Camera(12, -7, 2, 0, -10)),
  _interface(UserInterface()),
  _3dEngine(Engine("ALU-Testlauf", 640, 480, false, &_camera, &_interface, _world.GetDrawableWorld())),
  _controller(InputController(this, &_camera)),
  _listener(InputListener(&_controller, _3dEngine.GetWindowHandle())) {
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
