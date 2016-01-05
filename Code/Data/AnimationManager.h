#pragma once
#include <Data/Model.h>
#include <Data/Math/MathLib.h>
#include <stdio.h>
#include <string.h>
using namespace std;

#include <Visualization/matrix.h>


/** Manages animations and interpolation for a model. */
class AnimationManager {

  private:
    vector<Sequence>& _sequences;  // All available sequences.
    vector<Bone2>& _bones;         // The model's skeleton.
    int _curFrame;                 // Current frame.
    Sequence* _sequence = NULL;    // The animation selected to play.
    
    /* Transformation set indexing variables. */
    int* _trIndex;  int* _trLength;  // Current translation index and total length (per bone).
    int* _rotIndex; int* _rotLength; // Current rotation index and total length (per bone).
    int* _scaIndex; int* _scaLength; // Current scaling index and total length (per bone).


    int AdvanceIndex(int boneIdx, int* tdIdx, int* tdLength, vector<TransformationDirective>& TD) {
      int next = tdIdx[boneIdx] + 1;            //| 'next' holds the index of the following
      if (next == tdLength[boneIdx]) next = 0;  //| frame to interpolate with.

      // Time to advance out index pointer. 
      else if (_curFrame >= TD[next].Frame) { 
        tdIdx[boneIdx] = next;                              // Set 'next' as current.
        next = AdvanceIndex(boneIdx, tdIdx, tdLength, TD);  // Get new 'next'.
      }
      return next;
    }


    /** Linear frame interpolation function.  */
    Float4 Interpolate(vector<TransformationDirective>& TD, int curIdx, int nextIdx, bool isFloat4 = false) {
      if (nextIdx > curIdx) {
        int distance = TD[nextIdx].Frame - TD[curIdx].Frame;
        int pos = _curFrame - TD[curIdx].Frame;
        float factor = (float) pos / distance;
        float inv = 1.0f - factor;
        return Float4(
          inv*TD[curIdx].X  +  factor*TD[nextIdx].X,
          inv*TD[curIdx].Y  +  factor*TD[nextIdx].Y,
          inv*TD[curIdx].Z  +  factor*TD[nextIdx].Z,
          isFloat4? (inv*TD[curIdx].W  +  factor*TD[nextIdx].W) : 0.0f
        );
      }
      // There's nothing to interpolate here. Just return current frame.
      else return Float4(TD[curIdx].X, TD[curIdx].Y, TD[curIdx].Z, TD[curIdx].W);
    }


  public:

    int Frameskip = 5;  // The frameskip governs the animation playback speed.

    /** Create a new animation manager.
     * @param model The model to animate. */
    AnimationManager(Model2* model) : 
      _sequences(model->Sequences), 
      _bones(model->Bones) {
      int nb = _bones.size();
      _trIndex  = new int[nb];  _trLength  = new int[nb];
      _rotIndex = new int[nb];  _rotLength = new int[nb];
      _scaIndex = new int[nb];  _scaLength = new int[nb];
    }


    /** Delete this animation manager. Free all allocated memory. */
    ~AnimationManager() {
      delete _trIndex;   delete _trLength;
      delete _rotIndex;  delete _rotLength;
      delete _scaIndex;  delete _scaLength;
    }


    /** Play an animation.
     * @param animName The name of the animation to play. */
    void Play(const char* animName) {
      
      // Search for the animation.
      for (uint i = 0; i < _sequences.size(); i ++) {
        if (strstr(animName, _sequences[i].Name) != NULL) {
          _sequence = &_sequences[i];
          break;
        }
      }

      // Reset all playback variables.
      if (_sequence != NULL) {
        _curFrame = 0; // Start at the beginning.
        for (uint i = 0; i < _bones.size(); i ++) {
          _trIndex[i] = 0; 
          _rotIndex[i] = 0; 
          _scaIndex[i] = 0;
          Bone2* bone = &_bones[i];
          if (_sequence->Transformations.count(bone)) {
            _trLength[i]  = _sequence->Transformations[bone].Translations.size();
            _rotLength[i] = _sequence->Transformations[bone].Rotations.size();
            _scaLength[i] = _sequence->Transformations[bone].Scalings.size();
          }
          else {
            _trLength[i] = 0; 
            _rotLength[i] = 0; 
            _scaLength[i] = 0;
          }
        }
      }
    }


