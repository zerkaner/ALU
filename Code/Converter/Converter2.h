#pragma once
#pragma warning(disable: 4996)
#include <Data/Primitives.h>
#include <Data/Model.h>
#include <Execution/ALU.h>
#include <stdio.h>



namespace FileUtils {
  
  /** Splits a file string into file name and file ending.
  * @param file The combined file name (input).
  * @param outName File name substring (output 1).
  * @param outEnding File extension (output 2). */
  static void SplitFileEnding(char* file, char* outName, char* outEnding) {
    int length = strlen(file);
    int splitpos = length - 1;
    while (file[splitpos] != '.') splitpos --;
    strncpy(outName, file, splitpos); outName[splitpos] = '\0';
    strncpy(outEnding, &file[splitpos], length - splitpos + 1);
  }


  /** Splits a path string into the two substrings directory and file.
  * @param path The complete path (input).
  * @param outDir Allocated memory for directory substring (output 1).
  * @param outFile String storage for file substring (output 2). */
  static void SplitPath(const char* path, char* outDir, char* outFile) {
    int length = strlen(path);
    int splitpos = length - 1;
    while (!(path[splitpos] == '/' || path[splitpos] == '\\') && splitpos > 0) splitpos --;
    if (splitpos == 0) {     // No directory in path detected. Return empty dir substring.
      strcpy(outDir, "");
      strcpy(outFile, path);
    }
    else {
      strncpy(outDir, path, splitpos); outDir[splitpos] = '\0';  // Add manual string delimiter.
      strncpy(outFile, &path[splitpos + 1], length - splitpos);  // Copy rest of original string.
    }
  }


  /** Gets the value from a JSON line.
   * @param in Char array containing the JSON line. 
   * @param out String with the value (output). In case of number, additional conversion needed. */
  static void JSON_getString(char* in, char* out) {
    if (strstr(in, "\"\"") != NULL) { strcpy(out, ""); return; }  // Check for empty string.
    char* substring = strstr(in, ":");
    int start = 1;
    while (substring[start] == ' ' || substring[start] == '\"') start ++;
    int stop = start + 1;
    while (!(substring[stop]=='\"' || substring[stop]==',' || substring[stop]=='\0')) stop ++;
    int length = stop - start;
    strncpy(out, &substring[start], length); out[length] = '\0';
  }


  static int JSON_getInteger(char* in){
    char val[16]; int i;
    JSON_getString(in, val);
    sscanf(val, "%d", &i);
    return i;
  }

  static float JSON_getFloat(char* in) {
    char val[32]; float f;
    JSON_getString(in, val);
    sscanf(val, "%f", &f);
    return f;
  }

  static bool JSON_getBool(char* in) {
    char val[8];
    JSON_getString(in, val);
    if (strstr(val, "true") != NULL) return true;
    return false;
  }

  static void JSON_getFloatArray(char* in, float* out, int& size) {
    size = 0;
    char* splitPtr = strtok(in, "[");
    for (int i = 0; splitPtr != NULL; i ++, splitPtr = strtok(NULL, ",] ")) {
      if (i == 0) continue;
      sscanf(splitPtr, "%f", &out[size]);
      size ++;
    }
  }
}


/** The second-generation model converter. */
class Converter2 {


  public:

    void ConvertObj(const char* filename, bool isObj) {
      if (isObj) {
        Model2* model = ReadWgl(filename);
        //WriteJson(model, "test.m4l");
        //ModelUtils::ScaleModel(model, 0.3f);
        ALU alu = ALU();
        alu.TestConvertedModel(model);
        alu.Start();
        delete model;
        return;
      }


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
        ModelUtils::ScaleModel(model, 0.3f);
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
