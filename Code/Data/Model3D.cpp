#pragma warning(disable: 4996) // Skip MSVC warnings.
#include "Model3D.h"
#include "Primitives.h"
#include "Textures/Texture.h"
#include <cmath>
#include <stdio.h>
#include <stdlib.h>


Model3D::Model3D() {
}



Model3D::Model3D(const char* filepath) {
  LoadFile(filepath);
  RenderingMode = Model3D::DIRECT;
}



Model3D::~Model3D() {
  ClearModel();
}



void Model3D::ClearModel() {
  
  // Clear all arrays of the geoset structures. 
  for (unsigned int i = 0; i < Geosets.size(); i ++) {
    delete[] Geosets[i]->vertices;
    delete[] Geosets[i]->normals;
    delete[] Geosets[i]->texVects;
    delete[] Geosets[i]->geometries;
    delete Geosets[i];
  }
  Geosets.clear();

  // Delete textures.
  for (unsigned int i = 0; i < Textures.size(); i ++) {
    delete Textures[i];
  }
  Textures.clear();
}



void Model3D::LoadFile(const char* filepath) {
  
  // Unload previous model data (if existent).
  if (Geosets.size() > 0) ClearModel();

  // Try to open the file.
  printf("Loading file '%s' ", filepath);
  FILE* fp = fopen(filepath, "rb");
  if (fp == NULL) {
    printf("[ERROR]\n");
    return;
  }
      
  // Echo file size.
  fseek(fp, 0L, SEEK_END);
  unsigned long bytes = ftell(fp);
  fseek(fp, 0L, SEEK_SET);
  printf("[%lu bytes] ", bytes);


  // Read number of geosets, create structure and loop over them.
  int nrGeosets;
  fread(&nrGeosets, sizeof(int), 1, fp);
  for (int i, j, g = 0; g < nrGeosets; g ++) {
    Geosets.push_back(new Geoset());

    // Read identifier, texture association and rendering status.
    fread(&Geosets[g]->id,        sizeof(int),  1, fp);
    fread(&Geosets[g]->textureID, sizeof(int),  1, fp);
    fread(&Geosets[g]->enabled,   sizeof(bool), 1, fp);

    // Read vertices.
    fread(&Geosets[g]->nrV, sizeof(long), 1, fp);
    Geosets[g]->vertices = (Float3*) calloc(Geosets[g]->nrV, sizeof(Float3));
    for (i = 0; i < Geosets[g]->nrV; i ++) {
      fread(&Geosets[g]->vertices[i].X, sizeof(float), 1, fp);
      fread(&Geosets[g]->vertices[i].Y, sizeof(float), 1, fp);
      fread(&Geosets[g]->vertices[i].Z, sizeof(float), 1, fp);
    }

    // Read normal vectors.
    fread(&Geosets[g]->nrN, sizeof(long), 1, fp);
    Geosets[g]->normals = (Float3*) calloc(Geosets[g]->nrN, sizeof(Float3));
    for (i = 0; i < Geosets[g]->nrN; i ++) {
      fread(&Geosets[g]->normals[i].X, sizeof(float), 1, fp);
      fread(&Geosets[g]->normals[i].Y, sizeof(float), 1, fp);
      fread(&Geosets[g]->normals[i].Z, sizeof(float), 1, fp);
    }

    // Read texture coordinates.
    fread(&Geosets[g]->nrT, sizeof(long), 1, fp);
    Geosets[g]->texVects = (Float2*) calloc(Geosets[g]->nrT, sizeof(Float2));
    for (i = 0; i < Geosets[g]->nrT; i ++) {
      fread(&Geosets[g]->texVects[i].X, sizeof(float), 1, fp);
      fread(&Geosets[g]->texVects[i].Y, sizeof(float), 1, fp);
    }

    // Read geometries.
    fread(&Geosets[g]->nrG, sizeof(long), 1, fp);
    Geosets[g]->geometries = (Geometry*) calloc(Geosets[g]->nrG, sizeof(Geometry));
    for (i = 0; i < Geosets[g]->nrG; i ++) {
      
      // If indices are symmetric, read one and apply to each. Otherwise read in sequential.
      fread(&Geosets[g]->geometries[i].symIndices, sizeof(bool), 1, fp);
      for (j = 0; j < 3; j ++) {  
        fread(&Geosets[g]->geometries[i].vIdx[j], sizeof(long), 1, fp); 
        if (Geosets[g]->geometries[i].symIndices) {
          Geosets[g]->geometries[i].nIdx[j] = Geosets[g]->geometries[i].vIdx[j];
          Geosets[g]->geometries[i].tIdx[j] = Geosets[g]->geometries[i].vIdx[j];
        }
        else {
          fread(&Geosets[g]->geometries[i].nIdx[j], sizeof(long), 1, fp); 
          fread(&Geosets[g]->geometries[i].tIdx[j], sizeof(long), 1, fp); 
        }        
      }
    }
  }

  // Read textures from file.
  int nrTextures;
  fread(&nrTextures, sizeof(int), 1, fp);
  for(int t = 0; t < nrTextures; t ++) {
    unsigned long size;
    fread(&size, sizeof(unsigned long), 1, fp);
    BYTE* data = (BYTE*) calloc(size, sizeof(BYTE));
    fread(data, sizeof(BYTE), size, fp);    
    Textures.push_back(new SimpleTexture(data, size, "-internal-"));
  }

  // Close file stream and quit.
  fclose(fp);
  printf("[OK]\n");
}



