#include <Converter/Converter2.h>
#include <map>

/** Utility method to check the beginning of a string. [internal]
 * @param string The string to check.
 * @param prefix The beginning to search for.
 * @return '1', if true, '0' otherwise. */
int startsWith(const char* string, const char* prefix) {
  size_t lenpre = strlen(prefix), lenstr = strlen(string);
  return (lenstr < lenpre) ? 0 : (strncmp(prefix, string, lenpre) == 0);
}


/** Stores vertex, normal and texture indices for a face (.obj has no common indices). */
struct UnevenTriangle {
  DWORD vIdx[3];  // The three vertices ...
  DWORD nIdx[3];  // ... normal vectors ...
  DWORD tIdx[3];  // ... and texture vectors to use.
};


/** Forward-declaration of AlignIndices() function. [internal]
 *  This function performs vector duplication and shifting to create a common index array.
 * @param model The model structure to rectify.
 * @param faces List of above triangle-index specifications. */
void AlignIndices(Model2* model, vector<UnevenTriangle>* faces);


/** Import a model from an OBJ file.
 * @param fp Pointer to the opened file.
 * @return The imported and index-aligned model. */
Model2* Converter2::ReadObj(FILE* fp) {
  printf("OBJ converter loaded.\n");

  char buffer[256];   // Input buffer for file read-in.
  float x, y, z;      // Float input buffers.
  DWORD i1, i2, i3;   // Index input buffers.

  Model2* model = new Model2();   // Create model structure.

  // Temporary face buffer. Because .obj has asymmetric indices, we'll have
  // to align them before they can be written into the model structure.
  vector<UnevenTriangle> triangles;
  Mesh2* curMesh = NULL;  // Currently selected mesh.
  DWORD indicesRead = 0;    // Number of indices that belong to the current mesh.
  DWORD indicesOffset = 0;  // Position of current mesh's first index in total array.  

  // Main read-in loop. Runs over the entire file and evaluates the read line.
  while (fgets(buffer, 256, fp) != NULL) {

    // Vertices: =>  v -1.318927 0.870578 3.264855
    if (startsWith(buffer, "v ")) {
      sscanf(buffer, "v %f %f %f", &x, &y, &z);
      model->Vertices.push_back(Float3(x, y, z));
    }

    // Normal vectors: =>  vn 0.047200 0.994600 -0.092900
    else if (startsWith(buffer, "vn ")) {
      sscanf(buffer, "vn %f %f %f", &x, &y, &z);
      model->Normals.push_back(Float3(x, y, z));
    }

    // Texture coordinates: =>  vt 0.544815 0.024181
    else if (startsWith(buffer, "vt ")) {
      sscanf(buffer, "vt %f %f", &x, &y);
      model->UVs.push_back(Float2(x, y));
    }

    // Faces definition: =>  f 287/323/324 288/324/325 309/345/346
    else if (startsWith(buffer, "f ")) {
      UnevenTriangle* triangle = new UnevenTriangle();

      char* splitPtr = strtok(buffer, " ");   // Split of first segment.   
      for (int i = 0;  splitPtr != NULL;      // While there's something left.
        i ++, splitPtr = strtok(NULL, " ")) { // Advance counter and take next bit.

        // Dereference the vertices, normals and texture crap.
        if (sscanf(splitPtr, "%d/%d/%d", &i1, &i2, &i3) == 3) {  // Vertex, texeland normal.
          triangle->vIdx[i] = i1 - 1;  triangle->tIdx[i] = i2 - 1;  triangle->nIdx[i] = i3 - 1;
        }

        else if (sscanf(splitPtr, "%d//%d", &i1, &i2) == 2) {   // Vertex and normal.
          triangle->vIdx[i] = i1 - 1;  triangle->nIdx[i] = i2 - 1;  triangle->tIdx[i] = 0;
        }

        else if (sscanf(splitPtr, "%d/%d", &i1, &i2) == 2) {    // Vertex and texel.
          triangle->vIdx[i] = i1 - 1;  triangle->nIdx[i] = 0;  triangle->tIdx[i] = i2 - 1;
        }

        else if (sscanf(splitPtr, "%d", &i1) == 1) {           // Vertex only.
          triangle->vIdx[i] = i1 - 1;  triangle->nIdx[i] = 0;  triangle->tIdx[i] = 0;
        }

        else i --;
      }
      triangles.push_back(*triangle);
      indicesRead += 3;
    }

    // Material definition: =>  usemtl ElepEYE
    else if (startsWith(buffer, "usemtl ")) {
      char mtl[32];
      sscanf(buffer, "usemtl %s", &mtl);

      // Save current mesh before we create the new one.
      if (curMesh != NULL) {
        curMesh->Submeshes.push_back(Submesh2());
        curMesh->Submeshes[0].IndexOffset = indicesOffset;
        curMesh->Submeshes[0].IndexLength = indicesRead;
        curMesh->Submeshes[0].BoneCount = 0;  //TODO We will take care of these someday later!
        curMesh->Submeshes[0].BoneOffset = 0;
        model->Meshes.push_back(*curMesh);
      }

      // Create the new mesh.
      curMesh = new Mesh2();
      strcpy(curMesh->ID, mtl);
      strcpy(curMesh->Texture, ""); //TODO Read the texture name from the MTLLIB.

      // Increase index offset and reset current index counter.
      indicesOffset += indicesRead;
      indicesRead = 0;
    }
  }

  // End of file reached. Write the currently open mesh to the structure.
  // If no mesh was declared inside the loop we'll have to be set a default one.
  if (curMesh == NULL) {
    curMesh = new Mesh2();
    strcpy(curMesh->ID, "-default-");
    strcpy(curMesh->Texture, "");
  }

  curMesh->Submeshes.push_back(Submesh2());
  curMesh->Submeshes[0].IndexOffset = indicesOffset;
  curMesh->Submeshes[0].IndexLength = indicesRead;
  curMesh->Submeshes[0].BoneCount = 0;  //TODO We will take care of these someday later!
  curMesh->Submeshes[0].BoneOffset = 0;
  model->Meshes.push_back(*curMesh);


  // Print parser results.
  printf("Parser results:\n"
    " - Meshes    : %d\n"
    " - Vertices  : %d\n"
    " - Normals   : %d\n"
    " - TexCoords : %d\n"
    " - Geometries: %d\n",
    model->Meshes.size(), model->Vertices.size(), model->Normals.size(), model->UVs.size(), triangles.size());

  // Rectify the indices.
  AlignIndices(model, &triangles);

  return model;
}




