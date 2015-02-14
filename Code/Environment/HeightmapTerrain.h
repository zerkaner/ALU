#pragma once
#pragma warning(disable: 4996) // Skip fopen warnings.

#include <Data/Model3D.h>
#include <stdio.h>
#include <stdlib.h>


class HeightmapTerrain : public Model3D {

  private:

    int _width;                      // Width of grid (x value).
    int _height;                     // Height of grid (y value).
    const int levelOfDetail = 8;     // The LOD determines interpolation ratio.
    const int DrawMode = 0;          // Terrain rendering mode.


    /** Load a new heightmap file.
     * @param filename The file path. */
    void LoadHeightmap(const char* filename) {
      FILE* file = fopen(filename, "rb");          // Open file stream.
      if (file == NULL) {
        printf("[HeightmapTerrain] Error loading heightmap file '%s'.\n", filename);
        return;
      }    
      
      // Read in dimensions (width, height) and height data.
      fread(&_width,  sizeof(int), 1, file);
      fread(&_height, sizeof(int), 1, file);
      BYTE* heights = (BYTE*) calloc (_width*_height, sizeof (BYTE));
      fread (heights, sizeof (BYTE), _width*_height, file);
      fclose(file);


      // Iterate over height map and generate triangles.
      for (int x = 0; x < _width - levelOfDetail; x+= levelOfDetail) {  // X values.
        for (int y = 0; y < _height; y+=levelOfDetail) {                // Y values.
          /*
          // We compute two triangles each time. First, create its vertices.
          Vertex* vertices[6];
          for (int i = 0; i < 6; i++) {
            
            int valX = x + ((i == 1 || i == 2 || i == 5) ? levelOfDetail : 0);
            int valY = y + ((i == 1 || i == 4 || i == 5) ? levelOfDetail : 0);
            float valZ = heights[valX*_width + valY];

            Float3 coordinates = Float3((float)valX, (float)valY, valZ);
            Float2 texVector = Float2((float) valX/_width, (float) valY/_height);
            vertices[i] = new Vertex(coordinates, texVector);     
          }
          
          // Set up both triangles.
          Triangles.push_back(new Geometry(vertices[0], vertices[1], vertices[2]));
          Triangles.push_back(new Geometry(vertices[3], vertices[4], vertices[5]));
          */
        }       
      }

      // Delete reserved space for read-in data.
      free (heights);
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
      LoadHeightmap(filename);
      RenderingMode = Model3D::MESH;
    }
};
