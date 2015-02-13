#pragma once
#include <Data/Object3D.h>
#include <SDL_opengl.h>
#include <vector>


/** GL rendering code for 3D objects. */
class GLDrawer {

  public:

    /** Draws this model. */
    static void Draw(Object3D* obj) {

      // Instant skip, if no model is assigned or it is disabled.
      if (obj->Model == NULL || obj->Model->RenderingMode == Model3D::OFF) return;

      glPushMatrix();    // Use designated matrix for object rendering.

      // Displace and rotate model based on position and orientation vector.
      glTranslatef(obj->Position.X, obj->Position.Y, obj->Position.Z);  // Translate object.
      glRotatef(obj->Heading.X, 0.0f, 0.0f, -1.0f);   // Rotate on z [height]-Axis (set yaw).       
      glRotatef(obj->Heading.Y, 1.0f, 0.0f, 0.0f);    // Rotate on x-Axis (set pitch).  

      // Draw on selected rendering mode.
      int size = obj->Model->Triangles.size();
      Vertex** vrt;    
      switch (obj->Model->RenderingMode) {
       
        case Model3D::POINTS: {  // Output point cloud.
          glBegin(GL_POINTS);
          for (int i = 0; i < size; i ++) {
            vrt = obj->Model->Triangles[i]->Vertices;
            for (int i = 0; i < 3; i ++) {
              glVertex3f(vrt[i]->Point.X, vrt[i]->Point.Y, vrt[i]->Point.Z);
            }
          }
          glEnd();
          break;
        }
      
        case Model3D::MESH: {  // Draw a triangle mesh. 
          for (int i = 0; i < size; i ++) {
            vrt = obj->Model->Triangles[i]->Vertices;
            glBegin(GL_LINE_LOOP);
            for (int i = 0; i < 3; i ++) {
              glVertex3f(vrt[i]->Point.X, vrt[i]->Point.Y, vrt[i]->Point.Z);
            }
            glEnd();
          } 
          break;
        }

        case Model3D::DIRECT: {  // Direct CPU rendering.
          glBegin(GL_TRIANGLES);
          for (int i = 0; i < size; i ++) {
            vrt = obj->Model->Triangles[i]->Vertices;
            for (int i = 0; i < 3; i ++) {
              glTexCoord2f(vrt[i]->Texture.X, vrt[i]->Texture.Y);
              glNormal3f(vrt[i]->Normal.X, vrt[i]->Normal.Y, vrt[i]->Normal.Z);
              glVertex3f(vrt[i]->Point.X,  vrt[i]->Point.Y,  vrt[i]->Point.Z);
            }
          }          
          glEnd();
          break;
        }

        case Model3D::VBO: {  // Draw model via vertex buffer objects on the GPU.
          //TODO Not implemented yet!
          break;
        }
      
        // Illegal rendering mode.
        default: break;
      }

      glPopMatrix();
    }
};
