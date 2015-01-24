#pragma once


/** Interface class that defines rendering methods for 3D objects. */
class IDrawable {
  
  public:
    
    /** Virtual empty destructor. */
    virtual ~IDrawable() {}


    /** Rendering function called by 3D engine.
     *  Has to be implemented with concrete OpenGL code. */
    virtual void Draw() = 0;
};
