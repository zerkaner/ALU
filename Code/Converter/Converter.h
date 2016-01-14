#pragma once
#pragma warning(disable: 4996)
#include <Converter/Utils/FileUtils.h> // Contains <stdio>, used by all modules. 
#include <Data/Primitives.h>
#include <Data/Model.h>
class MemoryStream;


/** The second-generation model converter. */
class Converter {

  private:

    /** Read a .glf binary font definition and output data as source code.
     * @param inputfile The file to load.
     * @param savename The name to save the output file at. */
    static void ReadGlf(const char* inputfile, const char* savename);


    /** Imports a model from an OBJ file.
     * @param inputfile The file to load.
     * @return The imported and index-aligned model. */
    static Model* ReadObj(const char* inputfile);


    /** Reads a Blizzard MDX file (WarCraft 3).
     * @param inputfile The file to load. 
     * @return The loaded model. No need for index alignment here! */
    static Model* ReadMdx(const char* inputfile);


    /** Writes a model to a JSON file.
     * @param model The model to output.
     * @param savefile Name of the file to write. */
    static void WriteJson(Model* model, const char* savefile);




  public:

    /** External start: Create converter and perform file conversion.
     * @param argc Argument counter.
     * @param argv The in- and output files and additional conversion parameters. */
    Converter(int argc, char** argv);


    /** Loads a file. If necessary, this method launches an import module.
     * @param inputfile Path to the model to load.
     * @return Pointer to the loaded model. 'NULL' on error. */
    static Model* LoadFile(const char* inputfile);


    /** Write a model to disk. Also used to export models to different formats.
     * @param model The model to save / export. 
     * @param savefile The path and file to save to. */
    static void WriteFile(Model* model, const char* savefile);


    /** Load a Blizzard M2 file. This function is public to be also called
     *  by the MPQ reader toolchain (World of Warcraft NPC import). 
     * @param stream Opened memory stream to the M2 file.
     * @return Pointer to the imported model. */
    static Model* ReadM2(MemoryStream* stream);
};
