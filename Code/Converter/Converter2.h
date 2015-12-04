#pragma once
#pragma warning(disable: 4996)
#include <Data/Primitives.h>
#include <Execution/ALU.h>
#include <stdio.h>



/** The second-generation model converter. */
class Converter2 {


  //TODO Move these functions to a static ModelUtils:: class.
  void ScaleModel(Model2* model, float factor) {
    printf("Scaling model by factor '%6.4f'.\n", factor);
    for (uint i = 0; i < model->Vertices.size(); i ++) {
      model->Vertices[i].X *= factor;
      model->Vertices[i].Y *= factor;
      model->Vertices[i].Z *= factor;
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



  void ScaleModelToExtent(Model2* model, char axis, float value) {
    printf("Scaling model to extent '%f' on axis '%c'.\n", value, axis);

    // Parameter sanity check.
    if (value <= 0) {
      printf("Model scaling failed: Value is '%f' (must be greater than 0).\n", value);
      return;
    }
    if (!(axis == 'x' || axis == 'y' || axis == 'z' || axis == 'X' || axis == 'Y' || axis == 'Z')) {
      printf("Model scaling failed: Axis '%c' invalid (supported: x, y, z).\n", axis);
      return;
    }

    // Get minimal and maximal value.
    float minVal, maxVal;
    minVal = maxVal = _getValue(model->Vertices[0], axis);
    for (uint i = 0; i < model->Vertices.size(); i ++) {
      float val = _getValue(model->Vertices[i], axis);
      if (val > maxVal) maxVal = val;
      else if (val < minVal) minVal = val;
    }

    // Calculate maximal extents and perform scaling based on desired value. 
    float factor = value / (maxVal - minVal);
    ScaleModel(model, factor);
  }



  public:

    void ConvertObj(const char* filename, bool isObj) {

      // Try to open filestream.
      FILE* fp = fopen(filename, (isObj)? "r" : "rb");
      if (fp == NULL) {
        printf("\nError opening file '%s'!\n", filename);
        return;
      }

      // Get the length of the file and print status output.
      fseek(fp, 0L, SEEK_END);
      unsigned long bytes = ftell(fp);
      fseek(fp, 0L, SEEK_SET);
      printf("\nOpening file '%s' [%lu bytes].\n", filename, bytes);

      // Split file name from the full save path.
      char temp[20];
      strcpy(temp, filename);
      char* modelname = strtok((char*) temp, "\\/");
      char* probePtr;
      while ((probePtr = strtok(NULL, "\\/")) != NULL) modelname = probePtr;


      Model2* model = NULL;
      
      if (isObj) {
        model = ReadObj(fp);
        strcpy(model->Name, modelname); // Set the model name.
        model->Version = 2;             // Set version to '2' (latest).
        WriteJson(model, "test.m4l");
      }
      else {
        model = ReadMdx(fp);
        ScaleModel(model, 0.03f);
        strcpy(model->Name, modelname); // Set the model name.
        model->Version = 2;             // Set version to '2' (latest).
        WriteJson(model, "test.m4l");
      }
      // Close input file stream after conversion.
      fclose(fp);



      // Start the runtime environment and display converted model.
      if (model != NULL) {
        ALU* alu = new ALU();
        alu->TestConvertedModel(model);
        alu->Start();
        delete(alu);
      }
    }


    /** Imports a model from an OBJ file.
     * @param fp Pointer to the opened file.
     * @return The imported and index-aligned model. */
    Model2* ReadObj(FILE* fp);


    /** Reads a Blizzard MDX file (WarCraft 3).
     * @param fp Pointer to the opened file stream. 
     * @return The loaded model. No need for index alignment here! */
    Model2* ReadMdx(FILE* fp);


    /** Writes a model to a JSON file.
    * @param model The model to output.
    * @param savefile Name of the file to write. */
    void WriteJson(Model2* model, const char* savefile);
};
