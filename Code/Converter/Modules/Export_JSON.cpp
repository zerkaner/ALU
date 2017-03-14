#include <Converter/Converter.h>


/** Writes a model to a JSON file.
 * @param model The model to output.
 * @param savefile Name of the file to write. */
void Converter::WriteJson(Model* model, const char* savefile) {

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
    fprintf(writer, "%lu%s", model->Indices[i], (i < nrIdx - 1) ? "," : "");
  } fprintf(writer, "],\n\n\n");



  // Write all meshes and submeshes.
  fprintf(writer, "  \"Meshes\": [");
  int nrMeshes = model->Meshes.size();
  for (int i = 0; i < nrMeshes; i ++) {
    fprintf(writer, "\n    {\n");
    fprintf(writer, "      \"ID\":          \"%s\",\n", model->Meshes[i].ID);
    fprintf(writer, "      \"Enabled\":     %s,\n", model->Meshes[i].Enabled? "true" : "false");
    fprintf(writer, "      \"Texture\":     \"%s\",\n", model->Meshes[i].Texture);
    fprintf(writer, "      \"IndexOffset\": %lu,\n", model->Meshes[i].IndexOffset);
    fprintf(writer, "      \"IndexLength\": %lu,\n", model->Meshes[i].IndexLength);
    fprintf(writer, "      \"Attached\":    %s\n", model->Meshes[i].Attached? "true" : "false");
    fprintf(writer, "    }%s", (i < nrMeshes-1)? "," : "\n  ");
  } fprintf(writer, "],\n\n\n");



  // Write all bones.
  fprintf(writer, "  \"Bones\": [");
  int nrBones = model->Bones.size();
  for (int i = 0; i < nrBones; i ++) {
    fprintf(writer, "\n    [\"%s\", %d, [%f, %f, %f]]%s",
      model->Bones[i].Name, model->Bones[i].Parent, model->Bones[i].Pivot.X,
      model->Bones[i].Pivot.Y, model->Bones[i].Pivot.Z, (i < nrBones - 1) ? "," : "\n  "
    );
  } fprintf(writer, "],\n\n\n");


  // Write the animations.
  fprintf(writer, "  \"Sequences\": [");
  int nrSequences = model->Sequences.size();
  for (int i = 0; i < nrSequences; i ++) {
    Sequence& seq = model->Sequences[i];
    fprintf(writer, "\n    {\n");
    fprintf(writer, "      \"Name\":   \"%s\",\n", seq.Name);
    fprintf(writer, "      \"ID\":     %d,\n", seq.ID);
    fprintf(writer, "      \"Length\": %d,\n", seq.Length);
    fprintf(writer, "      \"Loop\":   %s,\n", seq.Loop ? "true" : "false");
    fprintf(writer, "      \"Transformations\": [");
    int nrTrans = seq.Transformations.size();
    int curTrans = 0;
    std::map<Bone*, TransformationSet>::iterator iter;
    for (iter = seq.Transformations.begin(); iter != seq.Transformations.end(); iter ++, curTrans ++) {
      std::vector<TransformationDirective>* t = &iter->second.Translations;
      std::vector<TransformationDirective>* r = &iter->second.Rotations;
      std::vector<TransformationDirective>* s = &iter->second.Scalings;
      fprintf(writer, "\n        {\n");
      fprintf(writer, "          \"Bone\":         \"%s\",\n", iter->first->Name);

      fprintf(writer, "          \"Translations\": [");
      for (uint ti = 0; ti < iter->second.Translations.size(); ti ++) {
        TransformationDirective td = iter->second.Translations[ti];
        fprintf(writer, "[%d, [%f, %f, %f]]%s",
          td.Frame, td.X, td.Y, td.Z, (ti < iter->second.Translations.size()-1)? ", ":"");
      } fprintf(writer, "],\n");

      fprintf(writer, "          \"Rotations\":    [");
      for (uint ti = 0; ti < iter->second.Rotations.size(); ti ++) {
        TransformationDirective td = iter->second.Rotations[ti];
        fprintf(writer, "[%d, [%f, %f, %f, %f]]%s",
          td.Frame, td.X, td.Y, td.Z, td.W, (ti < iter->second.Rotations.size() - 1) ? ", " : "");
      } fprintf(writer, "],\n");

      fprintf(writer, "          \"Scalings\":     [");
      for (uint ti = 0; ti < iter->second.Scalings.size(); ti ++) {
        TransformationDirective td = iter->second.Scalings[ti];
        fprintf(writer, "[%d, [%f, %f, %f]]%s",
          td.Frame, td.X, td.Y, td.Z, (ti < iter->second.Scalings.size() - 1) ? ", " : "");
      } fprintf(writer, "]\n");

      fprintf(writer, "        }%s", (curTrans < nrTrans-1)? "," : "\n");
    }
    fprintf(writer, "      ]\n");
    fprintf(writer, "    }%s", (i < nrSequences - 1) ? "," : "\n  ");
  } fprintf(writer, "]\n");



  // Close output file stream.
  fprintf(writer, "}");
  printf("Written %lu bytes to file '%s'.\n", ftell(writer), savefile);
  fclose(writer);
}
