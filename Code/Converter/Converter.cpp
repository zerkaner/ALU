#include "Converter.h"
#include <Converter/Utils/MemoryStream.h>
#include <Execution/ALU.h>


/** String contains shortcut. */
bool Contains(const char* string, const char* word) {
  return (strstr(string, word) != NULL);
}

/** Structure for parsed parameters. */
struct ConverterParams {
  char inputfile[256];   // Input file path.
  char outputfile[256];  // Output file.
  bool scaling = false;  // Model scaling requested?
  char scaleAxis = ' ';  // Axis to scale on.
  float scaleVal;        // Scaling value.
  bool display = false;  // Display model after conversion?
};


/** Parse the supplied parameters. */
ConverterParams ParseParameters(int argc, char** argv) {
  ConverterParams params = ConverterParams();
  strcpy(params.inputfile,  argv[argc-2]);  // Input file is the forelast parameter.
  strcpy(params.outputfile, argv[argc-1]);  // Output file is always last.
  for (int i = 0; i < argc - 2; i ++) {

    // Scaling argument.
    if (Contains(argv[i], "--scale")) {
      params.scaling = true;
      char* splitPtr = strtok(argv[i], "=");
      splitPtr = strtok(NULL, "=");       // Now we have everything after "--scale=".    
      if (strstr(splitPtr, ":") != NULL) {
        splitPtr = strtok(splitPtr, ":"); //| If an axis is supplied, we now have it
        params.scaleAxis = splitPtr[0];   //| (it is before ':') and advance once more.    
        splitPtr = strtok(NULL, ":");
      }
      else splitPtr = strtok(splitPtr, ":"); // Otherwise we already are at the value.  
      sscanf(splitPtr, "%f", &params.scaleVal);
    }

    // Preview argument.
    else if (Contains(argv[i], "--display")) {
      params.display = true;
    }
  }  
  return params;
}



/** Create converter and start file conversion.
 * @param argc Argument counter.
 * @param argv The in- and output files and additional conversion parameters. */
Converter::Converter(int argc, char** argv) {
  if (argc < 2) {
    printf("Print usage...\n");
    return;
  }
  
  ConverterParams params = ParseParameters(argc, argv);
  Model2* model = LoadFile(params.inputfile); // Load/import the model.
  if (model != NULL) {

    // Perform additional conversions.
    if (params.scaling) {  // Scaling operation.
      if (params.scaleAxis == ' ') ModelUtils::ScaleModel(model, params.scaleVal);
      else ModelUtils::ScaleModelToExtent(model, params.scaleAxis, params.scaleVal);
    }

    // Write output file.
    WriteFile(model, params.outputfile);

    // Display the converted model.
    if (params.display) {
      ALU alu = ALU();
      alu.TestConvertedModel(model);
      alu.Start();
    }

    // Free model-allocated memory before exit.
    delete model;
  }
}


/** Loads a file. If necessary, this method launches an import module.
 * @param inputfile Path to the model to load.
 * @return Pointer to the loaded model. 'NULL' on error. */
Model2* Converter::LoadFile(const char* inputfile) {  
  enum FORMATS { GLF, M2, M4, MDX, OBJ, WGL };
  const char* fileendings[6] = {".glf", ".m2", ".m4", ".mdx", ".obj", ".wglmodel"};

  // Detect file format.
  int fileformat = -1;
  for (int i = 0; i < 6; i ++) {
    if (Contains(inputfile, fileendings[i])) { fileformat = i; break; }
  }

  // Execute corresponding converter function.
  switch (fileformat) {
    case GLF: ReadGlf(inputfile, "font_export.h"); return NULL;
    case OBJ: return ReadObj(inputfile);
    case MDX: return ReadMdx(inputfile);
    case WGL: return ReadWgl(inputfile);
    case M4:  return ModelUtils::Load(inputfile); // Ahh, it's a home match... :-)
    
    // M2 is a little bit more complicated. Wrap input file in a memory stream first!
    case M2:
      MemoryStream* stream = new MemoryStream(inputfile);
      Model2* model = ReadM2(stream);
      delete stream;
      return model;
  }

  // No case matched. Seems we have -1 here.
  printf("Error reading file '%s'! Please make sure it has a proper ending.\n"
         "Formats currently supported: .glf, .m2, .mdx, .obj, .wglmodel\n", inputfile);
  return NULL;
}


/** Write a model to disk. Also used to export models to different formats.
 * @param model The model to save / export.
 * @param savefile The path and file to save to. */
void Converter::WriteFile(Model2* model, const char* savefile) {
  if (Contains(savefile, ".json") || Contains(savefile, ".m4l")) WriteJson(model, savefile);
  else if (Contains(savefile, ".m4")) ModelUtils::Save(model, savefile);
  else printf("Error saving model to file '%s'. Format not recognized!\n", savefile);
}
