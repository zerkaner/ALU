#pragma once
#pragma warning(disable: 4996)
#include <Data/Primitives.h>
#include <Execution/ALU.h>
#include <stdio.h>
#include <vector>
using namespace std;


/** The second-generation model converter. */
class Converter2 {

  public:

    /** Writes a model to a JSON file.
    * @param model The model to output.
    * @param savefile Name of the file to write. */
    void WriteJson(Model2* model, const char* savefile);

    void ConvertObj(const char* filename) {

      // Try to open filestream.
      FILE* fp = fopen(filename, "r");
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


      Model2* model = ReadObj(fp);
      strcpy(model->Name, modelname); // Set the model name.
      model->Version = 2;             // Set version to '2' (latest).

      WriteJson(model, "test.m4l");

      // Close input file stream after conversion.
      fclose(fp);



      // Start the runtime environment and enter main loop.
      ALU* alu = new ALU();
      alu->TestConvertedModel(model);
      alu->Start();
      delete(alu);
    }


    /** Import a model from an OBJ file.
     * @param fp Pointer to the opened file.
     * @return The imported and index-aligned model. */
    Model2* ReadObj(FILE* fp);
};