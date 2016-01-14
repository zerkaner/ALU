#pragma warning(disable: 4996)
#include <Converter/DBCParser.h>
#include <Converter/Converter.h>
#include <Converter/MPQReader.h>
#include <Execution/ALU.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>




DBCParser::DBCParser(int argc, char** argv) {
  
  // Read DBC files from MPQ archive.
  MPQReader reader = MPQReader("E:/Spiele/World of Warcraft 2.4.3 [8606]/Data/deDE/patch-deDE.mpq");
  _dbcItems  = new MemoryStream(&reader, "DBFilesClient\\ItemDisplayInfo.dbc");
  _dbcInfo   = new MemoryStream(&reader, "DBFilesClient\\CreatureDisplayInfo.dbc");
  _dbcExtra  = new MemoryStream(&reader, "DBFilesClient\\CreatureDisplayInfoExtra.dbc");
  _dbcModel  = new MemoryStream(&reader, "DBFilesClient\\CreatureModelData.dbc");
  _dbcHair   = new MemoryStream(&reader, "DBFilesClient\\CharHairGeosets.dbc");
  _dbcFacial = new MemoryStream(&reader, "DBFilesClient\\CharacterFacialHairStyles.dbc");
  reader.Close();
}



DBCParser::~DBCParser() {
  delete _dbcItems;
  delete _dbcInfo;
  delete _dbcExtra;
  delete _dbcModel;
  delete _dbcHair;
  delete _dbcFacial;
}



char* StringReplace(char* c_str, const char* c_from, const char* c_to) {
  std::string str(c_str);
  std::string from(c_from);
  std::string to(c_to);
  size_t start_pos = str.find(from);
  if (start_pos != std::string::npos) {
    str.replace(start_pos, from.length(), to);
  }
  char* arr = (char*) malloc(str.size() + 1);
  arr[str.size()] = '\0';
  memcpy(arr, str.c_str(), str.size());
  return arr;
}



void DBCParser::Test() {
  ModelData* modeldata = FetchNPCInformation(7274);

  if (modeldata != NULL) {
    FetchItemInformationForModel(modeldata);   // Load item data.
    FetchFacialInformationForModel(modeldata); // Load hair and facial geosets.

    // Setup geoset list.
    for (int i = 0; i < 11; i ++) {
      int geosetID = modeldata->equip[i].geoset;
      if (geosetID != -1) modeldata->geosets.push_back(geosetID);
    }



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
      printf(" - Facial   : %d\n", modeldata->body[4]);
      printf("-------------------\n");
      printf(" - Helmet   : %5d  %s  %d\n", modeldata->equip[0].id, (modeldata->equip[0].id != 0) ? modeldata->equip[0].mdx : "", modeldata->equip[0].geoset);
      printf(" - Shoulder : %5d  %s  %d\n", modeldata->equip[1].id, (modeldata->equip[1].id != 0) ? modeldata->equip[1].mdx : "", modeldata->equip[1].geoset);
      printf(" - Shirt    : %5d  %s  %d\n", modeldata->equip[2].id, (modeldata->equip[2].id != 0) ? modeldata->equip[2].mdx : "", modeldata->equip[2].geoset);
      printf(" - Chest    : %5d  %s  %d\n", modeldata->equip[3].id, (modeldata->equip[3].id != 0) ? modeldata->equip[3].mdx : "", modeldata->equip[3].geoset);
      printf(" - Belt     : %5d  %s  %d\n", modeldata->equip[4].id, (modeldata->equip[4].id != 0) ? modeldata->equip[4].mdx : "", modeldata->equip[4].geoset);
      printf(" - Legs     : %5d  %s  %d\n", modeldata->equip[5].id, (modeldata->equip[5].id != 0) ? modeldata->equip[5].mdx : "", modeldata->equip[5].geoset);
      printf(" - Boots    : %5d  %s  %d\n", modeldata->equip[6].id, (modeldata->equip[6].id != 0) ? modeldata->equip[6].mdx : "", modeldata->equip[6].geoset);
      printf(" - Wrist    : %5d  %s  %d\n", modeldata->equip[7].id, (modeldata->equip[7].id != 0) ? modeldata->equip[7].mdx : "", modeldata->equip[7].geoset);
      printf(" - Gloves   : %5d  %s  %d\n", modeldata->equip[8].id, (modeldata->equip[8].id != 0) ? modeldata->equip[8].mdx : "", modeldata->equip[8].geoset);
      printf(" - Tabard   : %5d  %s  %d\n", modeldata->equip[9].id, (modeldata->equip[9].id != 0) ? modeldata->equip[9].mdx : "", modeldata->equip[9].geoset);
      printf(" - Cape     : %5d  %s  %d\n", modeldata->equip[10].id, (modeldata->equip[10].id != 0) ? modeldata->equip[10].mdx : "", modeldata->equip[10].geoset);
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


    const char* archive = "E:/Spiele/World of Warcraft 2.4.3 [8606]/Data/common.mpq";
    DWORD filesize;
    BYTE* data = MPQReader::ExtractFileFromArchive(archive, modeldata->model, &filesize);
    MemoryStream modelstream = MemoryStream(data, filesize);
    
    // Start model loader!
    Model* model = Converter::ReadM2(&modelstream);
    
    // Enable the needed geosets (maybe later we'll delete the rest).
    for (unsigned int i = 0; i < model->Meshes.size(); i ++) {
      model->Meshes[i].Enabled = false;
      for (unsigned int j = 0; j < modeldata->geosets.size(); j ++) {
        int id;
        sscanf(model->Meshes[i].ID, "Geoset %d", &id);
        if (id == modeldata->geosets[j]) {
          model->Meshes[i].Enabled = true;
          break;
        }
      }
    }

    // Close model file stream.
    ModelUtils::Save(model, "test.m4");

    //ALU test start.
    ALU alu = ALU();
    alu.TestConvertedModel(model);
    alu.Start();

    delete model; 
  }

  else printf("Dat gibt's hier nicht!\n");
}



