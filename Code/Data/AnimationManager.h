//#pragma once
//#include <stdio.h>
//#include <stdlib.h>
//#include <Data/Model3D.h>
//#include <Data/Primitives.h>
//#include <map>
//#include <string>
//
//using namespace std;
//
//
///** Manages animations and interpolation for a model. */
//class AnimationManager {
//private:
//  Model3D* _model;                     // 3D model base.
//  Mesh* _baseMesh;                     // Model base mesh when no animation is applied.
//  Bone_t* _skeleton;                   // The model's skeleton.
//  map<string, Animation*> _animations; // Available animation definitions.
//  Animation* _curAnim = NULL;          // Animation in use.
//  Mesh* _curMesh = NULL;               // Mesh copy for applied transformations.
//  float _curFrame = 0;                 // Counter for currently displayed frame.
//
//
//public:
//  float Frameskip;  // Defines the frame advance per call.
//
//
//  /** Create a new animation manager.
//   * @param model The underlying 3D model with mesh and bone structure. 
//   * @param defFrameSkip Default frame skip value [default: 1]. */
//  AnimationManager(Model3D* model, float defFrameSkip = 1) {
//    _model = model;
//    //_skeleton = _model->Bones2;
//    BuildMesh();
//    Frameskip = defFrameSkip;
//  }
//
//
//  /** Builds an animation mesh based on the model's geosets. It is basically a deep copy. */
//  void BuildMesh() {
//    _baseMesh = new Mesh();
//    _baseMesh->nrElements = _model->Geosets.size();
//    _baseMesh->geosets = (Geoset**) calloc(sizeof(Geoset*), _baseMesh->nrElements);
//    for (int i = 0; i < _baseMesh->nrElements; i ++) {
//      //TODO Deep Copy needed!
//      _baseMesh->geosets[i] = _model->Geosets[i];
//    }
//  }
//
//
//  /** Add a new animation to the set.
//  * @param anim Animation structure. */
//  void AddAnimation(Animation* anim) {
//    string str(anim->Name);
//    _animations[str] = anim;
//  }
//
//
//
//  /** Selects an animation to display.
//   * @param animId Animation identifier (string).
//   * @param startFrame Frame to begin with [default: 0]. */
//  void SetAnimation(char* animId, float startFrame = 0) {
//    string anim(animId);
//    if (!_animations.count(anim)) {
//      printf("[AnimationManager] Error setting animation: '%s' does not exist!\n", animId);
//      _curAnim = NULL;
//      return;
//    }
//
//    // Animation found. Check start frame.
//    else {
//      if (startFrame > _animations[anim]->FrameCount) { // Frame interval exceeded!
//        printf("[AnimationManager] Error setting animation '%s': Start frame '%d' out of bounds!\n", animId);
//        _curAnim = NULL;
//        return;
//      }
//
//      // All input values validated. Set them as current selection.
//      else {
//        _curAnim = _animations[anim];
//        _curFrame = startFrame;
//      }
//    }
//  }
//
//
//
//  /** Advances the animation with a given frameskip.
//   * @param frameskip This value defines how much animation frames will be advanced.
//   * @return A model mesh that represents the current animation state. */
//  Mesh* AdvanceAnimation(float frameskip) {
//    if (_curAnim != NULL) {
//
//      // Update the frame pointer and adjust it, if it exceeds boundaries.
//      _curFrame += frameskip;
//      if (_curFrame > _curAnim->FrameCount) _curFrame = _curFrame - _curAnim->FrameCount;
//      else if (_curFrame < 0)              _curFrame = _curAnim->FrameCount + _curFrame;
//
//      //TODO
//
//
//
//
//      return _curMesh;
//    }
//    else return _baseMesh;  // No animation in progress, just return the base mesh.
//  }
//
//
//
//  /** Advances the animation with the default frameskip.
//   * @return Transformed mesh, ready to render. */
//  Mesh* AdvanceAnimation() {
//    return AdvanceAnimation(Frameskip);
//  }
//};