void Model3D::WriteFile(const char* filepath) {

  // Open writer stream.
  printf("Writing file '%s' ", filepath);
  FILE* fp = fopen(filepath, "wb");
  if (fp == NULL) {
    printf("[ERROR]\n");
    return;
  }


  // Write number of geosets and loop over them.
  int nrGeosets = Geosets.size();
  fwrite(&nrGeosets, sizeof(int), 1, fp);
  for (int i, j, g = 0; g < nrGeosets; g ++) {
  
    // Write identifier, texture association and rendering status.
    fwrite(&Geosets[g]->id,        sizeof(int),  1, fp);
    fwrite(&Geosets[g]->textureID, sizeof(int),  1, fp);
    fwrite(&Geosets[g]->enabled,   sizeof(bool), 1, fp);

    // Write vertices.
    fwrite(&Geosets[g]->nrV, sizeof(long), 1, fp);
    for (i = 0; i < Geosets[g]->nrV; i ++) {
      fwrite(&Geosets[g]->vertices[i].X, sizeof(float), 1, fp);
      fwrite(&Geosets[g]->vertices[i].Y, sizeof(float), 1, fp);
      fwrite(&Geosets[g]->vertices[i].Z, sizeof(float), 1, fp);
    }

    // Write normal vectors.
    fwrite(&Geosets[g]->nrN, sizeof(long), 1, fp);
    for (i = 0; i < Geosets[g]->nrN; i ++) {
      fwrite(&Geosets[g]->normals[i].X, sizeof(float), 1, fp);
      fwrite(&Geosets[g]->normals[i].Y, sizeof(float), 1, fp);
      fwrite(&Geosets[g]->normals[i].Z, sizeof(float), 1, fp);
    }

    // Write texture coordinates.
    fwrite(&Geosets[g]->nrT, sizeof(long), 1, fp);
    for (i = 0; i < Geosets[g]->nrT; i ++) {
      fwrite(&Geosets[g]->texVects[i].X, sizeof(float), 1, fp);
      fwrite(&Geosets[g]->texVects[i].Y, sizeof(float), 1, fp);
    }

    // Write geometries.
    fwrite(&Geosets[g]->nrG, sizeof(long), 1, fp);
    for (i = 0; i < Geosets[g]->nrG; i ++) {
      fwrite(&Geosets[g]->geometries[i].symIndices, sizeof(bool), 1, fp);     
      for (j = 0; j < 3; j ++) {  
        fwrite(&Geosets[g]->geometries[i].vIdx[j], sizeof(long), 1, fp); 
        if (!Geosets[g]->geometries[i].symIndices) {
          fwrite(&Geosets[g]->geometries[i].nIdx[j], sizeof(long), 1, fp); 
          fwrite(&Geosets[g]->geometries[i].tIdx[j], sizeof(long), 1, fp); 
        }        
      }
    }
  }

  // Write textures to file.
  int texSize = Textures.size();
  fwrite(&texSize, sizeof(int), 1, fp);
  for(int t = 0; t < texSize; t ++) {
    unsigned long size = Textures[t]->Size();
    BYTE* data = Textures[t]->Data();
    fwrite(&size, sizeof(unsigned long), 1, fp);   
    fwrite(data, sizeof(BYTE), size, fp);
  }

  // Close file stream and quit.
  printf("[%lu bytes out]\n", (unsigned long) ftell(fp));
  fclose(fp);
}



