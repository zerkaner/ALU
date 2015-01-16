#include <Data/StatsInfo.h>
#include <UserInterface/UserInterface.h>
#include <Visualization/Camera.h>
#include <Visualization/Engine.h>

#include <gl/GLU.h>
#include <stdio.h>

#include <Environment/GridTerrain.h>  //DBG
GridTerrain gt = GridTerrain(30, 20); 


/** Create a new OpenGL engine instance.
 * @param winname String with the window title. 
 * @param width The window's starting width.
 * @param height The initial height of the window.
 * @param fullscreen If 'true', supplied width and height are ignored.
 * @param camera Pointer to the associated camera.
 * @param ui Pointer to the user interface. */
Engine::Engine(char* winname, int width, int height, bool fullscreen, Camera* camera, UserInterface* ui) :
  _camera(camera), _ui(ui) {
  
  // Initialize the SDL video subsystem.   
  if (SDL_Init (SDL_INIT_VIDEO) != 0) printf ("[ERROR] SDL initialization failed.\n");
  else                                printf ("SDL system initialized.\n");
  
  // Create the window and the OpenGL context handler.
  _window = SDL_CreateWindow (winname, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              width, height, SDL_WINDOW_OPENGL);
  if (_window == NULL) printf ("[ERROR] Could not create window.\n");
  else                 printf ("Window loaded.\n");
  _glcontext = SDL_GL_CreateContext(_window);
  if (_glcontext == NULL) printf ("[ERROR] OpenGL context handler could not be created.\n");
  else                    printf ("Created OpenGL context handler.\n");
  
  // OpenGL initialization.
  glClearColor (0.0f, 0.0f, 0.0f, 1.0f);              // Set clear color.
  glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Highest quality desired.
  glShadeModel (GL_SMOOTH); // Shading type 'smooth' (def.). Other option: 'flat'. 
  glEnable (GL_DEPTH_TEST); // Enable depth test (to avoid rendering hidden points).
  glDepthFunc (GL_LEQUAL);  // Only render points with less or equal depth (def.: GL_LESS). 
  glClearDepth (1.0f);      // Set depth buffer clearing value to maximum.  
  
  // Initialize viewport and resolution.
  SetResolution (width, height, fullscreen); 
}


/** Terminate the SDL / OpenGL engine and return to main program. */
Engine::~Engine () { 
  SDL_GL_DeleteContext(_glcontext);
  SDL_DestroyWindow(_window);
  SDL_Quit();
  printf("SDL system shut down!\n");
}


/** Set the window resolution, update the viewport and adjust perspective. 
 * @param width The window's width.
 * @param height The height of the window.
 * @param fullscreen If 'true', supplied width and height are ignored. */
void Engine::SetResolution (int width, int height, bool fullscreen = false) {

  // Set resolution to native fullscreen or given window size.
  if (fullscreen) SDL_SetWindowFullscreen (_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
  else {
    SDL_SetWindowFullscreen (_window, SDL_FALSE);
    SDL_SetWindowSize (_window, width, height);
  }

  // Update the OpenGL viewport and adjust the user perspective.
  SDL_GetWindowSize (_window, &_width, &_height);
  glViewport (0, 0, _width, _height); 
  StatsInfo::WINDOW_X = _width;
  StatsInfo::WINDOW_Y = _height;
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective(40.0, (double) _width/_height, 1.0, 1000.0),
  glMatrixMode (GL_MODELVIEW);
}


/** Clear screen and render all objects. */
void Engine::Render () {
  glClear (0x4100);    // Clear current buffer (color & depth buffer bit).
  glLoadIdentity();    // Reset model matrix.
  _camera->Update();   // Position calculation and camera update.
  gt.DrawGrid();       // Draw the world.
  
  // Switch to 2D rendering for the user interface.
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0.0, _width, _height, 0.0, -1.0, 10.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glDisable(GL_CULL_FACE);
  glClear(GL_DEPTH_BUFFER_BIT);

  _ui->Update();  // Call user interface rendering method.

  // Return to 3D rendering mode.
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);

  SDL_GL_SwapWindow (_window);   // Swap active and standby buffers.
}
