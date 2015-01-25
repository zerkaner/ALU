#pragma once

#include "IDrawable.h"
#include <Data/Primitives.h>
#include <SDL_opengl.h>
#include <vector>

using namespace std;


/** Base class for 3D models that implements some rendering functions. */
class Model3D : public IDrawable {

  protected:

    /** An enumeration of available rendering modes. */
    enum RenderingMode {OFF, POINTS, MESH, DIRECT, VBO};

    RenderingMode _renderingMode;   // Current rendering mode.
    vector<Geometry*> _triangles;   // List with all triangles.

    /** Returns the position of this object. [virtual]
     * @return The position as a Float3 (x,y,z). */
    virtual Float3 GetPosition() = 0;

    /** Returns the orientation of this object. [virtual]
     * @return A Float2 structure with yaw (x) and pitch (y). */
    virtual Float2 GetOrientation() = 0;


  public:

    /** Virtual empty destructor. */
    virtual ~Model3D() {}


    /** Draws this model. */
    void Draw() {
      if (_renderingMode == OFF) return;  // Instant skip on disabled model.

      // Displace and rotate model based on position and orientation vector.
      Float3 pos = GetPosition();
      Float2 ortn = GetOrientation();
      glRotatef(ortn.Y, 1.0f, 0.0f, 0.0f);  // Rotate on x-Axis (set pitch).
      glRotatef(ortn.X, 0.0f, 0.0f, -1.0f); // Rotate on y-Axis (set yaw).      
      glTranslatef(pos.X, pos.Y, pos.Z);    // Translate object in space.

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


      //TODO Translation and rotation!
      //TODO ...
 
    }
};
