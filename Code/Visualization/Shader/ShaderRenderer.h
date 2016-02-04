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


    void BindVBO(VertexBufferObject* vbo);

    void DrawVBO(VertexBufferObject* vbo, int offset, int length);

    void SetupShader();

    void Dbg(unsigned int tex, unsigned int offset, unsigned int length, VertexBufferObject* vbo);
    //void Dbg2();
};
