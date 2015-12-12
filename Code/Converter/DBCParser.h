#pragma once
#include <Converter/Utils/MemoryStream.h>
#include <Data/Primitives.h>
#include <vector>


/** Auxiliary structures for read-in process and enums for the column indices. */
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


  /** Item data structure for model equipment. */
  struct ItemData {
    int id = 0;          // Item ID.
    char mdx[120];       // Item model path.
    int geoset = -1;     // Geoset to load on model.
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
    ItemData equip[11];        // Equipment configuration.
    std::vector<int> geosets;  // Geosets to render (for common character models). 
  };
};

using namespace DBCFields;


/** Parser class for deferencing Blizzards DBC (database cache) files. */
class DBCParser {

  private:
    MemoryStream* _dbcInfo;   // CreatureDisplayInfo.dbc: Base model information with indices to the other tables.
    MemoryStream* _dbcItems;  // ItemDisplayInfo.dbc: Item properties and additional models.
    MemoryStream* _dbcExtra;  // CreatureDisplayInfoExtra.dbc: Contains model configurations for NPCs.
    MemoryStream* _dbcModel;  // CreatureModelData.dbc: Main model data information.
    MemoryStream* _dbcHair;   // CharHairGeosets.dbc: Tells us which geosets to use for the hair. 
    MemoryStream* _dbcFacial; // CharacterFacialHairStyles.dbc: Information about beards, markings and jewelry.


    /** Reads a single data row from a DBC file and returns it with its properties.
     * @param dbc Memory stream to the DBC file to use.
     * @param key The key of the row to search for.
     * @param resolveStr Number of strings to resolve (use '0' to disable).
     * @param strPos Integer array with the column indices of the strings to resolve ('null', if none).
     * @return The content of the data row (with the associated properties). */
    DBCEntry* ReadDataFromDBC(MemoryStream* dbc, int key, int resolveStr, int* strPos);


    /** Reads a single data row from a DBC file and returns it with its properties.
     * @param dbc Memory stream to the DBC file to use.
     * @param keys The number of keys (array sizes).
     * @param cols Key columns.
     * @param vals Key values.
     * @param resolveStr Number of strings to resolve (use '0' to disable).
     * @param strPos Integer array with the column indices of the strings to resolve ('null', if none). 
     * @return The content of the data row (with the associated properties). */
    DBCEntry* ReadDataFromDBC(MemoryStream* dbc, int keys, int* cols, int* vals, int resolveStr, int* strPos);


  public:

    void Test();
    
    /** Create a new DBC parser.
     * @param argc Argument counter. 
     * @param argv String array with parameters. */
    DBCParser(int argc, char** argv);


    /** Destructor call to free DBC memory. */
    ~DBCParser();


    /** Retrieves data for a creature (NPC).
     * @param id The ID of the creature to fetch. 
     * @return A model data structure with all information for M2 processing. */
    ModelData* FetchNPCInformation(int id);


    /** Load geoset IDs for the appearance configuration of a model.
     * @param modeldata The model container with hair and facial identifier set. */
    void FetchFacialInformationForModel(ModelData* modeldata);


    /** Load item information for a model.
     * @param modeldata The model container with item IDs set. */
    void FetchItemInformationForModel(ModelData* modeldata);
};
