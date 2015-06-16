#pragma warning(disable: 4996)
#include <Converter/M2Loader.h>
#include <Data/Primitives.h>
#include <stdlib.h>
#include <string.h>


Model3D* M2Loader::LoadM2(MemoryStream* m2) {
  DWORD dbuf;
  m2->Read(&dbuf, sizeof(DWORD));  // Skip "MD20".
  m2->Read(&dbuf, sizeof(DWORD));  // Skip version number (for now).

  int namelength, nameoffset;
  m2->Read(&namelength, sizeof(int));  // Read model name length.
  m2->Read(&nameoffset, sizeof(int));  // Read model name offset.
  char* modelname = (char*) calloc(namelength, sizeof(char));

  // Skip next 13 fields (ignored until needed).
  for (int i = 0; i < 13; i ++) m2->Read(&dbuf, sizeof(DWORD));

  int nrVertices, offsetVertices, nrLOD, offsetLOD;
  m2->Read(&nrVertices,     sizeof(int)); // Read number of vertices.
  m2->Read(&offsetVertices, sizeof(int)); // Offset to vertex definitions.
  m2->Read(&nrLOD,          sizeof(int)); // Read number of LOD groups.
  m2->Read(&offsetLOD,      sizeof(int)); // LOD offset.


  // Read model name.
  m2->Seek(MemoryStream::START, nameoffset);
  m2->Read(modelname, sizeof(char) * namelength);

  // Advance to vertex definitions. Reserve memory, then start read-in.
  m2->Seek(MemoryStream::START, offsetVertices);
  Float3* vertices = (Float3*) calloc(nrVertices, sizeof(Float3));
  Float3* normals  = (Float3*) calloc(nrVertices, sizeof(Float3));
  Float2* textures = (Float2*) calloc(nrVertices, sizeof(Float2));
  for (int i = 0; i < nrVertices; i ++) {
    m2->Read(&vertices[i], sizeof(float) * 3); // Read position.
    m2->Read(&dbuf,        sizeof(DWORD));     // [4x8] Skip bone weights (0 to 255).
    m2->Read(&dbuf,        sizeof(DWORD));     // [4x8] Skip bone indices (0 to nBones - 1).
    m2->Read(&normals[i],  sizeof(float) * 3); // Read normal vector.
    m2->Read(&textures[i], sizeof(float) * 2); // Read texture coordinates.
    m2->Seek(MemoryStream::CURRENT, 2*sizeof(float)); // Skip two (useless) floats.
  }

  // Read LOD definitions. Four headers with 0x2C bytes each, then follows the data block.
  // Because these definitions seems to be identical, we just take the first one.
  int nrIndices, nrTriangles, nrProperties, nrSubmeshes, nrTextures;
  int offIndices, offTriangles, offProperties, offSubmeshes, offTextures;
  m2->Read(&nrIndices,     sizeof(int));  // [0x00]: Number of elements in the index list.
  m2->Read(&offIndices,    sizeof(int));  // [0x04]: Offset to the index list.
  m2->Read(&nrTriangles,   sizeof(int));  // [0x08]: Number of elements in the triangle list.
  m2->Read(&offTriangles,  sizeof(int));  // [0x0C]: Offset to the triangle list.
  m2->Read(&nrProperties,  sizeof(int));  // [0x10]: Number of elements in the vertex property list.
  m2->Read(&offProperties, sizeof(int));  // [0x14]: Offset to the vertex property list.
  m2->Read(&nrSubmeshes,   sizeof(int));  // [0x18]: Number of elements in the submesh list.
  m2->Read(&offSubmeshes,  sizeof(int));  // [0x1C]: Offset to the submesh list.
  m2->Read(&nrTextures,    sizeof(int));  // [0x20]: Number of elements in the texture list.
  m2->Read(&offTextures,   sizeof(int));  // [0x24]: Offset to the texture list.
  m2->Seek(MemoryStream::CURRENT, sizeof(float)); // [0x28]: Skip a float (LOD distance or whatever).

  // Read vertex and triangle indices.
  short* indices = (short*) calloc(nrIndices, sizeof(short));       // Global indices.
  short* triangleInd = (short*) calloc(nrTriangles, sizeof(short)); // Local indices.
  m2->Seek(MemoryStream::START, offIndices);
  m2->Read(indices, sizeof(short) * nrIndices);
  m2->Seek(MemoryStream::START, offTriangles);
  m2->Read(triangleInd, sizeof(short) * nrTriangles);

  // Set up geometries.
  Geometry* geometries = (Geometry*) calloc(nrTriangles / 3, sizeof(Geometry));
  for (int x, y, z, i = 0; i < nrTriangles / 3; i ++) {
    geometries[i].symIndices = true;
    x = indices[triangleInd[3 * i]];       //| TriangleInd holds index to the index list, 
    y = indices[triangleInd[3 * i + 1]];   //| because Blizzard invented a doubled 
    z = indices[triangleInd[3 * i + 2]];   //| indirection. Only God knows why ...
    geometries[i].vIdx[0] = x; geometries[i].vIdx[1] = y; geometries[i].vIdx[2] = z;
    geometries[i].nIdx[0] = x; geometries[i].nIdx[1] = y; geometries[i].nIdx[2] = z;
    geometries[i].tIdx[0] = x; geometries[i].tIdx[1] = y; geometries[i].tIdx[2] = z;
  }


  // Set up model and set stream to right position. 
  Model3D* model = new Model3D();
  m2->Seek(MemoryStream::START, offSubmeshes);
  short sVtx, sTri;   // Variables for 16-bit integer read-in (start of vertices / geometries).

  // Read all submeshes and build the geosets.
  for (int i = 0; i < nrSubmeshes; i ++) {
    Geoset* geoset = new Geoset();         // Create new geoset.
    m2->Read(&geoset->id,  sizeof(int));   // Read geoset ID.
    m2->Read(&sVtx,        sizeof(short)); // Start index of vertices.
    m2->Read(&geoset->nrV, sizeof(short)); // Amount of vertices.
    m2->Read(&sTri,        sizeof(short)); // Geometry start index.
    m2->Read(&geoset->nrG, sizeof(short)); // Number of geometries (WARNING: reduce it by 3).
    geoset->nrG /= 3;
    geoset->nrN = geoset->nrV;  //| For every vertex exists also 
    geoset->nrT = geoset->nrV;  //| a normal vector and a texel.

    // Allocate memory for the arrays.
    geoset->vertices = (Float3*) calloc(geoset->nrV, sizeof(Float3));
    geoset->normals  = (Float3*) calloc(geoset->nrN, sizeof(Float3));
    geoset->texVects = (Float2*) calloc(geoset->nrT, sizeof(Float2));
    geoset->geometries = (Geometry*) calloc(geoset->nrG, sizeof(Geometry));

    // Copy vector arrays from the global lists to these local arrays.
    memcpy(geoset->vertices, &vertices[sVtx], sizeof(Float3) * geoset->nrV);
    memcpy(geoset->normals, &normals[sVtx], sizeof(Float3) * geoset->nrN);
    memcpy(geoset->texVects, &textures[sVtx], sizeof(Float2) * geoset->nrT);
    memcpy(geoset->geometries, &geometries[sTri / 3], sizeof(Geometry) * geoset->nrG);

    // For the geometries, it gets now a little bit tricky. The global referencing doesn't work anymore!
    for (int geom = 0; geom < geoset->nrG; geom ++) {
      for (int cmp = 0; cmp < 3; cmp ++) {
        geoset->geometries[geom].vIdx[cmp] -= sVtx;  //| Reduce all index
        geoset->geometries[geom].nIdx[cmp] -= sVtx;  //| components by the
        geoset->geometries[geom].tIdx[cmp] -= sVtx;  //| current vertex offset!
      }
    }

    m2->Seek(MemoryStream::CURRENT, 36);  // Skip the bones (until animation is integrated later). 
    //printf("[%02d]: ID: %4d, sVtx: %4d, nrV: %3d | sTri: %d, nrG: %d\n",
    //  i, geoset->id, sVtx, geoset->nrV, sTri, geoset->nrG);

    geoset->textureID = -1;
    model->Geosets.push_back(geoset);  // Write geoset to model.
  }


  // Free temporary arrays and return the model.
  free(indices);
  free(triangleInd);
  free(modelname);
  return model;
}
