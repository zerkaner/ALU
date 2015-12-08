#include <Converter/Converter2.h>


/** Writes a model to a JSON file.
 * @param model The model to output.
 * @param savefile Name of the file to write. */
void Converter2::WriteJson(Model2* model, const char* savefile) {

  // Try to open writer stream. Quit on failure.
  FILE* writer = fopen(savefile, "w");
  if (writer == NULL) {
    printf("\nError writing to file '%s'!\n", savefile);
    return;
  }


  // Write JSON file head.
  fprintf(writer, "{\n");
  fprintf(writer, "  \"Name\":     \"%s\",\n", model->Name);
  fprintf(writer, "  \"Version\":  %d,\n", model->Version);

  // Vertex output.
  fprintf(writer, "  \"Vertices\": [");
  int nrVtx = model->Vertices.size();
  for (int i = 0; i < nrVtx; i ++) {
    Float3 vertex = model->Vertices[i];
    fprintf(writer, "%f,%f,%f%s", vertex.X, vertex.Y, vertex.Z, (i < nrVtx - 1) ? "," : "");
  } fprintf(writer, "],\n");

  // Normals output.
  fprintf(writer, "  \"Normals\":  [");
  int nrNor = model->Normals.size();
  for (int i = 0; i < nrNor; i ++) {
    Float3 normal = model->Normals[i];
    fprintf(writer, "%f,%f,%f%s", normal.X, normal.Y, normal.Z, (i < nrNor - 1) ? "," : "");
  } fprintf(writer, "], \n");

  // Texture vector output.
  fprintf(writer, "  \"UVs\":      [");
  int nrUVs = model->UVs.size();
  for (int i = 0; i < nrUVs; i ++) {
    Float2 texCoord = model->UVs[i];
    fprintf(writer, "%f,%f%s", texCoord.X, texCoord.Y, (i < nrUVs - 1) ? "," : "");
  } fprintf(writer, "],\n");

  // Triangle indices output.
  fprintf(writer, "  \"Indices\":  [");
  int nrIdx = model->Indices.size();
  for (int i = 0; i < nrIdx; i ++) {
    fprintf(writer, "%d%s", model->Indices[i], (i < nrIdx - 1) ? "," : "");
  } fprintf(writer, "],\n\n\n");



  // Write all meshes and submeshes.
  fprintf(writer, "  \"Meshes\": [");
  int nrMeshes = model->Meshes.size();
  for (int i = 0; i < nrMeshes; i ++) {
    fprintf(writer, "\n    {\n");
    fprintf(writer, "      \"ID\":          \"%s\",\n", model->Meshes[i].ID);
    fprintf(writer, "      \"Texture\":     \"%s\",\n", model->Meshes[i].Texture);
    fprintf(writer, "      \"IndexOffset\": %d,\n", model->Meshes[i].IndexOffset);
    fprintf(writer, "      \"IndexLength\": %d,\n", model->Meshes[i].IndexLength);
    fprintf(writer, "      \"BoneOffset\":  %d,\n", model->Meshes[i].BoneOffset);
    fprintf(writer, "      \"BoneCount\":   %d\n", model->Meshes[i].BoneCount);
    fprintf(writer, "    }%s", (i < nrMeshes-1)? "," : "\n  ");
  } fprintf(writer, "],\n\n\n");



  // Write all bones.
  fprintf(writer, "  \"Bones\": [");
  int nrBones = model->Bones.size();
  for (int i = 0; i < nrBones; i ++) {
    fprintf(writer, "\n    {\n");
    fprintf(writer, "      \"Name\":     \"%s\",\n", model->Bones[i].Name);
    fprintf(writer, "      \"Parent\":   %d,\n", model->Bones[i].Parent);
    fprintf(writer, "      \"Position\": [%f, %f, %f],\n", 
      model->Bones[i].Position.X, 
      model->Bones[i].Position.Y, 
      model->Bones[i].Position.Z);
    fprintf(writer, "      \"Rotation\": [%f, %f, %f, %f],\n", 
      model->Bones[i].Rotation.X,
      model->Bones[i].Rotation.Y,
      model->Bones[i].Rotation.Z,
      model->Bones[i].Rotation.W);

    //TODO Additional bone write-out.

    fprintf(writer, "    }%s", (i < nrBones - 1) ? "," : "\n  ");
  } fprintf(writer, "],\n\n\n");



  // Write the animations.
  fprintf(writer, "  \"Animations\": [");
  int nrAnimations = model->Animations.size();
  for (int i = 0; i < nrAnimations; i ++) {
    fprintf(writer, "\n    {\n");
    //TODO Animation format output.
    fprintf(writer, "    }%s", (i < nrAnimations - 1) ? "," : "\n  ");
  } fprintf(writer, "]\n");



  // Close output file stream.
  fprintf(writer, "}");
  printf("Written %d bytes to file '%s'.\n", ftell(writer), savefile);
  fclose(writer);
}
