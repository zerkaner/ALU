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
};

// http://www.cprogramming.com/tutorial/function-pointers.html 


/*
GLuint vtxBuf, norBuf, texBuf, idxBuf;
int vertexPositionAttribute, vertexNormalAttribute, textureCoordAttribute;


bool testFkt() {

  float vertices[] = {
    -1, -1, 1, 1, -1, 1, 1, 1, 1, -1, 1, 1,
    -1, -1, -1, -1, 1, -1, 1, 1, -1, 1, -1, -1,
    -1, 1, -1, -1, 1, 1, 1, 1, 1, 1, 1, -1,
    -1, -1, -1, 1, -1, -1, 1, -1, 1, -1, -1, 1,
    1, -1, -1, 1, 1, -1, 1, 1, 1, 1, -1, 1,
    -1, -1, -1, -1, -1, 1, -1, 1, 1, -1, 1, -1
  };
  
  float normals[] = {
    0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1,
    0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1,
    1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,
    0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
    -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0,
    0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0
  };
  
  float texcoords[] = {
    0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0,
    1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0,
    1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0,
    1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0
  };
  
  GLuint indices[] = {
    0, 1, 2, 0, 2, 3,
    4, 5, 6, 4, 6, 7,
    8, 9, 10, 8, 10, 11,
    12, 13, 14, 12, 14, 15,
    16, 17, 18, 16, 18, 19,
    20, 21, 22, 20, 22, 23
  };

  glUniform1i(useLightingUniform, true);
//  glUniform1i(samplerUniform, 0);
  glUniform1i(useTexturesUniform, false);
//  glUniform1i(showSpecularHighlightsUniform, true);
//  glUniform1f(materialShininessUniform, (10 / 12.5f));
  glUniform3f(pointLightingLocationUniform, 2, 4, 10);
  glUniform3f(ambientColorUniform, 0.3f, 0.3f, 0.3f);
  glUniform3f(pointLightingSpecularColorUniform, 1.0f, 1.0f, 1.0f);
  glUniform3f(pointLightingDiffuseColorUniform, 0.8f, 0.8f, 0.8f);

  glGenBuffers(1, &vtxBuf);
  glBindBuffer(GL_ARRAY_BUFFER, vtxBuf);
  glBufferData(GL_ARRAY_BUFFER, 24 * 3*sizeof(float), vertices, GL_STATIC_DRAW);
  
  glGenBuffers(1, &norBuf);
  glBindBuffer(GL_ARRAY_BUFFER, norBuf);
  glBufferData(GL_ARRAY_BUFFER, 24 * 3*sizeof(float), normals, GL_STATIC_DRAW);
  
  glGenBuffers(1, &texBuf);
  glBindBuffer(GL_ARRAY_BUFFER, texBuf);
  glBufferData(GL_ARRAY_BUFFER, 24 * 2*sizeof(float), texcoords, GL_STATIC_DRAW);
  
  glGenBuffers(1, &idxBuf);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idxBuf);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLuint), indices, GL_STATIC_DRAW);

  return true;
}


void DrawIt() {
  glEnable(GL_LIGHTING);
  
  glBindBuffer(GL_ARRAY_BUFFER, vtxBuf);  // Load vertex buffer.
  glVertexAttribPointer(vertexPositionAttribute, 3, GL_FLOAT, false, 0, 0);
  
  glBindBuffer(GL_ARRAY_BUFFER, texBuf);       // Load texture coordinate buffer.
  glVertexAttribPointer(textureCoordAttribute, 2, GL_FLOAT, false, 0, 0);
  
  glBindBuffer(GL_ARRAY_BUFFER, norBuf);   // Load normal vector buffer.
  glVertexAttribPointer(vertexNormalAttribute, 3, GL_FLOAT, false, 0, 0);
  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idxBuf); // Load vertex index buffer.
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
}
*/