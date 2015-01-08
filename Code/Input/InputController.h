#pragma once
class ALU;
class InputListener; 
class Camera;


/** This controller class receives parsed user input and acts accordingly. */
class InputController {

  private:
    ALU* _runtime;
    enum CameraMode {LOCK, MOVE, ROTATE}; // List of available camera modes.
    Camera* _camera;            // Camera reference.
    CameraMode _cameraMode;     // Current mode.
    InputListener* _listener;   // The listener that feeds this controller.
    bool _relMouse;

  public:

    enum MouseButton {LEFT=1, MIDDLE, RIGHT, MOUSE4, MOUSE5};
    enum Modifier    {NONE, CTRL, SHIFT, ALT};
    enum SpecialKey  {};


    InputController(ALU* runtime, Camera* camera);
    
    void SetListenerReference(InputListener* listener);

    void MouseClick(MouseButton button);

    void MouseMove(int x, int y);

    void KeyPressed(char key, Modifier mod);

    void ExitCalled();
};
