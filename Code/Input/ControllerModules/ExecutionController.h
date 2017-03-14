#pragma once
#include "../IControllerModule.h"
#include <Execution/ALU.h>


/** This controller governs the ALU main routine. */
class ExecutionController : public IControllerModule {

  private:
    ALU* _runtime;  // ALU reference.


  public:

    /** Create an execution controller.
     * @param runtime ALU reference. */
    ExecutionController(ALU* runtime) {
      _runtime = runtime;
    }


    /** A key was pressed.
     * @param key The pressed key (Int32 enum value).
     * @param mod The used modifier (Ctrl, Shift...). */
    void KeyPressed(Key key, Modifier mod) {
      if (key == 'c' && mod == CTRL) ExitCalled();
    }


    /** The exit event was triggered. */
    void ExitCalled() {
      _runtime->Stop();
    }
};
