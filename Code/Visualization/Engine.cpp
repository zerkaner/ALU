#include <Data/StatsInfo.h>
#include <UserInterface/UserInterface.h>
#include <Visualization/Camera.h>
#include <Visualization/Engine.h>
#include <Visualization/IDrawable.h>

#include <gl/GLU.h>
#include <stdio.h>



Engine::Engine(char* winname, int width, int height, bool fullscreen, Camera* camera, UserInterface* ui, IDrawable* world) :
  _camera(camera), _ui(ui), _world(world) {
  
  // Initialize the SDL video subsystem.   
  if (SDL_Init(SDL_INIT_VIDEO) != 0) printf("[ERROR] SDL initialization failed.\n");
  else                               printf("SDL system initialized.\n");
  
  // Create the window and the OpenGL context handler.
  _window = SDL_CreateWindow (winname, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              width, height, SDL_WINDOW_OPENGL);
  if (_window == NULL) printf ("[ERROR] Could not create window.\n");
  else                 printf ("Window loaded.\n");
  _glcontext = SDL_GL_CreateContext(_window);
  if (_glcontext == NULL) printf ("[ERROR] OpenGL context handler could not be created.\n");
  else                    printf ("Created OpenGL context handler.\n");
  
  // OpenGL initialization.
  glEnable(GL_ALPHA_TEST);      // Enable check for transparent surfaces.
  glEnable(GL_BLEND);           // Enable blending (texture transparency).
  glEnable(GL_DEPTH_TEST);      // Enable depth test (to avoid rendering hidden points).   
  glAlphaFunc(GL_GREATER, 0.5); // Alpha testing threshold.
  glBlendFunc(0x0302, 0x0303);  // Blending function: Source alpha to (1 - source alpha).
  glDepthFunc(GL_LEQUAL);       // Only render points with less or equal depth (def.: GL_LESS).
  glShadeModel(GL_SMOOTH);      // Shading type 'smooth' (def.). Other option: 'flat'. 
  
  glClearColor(0.5f, 0.5f, 0.5f, 1);                 // Grey background color.
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Highest quality desired.


  // Initialize viewport and resolution.
  SetResolution (width, height, fullscreen); 
}



Engine::~Engine () { 
  SDL_GL_DeleteContext(_glcontext);
  SDL_DestroyWindow(_window);
  SDL_Quit();
  printf("SDL system shut down!\n");
}



SDL_Window** Engine::GetWindowHandle() {
  return &_window;
}



void Engine::SetResolution (int width, int height, bool fullscreen) {

  // Set resolution to native fullscreen or given window size.
  if (fullscreen) SDL_SetWindowFullscreen (_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
  else {
    SDL_SetWindowFullscreen (_window, SDL_FALSE);
    SDL_SetWindowSize (_window, width, height);
  }

  // Update the OpenGL viewport and adjust the user perspective.
  SDL_GetWindowSize(_window, &_width, &_height);
  glViewport(0, 0, _width, _height); 
  StatsInfo::WINDOW_X = _width;
  StatsInfo::WINDOW_Y = _height;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0, (double) _width/_height, 0.2, 750.0),
  glMatrixMode(GL_MODELVIEW);
}



void Engine::Render() {
  glClear(0x4100);     // Clear current buffer (color & depth buffer bit).
  glLoadIdentity();    // Reset model matrix.
  _camera->Update();   // Position calculation and camera update.
  _world->Draw();      // Draw the world.
  
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

  SDL_GL_SwapWindow(_window);   // Swap active and standby buffers.
}
