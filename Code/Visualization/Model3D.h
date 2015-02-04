#pragma once
#include "IDrawable.h"
#include <Data/Object3D.h>
#include <SDL_opengl.h>
#include <vector>

using namespace std;


/** Base class for 3D models that implements some rendering functions. */
class Model3D : public Object3D, public IDrawable {

  protected:

    /** An enumeration of available rendering modes. */
    enum RenderingMode {OFF, POINTS, MESH, DIRECT, VBO};

    RenderingMode _renderingMode;   // Current rendering mode.
    vector<Geometry*> _triangles;   // List with all triangles.


  public:

    /** Virtual empty destructor. */
    virtual ~Model3D() {}


    /** Draws this model. */
    void Draw() {
      if (_renderingMode == OFF) return;  // Instant skip on disabled model.
      glPushMatrix();                     // Use designated matrix for object rendering.

      // Displace and rotate model based on position and orientation vector.
      glTranslatef(Position.X, Position.Y, Position.Z);  // Translate object in space.
      glRotatef(Heading.X, 0.0f, 0.0f, -1.0f);           // Rotate on z-Axis (set yaw).       
      glRotatef(Heading.Y, 1.0f, 0.0f, 0.0f);            // Rotate on x-Axis (set pitch).  

      // Draw on selected rendering mode.
      int size = _triangles.size();
      Vertex** vrt;    
      switch (_renderingMode) {
       
        case POINTS: {  // Output point cloud.
          glBegin(GL_POINTS);
          for (int i = 0; i < size; i ++) {
            vrt = _triangles[i]->Vertices;
            for (int i = 0; i < 3; i ++) {
              glVertex3f(vrt[i]->Point.X, vrt[i]->Point.Y, vrt[i]->Point.Z);
            }
          }
          glEnd();
          break;
        }
      
        case MESH: {  // Draw a triangle mesh. 
          for (int i = 0; i < size; i ++) {
            vrt = _triangles[i]->Vertices;
            glBegin(GL_LINE_LOOP);
            for (int i = 0; i < 3; i ++) {
              glVertex3f(vrt[i]->Point.X, vrt[i]->Point.Y, vrt[i]->Point.Z);
            }
            glEnd();
          } 
          break;
        }

        case DIRECT: {  // Direct CPU rendering.
          glBegin(GL_TRIANGLES);
          for (int i = 0; i < size; i ++) {
            vrt = _triangles[i]->Vertices;
            for (int i = 0; i < 3; i ++) {
              glTexCoord2f(vrt[i]->Texture.X, vrt[i]->Texture.Y);
              glNormal3f(vrt[i]->Normal.X, vrt[i]->Normal.Y, vrt[i]->Normal.Z);
              glVertex3f(vrt[i]->Point.X,  vrt[i]->Point.Y,  vrt[i]->Point.Z);
            }
          }          
          glEnd();
          break;
        }

        case VBO: {  // Draw model via vertex buffer objects on the GPU.
          //TODO Not implemented yet!
          break;
        }
      
        // Illegal rendering mode.
        default: break;
      }

      glPopMatrix();
    }
};
