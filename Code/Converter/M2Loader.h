#pragma once
#pragma warning(disable: 4996)
#include <Data/Primitives.h>
#include <Data/Model3D.h>
#include <stdio.h>
#include <stdlib.h>


/** Loading utility for Blizzard's M2 model format. */
class M2Loader {

  //TODO 1) Laden mit Datenstrom-Zeiger (integrierter Aufruf).
  //TODO 2) Eigenständiges Laden (Pfadangabe), vorgeschaltet.



  public:

    M2Loader() {
      ConvertM2("C:\\Users\\Jan Dalski\\Downloads\\Warcraft 3 Mod-Tools\\01 - M2-Exporte\\HumanMale.m2");
      getchar();
    }
    
    void ConvertM2(const char* filepath) {

      // Try to open filestream.
      FILE* fp = fopen(filepath, "rb");
      if (fp == NULL) {
        printf("\nError opening file '%s'!\n", filepath);
        return;
      }

      // Get the length of the file and print status output.
      fseek(fp, 0L, SEEK_END);
      unsigned long bytes = ftell(fp);
      fseek(fp, 0L, SEEK_SET);
      printf("\nOpening file '%s' [%lu bytes].\n", filepath, bytes);

      // File stream okay. Start converter.
      Model3D* model = LoadM2(fp);
      
      // After successful conversion write model to disk.
      if (model != NULL) {
        model->Echo(true);
        model->WriteFile("test.m4");
        delete model;
      }
      printf("Converter done.\n");
      fclose(fp);
    }



