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
class IDrawable;


/* This is the OpenGL engine, responsible for 3D rendering. */
class Engine {

  private:
    SDL_Window* _window;        // SDL window handler.
    SDL_GLContext _glcontext;   // OpenGL context.
    Camera* _camera;            // 3D user perspective.
    UserInterface* _ui;         // 2D user interface overlay.
    IDrawable* _world;          // The world to draw.
    int _width, _height;        // Screen dimensions.

  public:

    /** Create a new OpenGL engine instance.
     * @param winname String with the window title. 
     * @param width The window's starting width.
     * @param height The initial height of the window.
     * @param fullscreen If 'true', supplied width and height are ignored.
     * @param camera Pointer to the associated camera.
     * @param ui Pointer to the user interface.
     * @param world The world to draw. */
    Engine(char* winname, int width, int height, bool fullscreen, Camera* camera, UserInterface* ui, IDrawable* world);
    

    /** Terminate the SDL / OpenGL engine and return to main program. */
    ~Engine();


    /** Return the pointer to the SDL window handle.
     * @returns Address of pointer to accociated SDL_Window. */
    SDL_Window** GetWindowHandle();


    /** Set the window resolution, update the viewport and adjust perspective. 
     * @param width The window's width.
     * @param height The height of the window.
     * @param fullscreen If 'true', supplied width and height are ignored. */
    void SetResolution(int width, int height, bool fullscreen = false);


    /** Render all 3D objects and print the user interface on top of it. */
    void Render();
};
