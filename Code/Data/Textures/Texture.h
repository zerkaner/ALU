#pragma once
struct SDL_Surface;


/** Base class, defines common functions needed for a texture. */
class Texture {

  public:
    virtual ~Texture() {}

    /* Various getter functions. */
    virtual unsigned int ID()       const = 0;
    virtual unsigned char* Data()   const = 0;
    virtual unsigned long Size()    const = 0;
    virtual const char* Name()      const = 0;
    virtual int Width()             const = 0;
    virtual int Height()            const = 0;
    virtual int StoredWidth()       const = 0;
    virtual int StoredHeight()      const = 0;
    virtual int OffsetX()           const = 0;
    virtual int OffsetY()           const = 0;
    virtual const Texture* Parent() const = 0;

    /** Take a slice of this texture.
     * @param x x-coordinate of slice beginning.
     * @param y y-coordinate of slice beginning.
     * @param width Slice width (x-range).
     * @param height Slice height (y-range).
     * @return Pointer to new texture object. */
    Texture* Slice(int x, int y, int width, int height);
};


/** A basic texture class, extends the abstract one. */
class SimpleTexture : public Texture {
  
  private:
    unsigned int _id;     // OpenGL ID of texture.
    unsigned char* _data; // Image data array.
    unsigned long _size;  // Texture data array size.
    const char* _name;    // Name of texture (file path).
    int _width;           // Texture width.
    int _height;          // Texture height.
    int _storedWidth;     // Actual width in memory.
    int _storedHeight;    // Actual height in memory.


  public:

    /** Create a texture with given image data (preloaded). 
     * @param data Image data array.
     * @param size Texture data array size.
     * @param name Name of texture (file path). */
    SimpleTexture(unsigned char* data, unsigned long size, const char* name);

    /** Delete the texture and free the memory. */
    ~SimpleTexture();

    /* Getter functions implementations. */
    unsigned int ID()       const { return _id; }
    unsigned char* Data()   const { return _data; }
    unsigned long Size()    const { return _size; }
    const char* Name()      const { return _name; }
    int Width()             const { return _width; }
    int Height()            const { return _height; }
    int StoredWidth()       const { return _storedWidth; }
    int StoredHeight()      const { return _storedHeight; }
    int OffsetX()           const { return 0; }
    int OffsetY()           const { return 0; }
    const Texture* Parent() const { return 0; }   

    /** Initialize the OpenGL buffer for this texture. */
    void SetupGLTextureBuffer();
};


/* Represents a slice of an texture. */
class TextureSlice : public Texture {
  
  private:
    Texture* _parent; // Parent (=major) texture.
    int _offsetX;     // X offset of this slice.
    int _offsetY;     // Y offset of slice.
    int _width;       // The width this slice has.
    int _height;      // The height.
  
  public:

    /** Create a new texture slice.
     * @param parent Major texture (to slice from).
     * @param x x-coordinate of slice beginning.
     * @param y y-coordinate of slice beginning.
     * @param width Slice width (x-range).
     * @param height Slice height (y-range). */
    TextureSlice(Texture* parent, int x, int y, int width, int height);

    /* Getter functions implementations. */
    unsigned int ID()       const { return _parent->ID(); }
    unsigned char* Data()   const { return _parent->Data(); }
    unsigned long Size()    const { return _parent->Size(); }
    const char* Name()      const { return _parent->Name(); }
    int Width()             const { return _width; }
    int Height()            const { return _height; }
    int StoredWidth()       const { return _parent->StoredWidth(); }
    int StoredHeight()      const { return _parent->StoredHeight(); }
    int OffsetX()           const { return _offsetX; }
    int OffsetY()           const { return _offsetY; }
    const Texture* Parent() const { return _parent; }
};