void Model3D::ScaleModel(float factor) {
  printf("Scaling model by factor '%6.4f'.\n", factor);
  for (unsigned int i = 0; i < Geosets.size(); i ++) {
    for (int v = 0; v < Geosets[i]->nrV; v ++) {
      Geosets[i]->vertices[v].X *= factor;
      Geosets[i]->vertices[v].Y *= factor;
      Geosets[i]->vertices[v].Z *= factor;
    }
  }
}



/* Internal helper function to get vector value based on axis. */
float _getValue(Float3 vtx, char axis) {
  switch (axis) {
  case 'X': case 'x': return vtx.X;
  case 'Y': case 'y': return vtx.Y;
  case 'Z': case 'z': return vtx.Z;
  default: return -1.0f;
  }
}



void Model3D::ScaleModelToExtent(char axis, float value) {
  printf("Scaling model to extent '%f' on axis '%c'.\n", value, axis);
  if (value <= 0) {
    printf("Model scaling failed: Value is '%f' (must be greater than 0).\n", value);
    return;
  }
  if (!(axis=='x' || axis=='y' || axis=='z' || axis=='X' || axis=='Y' || axis=='Z')) {
    printf("Model scaling failed: Axis '%c' invalid (supported: x, y, z).\n", axis);
    return;    
  }
  
  // Get minimal and maximal value.
  float minVal, maxVal;
  minVal = maxVal = _getValue(Geosets[0]->vertices[0], axis);
  for (unsigned int i = 0; i < Geosets.size(); i++) {
    for (int v = 0; v < Geosets[i]->nrV; v++) {
      float val = _getValue(Geosets[i]->vertices[v], axis);
      if (val > maxVal) maxVal = val;
      else if (val < minVal) minVal = val;
    }
  }

  // Calculate maximal extents and perform scaling based on desired value. 
  float factor = value / (maxVal - minVal);
  ScaleModel(factor);
}



/* Calculate vertex normals. */
void Model3D::CalculateNormals() {
  for (unsigned int i = 0; i < Geosets.size(); i ++) {
    Geoset* geo = Geosets[i];

    // Allocate normals array and set counter.
    if (geo->nrN > 0) delete[] geo->normals;
    geo->nrN = geo->nrV;
    geo->normals = new Float3 [geo->nrN];


    // Calculate all triangle normals and put them to the correspondent vertex normals.
    for (int g = 0; g < geo->nrG; g ++) {
      Geometry* tri = &geo->geometries[g];

      // Get edge vectors u and v (v1v2, v1v3).
      Float3 v1 = geo->vertices[tri->vIdx[2]];
      Float3 v2 = geo->vertices[tri->vIdx[1]];
      Float3 v3 = geo->vertices[tri->vIdx[0]];
      Float3 u = Float3(v2.X-v1.X, v2.Y-v1.Y, v2.Z-v1.Z);
      Float3 v = Float3(v3.X-v1.X, v3.Y-v1.Y, v3.Z-v1.Z);

      // Calculate triangle normal vector (cross product).
      Float3 nor = Float3(
        (u.Y*v.Z - u.Z*v.Y), 
        (u.Z*v.X - u.X*v.Z), 
        (u.X*v.Y - u.Y*v.X)
      );

      // Add calculated triangle normal vector to all involved vertices-normals.
      geo->normals[tri->vIdx[0]] += nor;
      geo->normals[tri->vIdx[1]] += nor;
      geo->normals[tri->vIdx[2]] += nor;

      // Set index references. These are the same as the vertex indices.
      tri->nIdx[0] = tri->vIdx[0];
      tri->nIdx[1] = tri->vIdx[1];
      tri->nIdx[2] = tri->vIdx[2];
    }


    // Normalize all calculated normal vectors to unit vectors.
    for (int n = 0; n < geo->nrN; n ++) {
      float length = sqrt(
        geo->normals[n].X*geo->normals[n].X + 
        geo->normals[n].Y*geo->normals[n].Y + 
        geo->normals[n].Z*geo->normals[n].Z
      );
      geo->normals[n].X /= length;
      geo->normals[n].Y /= length;
      geo->normals[n].Z /= length;    
    }
  }
}


