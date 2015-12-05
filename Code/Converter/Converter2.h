#pragma once
#pragma warning(disable: 4996)
#include <Data/Primitives.h>
#include <Data/Model.h>
#include <Execution/ALU.h>
#include <stdio.h>



/** The second-generation model converter. */
class Converter2 {

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
        ModelUtils::ScaleModel(model, 0.03f);
        strcpy(model->Name, modelname); // Set the model name.
        model->Version = 2;             // Set version to '2' (latest).
        WriteJson(model, "test.m4l");
        ModelUtils::Save(model, "test.m4");
        ModelUtils::Load("test.m4");
      }
      // Close input file stream after conversion.
      fclose(fp);
      return;

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
