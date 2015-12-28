#pragma once
#include <Data/Primitives.h>
#include <Data/Quaternion.h>
#include <Data/Textures/Texture.h>
#include <map>
#include <vector>

class AnimationManager;


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



//____________________________________________________________________________ [ANIMATION DATA]

/** The TransformationDirective (TD) is a translation, rotation or scaling instruction. */
struct TransformationDirective {
  int Frame;           // The frame this directive belongs to.
  float X, Y, Z, W;    // TRS values. W is needed for rotation (quaternion).
};


/** A TransformationSet (TS) contains all sequence TRS instructions for a bone. */
struct TransformationSet {
  std::vector<TransformationDirective> Translations; // Bone translation (x,y,z).
  std::vector<TransformationDirective> Rotations;    // Bone rotation (x,y,z,w).
  std::vector<TransformationDirective> Scalings;     // Dimension scalings (x,y,z).
};


/** The animation main structure. */
struct Sequence {
  char Name[80];  // Animation name.
  int ID;         // Identifier for internal referencing.
  int Length;     // Sequence length (animation frame span).
  bool Loop;      // Loop this sequence? Useful for animations like standing and walking.
  std::map<Bone2*, TransformationSet> Transformations; // Main array of TS for each bone.
};



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


/*  TEMPORARY STUFF HERE - TO BE DELETED WHEN MDX ANIMATION OPERATIONAL  */
struct BoneDir {
  Float3 Position;
  Float4 Rotation;
};
struct Keyframe {
  std::vector<BoneDir> BoneTrans;
};
struct Animation2 {
  int AnimVersion;
  char Name[32];     // beides!
  int ID;            // neu
  int FrameRate;
  int Duration;
  int FrameCount;
  std::vector<char*> Bones;
  std::vector<Keyframe> Keyframes;
};
// ----------------------------------------------------------------------

struct Model2 {
  char Name[80];
  int Version;
  std::vector<Float3> Vertices;
  std::vector<Float3> Normals;
  std::vector<Float2> UVs;
  std::vector<DWORD> Indices;
  std::vector<Mesh2> Meshes;
  std::vector<Bone2> Bones;
  std::vector<Sequence> Sequences;     // <== about to replace the Animation2
  AnimationManager* AnimMgr = 0;       // This is crap cause of double ref.
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


    /** Write model properties as debug information to a text file.
     * @param model The model to output. */
    static void PrintDebug(Model2* model);
};
