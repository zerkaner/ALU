#include "Engine.h"

#include <stdio.h>
#include <SDL.h>
#include <SDL_opengl.h>


/* Create a new OpenGL engine instance. */
Engine::Engine(char* winname, int width, int height, bool fullscreen) {
  
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
  
  //TODO Altcode der ersten Generation. Umstellen auf OpenGL Version 3/4 !
  //_initExtensions ();       // Initialize OpenGL extentions.
  
  // Initialize camera and resolution.
  SetResolution (width, height, fullscreen);
  //_objects = vector <IObjectGL*> ();
  //camera = new Camera (_window); 
}



/* Terminate the SDL / OpenGL engine and return to main program. */
Engine::~Engine () { 
  SDL_GL_DeleteContext(_glcontext);
  SDL_DestroyWindow(_window);
  SDL_Quit();
  printf("SDL system shut down!\n");
}



/* Set the window resolution. */
void Engine::SetResolution (int width, int height, bool fullscreen) {
  
  // Set resolution to native fullscreen or given window size.
  if (fullscreen) SDL_SetWindowFullscreen (_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
  else {
    SDL_SetWindowFullscreen (_window, SDL_FALSE);
    SDL_SetWindowSize (_window, width, height);
  }

  // Update the OpenGL viewport and adjust the user perspective.
  int x, y;
  SDL_GetWindowSize (_window, &x, &y);
  glViewport (0, 0, x, y); 
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  //gluPerspective(60.0, (double) x/y, 1.0, 1000.0),
  glMatrixMode (GL_MODELVIEW);
}



/* Clear screen and render all objects. */
void Engine::Render () {
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear current buffer.
  glLoadIdentity();                                    // Reset model matrix.

  // Position calculation and camera update.
  //camera->Update ();

  // Render objects.
  //for (unsigned int i = 0; i < _objects.size(); i ++) _objects[i]->Render ();
  SDL_GL_SwapWindow (_window);   // Swap active and standby buffers.
}
