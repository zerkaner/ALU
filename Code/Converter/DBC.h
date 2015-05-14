#pragma once
#pragma warning(disable: 4996)
#include <Data/Primitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/** Parser class for deferencing Blizzards DBC (database cache) files. */
class DBCParser {

  private:
    const char* _dbcInfo  = "C:/Users/Jan Dalski/Downloads/Warcraft 3 Mod-Tools/DBC/CreatureDisplayInfo.dbc";
    const char* _dbcExtra = "C:/Users/Jan Dalski/Downloads/Warcraft 3 Mod-Tools/DBC/CreatureDisplayInfoExtra.dbc";
    const char* _dbcModel = "C:/Users/Jan Dalski/Downloads/Warcraft 3 Mod-Tools/DBC/CreatureModelData.dbc";

  public:
    
    DBCParser(int argc, char** argv) {
      Test();
    }


    struct DBCEntry {
      int columns, size, stringCount;
      DWORD* content;
      char** strings;
    };


    void Test() {

      int id = 43;
      int strs[3] = {2};
      DBCEntry* entry = ReadDataFromDBC(_dbcModel, id, 0, 1, strs);

      if (entry != NULL) {
        printf("Gefunden! Cols: %d, Size total: %d\n", entry->columns, entry->size);
        for (int j = 0; j < entry->columns; j ++) {
          if (j > 0) printf(" | ");
          if      (j == 2) printf("%s", entry->strings[0]);
          //else if (j == 20) printf("%s", entry->strings[1]);
          //else if (j == 8) printf("%s", entry->strings[2]);
          else           printf("%lu", entry->content[j]);
        }
        printf("\n");
      }

      else printf("Dat gibt's hier nicht!\n");
      getchar();
    }


    /** Reads a single data row from a DBC file and returns it with its properties.
     * @param filepath Path to the DBC file to load. 
     * @param key The key of the row to search for.
     * @param keyCol The column where the given key shall be found.
     * @param resolveStr Number of strings to resolve (use '0' to disable).
     * @param strPos Integer array with the column indices of the strings to resolve ('null', if none). 
     * @return The content of the data row (with the associated properties). */
    DBCEntry* ReadDataFromDBC(const char* filepath, int key, int keyCol, int resolveStr, int* strPos) {
      DBCEntry* result = NULL;

      // Try to open filestream.
      FILE* fp = fopen(filepath, "rb");
      if (fp == NULL) {
        printf("\nError opening file '%s'!\n", filepath);
        return NULL;
      }

      // Stores file header information.
      struct DBCHeader { DWORD rows, columns, size, strSize; };

      DWORD dbuf;
      DBCHeader dbcHeader;
      fread(&dbuf, sizeof(DWORD), 1, fp);           // Skip 'WDBC'.
      fread(&dbcHeader, sizeof(DBCHeader), 1, fp);  // Read file header.

      if (dbcHeader.size / 4 != dbcHeader.columns) printf("Warning: Field size != 4!\n");
      DWORD* content = (DWORD*) calloc(dbcHeader.columns, sizeof(DWORD));
      bool found = false;

      // Loop over all entries until found.
      for (unsigned int i = 0; i < dbcHeader.rows; i ++) {
        fread(content, sizeof(DWORD), dbcHeader.columns, fp);
        if (content[keyCol] == key) {
          found = true;
          break;
        }
      }

      // Data row was successfully retrieved. Copy data to return structure.
      if (found) {
        result = new DBCEntry();
        result->columns = dbcHeader.columns;  // Columns of data row.
        result->size = dbcHeader.size;        // Overall data row size.
        result->content = content;            // Data payload as 32-bit integers.

        // Resolve strings, if asked for. First of all, move the file pointer accordingly.
        if (resolveStr > 0) {
          long offset = 5 * sizeof(DWORD) + dbcHeader.rows * dbcHeader.size;
          fseek(fp, offset, SEEK_SET);

          // We need multiple iterations, so we copy the file content to speed things up.
          char* text = (char*) calloc(dbcHeader.strSize, sizeof(char));
          fread(text, sizeof(char), dbcHeader.strSize, fp);

          // Setup string storage in result structure.
          result->strings = (char**) calloc(resolveStr, sizeof(char*));
          result->stringCount = resolveStr;

          // Loop over all string requests. 
          for (int strRequest = 0; strRequest < resolveStr; strRequest ++) { 
            int start = result->content[strPos[strRequest]];  // This is the index of the searched string!
            int length = 1; 
            while (text[start + (length - 1)] != '\0') length ++;               // Determine string length.
            result->strings[strRequest] = (char*) calloc(length, sizeof(char)); // Reserve memory.
            strcpy(result->strings[strRequest], &text[start]);                  // Copy over string.
          }

          free(text);
        }
      }
      
      fclose(fp);     // Close file pointer.
      return result;  // Return the results.
    }
};
