#pragma once
#include <Data/Primitives.h>
#include <Data/Textures/Texture.h>
#include <map>
#include <vector>
class AnimationManager;


struct Bone {
  char Name[32];
  int Parent;
  Float3 Pivot;
  Float3 Position;
  Float4 Rotation;
  float WorldMatrix[16];
};


/** Vertex-to-bone assignment and weighting. */
struct BoneWeight {
  BYTE BoneIDs[4];  // Bone indices (0-4). '255' means unassigned.
  float Factor[4];  // Weighting factor of associated bone. Stored as byte in file.
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
  std::map<Bone*, TransformationSet> Transformations; // Main array of TS for each bone.
};



struct Mesh {
  char ID[32];           // Mesh identifier.
  bool Enabled;          // Shall this mesh be rendered?
  char Texture[80];      // Texture path.
  short TextureIdx = -1; // Index to the appended texture chunk. 
  DWORD IndexOffset;     // Offset to first vertex.
  DWORD IndexLength;     // Number of indices. 
  DWORD WeightOffset;    // Bone weighting offset. -1, if unattached.  
};



struct Model {
  char Name[80];
  int Version;
  std::vector<Float3> Vertices;
  std::vector<Float3> Normals;
  std::vector<Float2> UVs;
  std::vector<DWORD> Indices;
  std::vector<Mesh> Meshes;
  std::vector<Bone> Bones;
  std::vector<BoneWeight> Weights;
  std::vector<Sequence> Sequences;
  AnimationManager* AnimMgr = 0;
  std::vector<SimpleTexture*> Textures;
  int _renderMode = 3; // Debug flag for visual testing. Not part of the model!
};





/** Static model class that provides loading, editing and saving of models. */
class ModelUtils {

  public:

    /** Load a M4 binary file.
     * @param filepath Path to the file to load. 
     * @return Pointer to the instantiated model. */
    static Model* Load(const char* filepath);


    /** Save a model in the native M4 file format.
     * @param model The model to save.
     * @param savefile Name of the save file. */
    static void Save(Model* model, const char* savefile);


    /** Scales the model's vertices.
     * @param model The model to scale.
     * @param factor The scaling factor. */
    static void ScaleModel(Model* model, float factor);


    /** Scales the model to a specific extent.
     * @param model The model to scale.
     * @param axis The axis to use as reference.
     * @param value The total extent on that axis. */
    static void ScaleModelToExtent(Model* model, char axis, float value);


    /** Write model properties as debug information to a text file.
     * @param model The model to output. */
    static void PrintDebug(Model* model);
};