    Model3D* LoadM2(FILE* fp) {
      DWORD dbuf;
      fread(&dbuf, sizeof(DWORD), 1, fp);  // Skip "MD20".
      fread(&dbuf, sizeof(DWORD), 1, fp);  // Skip version number (for now).

      int namelength, nameoffset;
      fread(&namelength, sizeof(int), 1, fp);  // Read model name length.
      fread(&nameoffset, sizeof(int), 1, fp);  // Read model name offset.
      char* modelname = (char*) calloc(namelength, sizeof(char));    

      // Skip next 13 fields (ignored until needed).
      for (int i = 0; i < 13; i ++) fread(&dbuf, sizeof(DWORD), 1, fp);
      
      int nrVertices, offsetVertices, nrLOD, offsetLOD;
      fread(&nrVertices, sizeof(int), 1, fp);     // Read number of vertices.
      fread(&offsetVertices, sizeof(int), 1, fp); // Offset to vertex definitions.
      fread(&nrLOD, sizeof(int), 1, fp);          // Read number of LOD groups.
      fread(&offsetLOD, sizeof(int), 1, fp);      // LOD offset.


      // Read model name.
      fseek(fp, nameoffset, SEEK_SET);
      fread(modelname, sizeof(char), namelength, fp);

      // Advance to vertex definitions. Reserve memory, then start read-in.
      fseek(fp, offsetVertices, SEEK_SET);
      Float3* vertices = (Float3*) calloc(nrVertices, sizeof(Float3));
      Float3* normals  = (Float3*) calloc(nrVertices, sizeof(Float3));
      Float2* textures = (Float2*) calloc(nrVertices, sizeof(Float2));
      for (int i = 0; i < nrVertices; i ++) {
        fread(&vertices[i], sizeof(float), 3, fp); // Read position.
        fread(&dbuf, sizeof(DWORD), 1, fp);        // [4x8] Skip bone weights (0 to 255).
        fread(&dbuf, sizeof(DWORD), 1, fp);        // [4x8] Skip bone indices (0 to nBones - 1).
        fread(&normals[i], sizeof(float), 3, fp);  // Read normal vector.
        fread(&textures[i], sizeof(float), 2, fp); // Read texture coordinates.
        fseek(fp, 2*sizeof(float), SEEK_CUR);      // Skip two (useless) floats.
      }

      // Read LOD definitions. Four headers with 0x2C bytes each, then follows the data block.
      // Because these definitions seems to be identical, we just take the first one.
      int nrIndices, nrTriangles, nrProperties, nrSubmeshes, nrTextures;
      int offIndices, offTriangles, offProperties, offSubmeshes, offTextures;
      fread(&nrIndices,     sizeof(int), 1, fp);  // [0x00]: Number of elements in the index list.
      fread(&offIndices,    sizeof(int), 1, fp);  // [0x04]: Offset to the index list.
      fread(&nrTriangles,   sizeof(int), 1, fp);  // [0x08]: Number of elements in the triangle list.
      fread(&offTriangles,  sizeof(int), 1, fp);  // [0x0C]: Offset to the triangle list.
      fread(&nrProperties,  sizeof(int), 1, fp);  // [0x10]: Number of elements in the vertex property list.
      fread(&offProperties, sizeof(int), 1, fp);  // [0x14]: Offset to the vertex property list.
      fread(&nrSubmeshes,   sizeof(int), 1, fp);  // [0x18]: Number of elements in the submesh list.
      fread(&offSubmeshes,  sizeof(int), 1, fp);  // [0x1C]: Offset to the submesh list.
      fread(&nrTextures,    sizeof(int), 1, fp);  // [0x20]: Number of elements in the texture list.
      fread(&offTextures,   sizeof(int), 1, fp);  // [0x24]: Offset to the texture list.
      fseek(fp, sizeof(float), SEEK_CUR);         // [0x28]: Skip a float (LOD distance or whatever).

      // Read vertex and triangle indices.
      short* indices = (short*) calloc(nrIndices, sizeof(short));       // Global indices.
      short* triangleInd = (short*) calloc(nrTriangles, sizeof(short)); // Local indices.
      fseek(fp, offIndices, SEEK_SET);
      fread(indices, sizeof(short), nrIndices, fp);     
      fseek(fp, offTriangles, SEEK_SET);
      fread(triangleInd, sizeof(short), nrTriangles, fp);
      
      // Set up geometries.
      Geometry* geometries = (Geometry*) calloc(nrTriangles / 3, sizeof(Geometry));
      for (int x, y, z, i = 0; i < nrTriangles / 3; i ++) {
        geometries[i].symIndices = true;
        x = indices[triangleInd[3*i]];       //| TriangleInd holds index to the index list, 
        y = indices[triangleInd[3*i + 1]];   //| because Blizzard invented a doubled 
        z = indices[triangleInd[3*i + 2]];   //| indirection. Only God knows why ...
        geometries[i].vIdx[0] = x; geometries[i].vIdx[1] = y; geometries[i].vIdx[2] = z;
        geometries[i].nIdx[0] = x; geometries[i].nIdx[1] = y; geometries[i].nIdx[2] = z;
        geometries[i].tIdx[0] = x; geometries[i].tIdx[1] = y; geometries[i].tIdx[2] = z;
      }


      // Set up model and set stream to right position. 
      Model3D* model = new Model3D();            
      fseek(fp, offSubmeshes, SEEK_SET);

      short sVtx, sTri;   // Variables for 16-bit integer read-in (start of vertices / geometries).
      int ptrVtx, ptrTri; // Pointer to the current offset (increased by above value after each iteration).

      // Read all submeshes and build the geosets.
      for (int i = 0; i < nrSubmeshes; i ++) {
        Geoset* geoset = new Geoset();             // Create new geoset.
        fread(&geoset->id, sizeof(int), 1, fp);    // Read geoset ID.
        fread(&sVtx, sizeof(short), 1, fp);        // Start index of vertices.
        fread(&geoset->nrV, sizeof(short), 1, fp); // Amount of vertices.
        fread(&sTri, sizeof(short), 1, fp);        // Geometry start index.
        fread(&geoset->nrG, sizeof(short), 1, fp); // Number of geometries (WARNING: reduce it by 3).
        geoset->nrG /= 3;
        geoset->nrN = geoset->nrV;  //| For every vertex exists also 
        geoset->nrT = geoset->nrV;  //| a normal vector and a texel.

        // Allocate memory for the arrays.
        geoset->vertices   =   (Float3*) calloc(geoset->nrV, sizeof(Float3));
        geoset->normals    =   (Float3*) calloc(geoset->nrN, sizeof(Float3));
        geoset->texVects   =   (Float2*) calloc(geoset->nrT, sizeof(Float2));
        geoset->geometries = (Geometry*) calloc(geoset->nrG, sizeof(Geometry));

        // Copy vector arrays from the global lists to these local arrays.
        memcpy(geoset->vertices,   &vertices[sVtx],     sizeof(Float3) * geoset->nrV);
        memcpy(geoset->normals,    &normals[sVtx],      sizeof(Float3) * geoset->nrN);
        memcpy(geoset->texVects,   &textures[sVtx],     sizeof(Float2) * geoset->nrT);      
        memcpy(geoset->geometries, &geometries[sTri/3], sizeof(Geometry) * geoset->nrG);

        // For the geometries, it gets now a little bit tricky. The global referencing doesn't work anymore!
        for (int geom = 0; geom < geoset->nrG; geom ++) {
          for (int cmp = 0; cmp < 3; cmp ++) {
            geoset->geometries[geom].vIdx[cmp] -= sVtx;  //| Reduce all index
            geoset->geometries[geom].nIdx[cmp] -= sVtx;  //| components by the
            geoset->geometries[geom].tIdx[cmp] -= sVtx;  //| current vertex offset!
          }       
        }


        fseek(fp, 36, SEEK_CUR);  // Skip the bones (until animation is integrated later). 
        printf("[%d]: ID: %d, sVtx: %d, nrV: %d | sTri: %d, nrG: %d\n", i, geoset->id, sVtx, geoset->nrV, sTri, geoset->nrG);

        geoset->textureID = -1;
        model->Geosets.push_back(geoset);        // Write geoset to model.
      }


      //TODO Submeshes, textures and all that stuff. Now, just a test!!

      /*
      Geoset* geoset = new Geoset();
      model->Geosets.push_back(geoset);
      geoset->id = 0;
      geoset->enabled = true;
      geoset->geometries = geometries;
      geoset->nrG = nrTriangles / 3;
      geoset->nrV = geoset->nrN = geoset->nrT = nrVertices;
      geoset->vertices = vertices;
      geoset->normals = normals;
      geoset->texVects = textures;
      geoset->textureID = -1;
      */
      printf(
        "***** %s *****\n"
        "- Vertices : %d @ %d\n"
        "- Indices  : %d @ %d\n"
        "- Triangles: %d @ %d\n"
        "- Submeshes: %d @ %d\n"
        "- Textures : %d @ %d\n",
        modelname, nrVertices, offsetVertices, nrIndices, offIndices, nrTriangles, offTriangles,
        nrSubmeshes, offSubmeshes, nrTextures, offTextures           
      );
      
      //TODO modelname not used!


      // Free temporary arrays and return the model.
      free(indices);
      free(triangleInd);
      free(modelname);
      return model;
    }


  //TODO 3) Testoption einbauen: ALU-Start im Anzeigemodus.
};
