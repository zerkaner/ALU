#pragma once
#include <Data/Primitives.h>
#include <Data/Quaternion.h>
#include <Data/Textures/Texture.h>
#include <vector>

struct Mesh2 {
  char ID[32];
  bool Enabled;           // Shall this mesh be rendered?
  char Texture[80];       // Texture path.
  short TextureIdx = -1;  // Index to the appended texture chunk. 
  DWORD IndexOffset;
  DWORD IndexLength;
  DWORD BoneOffset;
  DWORD BoneCount;
};
struct Bone2 {
  char Name[32];
  int Parent;
  Float3 Position;
  Float4 Rotation;
  bool Skinned;
  float BindPoseMat[16];
  Float3 WorldPos;
  Float4 WorldRot;
  float BoneMat[16];
};


struct BoneDir {
  Float3 Position;
  Float4 Rotation;
};


struct Keyframe {
  std::vector<BoneDir> BoneTrans;
};


struct Animation2 {
  int AnimVersion;
  char Name[32];
  int FrameRate;
  int Duration;
  int FrameCount;
  std::vector<char*> Bones;
  std::vector<Keyframe> Keyframes;
};


struct Model2 {
  char Name[80];
  int Version;
  std::vector<Float3> Vertices;
  std::vector<Float3> Normals;
  std::vector<Float2> UVs;
  std::vector<DWORD> Indices;
  std::vector<Mesh2> Meshes;
  std::vector<Bone2> Bones;
  std::vector<Animation2> Animations;
  std::vector<SimpleTexture*> Textures;
  int _renderMode = 1; // Debug flag for visual testing. Not part of the model!
};



/** Static model class that provides loading, editing and saving of models. */
class ModelUtils {

  public:

    /** Load a M4 binary file.
     * @param filepath Path to the file to load. 
     * @return Pointer to the instantiated model. */
    static Model2* Load(const char* filepath);


    /** Save a model in the native M4 file format.
     * @param model The model to save.
     * @param savefile Name of the save file. */
    static void Save(Model2* model, const char* savefile);


    /** Scales the model's vertices.
     * @param model The model to scale.
     * @param factor The scaling factor. */
    static void ScaleModel(Model2* model, float factor);


    /** Scales the model to a specific extent.
     * @param model The model to scale.
     * @param axis The axis to use as reference.
     * @param value The total extent on that axis. */
    static void ScaleModelToExtent(Model2* model, char axis, float value);
};