/** The following two structures belong to the AlignIndices() function
 *  but have to be declared outside to be GCC (C++ 2003) compliant. **/

/** Intermediate structure to associate position-, normal- and texture vector. */
struct Vertex { Float3 position; Float3 normal; Float2 texel; };

/** Indexing structure; corresponds to a face definition. */
struct VertexIndex {
  int v, vn, vt;
  bool operator<(const VertexIndex& other) const {
    char s1[20]; char s2[20];
    sprintf(s1, "%d-%d-%d", v, vn, vt);
    sprintf(s2, "%d-%d-%d", other.v, other.vn, other.vt);
    return (strcmp(s1, s2) < 0);
  }
};


/** AlignIndices() implementation. */
void AlignIndices(Model2* model, vector<UnevenTriangle>* faces) {
  printf("Aligning indices ... ");

  map<VertexIndex, int> indexMapping; // Triangle indices to new (uniform) index mapping.
  vector<Vertex> vertexBuffer;        // Buffer for rearranged vertices (interleaved v/n/t).

  for (uint t = 0; t < faces->size(); t ++) { // Loop over all triangles.
    for (int i = 0; i < 3; i ++) {            // Loops over the three vertices of a triangle.

      VertexIndex indices = VertexIndex();
      indices.v =  (*faces)[t].vIdx[i];
      indices.vn = (*faces)[t].nIdx[i];
      indices.vt = (*faces)[t].tIdx[i];

      int index = -1;

      // If this <v,vn,vt> pair already has an index, we use it!
      if (indexMapping.count(indices)) index = indexMapping[indices];
      
      else { // Otherwise we append a new vertex and index.    
        index = vertexBuffer.size();
        Vertex vtx = Vertex();
        vtx.position = model->Vertices[indices.v];
        vtx.normal = model->Normals[indices.vn];
        vtx.texel = model->UVs[indices.vt];
        vertexBuffer.push_back(vtx);
        indexMapping[indices] = index;
      }

      model->Indices.push_back(index);
    }
  }

  // Clear the old vertex, normal and UV arrays and redimension them.
  uint nrElem = vertexBuffer.size();
  model->Vertices.clear();  model->Vertices.reserve(nrElem);
  model->Normals.clear();   model->Normals.reserve(nrElem);
  model->UVs.clear();       model->UVs.reserve(nrElem);

  // Write new values to the model array.  
  for (uint i = 0; i < nrElem; i ++) {
    model->Vertices.push_back(vertexBuffer[i].position);
    model->Normals.push_back(vertexBuffer[i].normal);
    model->UVs.push_back(vertexBuffer[i].texel);
  }

  printf("[finished]\n"
         " - new V/N/T sizes: %d\n"
         " - new index size : %d\n", nrElem, model->Indices.size());
}
