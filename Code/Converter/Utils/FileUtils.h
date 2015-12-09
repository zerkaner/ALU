#pragma once
#include <stdio.h>

/** Static class with various helper functions for file processing. */
class FileUtils {
 
  public:

    /** Opens a file and print status output.
     * @param file The file path to open.
     * @param binary Flag, whether to open as binary or as text.
     * @return File pointer or 'null' on error. */
    static FILE* File_open(char* file, bool binary);


    /** Splits a file string into file name and file ending.
     * @param file The combined file name (input).
     * @param outName File name substring (output 1).
     * @param outEnding File extension (output 2). */
    static void File_splitEnding(char* file, char* outName, char* outEnding);


    /** Splits a path string into the two substrings directory and file.
     * @param path The complete path (input).
     * @param outDir Allocated memory for directory substring (output 1).
     * @param outFile String storage for file substring (output 2). */
    static void File_splitPath(const char* path, char* outDir, char* outFile);


    /** Gets the value from a JSON line.
     * @param in Char array containing the JSON line. 
     * @param out String with the value (output). In case of number, additional conversion needed. */
    static void JSON_getString(char* in, char* out);


    /** Parses a JSON line value as integer.
     * @param in The input line. 
     * @return The parsed integer. */
    static int JSON_getInteger(char* in);


    /** Parses a JSON line as float value.
     * @param in The input line.
     * @return The float result. */
    static float JSON_getFloat(char* in);


    /** Parses a JSON value as boolean.
     * @param in The input line.
     * @return Boolean value. */
    static bool JSON_getBool(char* in);


    /** Parses a JSON value as array of floats.
     * @param in The input line. 
     * @param out Allocated float array for value output. 
     * @param size Address of integer to store number of value entries. */
    static void JSON_getFloatArray(char* in, float* out, int& size);
};
