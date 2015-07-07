#pragma once
#include <stdio.h>
#include <Data/Primitives.h>

//TODO Gehören in die Definition des AnimationManagers!
#include <map>
#include <string>
using namespace std;

void test() {
  printf("test fkt!\n");
}

// http://content.gpwiki.org/index.php/OpenGL:Tutorials:Basic_Bones_System
namespace AnimTest {


  struct Bone {
    int id;        // Bone identifier.
    Float3 pos1;   // Position value 1.
    Float3 pos2;   // 2nd position.
    Bone* parent;  // Parent bone. 'null' on root. 
    Bone* childs;  // Pointer to child nodes.
    int nrChilds;  // Number of child nodes.
    //TODO Vertex list.
  };

  struct Keyframe {

  };

  struct Animation {
    char* id;
    float lastFrame;
  };



  struct Mesh {
  };




  /** Intermediate model structure for modelling purpose. Will be integrated later! */
  class Model3D_t {
    
  };




  /** Manages animations and interpolation for a model. */
  class AnimationManager {
    private:
      Mesh* _baseMesh;                     // Model base mesh when no animation is applied.
      Bone* _skeleton;                     // The model's skeleton.
      map<string, Animation*> _animations; // Available animation definitions.
     
      Animation* _curAnim = NULL;   // Animation in use.
      Mesh* _curMesh = NULL;        // Mesh copy for applied transformations.
      float _curFrame = 0;          // Counter for currently displayed frame.


    public:

      float Frameskip;  // Defines the frame advance per call.


      /** Create a new animation manager. 
       * @param basemesh The unanimated default mesh.
       * @param skeleton Bone hierarchy defining the model's skeleton.
       * @param defFrameSkip Default frame skip value [default: 1]. */
      AnimationManager(Mesh* basemesh, Bone* skeleton, float defFrameSkip = 1) {
        _baseMesh = basemesh;
        _skeleton = skeleton;
        Frameskip = defFrameSkip;
      }



      /** Add a new animation to the set.
       * @param anim Animation structure. */
      void AddAnimation(Animation* anim) {
        string str(anim->id);
        _animations[str] = anim;
      }



      /** Selects an animation to display.
       * @param animId Animation identifier (string). 
       * @param startFrame Frame to begin with [default: 0]. */
      void SetAnimation(char* animId, float startFrame = 0) {
        string anim(animId);
        if (!_animations.count(anim)) {
          printf("[AnimationManager] Error setting animation: '%s' does not exist!\n", animId);
          _curAnim = NULL;
          return;
        }

        // Animation found. Check start frame.
        else {
          if (startFrame > _animations[anim]->lastFrame) { // Too big!
            printf("[AnimationManager] Error setting animation '%s': Start frame '%d' out of bounds!\n", animId);
            _curAnim = NULL;
            return;
          }

          // All input values validated. Set them as current selection.
          else {
            _curAnim = _animations[anim];
            _curFrame = startFrame;
          }
        }
      }



      /** Advances the animation with a given frameskip.
       * @param frameskip This value defines how much animation frames will be advanced.
       * @return A model mesh that represents the current animation state. */
      Mesh* AdvanceAnimation(float frameskip) {
        if (_curAnim != NULL) {

          // Update the frame pointer and adjust it, if it exceeds boundaries.
          _curFrame += frameskip;
          if (_curFrame > _curAnim->lastFrame) _curFrame = _curFrame - _curAnim->lastFrame;
          else if (_curFrame < 0)              _curFrame = _curAnim->lastFrame + _curFrame;

          //TODO




          return _curMesh;
        }      
        else return _baseMesh;  // No animation in progress, just return the base mesh.
      }



      /** Advances the animation with the default frameskip.
       * @return Transformed mesh, ready to render. */
      Mesh* AdvanceAnimation() {
        return AdvanceAnimation(Frameskip);
      }
  };
};

