#include "GLDrawer.h"
#include <Data/Object3D.h>
#include <Data/Textures/Texture.h>
#include <SDL_opengl.h>
#include <cmath>


void GLDrawer::Draw(Object3D* obj) {

  // Instant skip, if no model is assigned or it is disabled.
  if (obj->Model == NULL || obj->Model->_renderMode == 0) return;

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
  
  // Draw on selected rendering mode.
  Model2* mdl = obj->Model;  
  switch (mdl->_renderMode) {

    case 1: {  // Output point cloud.
      glBegin(GL_POINTS);
      for (uint i = 0; i < mdl->Vertices.size(); i ++) {
        glVertex3f(mdl->Vertices[i].X, mdl->Vertices[i].Y, mdl->Vertices[i].Z);
      }
      glEnd();
      break;
    }

    case 2: {  // Draw a triangle mesh. 
      for (uint m = 0; m < mdl->Meshes.size(); m ++) { // Mesh loop.
        if (!mdl->Meshes[m].Enabled) continue;

        DWORD idxStart = mdl->Meshes[m].IndexOffset;
        DWORD idxLength = mdl->Meshes[m].IndexLength;

        for (DWORD i = 0; i < idxLength; i += 3) {  // Triangle loop.
          DWORD i0 = mdl->Indices[idxStart + i];
          DWORD i1 = mdl->Indices[idxStart + i + 1];
          DWORD i2 = mdl->Indices[idxStart + i + 2];

          glBegin(GL_LINE_LOOP);
          glVertex3f(mdl->Vertices[i0].X, mdl->Vertices[i0].Y, mdl->Vertices[i0].Z);
          glVertex3f(mdl->Vertices[i1].X, mdl->Vertices[i1].Y, mdl->Vertices[i1].Z);
          glVertex3f(mdl->Vertices[i2].X, mdl->Vertices[i2].Y, mdl->Vertices[i2].Z);
          glEnd();
        }
      }

      for (uint b = 0; b < mdl->Bones.size(); b ++) { // Bone loop.
        glColor3f(1.0, 0.0, 0.0);
        Float3 p = mdl->Bones[b].Position;
        float o = 0.015f;
        glBegin(GL_LINE_LOOP); // bottom
        glVertex3f(p.X - o, p.Y - o, p.Z - o);  
        glVertex3f(p.X + o, p.Y - o, p.Z - o);  
        glVertex3f(p.X, p.Y + o, p.Z - o);
        glEnd();

        glBegin(GL_LINE_LOOP); // front
        glVertex3f(p.X - o, p.Y - o, p.Z - o);  
        glVertex3f(p.X + o, p.Y - o, p.Z - o);  
        glVertex3f(p.X, p.Y, p.Z + o);
        glEnd();

        glBegin(GL_LINE_LOOP); // left/back
        glVertex3f(p.X - o, p.Y - o, p.Z - o);  
        glVertex3f(p.X, p.Y + o, p.Z - o);  
        glVertex3f(p.X, p.Y, p.Z + o);
        glEnd();

        glBegin(GL_LINE_LOOP); // right/back
        glVertex3f(p.X + o, p.Y - o, p.Z - o);  
        glVertex3f(p.X, p.Y + o, p.Z - o);  
        glVertex3f(p.X, p.Y, p.Z + o);
        glEnd();
        glColor3f(1.0, 1.0, 1.0);
      }
      break;
    }

    case 3: {  // Direct CPU rendering.
      glEnable(GL_LIGHTING);
      for (uint m = 0; m < mdl->Meshes.size(); m ++) { // Mesh loop.
        if (!mdl->Meshes[m].Enabled) continue;

        // Load and set texture, if available.
        if (mdl->Meshes[m].TextureIdx != -1) {
          glEnable(GL_TEXTURE_2D);
          glBindTexture(GL_TEXTURE_2D, mdl->Textures[mdl->Meshes[m].TextureIdx]->ID());
        }

        glBegin(GL_TRIANGLES);
        DWORD idxStart = mdl->Meshes[m].IndexOffset;
        DWORD idxLength = mdl->Meshes[m].IndexLength;
        for (DWORD i = 0; i < idxLength; i ++) {  // Index loop.
          DWORD idx = mdl->Indices[idxStart + i];
          glTexCoord2f(mdl->UVs[idx].X, mdl->UVs[idx].Y);
          glNormal3f(mdl->Normals[idx].X, mdl->Normals[idx].Y, mdl->Normals[idx].Z);
          glVertex3f(mdl->Vertices[idx].X, mdl->Vertices[idx].Y, mdl->Vertices[idx].Z);
        }
        glEnd();

        if (mdl->Meshes[m].TextureIdx != -1) glDisable(GL_TEXTURE_2D);
      }
      glDisable(GL_LIGHTING);
      break;
    }


    // Further rendering cases here.
  }
  glPopMatrix();
}
