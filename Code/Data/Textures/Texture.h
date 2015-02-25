#pragma once
struct SDL_Surface;


/** Base class, defines common functions needed for a texture. */
class Texture {

  public:
    virtual ~Texture() {}

    /* Various getter functions. */
    virtual unsigned int ID()       const = 0;
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
    unsigned int _id;   // OpenGL ID of texture.
    int _width;         // Texture width.
    int _height;        // Texture height.
    int _storedWidth;   // Actual width in memory.
    int _storedHeight;  // Actual height in memory.
  
    /** Initialize the texture.
     * @param data Image data array.
     * @param resX Horizontal texture resolution.
     * @param resY Vertical texture resolution.
     * @param cmp Image components [RGB/A] (24 bit: 3, 32 bit: 4). */
    void InitTexture(unsigned char* data, int resX, int resY, int cmp);

  public:

    /** Create a texture with given image data (preloaded). 
     * @param data Image data array.
     * @param resX Horizontal texture resolution.
     * @param resY Vertical texture resolution.
     * @param cmp Image components [RGB/A] (24 bit: 3, 32 bit: 4). */
    SimpleTexture(unsigned char* data, int resX, int resY, int cmp);
    
    /** Load a texture from a file.
     * @param filename The file to load from. */
    SimpleTexture(const char* filename);

    /** Delete the texture and free the memory. */
    ~SimpleTexture();

    /* Getter functions implementations. */
    unsigned int ID()       const { return _id; }
    int Width()             const { return _width; }
    int Height()            const { return _height; }
    int StoredWidth()       const { return _storedWidth; }
    int StoredHeight()      const { return _storedHeight; }
    int OffsetX()           const { return 0; }
    int OffsetY()           const { return 0; }
    const Texture* Parent() const { return 0; }   
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
    int Width()             const { return _width; }
    int Height()            const { return _height; }
    int StoredWidth()       const { return _parent->StoredWidth(); }
    int StoredHeight()      const { return _parent->StoredHeight(); }
    int OffsetX()           const { return _offsetX; }
    int OffsetY()           const { return _offsetY; }
    const Texture* Parent() const { return _parent; }
};
