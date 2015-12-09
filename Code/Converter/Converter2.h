#pragma once
#pragma warning(disable: 4996)
#include <Converter/Utils/FileUtils.h>
#include <Data/Primitives.h>
#include <Data/Model.h>
#include <Execution/ALU.h>


/** The second-generation model converter. */
class Converter2 {

private:

  FILE* _fp = NULL;        // File pointer for input stream.
  Model3D* _model = NULL;  // Empty model.
  const char* _savename;   // Path to output file (argument).
  char _scaleAxis = ' ';   // Scaling axis (optional set).
  float _scaleVal = -1.0f; // Scaling value (optional set).


  /** Parse the scaling parameter. */
  void ParseScalingParam(char arg[]) {
    char* splitPtr = strtok(arg, "=");
    splitPtr = strtok(NULL, "=");         // Now we have everything after "scale=".    
    if (strstr(splitPtr, ":") != NULL) {
      splitPtr = strtok(splitPtr, ":");   //| If an axis is supplied, we now have it
      _scaleAxis = splitPtr[0];           //| (it is before ':') and advance once more.    
      splitPtr = strtok(NULL, ":");
    }
    else splitPtr = strtok(splitPtr, ":"); // Otherwise we already are at the value.  
    sscanf(splitPtr, "%f", &_scaleVal);
  }






  public:

    /** Create converter and start file conversion.
     * @param argc Argument counter. Has to be '2'.
     * @param argv Parameter array. Should consist of input and output paths. */
    /*Converter2(int argc, char** argv) {

      // Scaling argument supplied. Read it first, then start conversion.
      if (argc == 3 && (strstr(argv[0], "--scale=") != NULL)) {
        ParseScalingParam(argv[0]);
        _savename = argv[2];
        StartConversion(argv[1]);
      }

      // Only source and target specified.  
      else if (argc == 2) {
        _savename = argv[1];
        StartConversion(argv[0]);
      }
      else {  // Print syntax error.
        printf("[Error] Invalid converter parameter syntax!\n"
          "Usage: ALU.exe --conv <inputfile> <outputfile>\n");
      }
    }



    
    void StartConversion(const char* filename) {
      ConvertObj(filename, true);
      return;




      // File recognition information.
      enum FORMATS { GLF, MDX, OBJ, WGL }; // Attention: Make sure 'fileendings' is equal!
      const char* fileendings[4] = {".glf", ".mdx", ".obj", ".wglmodel"};

      // Detect file format.
      int fileformat = -1;
      for (int i = 0; i < nrFormats; i ++) {
        if (strstr(filename, fileendings[i]) != NULL) {
          fileformat = i;
          break;
        }
      }

      // Print error and return on unrecognized file format.
      if (fileformat == -1) {
        printf("\nError reading file '%s'! Please make sure it has a proper ending.\n"
          "Formats currently supported: .glf, .mdl, .mdx, .obj, .raw, .ms3d\n", filename);
        return;
      }

      // Try to open filestream.
      _fp = fopen(filename, (fileformat < nrBinary) ? "rb" : "r");
      if (_fp == NULL) {
        printf("\nError opening file '%s'!\n", filename);
        return;
      }

      // Get the length of the file and print status output.
      fseek(_fp, 0L, SEEK_END);
      unsigned long bytes = ftell(_fp);
      fseek(_fp, 0L, SEEK_SET);
      //TODO printf("\nOpening file '%s' [%lu bytes].\n", filename, bytes);


      // Execute corresponding converter function.
      switch (fileformat) {
        case GLF: ReadGlf(_fp, _savename);  break;
        case OBJ: ConvertObj(filename, true); return; //TODO ← Hier eingehängt!!
        case MDX: ConvertObj(filename, false); return;
        default: printf("Sorry, converter module is not implemented yet!\n");
      }

      // Close input file stream after conversion.
      fclose(_fp);

      // Model conversion successful. Apply transformatons, then write it to disk.
      if (_model != NULL) {

        // Look out for scaling parameter.
        if (_scaleAxis != ' ' && _scaleVal > 0) _model->ScaleModelToExtent(_scaleAxis, _scaleVal);
        else if (_scaleVal > 0) _model->ScaleModel(_scaleVal);

        if (strstr(_savename, ".json") != NULL) WriteJson(_model); // Save in JSON format.   
        else _model->WriteFile(_savename);                         // Default save as M4.
        delete _model;
      }
    }
    */



    // http://content.gpwiki.org/index.php/OpenGL:Tutorials:Basic_Bones_System

    void ConvertObj(const char* filename, bool isObj) {
      /*
      if (isObj) {
        Model2* model = ReadWgl(filename);
        //WriteJson(model, "test.m4l");
        //ModelUtils::ScaleModel(model, 0.3f);
        ALU alu = ALU();
        alu.TestConvertedModel(model);
        alu.Start();
        delete model;
        return;
      }*/


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
        ModelUtils::ScaleModel(model, 0.1f);
        strcpy(model->Name, modelname); // Set the model name.
        model->Version = 2;             // Set version to '2' (latest).
        //WriteJson(model, "test1.m4l");
        ModelUtils::Save(model, "test.m4");
        model = ModelUtils::Load("test.m4");
      }
      // Close input file stream after conversion.
      fclose(fp);
      //return;

      // Start the runtime environment and display converted model.
      if (model != NULL) {
        ALU alu = ALU();
        alu.TestConvertedModel(model);
        alu.Start();
      }
    }


    /** Read a .glf binary font definition and output data as source code.
     * @param fp Pointer to the opened file.
     * @param savename The name to save the output file at. */
    void ReadGlf(FILE* fp, const char* savename);


    /** Imports a model from an OBJ file.
     * @param fp Pointer to the opened file.
     * @return The imported and index-aligned model. */
    Model2* ReadObj(FILE* fp);


    /** Reads a Blizzard MDX file (WarCraft 3).
     * @param fp Pointer to the opened file stream. 
     * @return The loaded model. No need for index alignment here! */
    Model2* ReadMdx(FILE* fp);


    /** Reads a WGL lump file (TojiCode).
     * @param filepath Path to a .wglvert or .wglmodel to load.
     * @return Loaded model structure. */
    Model2* ReadWgl(const char* filepath);


    /** Writes a model to a JSON file.
     * @param model The model to output.
     * @param savefile Name of the file to write. */
    void WriteJson(Model2* model, const char* savefile);
};
