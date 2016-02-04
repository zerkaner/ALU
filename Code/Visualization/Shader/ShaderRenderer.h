#pragma once
struct Model;


/** Vertex Buffer Object (VBO) structure. */
struct VertexBufferObject {
  unsigned int vBuf, nBuf, tBuf, iBuf; // Vertex/normal/texel/index buffer references.
  bool isBound;                        // Is this buffer currently bound to the GPU?
};


class ShaderRenderer {
  private:

    unsigned int shaderProgram;     // Shader program identifier,
    int posAttr, norAttr, texAttr;  // Positions of VNT attributes.

    /** Builds an OpenGL shader program.
     * @param vss Vertex shader source.
     * @param fss Fragment shader source.
     * @return Program ID (positive number), or 0 in case of an error. */
    unsigned int BuildShaderProgram(const char* vss, const char* fss);


    /** Initializes the OpenGL extentions. */
    void InitExtensions();


    /** Prints the shader program linking log (error message).
     * @param program The shader program to output. */
    void PrintProgramLog(unsigned int program);


    /** Prints the shader compile log (error message).
     * @param shader The shader to output.
     * @param isVtx 'true' on vertex shader, 'false' on fragment shader. */
    void PrintShaderLog(unsigned int shader, bool isVtx);


  public:

    /** Create a new shader renderer with the default shader. */
    ShaderRenderer();


    /** Create a shader renderer with the specified shaders.
     * @param vsFile Vertex shader file. 
     * @param fsFile Fragment shader file. */
    ShaderRenderer(const char* vsFile, const char* fsFile);


    /** Create a vertex buffer object (VBO) for shader rendering.
     * @params Linear VNTI arrays, their respective sizes and return array (uint[4]).
     * @return Pointer to vertex buffer object. */
    VertexBufferObject* CreateVBO(float* vtx, float* nor, float* tex, unsigned int* idx,
      int nrVtx, int nrNor, int nrTex, int nrIdx);


    /** Create an VBO for our model structure.
     * @param mdl The model to create a VBO for. */
    void CreateModelVBO(Model* mdl);


    /** Set up the shader by obtaining the addresses and enabling the attibute pointers. */
    void SetupShader();


    /** Enable or disable the shader renderer. */
    void SetActive(bool enabled);


    /** Bind a vertex buffer object to the GPU buffers.
     * @param vbo VBO to bind. */
    void BindVBO(VertexBufferObject* vbo);


    /** Draw a part of a bound VBO with the given texture.
     * @param offset Start index offset for this call. 
     * @param length Number of indices to use for drawing call. 
     * @param texture Index to texture (GPU buffer). */
    void Draw(int offset, int length, unsigned int texture);
};