ModelData* DBCParser::FetchNPCInformation(int id) {

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

  // Query model path (and replace .mdx with actual ending .m2).
  int modelID = baseinfo->content[INFO_MODEL];
  int modelPathCol[1] = {MODEL_PATH};
  DBCEntry* model = ReadDataFromDBC(_dbcModel, modelID, 1, modelPathCol);
  modeldata->model = model->strings[0];
  char* shortenedPath = StringReplace(modeldata->model, ".mdx", ".m2");
  free(modeldata->model);
  modeldata->model = shortenedPath;


  // Get additional data, such as geosets to load and equipment.
  int dataID = baseinfo->content[INFO_EXTRA];
  if (dataID != 0) {
    int texturePathCol[1] = {DATA_TEX};
    DBCEntry* data = ReadDataFromDBC(_dbcExtra, dataID, 1, texturePathCol);
    if (data != NULL) {
      modeldata->configured = true;
      modeldata->race = data->content[DATA_RACE];
      modeldata->sex = data->content[DATA_SEX];
      modeldata->body[0] = data->content[DATA_SKIN];
      modeldata->body[1] = data->content[DATA_FACE];
      modeldata->body[2] = data->content[DATA_HAIR];
      modeldata->body[3] = data->content[DATA_HAIRCOL];
      modeldata->body[4] = data->content[DATA_BEARD];
      modeldata->equip[0].id = data->content[DATA_HELM];
      modeldata->equip[1].id = data->content[DATA_SHOULDER];
      modeldata->equip[2].id = data->content[DATA_SHIRT];
      modeldata->equip[3].id = data->content[DATA_CUIRASS];
      modeldata->equip[4].id = data->content[DATA_BELT];
      modeldata->equip[5].id = data->content[DATA_LEGS];
      modeldata->equip[6].id = data->content[DATA_BOOTS];
      modeldata->equip[7].id = data->content[DATA_WRIST];
      modeldata->equip[8].id = data->content[DATA_GLOVES];
      modeldata->equip[9].id = data->content[DATA_TABARD];
      modeldata->equip[10].id = data->content[DATA_CAPE];

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



void DBCParser::FetchFacialInformationForModel(ModelData* modeldata) {

  // Load hair style geosets.
  int hairKeyCols[3] = {1, 2, 3};  // Three keys: Race, sex and hair style type.
  int hairKeyVals[3] = {modeldata->race, modeldata->sex, modeldata->body[2]};
  DBCEntry* hair = ReadDataFromDBC(_dbcHair, 3, hairKeyCols, hairKeyVals, 0, NULL);
  if (hair != NULL) {
    modeldata->geosets.push_back(hair->content[4]);
    delete hair;
  }

  // Load facial geosets.
  int facialKeyCols[3] = {0, 1, 2};  // Three keys: Race, sex and facial style type.
  int facialKeyVals[3] = {modeldata->race, modeldata->sex, modeldata->body[4]};
  DBCEntry* facials = ReadDataFromDBC(_dbcFacial, 3, facialKeyCols, facialKeyVals, 0, NULL);
  if (facials != NULL) {
    int chin = facials->content[6];  //| Data are stored in columns 6 - 10, 
    int must = facials->content[7];  //| but only the first three are relevant.
    int side = facials->content[8];  //| (Column 4 is empty, 5 for tusks ?!).
    if (chin != 0) modeldata->geosets.push_back(100 + chin); // Chin beard configuration (group 100).
    if (side != 0) modeldata->geosets.push_back(200 + side); // Sideburns configuration  (group 200).
    if (must != 0) modeldata->geosets.push_back(300 + must); // Mustache configuration   (group 300).
    delete facials;
  }

  // Setup base geosets.
  modeldata->geosets.push_back(0);   // Core model.
  modeldata->geosets.push_back(702); // Add ears.
}



void DBCParser::FetchItemInformationForModel(ModelData* modeldata) {

  int str[1] = {1};  // Model string position.
  const int groupMapping[11] = {-1, -1, -1, 1000, 1800, 1300, 500, 800, 400, 1200, 1500};
  /* Geoset group mapping. Matching the item slot (index) to the correspondent geoset group (value).
  DATA_HELM        0     --
  DATA_SHOULDER    1     --
  DATA_SHIRT       2     --
  DATA_CUIRASS     3   1000
  DATA_BELT        4   1800
  DATA_LEGS        5   1300
  DATA_BOOTS       6    500
  DATA_WRIST       7    800
  DATA_GLOVES      8    400
  DATA_TABARD      9   1200
  DATA_CAPE       10   1500 */


  for (int i = 0; i < 11; i ++) {  // Loop over all equipment slots.  
    int geoset = -1;

    // Item equipped. Query DBC data. 
    if (modeldata->equip[i].id != 0) {
      DBCEntry* data = ReadDataFromDBC(_dbcItems, modeldata->equip[i].id, 1, str);
      if (data != NULL) {
        geoset = groupMapping[i];  // Determine item geoset group.

        // On geoset usage (all except helm/shoulder/shirt), read values and write them to item structure.
        if (geoset != -1) {
          if (data->content[7] != 0) geoset += data->content[7] + 1;
          else if (data->content[8] != 0) geoset += data->content[8] + 1;
          else if (data->content[9] != 0) geoset += data->content[9] + 1;
          else geoset = -1;
        }

        // Copy item texture path for dedicated MDX model.
        strcpy(modeldata->equip[i].mdx, data->strings[0]);
        delete data;
      }
      else printf("[DBCParser] Error loading item information for '%d'!\n", modeldata->equip[i].id);
    }


    // Set base geosets for some slots, if there is nothing equipped (hands, legs and back).
    if (geoset == -1) switch (i) {
      case 5: case 8: case 10: geoset = groupMapping[i] + 1; break;
    }

    // Write calculated geoset to model equipment table.
    modeldata->equip[i].geoset = geoset;
  }
}



DBCEntry* DBCParser::ReadDataFromDBC(MemoryStream* dbc, int key, int resolveStr, int* strPos) {
  int cols[1] = {0};
  int vals[1] = {key};
  return ReadDataFromDBC(dbc, 1, cols, vals, resolveStr, strPos);
}



DBCEntry* DBCParser::ReadDataFromDBC(MemoryStream* dbc, int keys, int* cols, int* vals, int resolveStr, int* strPos) {
  DBCEntry* result = NULL;
  dbc->Seek(MemoryStream::START); // (Re)Set stream to beginning.

  // Stores file header information.
  struct DBCHeader { DWORD rows, columns, size, strSize; };

  DWORD dbuf;
  DBCHeader dbcHeader;
  dbc->Read(&dbuf, sizeof(DWORD));          // Skip 'WDBC'.
  dbc->Read(&dbcHeader, sizeof(DBCHeader)); // Read file header.

  if (dbcHeader.size / 4 != dbcHeader.columns) printf("Warning: Field size != 4!\n");
  DWORD* content = (DWORD*) calloc(dbcHeader.columns, sizeof(DWORD));
  bool found = false;
  bool skip;


  // Loop over all entries until found.
  for (unsigned int i = 0; i < dbcHeader.rows; i ++) {
    dbc->Read(content, sizeof(DWORD) * dbcHeader.columns);
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
      dbc->Seek(MemoryStream::START, offset);

      // We need multiple iterations, so we copy the file content to speed things up.
      char* text = (char*) calloc(dbcHeader.strSize, sizeof(char));
      dbc->Read(text, sizeof(char) * dbcHeader.strSize-1);
      text[dbcHeader.strSize - 1] = '\0'; // Set final terminator.

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

  return result;  // Return the results.
}
