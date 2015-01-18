/*****************************************************************************/
/* File name   : Engine.h                                                    */
/* Last updated: Dec 23 2014                                                 */
/* Description : OpenGL engine header file.                                  */
/*****************************************************************************/

#pragma once
#include <SDL.h>
#include <SDL_opengl.h>
class Camera;
class UserInterface;


/* This is the OpenGL engine, responsible for 3D rendering. */
class Engine {

  private:
    SDL_Window* _window;        // SDL window handler.
    SDL_GLContext _glcontext;   // OpenGL context.
    Camera* _camera;            // 3D user perspective.
    UserInterface* _ui;         // 2D user interface overlay.
    int _width, _height;        // Screen dimensions.

  public:
    Engine(char* winname, int width, int height, bool fullscreen, Camera* camera, UserInterface* ui);
    ~Engine();

    /** Return the pointer to the SDL window handle.
     * @returns Address of pointer to accociated SDL_Window. */
    SDL_Window** GetWindowHandle();

    void SetResolution(int width, int height, bool fullscreen);
    void Render();
};
