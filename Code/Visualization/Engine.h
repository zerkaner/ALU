/*****************************************************************************/
/* File name   : Engine.h                                                    */
/* Last updated: Dec 23 2014                                                 */
/* Description : OpenGL engine header file.                                  */
/*****************************************************************************/

#pragma once
#include <SDL.h>
#include <SDL_opengl.h>
class Camera;


/* This is the OpenGL engine, responsible for 3D rendering. */
class Engine {

  private:
    SDL_Window* _window;        // SDL window handler.
    SDL_GLContext _glcontext;   // OpenGL context.
    Camera* _camera;            // User perspective.

  public:
    Engine(char* winname, int width, int height, bool fullscreen, Camera* camera);
    ~Engine();

    void SetResolution(int width, int height, bool fullscreen);
    void Render();
};
