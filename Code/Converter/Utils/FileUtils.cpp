#pragma warning(disable: 4996)
#include "FileUtils.h"
#include <string.h>


/** Opens a file and print status output.
 * @param file The file path to open.
 * @param binary Flag, whether to open as binary or as text.
 * @return File pointer or 'null' on error. */
FILE* FileUtils::File_open(char* file, bool binary) {
  FILE* fp = fopen(file, (binary) ? "rb" : "r");
  if (fp == NULL) {
    printf("Error opening file '%s'!\n", file);
    return NULL;
  }
  fseek(fp, 0L, SEEK_END);
  unsigned long bytes = ftell(fp);
  fseek(fp, 0L, SEEK_SET);
  printf("Opening file '%s' [%lu bytes].\n", file, bytes);
  return fp;
}


/** Splits a file string into file name and file ending.
 * @param file The combined file name (input).
 * @param outName File name substring (output 1).
 * @param outEnding File extension (output 2). */
void FileUtils::File_splitEnding(char* file, char* outName, char* outEnding) {
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
void FileUtils::File_splitPath(const char* path, char* outDir, char* outFile) {
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
void FileUtils::JSON_getString(char* in, char* out) {
  if (strstr(in, "\"\"") != NULL) { strcpy(out, ""); return; }  // Check for empty string.
  char* substring = strstr(in, ":");
  int start = 1;
  while (substring[start] == ' ' || substring[start] == '\"') start ++;
  int stop = start + 1;
  while (!(substring[stop] == '\"' || substring[stop] == ',' || substring[stop] == '\0')) stop ++;
  int length = stop - start;
  strncpy(out, &substring[start], length); out[length] = '\0';
}


/** Parses a JSON line value as integer.
 * @param in The input line.
 * @return The parsed integer. */
int FileUtils::JSON_getInteger(char* in) {
  char val[16]; int i;
  JSON_getString(in, val);
  sscanf(val, "%d", &i);
  return i;
}


/** Parses a JSON line as float value.
 * @param in The input line.
 * @return The float result. */
float FileUtils::JSON_getFloat(char* in) {
  char val[32]; float f;
  JSON_getString(in, val);
  sscanf(val, "%f", &f);
  return f;
}


/** Parses a JSON value as boolean.
 * @param in The input line.
 * @return Boolean value. */
bool FileUtils::JSON_getBool(char* in) {
  char val[8];
  JSON_getString(in, val);
  if (strstr(val, "true") != NULL) return true;
  return false;
}


/** Parses a JSON value as array of floats.
 * @param in The input line.
 * @param out Allocated float array for value output.
 * @param size Address of integer to store number of value entries. */
void FileUtils::JSON_getFloatArray(char* in, float* out, int& size) {
  size = 0;
  char* splitPtr = strtok(in, "[");
  for (int i = 0; splitPtr != NULL; i ++, splitPtr = strtok(NULL, ",] ")) {
    if (i == 0) continue;
    sscanf(splitPtr, "%f", &out[size]);
    size ++;
  }
}