/*
void Model3D::CalculateNormals() {
  for (unsigned int i = 0; i < Geosets.size(); i ++) {
    Geoset* geo = Geosets[i];

    // Allocate normals array and set counter.
    if (geo->nrN > 0) delete[] geo->normals;
    geo->nrN = geo->nrG;
    geo->normals = new Float3 [geo->nrN];

    for (int n = 0; n < geo->nrN; n ++) {
      Geometry* tri = &geo->geometries[n];

      // Get edge vectors u and v (p1p2, p1p3).
      Float3 p1 = geo->vertices[tri->vIdx[0]];
      Float3 p2 = geo->vertices[tri->vIdx[2]];
      Float3 p3 = geo->vertices[tri->vIdx[1]];
      Float3 u = Float3(p2.X-p1.X, p2.Y-p1.Y, p2.Z-p1.Z);
      Float3 v = Float3(p3.X-p1.X, p3.Y-p1.Y, p3.Z-p1.Z);

      // Calculate normal vector (cross product) and set index reference.
      geo->normals[n].X = u.Y*v.Z - u.Z*v.Y;
      geo->normals[n].Y = u.Z*v.X - u.X*v.Z;
      geo->normals[n].Z = u.X*v.Y - u.Y*v.X;
      tri->nIdx[0] = tri->nIdx[1] = tri->nIdx[2] = n;
      
      // Normalize the calculated vectors to unit vectors.
      float length = sqrt(
        geo->normals[n].X*geo->normals[n].X + 
        geo->normals[n].Y*geo->normals[n].Y + 
        geo->normals[n].Z*geo->normals[n].Z
      );
      geo->normals[n].X /= length;
      geo->normals[n].Y /= length;
      geo->normals[n].Z /= length;
    }
  }
}
*/


void Model3D::Echo(bool fileOutput, const char* filename) {
  if (fileOutput) {
    FILE* fp = fopen (filename, "w");
    fprintf (fp, "Detailed output for Model3D:\n");
    for (unsigned int i = 0; i < Geosets.size(); i ++) {
      fprintf (fp, " - Geoset [%02d] (%s)\n", Geosets[i]->id, 
              (Geosets[i]->enabled? "enabled" : "disabled"));
      
      // Vertex output for current geoset.
      fprintf (fp, "   - Vertices: %d\n", Geosets[i]->nrV);
      for (int j = 0; j < Geosets[i]->nrV; j ++) {
        fprintf (fp, "     [%02d] %8.4f, %8.4f, %8.4f\n", j, 
          Geosets[i]->vertices[j].X, Geosets[i]->vertices[j].Y, Geosets[i]->vertices[j].Z);
      }

      // Normal vectors output.
      fprintf (fp, "   - Normals: %d\n", Geosets[i]->nrN);
      for (int j = 0; j < Geosets[i]->nrN; j ++) {
        fprintf (fp, "     [%02d] %8.4f, %8.4f, %8.4f\n", j, 
          Geosets[i]->normals[j].X, Geosets[i]->normals[j].Y, Geosets[i]->normals[j].Z);
      }

      // Output for texture vectors.
      fprintf (fp, "   - Textures: %d\n", Geosets[i]->nrT);
      for (int j = 0; j < Geosets[i]->nrT; j ++) {
        fprintf (fp, "     [%02d] %8.4f, %8.4f\n", j, 
          Geosets[i]->texVects[j].X, Geosets[i]->texVects[j].Y);
      }

      // Geometry indices output.
      fprintf (fp, "   - Geometries: %d\n", Geosets[i]->nrG);
      fprintf (fp, "     Note: Output in .obj style (v/vt/vn), but indices reduced by one to match array fields.\n");
      for (int j = 0; j < Geosets[i]->nrG; j ++) {
        fprintf (fp, "     [%02d]", j);
        for (int k = 0; k < 3; k ++) {
          fprintf (fp, "  %d/%d/%d", Geosets[i]->geometries[j].vIdx[k], 
                                     Geosets[i]->geometries[j].tIdx[k], 
                                     Geosets[i]->geometries[j].nIdx[k]);
        }       
        fprintf (fp, "\n");
      }
    }
  }

  // Just output a brief overview on the console window.
  else {
    printf ("Overview for Model3D:\n");
    for (unsigned int i = 0; i < Geosets.size(); i ++) {
      printf (" - Geoset [%02d] (%s)\n"
              "   - Vertices  : %d\n"
              "   - Normals   : %d\n"
              "   - Textures  : %d\n"
              "   - Geometries: %d\n",
              Geosets[i]->id, (Geosets[i]->enabled? "enabled" : "disabled"),
              Geosets[i]->nrV, Geosets[i]->nrN, Geosets[i]->nrT, Geosets[i]->nrG);
    }
  }
  printf ("\n");
}
