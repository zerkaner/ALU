#include "GLDrawer.h"
#include <Data/AnimationManager.h>
#include <Data/Object3D.h>
#include <Data/Textures/Texture.h>
#include <SDL_opengl.h>


static Float3 CalculateVertex(Float3 v, BoneWeight w, vector<Bone>& bones) {
  if (w.BoneIDs[0] != 255) {
    int mi;
    float mat[16];
    float* wm1 = bones[w.BoneIDs[0]].WorldMatrix;
    float f1 = w.Factor[0];
    for (mi = 0; mi < 16; mi ++) mat[mi] = f1 * wm1[mi];
    if (w.BoneIDs[1] != 255) {
      float* wm2 = bones[w.BoneIDs[1]].WorldMatrix;
      float f2 = w.Factor[1];
      for (mi = 0; mi < 16; mi ++) mat[mi] += f2 * wm2[mi];
      if (w.BoneIDs[2] != 255) {
        float* wm3 = bones[w.BoneIDs[2]].WorldMatrix;
        float f3 = w.Factor[2];
        for (mi = 0; mi < 16; mi ++) mat[mi] += f3 * wm3[mi];
        if (w.BoneIDs[3] != 255) {
          float* wm4 = bones[w.BoneIDs[3]].WorldMatrix;
          float f4 = w.Factor[3];
          for (mi = 0; mi < 16; mi ++) mat[mi] += f4 * wm4[mi];
        }
      }
    } 
    v = MathLib::TransformByMatrix(v, mat);
  }
  return v;
}



void GLDrawer::Draw(Object3D* obj) {

  // Instant skip, if no model is assigned or it is disabled.
  if (obj->Model3D == NULL || obj->Model3D->_renderMode == 0) return;

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


  Model* mdl = obj->Model3D;
  if (mdl->AnimMgr != NULL) mdl->AnimMgr->Tick();


  // Draw on selected rendering mode.
  switch (mdl->_renderMode) {

    case 1: {  // Output point cloud.
      glPointSize(1);
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
          Float3 v1 = CalculateVertex(mdl->Vertices[i0], mdl->Weights[i0], mdl->Bones);
          Float3 v2 = CalculateVertex(mdl->Vertices[i1], mdl->Weights[i1], mdl->Bones);
          Float3 v3 = CalculateVertex(mdl->Vertices[i2], mdl->Weights[i2], mdl->Bones);
          glBegin(GL_LINE_LOOP);
          glVertex3f(v1.X, v1.Y, v1.Z);
          glVertex3f(v2.X, v2.Y, v2.Z);
          glVertex3f(v3.X, v3.Y, v3.Z);
          glEnd();
        }
      }
      
      // Display the bones and connections.
      glPointSize(3);          
      for (uint b = 0; b < mdl->Bones.size(); b ++) { 
        Bone* bone = &mdl->Bones[b];
        glBegin(GL_POINTS);
        glColor3f(1.0, 0.0, 0.0); 
        glVertex3f(bone->Position.X, bone->Position.Y, bone->Position.Z);
        glEnd();
        if (bone->Parent != -1) {
          Bone* parent = &mdl->Bones[bone->Parent];
          glBegin(GL_LINES);
          glColor3f(0.85f, 0.54f, 0.25f);
          glVertex3f(bone->Position.X, bone->Position.Y, bone->Position.Z);
          glVertex3f(parent->Position.X, parent->Position.Y, parent->Position.Z);
          glEnd();
        }
      }   
      glColor3f(1.0, 1.0, 1.0);
      break;
    }

    case 3: {  // Direct CPU rendering.

      //glEnable(GL_LIGHTING);
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
          Float3 vtx = mdl->Vertices[idx];
          if (mdl->Meshes[m].Attached) vtx = CalculateVertex(vtx, mdl->Weights[idx], mdl->Bones);
          glVertex3f(vtx.X, vtx.Y, vtx.Z);
        }
        glEnd();

        if (mdl->Meshes[m].TextureIdx != -1) glDisable(GL_TEXTURE_2D);
      }
      //glDisable(GL_LIGHTING);
      break;
    }


    // Further rendering cases here.
  }
  glPopMatrix();
}
