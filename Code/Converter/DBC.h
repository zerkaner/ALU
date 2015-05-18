#pragma once
#pragma warning(disable: 4996)
#include <Data/Primitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>


namespace DBCFields {
  enum InfoDBC  { INFO_MODEL = 1, INFO_EXTRA = 3, INFO_TEX1 = 6, INFO_TEX2, INFO_TEX3 };
  enum ModelDBC { MODEL_PATH = 2 };
  enum DataDBC  { DATA_RACE=1, DATA_SEX, DATA_SKIN, DATA_FACE, DATA_HAIR, DATA_HAIRCOL, DATA_BEARD, // NPC body.
                  DATA_HELM, DATA_SHOULDER, DATA_SHIRT, DATA_CUIRASS, DATA_BELT, DATA_LEGS,         //| Equipment
                  DATA_BOOTS, DATA_WRIST, DATA_GLOVES, DATA_TABARD, DATA_CAPE, DATA_TEX=20 };       //| configuration.

  /** Result structure for a single DBC query. */
  struct DBCEntry {
    int columns, size, stringCount;
    DWORD* content;
    char** strings;
  };

  /** Model data container (assembled by queries over multiple DBCs). */
  struct ModelData {
    int id;
    char* model;
    std::vector<char*> textures;
    bool configured = false;   // If set to 'true', the following fields will be used: 
    int race;                  // Model race.
    int sex;                   // 1: female, 0: male
    int body[5];               // Model configuration (skin, face, hair, haircolor, beard).
    int equip[11];             // Equipment configuration (reference IDs).
    std::vector<int> geosets;  // Geosets to render (for common character models). 
  };
};

using namespace DBCFields;


/** Parser class for deferencing Blizzards DBC (database cache) files. */
class DBCParser {

  private:
    const char* _dbcInfo  = "C:/Users/Jan Dalski/Downloads/Warcraft 3 Mod-Tools/DBC/CreatureDisplayInfo.dbc";
    const char* _dbcItems = "C:/Users/Jan Dalski/Downloads/Warcraft 3 Mod-Tools/DBC/ItemDisplayInfo.dbc";
    const char* _dbcExtra = "C:/Users/Jan Dalski/Downloads/Warcraft 3 Mod-Tools/DBC/CreatureDisplayInfoExtra.dbc";
    const char* _dbcModel = "C:/Users/Jan Dalski/Downloads/Warcraft 3 Mod-Tools/DBC/CreatureModelData.dbc";
    const char* _dbcHair  = "C:/Users/Jan Dalski/Downloads/Warcraft 3 Mod-Tools/DBC/CharHairGeosets.dbc";

  public:
    
    DBCParser(int argc, char** argv) {
      Test();
    }





    void Test() {
      ModelData* modeldata = FetchNPCInformation(7274);

      if (modeldata != NULL) {
        printf("Parser debug report:\n");
        printf(" - ID       : %d\n", modeldata->id);
        printf(" - Modelpath: '%s'\n", modeldata->model);
        if (modeldata->configured) {
          printf(" - Race     : %d\n", modeldata->race);
          printf(" - Sex      : %d\n", modeldata->sex);
          printf(" - Skin     : %d\n", modeldata->body[0]);
          printf(" - Face     : %d\n", modeldata->body[1]);
          printf(" - Hairstyle: %d\n", modeldata->body[2]);
          printf(" - Haircolor: %d\n", modeldata->body[3]);
          printf(" - Beard    : %d\n", modeldata->body[4]);
          printf("-------------------\n");
          printf(" - Helmet   : %d\n", modeldata->equip[0]);
          printf(" - Shoulder : %d\n", modeldata->equip[1]);
          printf(" - Shirt    : %d\n", modeldata->equip[2]);
          printf(" - Chest    : %d\n", modeldata->equip[3]);
          printf(" - Belt     : %d\n", modeldata->equip[4]);
          printf(" - Legs     : %d\n", modeldata->equip[5]);
          printf(" - Boots    : %d\n", modeldata->equip[6]);
          printf(" - Wrist    : %d\n", modeldata->equip[7]);
          printf(" - Gloves   : %d\n", modeldata->equip[8]);
          printf(" - Tabard   : %d\n", modeldata->equip[9]);
          printf(" - Cape     : %d\n", modeldata->equip[10]);
          printf("-------------------\n");
          printf(" - Geosets  : ");
          for (unsigned int i = 0; i < modeldata->geosets.size(); i ++) {
            if (i != 0) printf(", ");
            printf("'%d'", modeldata->geosets[i]);
          }
          printf("\n");
        }

        printf(" - Textures : ");
        for (unsigned int i = 0; i < modeldata->textures.size(); i ++) {
          if (i != 0) printf(", ");
          printf("'%s'", modeldata->textures[i]);
        }
        printf("\n");
      }

      else printf("Dat gibt's hier nicht!\n");
      getchar();
    }



