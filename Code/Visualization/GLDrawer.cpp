#include "GLDrawer.h"
#include <Data/Object3D.h>
#include <SDL_opengl.h>


void GLDrawer::Draw(Object3D* obj) {

  // Instant skip, if no model is assigned or it is disabled.
  if (obj->Model == NULL || obj->Model->RenderingMode == Model3D::OFF) return;

  // Temporary variables for easier access.
  int size = obj->Model->Geosets.size();
  Model3D* mdl = obj->Model;
  Geoset* geoset;
  Geometry* tri;

  glPushMatrix();    // Use designated matrix for object rendering.

  // Displace and rotate model based on position and orientation vector.
  glTranslatef(obj->Position.X, obj->Position.Y, obj->Position.Z);  // Translate object.     
  glRotatef(obj->Heading.X, 0.0f, 0.0f, -1.0f);   // Rotate on z [height]-Axis (set yaw).       
  glRotatef(obj->Heading.Y, 1.0f, 0.0f, 0.0f);    // Rotate on x-Axis (set pitch).  
  glScalef(mdl->Scale, mdl->Scale, mdl->Scale);   // Scale model properly.


  //TODO Move lighting somewhere else. Maybe as a Engine.Add() function.
  glEnable (GL_LIGHTING);   // Enable lighting.
  glEnable (GL_LIGHT0);
  float lightpos [] = {15.0f, 15.0f, 25.0f, 0.0f};    // Position. 4th is directional (0) or positional. 
  float diffuseLight [] = {0.8f, 0.8f, 0.8f, 1.0f};   // Color for diffuse light (RGBA code).
  float ambientLight [] = {0.2f, 0.2f, 0.2f, 1.0f};   // Color for ambient (environmental) light.
  float specularLight[] = {1.0f, 1.0f, 1.0f, 1.0f};   // Specular (reflected) light.
  glLightfv (GL_LIGHT0, GL_DIFFUSE, diffuseLight);    // Set diffuse light.
  glLightfv (GL_LIGHT0, GL_AMBIENT, ambientLight);    // Set ambient light.
  glLightfv (GL_LIGHT0, GL_SPECULAR, specularLight);  // Set specular light. 
  

  // Draw on selected rendering mode.  
  switch (mdl->RenderingMode) {
       
    case Model3D::POINTS: {  // Output point cloud.
      glBegin(GL_POINTS);
      for (int i = 0; i < size; i ++) {
        geoset = mdl->Geosets[i];
        for (int v = 0; v < geoset->nrV; v ++) {
          glVertex3f(geoset->vertices[v].X, geoset->vertices[v].Y, geoset->vertices[v].Z);
        }
      }
      glEnd();
      break;
    }
        
    case Model3D::MESH: {  // Draw a triangle mesh. 
      for (int g, t, i = 0; i < size; i ++) {
        geoset = mdl->Geosets[i];
        for (g = 0; g < geoset->nrG; g ++) {
          glBegin(GL_LINE_LOOP);
          for (t = 0; t < 3; t ++) {
            tri = &geoset->geometries[g];
            glVertex3f(
              geoset->vertices[tri->vIdx[t]].X,
              geoset->vertices[tri->vIdx[t]].Y,
              geoset->vertices[tri->vIdx[t]].Z
            );
          }
          glEnd();
        }
      } 
      break;
    }
        
    case Model3D::DIRECT: {  // Direct CPU rendering.
      glBegin(GL_TRIANGLES);
      for (int g, t, i = 0; i < size; i ++) {
        geoset = mdl->Geosets[i];
        for (g = 0; g < geoset->nrG; g ++) {
          for (t = 0; t < 3; t ++) {
            tri = &geoset->geometries[g];
            glTexCoord2f(
              geoset->textures[tri->tIdx[t]].X,
              geoset->textures[tri->tIdx[t]].Y
            );                         
            glNormal3f(
              geoset->normals[tri->nIdx[t]].X,
              geoset->normals[tri->nIdx[t]].Y,
              geoset->normals[tri->nIdx[t]].Z
            );         
            glVertex3f(
              geoset->vertices[tri->vIdx[t]].X,
              geoset->vertices[tri->vIdx[t]].Y,
              geoset->vertices[tri->vIdx[t]].Z
            );                
          }
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
      
  glDisable(GL_LIGHTING);
  glPopMatrix();
}
