#include <Converter/Converter.h>
#include <Converter/Utils/MemoryStream.h>


/** Load a Blizzard M2 file. Called by converter or MPQ reader toolchain.
 * @param m2 Opened memory stream to the M2 file.
 * @return Pointer to the imported model. */
Model2* Converter::ReadM2(MemoryStream* m2) {
  
  Model2* model = new Model2();
  DWORD dbuf;
  m2->Read(&dbuf, sizeof(DWORD));  // Skip "MD20".
  m2->Read(&dbuf, sizeof(DWORD));  // Skip version number (for now).

  int namelength, nameoffset;
  m2->Read(&namelength, sizeof(int));  // Read model name length.
  m2->Read(&nameoffset, sizeof(int));  // Read model name offset.
  
  // Skip next 13 fields (ignored until needed).
  for (int i = 0; i < 13; i ++) m2->Read(&dbuf, sizeof(DWORD));

  int nrVertices, offsetVertices, nrLOD, offsetLOD;
  m2->Read(&nrVertices,     sizeof(int));  // Read number of vertices.
  m2->Read(&offsetVertices, sizeof(int));  // Offset to vertex definitions.
  m2->Read(&nrLOD,          sizeof(int));  // Read number of LOD groups.
  m2->Read(&offsetLOD,      sizeof(int));  // LOD offset.


  // Read model name.
  m2->Seek(MemoryStream::START, nameoffset);
  m2->Read(model->Name, namelength);

  // Advance to vertex definitions. Reserve memory, then start read-in.
  m2->Seek(MemoryStream::START, offsetVertices);
  model->Vertices.reserve(nrVertices);
  model->Normals.reserve(nrVertices);
  model->UVs.reserve(nrVertices);
  float f[8];
  for (int i = 0; i < nrVertices; i ++) {
    m2->Read(&f[0], sizeof(Float3));  // Read position.
    m2->Read(&dbuf, sizeof(DWORD));   // [4x8] Skip bone weights (0 to 255).
    m2->Read(&dbuf, sizeof(DWORD));   // [4x8] Skip bone indices (0 to nBones - 1).
    m2->Read(&f[3], sizeof(Float3));  // Read normal vector.
    m2->Read(&f[6], sizeof(Float2));  // Read texture coordinates.
    model->Vertices.push_back(Float3(f[0], f[1], f[2]));
    model->Normals.push_back(Float3(f[3], f[4], f[5]));
    model->UVs.push_back(Float2(f[6], f[7]));
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

  // Read vertex indices and triangle indices.
  short* indices = (short*) calloc(nrIndices, sizeof(short));       // Global indices.
  short* triangleInd = (short*) calloc(nrTriangles, sizeof(short)); // Local indices.
  m2->Seek(MemoryStream::START, offIndices);
  m2->Read(indices, sizeof(short) * nrIndices);
  m2->Seek(MemoryStream::START, offTriangles);
  m2->Read(triangleInd, sizeof(short) * nrTriangles);

  
  // TriangleInd holds index to the index list, because Blizzard 
  // invented a doubled indirection. Only God knows why ...
  for (int i = 0; i < nrTriangles; i ++) {
    model->Indices.push_back(indices[triangleInd[i]]);
  }


  // Reads all meshes.
  m2->Seek(MemoryStream::START, offSubmeshes);
  short sBuf, iO, iL; int iBuf;
  for (int i = 0; i < nrSubmeshes; i ++) {
    
    Mesh2* mesh = new Mesh2();              // Create new mesh.
    m2->Read(&iBuf, sizeof(int));           // Read ID.
    sprintf(mesh->ID, "Geoset %02d", iBuf); // Set ID.
    strcpy(mesh->Texture, "");

    m2->Read(&sBuf, sizeof(short));  // Start index of vertices.
    m2->Read(&sBuf, sizeof(short));  // Amount of vertices.
    m2->Read(&iO, sizeof(short));    // Geometry start index.
    m2->Read(&iL, sizeof(short));    // Number of geometries.
    mesh->IndexOffset = iO;
    mesh->IndexLength = iL;

    m2->Seek(MemoryStream::CURRENT, 36);  // Skip the bones (until animation is integrated later). 
    mesh->BoneOffset = 0;
    mesh->BoneCount = 0;

    model->Meshes.push_back(*mesh);  // Write geoset to model.
  }



  // Free temporary arrays and return the model.
  free(indices);
  free(triangleInd);
  return model;
}
