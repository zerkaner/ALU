#pragma once
#pragma warning(disable: 4996) // Skip fopen warnings.

#include <Data/Object3D.h>
#include <Data/Textures/ImageLoader.h>
#include <stdio.h>
#include <stdlib.h>


class HeightmapTerrain : public Object3D {

  private:

    int _width;                      // Width of grid (x value).
    int _height;                     // Height of grid (y value).
    const int levelOfDetail = 8;     // The LOD determines interpolation ratio.
    const int DrawMode = 0;          // Terrain rendering mode.


    /** Load a new heightmap file.
     * @param filename The file path. */
    Geoset* LoadHeightmap(const char* filename) {
      FILE* file = fopen(filename, "rb");          // Open file stream.
      if (file == NULL) {
        printf("[HeightmapTerrain] Error loading heightmap file '%s'.\n", filename);
        return NULL;
      }    
      
      // Read in dimensions (width, height) and height data.
      fread(&_width,  sizeof(int), 1, file);
      fread(&_height, sizeof(int), 1, file);
      BYTE* heights = (BYTE*) calloc(_width*_height, sizeof(BYTE));
      fread(heights, sizeof(BYTE), _width*_height, file);
      fclose(file);

      // Calculate storage space (same loop, only counter increase).
      long nrVNT = 0;
      for (int x = 0; x < _width-levelOfDetail; x += levelOfDetail) {
        for (int y = 0; y < _height; y += levelOfDetail) nrVNT += 6;
      }  //TODO Create formula to avoid this unnecessary loop!
      long nrG = nrVNT / 3;

      // Create geoset, set properties and then allocate arrays.
      Geoset* geoset = new Geoset();
      geoset->enabled = true;
      (geoset->nrV = geoset->nrN = geoset->nrT = nrVNT); geoset->nrG = nrG;
      geoset->vertices   = new Float3[nrVNT];
      geoset->normals    = new Float3[nrVNT];
      geoset->texVects   = new Float2[nrVNT];
      geoset->geometries = new Geometry[nrG];
           
      long cntVNT = 0, cntG = 0;  // Counter for array iteration.

      
      // Iterate over height map and generate triangles.
      for (int x = 0; x < _width-levelOfDetail; x += levelOfDetail) {  // X values.
        for (int y = 0; y < _height; y += levelOfDetail) {             // Y values.
          
          // We compute two triangles each time. First, create its vertices.
          for (int i = 0; i < 6; i ++, cntVNT ++) {       
            int valX = x + ((i == 1 || i == 2 || i == 5) ? levelOfDetail : 0);
            int valY = y + ((i == 1 || i == 4 || i == 5) ? levelOfDetail : 0);
            float valZ = heights[valX*_width + valY];
            
            geoset->vertices[cntVNT].X = (float) valX;
            geoset->vertices[cntVNT].Y = (float) valY;
            geoset->vertices[cntVNT].Z = valZ;
            geoset->texVects[cntVNT].X = (float) valX/_width;
            geoset->texVects[cntVNT].Y = (float) valY/_height;

            // Set up geometry. Triggers on '2' and '5'.
            if (i%3 == 2) {
              Geometry* geo = &geoset->geometries[cntG];
              geo->symIndices = true; // Same indices for vertices, textures and normals.
              geo->vIdx[0] = geo->nIdx[0] = geo->tIdx[0] = cntVNT-2;
              geo->vIdx[1] = geo->nIdx[1] = geo->tIdx[1] = cntVNT-1;
              geo->vIdx[2] = geo->nIdx[2] = geo->tIdx[2] = cntVNT;
              cntG ++;
            }
          }
        }       
      }

      // Delete reserved space for read-in data.
      free(heights);
      return geoset;
    }


  protected:

    /** Returns the position of this object. [override]
     * @return The position as a Float3 (x,y,z). */
    Float3 GetPosition() { 
      return Float3(0.0f, 0.0f, 0.0f); 
    }


    /** Returns the orientation of this object. [override]
     * @return A Float2 structure with yaw (x) and pitch (y). */
    Float2 GetOrientation() { 
      return Float2(0.0f, 0.0f);
    }


  public:
  
    /** Create a new heightmap based terrain.
     * @param filename The heightmap file to use. */
    HeightmapTerrain(const char* filename) {
      Position = Vector(-5, -5, -10);
      Model = new Model3D();
      Model->Geosets.push_back(LoadHeightmap(filename));
      Model->Textures.push_back(ImageLoader::LoadTexture("Other/Terrain.jpg"));
      Model->Geosets[0]->textureID = 0;
      Model->CalculateNormals();
      Model->RenderingMode = Model3D::DIRECT;
      Model->ScaleModel(0.04f);
    }
};
