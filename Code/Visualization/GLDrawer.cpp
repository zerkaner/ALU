#include "GLDrawer.h"
#include <Data/Object3D.h>
#include <Data/Textures/Texture.h>
#include <SDL_opengl.h>
#include <cmath>


void GLDrawer::Draw(Object3D* obj) {

  // Instant skip, if no model is assigned or it is disabled.
  if (obj->Model == NULL || obj->Model->DisplayMode == Model3D::OFF) return;

  //TODO Move lighting somewhere else. Maybe as a Engine.Add() function. 
  glEnable (GL_LIGHT0);
  float lightpos [] = {35.0f, 50.0f, 40.0f, 1.0f};    // Position. 4th is directional (0) or positional. 
  float diffuseLight [] = {0.8f, 0.8f, 0.8f, 1.0f};   // Color for diffuse light (RGBA code).
  float ambientLight [] = {0.2f, 0.2f, 0.2f, 1.0f};   // Color for ambient (environmental) light.
  float specularLight[] = {1.0f, 1.0f, 1.0f, 1.0f};   // Specular (reflected) light.
  glLightfv (GL_LIGHT0, GL_POSITION, lightpos);       // Set position of light source.
  glLightfv (GL_LIGHT0, GL_DIFFUSE, diffuseLight);    // Set diffuse light.
  glLightfv (GL_LIGHT0, GL_AMBIENT, ambientLight);    // Set ambient light.
  glLightfv (GL_LIGHT0, GL_SPECULAR, specularLight);  // Set specular light. 

  glPushMatrix();    // Use designated matrix for object rendering.
  
  // Displace and rotate model based on position and orientation vector.
  glTranslatef(obj->Position.X, obj->Position.Y, obj->Position.Z);  // Translate object.     
  glRotatef(obj->Heading.X, 0.0f, 0.0f, -1.0f);   // Rotate on z [height]-Axis (set yaw).       
  glRotatef(obj->Heading.Y, 1.0f, 0.0f, 0.0f);    // Rotate on x-Axis (set pitch).  
  
  // Temporary variables for easier access.
  Model3D* mdl = obj->Model;
  Mesh* mesh = mdl->GetMesh();
  Geoset* geoset;
  Geometry* tri;


  // Draw on selected rendering mode.  
  switch (mdl->DisplayMode) {
       
    case Model3D::POINTS: {  // Output point cloud.
      glBegin(GL_POINTS);
      for (int i = 0; i < mesh->nrElements; i ++) {
        geoset = mesh->geosets[i];
        if (!geoset->enabled) continue;
        for (int v = 0; v < geoset->nrV; v ++) {
          glVertex3f(
            geoset->vertices[v].X, 
            geoset->vertices[v].Y, 
            geoset->vertices[v].Z
          );
        }
      }
      glEnd();
      break;
    }
        
    case Model3D::MESH: {  // Draw a triangle mesh. 
      for (int g, t, i = 0; i < mesh->nrElements; i ++) {
        geoset = mesh->geosets[i];
        if (!geoset->enabled) continue;
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
      glEnable(GL_LIGHTING);
      for (int g, t, i = 0; i < mesh->nrElements; i ++) {
        geoset = mesh->geosets[i];
        if (!geoset->enabled) continue; 

        // Load and set texture, if available.
        if (geoset->textureID != -1) {
          glEnable(GL_TEXTURE_2D);
          glBindTexture(GL_TEXTURE_2D, mdl->Textures[geoset->textureID]->ID()); 
        }

        glBegin(GL_TRIANGLES);
        for (g = 0; g < geoset->nrG; g ++) {
          for (t = 0; t < 3; t ++) {
            tri = &geoset->geometries[g];
            glTexCoord2f(
              geoset->texVects[tri->tIdx[t]].X,
              geoset->texVects[tri->tIdx[t]].Y
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
        glEnd();  
        if (geoset->textureID != -1) glDisable(GL_TEXTURE_2D);
      }          
       

      glDisable(GL_LIGHTING);
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