    /** Retrieves data for a creature (NPC).
     * @param id The ID of the creature to fetch. 
     * @return A model data structure with all information for M2 processing. */
    ModelData* FetchNPCInformation(int id) {

      // Try to read base model information.
      int textures[3] = {INFO_TEX1, INFO_TEX2, INFO_TEX3};
      DBCEntry* baseinfo = ReadDataFromDBC(_dbcInfo, id, 3, textures);
      if (baseinfo == NULL) {
        printf("[DBCParser] Error, creature '%d' does not exist!\n", id);
        return NULL;
      }

      // Create model data structure, set ID and textures.
      ModelData* modeldata = new ModelData();
      modeldata->id = id;
      for (int i = 0; i < 3; i ++) {
        if (baseinfo->strings[i][0] != '\0') {
          modeldata->textures.push_back(baseinfo->strings[i]);
        }
      }

      // Query model path.
      int modelID = baseinfo->content[INFO_MODEL];
      int modelPathCol[1] = {MODEL_PATH};
      DBCEntry* model = ReadDataFromDBC(_dbcModel, modelID, 1, modelPathCol);
      modeldata->model = model->strings[0];


      // Get additional data, such as geosets to load and equipment.
      int dataID = baseinfo->content[INFO_EXTRA];
      if (dataID != 0) {
        int texturePathCol[1] = {DATA_TEX};
        DBCEntry* data = ReadDataFromDBC(_dbcExtra, dataID, 1, texturePathCol);
        if (data != NULL) {
          modeldata->configured = true;
          modeldata->race = data->content[DATA_RACE];
          modeldata->sex  = data->content[DATA_SEX];
          modeldata->body[0] = data->content[DATA_SKIN];
          modeldata->body[1] = data->content[DATA_FACE];
          modeldata->body[2] = data->content[DATA_HAIR];
          modeldata->body[3] = data->content[DATA_HAIRCOL];
          modeldata->body[4] = data->content[DATA_BEARD];
          modeldata->equip[0] = data->content[DATA_HELM];
          modeldata->equip[1] = data->content[DATA_SHOULDER];
          modeldata->equip[2] = data->content[DATA_SHIRT];
          modeldata->equip[3] = data->content[DATA_CUIRASS];
          modeldata->equip[4] = data->content[DATA_BELT];
          modeldata->equip[5] = data->content[DATA_LEGS];
          modeldata->equip[6] = data->content[DATA_BOOTS];
          modeldata->equip[7] = data->content[DATA_WRIST];
          modeldata->equip[8] = data->content[DATA_GLOVES];
          modeldata->equip[9] = data->content[DATA_TABARD];
          modeldata->equip[10] = data->content[DATA_CAPE];

          // Load hair style geosets.
          int hairKeyCols[3] = {1, 2, 3};  //| Three keys: Race, sex and style type.
          int haitKeyVals[3] = {modeldata->race, modeldata->sex, modeldata->body[2]};
          DBCEntry* hair = ReadDataFromDBC(_dbcHair, 3, hairKeyCols, haitKeyVals, 0, NULL);
          if (hair != NULL) {
            modeldata->geosets.push_back(hair->content[4]);
            delete hair;
          }


          // Items in Schleife laden.
          // Für jedes Item != 0, Modell und Geoset-IDs auslesen.


          //TODO What about the other geosets?
          // Face (?), armor, ...

          // Load specific NPC textures.
          if (data->strings[0][0] != '\0') modeldata->textures.push_back(data->strings[0]);
        }
        else printf("[DBCParser] Error loading additional data (%d) for model %d!\n", dataID, id);
        delete data;
      }


      // Clear query structures.
      delete baseinfo;
      delete model;
      return modeldata;
    }


    /** Reads a single data row from a DBC file and returns it with its properties.
    * @param filepath Path to the DBC file to load.
    * @param key The key of the row to search for.
    * @param resolveStr Number of strings to resolve (use '0' to disable).
    * @param strPos Integer array with the column indices of the strings to resolve ('null', if none).
    * @return The content of the data row (with the associated properties). */
    DBCEntry* ReadDataFromDBC(const char* filepath, int key, int resolveStr, int* strPos) {
      int cols[1] = {0};
      int vals[1] = {key};
      return ReadDataFromDBC(filepath, 1, cols, vals, resolveStr, strPos);
    }


    /** Reads a single data row from a DBC file and returns it with its properties.
     * @param filepath Path to the DBC file to load. 
     * @param keys The number of keys (array sizes).
     * @param cols Key columns.
     * @param vals Key values.
     * @param resolveStr Number of strings to resolve (use '0' to disable).
     * @param strPos Integer array with the column indices of the strings to resolve ('null', if none). 
     * @return The content of the data row (with the associated properties). */
    DBCEntry* ReadDataFromDBC(const char* filepath, int keys, int* cols, int* vals, int resolveStr, int* strPos) {
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
      bool skip;

      // Loop over all entries until found.
      for (unsigned int i = 0; i < dbcHeader.rows; i ++) {
        fread(content, sizeof(DWORD), dbcHeader.columns, fp);
        skip = false;
        for (int k = 0; k < keys; k ++) { 
          if (content[cols[k]] != vals[k]) { // Compare current key values.
            skip = true;                     // Skip on first mismatch.
            continue;                        // Leave comparison loop.
          }
        }
        // All values matched: Found!
        if (!skip) {
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