    /** Animation advance. Interpolation between frames and calculate transformation matrices. */
    void Tick() {
      if (_sequence == NULL) return;  // Skip on idling.


      // Loop over all bones. It's important that this flat hierarchy starts with the roots.
      for (uint i = 0; i < _bones.size(); i ++) {
        Bone2* bone = &_bones[i];
        
        Matrix4x4f m;
        m.loadTranslation(bone->Position);

        float mat2[16];
        Float3 npos = Float3(0,0,0);
        Float4 nrot = Float4(0,0,0,0);

        // If we have transformation directives for this bone, interpolate its pos and rot.
        if (_sequence->Transformations.count(bone)) {
          if (_trLength[i] > 0) {  // Step 1: TRANSLATION.  
            vector<TransformationDirective>& trans = _sequence->Transformations[bone].Translations;
            int next = AdvanceIndex(i, _trIndex, _trLength, trans);
            Float4 tl = Interpolate(trans, _trIndex[i], next);
            m.translate(Float3(tl.X, tl.Y, tl.Z));
            npos += Float3(tl.X, tl.Y, tl.Z);
          }     
          if (_rotLength[i] > 0) { // Step 2: ROTATION. 
            vector<TransformationDirective>& rot = _sequence->Transformations[bone].Rotations;
            int next = AdvanceIndex(i, _rotIndex, _rotLength, rot);
            Float4 qu = Interpolate(rot, _rotIndex[i], next, true);
            m.quaternionRotate(qu);
            nrot += qu;
          }
          if (_scaLength[i] > 0) { // Step 3: SCALING.
            vector<TransformationDirective>& sca = _sequence->Transformations[bone].Scalings;
            int next = AdvanceIndex(i, _scaIndex, _scaLength, sca);
            Float4 sc = Interpolate(sca, _scaIndex[i], next);
            m.scale(Float3(sc.X, sc.Y, sc.Z));
          }
        }

        MathLib::CreateRTMatrix(nrot, npos, mat2);

        Float3 np = Float3(-bone->Position.X, -bone->Position.Y, -bone->Position.Z);
        m.translate(np);



      
        // Apply parent transformation to this bone.
        if (bone->Parent != -1) {
          Bone2* parent = &_bones[bone->Parent];
          bone->mpos = parent->mpos * m;
        }
        else bone->mpos = m;
        
        bone->WorldPos = bone->mpos * bone->Position;


        /*
        if (strstr(bone->Name, "Mesh01") != NULL) {
          printf("[%04d] | WPos %7.4f, %7.4f, %7.4f\n",
            _curFrame, bone->WorldPos.X, bone->WorldPos.Y, bone->WorldPos.Z);
        }
        */
      }
      


      // Advance frame offset.
      _curFrame += Frameskip;
      if (_curFrame >= _sequence->Length) { // If loop, wrap frame pointer. Else stop.
        if (_sequence->Loop) {
          _curFrame -= _sequence->Length; // Rewind frame counter.
          for (uint i = 0; i < _bones.size(); i ++) {
            _trIndex[i] = 0; _rotIndex[i] = 0; _scaIndex[i] = 0;  // Reset TD indices.
          }

        }
        else _sequence = NULL;
      }
    }
};



//// Array output for debugs.
//for (uint i = 0; i < _bones.size(); i ++) {
//  printf("Bone %s: T: %d/%d  R: %d/%d  S: %d/%d\n", _bones[i].Name, 
//    _trIndex[i], _trLength[i], _rotIndex[i], _rotLength[i], _scaIndex[i], _scaLength[i]
//  );
//}
