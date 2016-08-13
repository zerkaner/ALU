#pragma warning(disable: 4996)
#include "ShaderRenderer.h"
#include <Data/Model.h>
#include <SDL_opengl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>


// Function pointers for initialization.
PFNGLATTACHSHADERPROC attachShader;
PFNGLCOMPILESHADERPROC compileShader;
PFNGLCREATEPROGRAMPROC createProgram;
PFNGLCREATESHADERPROC createShader;
PFNGLGETPROGRAMINFOLOGPROC getProgramInfoLog;
PFNGLGETPROGRAMIVPROC getProgramiv;
PFNGLGETSHADERINFOLOGPROC getShaderInfoLog;
PFNGLGETSHADERIVPROC getShaderiv;
PFNGLISPROGRAMPROC isProgram;
PFNGLISSHADERPROC isShader;
PFNGLLINKPROGRAMPROC linkProgram;
PFNGLSHADERSOURCEPROC shaderSource;

// Definition of our functions (common names). The PFNPROC-typedefs are in the SDL library.
PFNGLUSEPROGRAMPROC useProgram;
PFNGLDELETEPROGRAMPROC deleteProgram;
PFNGLGENBUFFERSPROC genBuffers;
PFNGLACTIVETEXTUREPROC activeTexture;
PFNGLBINDBUFFERPROC bindBuffer;
PFNGLBUFFERDATAPROC bufferData;
PFNGLDELETEBUFFERSPROC deleteBuffers;
PFNGLGETATTRIBLOCATIONPROC getAttribLocation;
PFNGLGETUNIFORMLOCATIONPROC getUniformLocation;
PFNGLENABLEVERTEXATTRIBARRAYPROC enableVertexAttribArray;
PFNGLDISABLEVERTEXATTRIBARRAYPROC disableVertexAttribArray;
PFNGLVERTEXATTRIBPOINTERPROC vertexAttribPointer;
PFNGLUNIFORM1FPROC uniform1f;
PFNGLUNIFORM1IPROC uniform1i;
PFNGLUNIFORM3FPROC uniform3f;


//TODO Das kann man bestimmt noch besser hinkriegen!
int pMatrixUniform, mvMatrixUniform, nMatrixUniform, samplerUniform, materialShininessUniform, showSpecularHighlightsUniform,
useTexturesUniform, useLightingUniform, ambientColorUniform, pointLightingLocationUniform,
pointLightingSpecularColorUniform, pointLightingDiffuseColorUniform;
//_________________________________________________________________


/** Helper function to read a GLSL shader file. */
std::string readShaderFromFile(const char* filepath) {
  FILE* fp = fopen(filepath, "r");
  if (fp == NULL) {
    printf("Error opening file '%s'!\n", filepath);
    return "";
  }
  char buffer[512];
  std::string content;
  while (fgets(buffer, 512, fp) != NULL) content.append(std::string(buffer) + "\n");
  return content;
}



/** Builds an OpenGL shader program.
 * @param vss Vertex shader source.
 * @param fss Fragment shader source.
 * @return Program ID (positive number), or 0 in case of an error. */
GLuint ShaderRenderer::BuildShaderProgram(const char* vss, const char* fss) {
  GLuint gProgramID = createProgram();  // Generate program.

  GLuint vertexShader = createShader(GL_VERTEX_SHADER); // Create vertex shader.
  shaderSource(vertexShader, 1, &vss, NULL);            // Set vertex source.
  compileShader(vertexShader);                          // Compile vertex source.

  GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER); // Create fragment shader.
  shaderSource(fragmentShader, 1, &fss, NULL);              // Set fragment source.  
  compileShader(fragmentShader);                            // Compile fragment source.

  // Check for success. Abort and print debug message in case of an error.
  int vCompiled = 0, fCompiled = 0;
  getShaderiv(vertexShader, GL_COMPILE_STATUS, &vCompiled);
  getShaderiv(fragmentShader, GL_COMPILE_STATUS, &fCompiled);
  if (vCompiled != 1 || fCompiled != 1) {
    if (vCompiled != 1) PrintShaderLog(vertexShader, true);
    if (fCompiled != 1) PrintShaderLog(fragmentShader, false);
    return 0;
  }

  // Attach single shaders to shader program and link program together.
  attachShader(gProgramID, vertexShader);
  attachShader(gProgramID, fragmentShader);
  linkProgram(gProgramID);

  // Check for linking errors.
  int programSuccess = 0;
  getProgramiv(gProgramID, GL_LINK_STATUS, &programSuccess);
  if (programSuccess != 1) {
    printf("Error linking program %d!\n", gProgramID);
    PrintProgramLog(gProgramID);
    return 0;
  }

  // All instructions succeeded. Print information and return program ID!
  printf("[ShaderRenderer] OpenGL version: %s\n", glGetString(GL_VERSION));
  printf("[ShaderRenderer] GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
  printf("[ShaderRenderer] Vendor: %s\n", glGetString(GL_VENDOR));
  printf("[ShaderRenderer] Renderer: %s\n", glGetString(GL_RENDERER));
  return gProgramID;
}



/** Initializes the OpenGL extentions. */
void ShaderRenderer::InitExtensions() {
  useProgram = (PFNGLUSEPROGRAMPROC) wglGetProcAddress("glUseProgram");
  createProgram = (PFNGLCREATEPROGRAMPROC) wglGetProcAddress("glCreateProgram");
  deleteProgram = (PFNGLDELETEPROGRAMPROC) wglGetProcAddress("glDeleteProgram");
  genBuffers = (PFNGLGENBUFFERSARBPROC) wglGetProcAddress("glGenBuffers");
  activeTexture = (PFNGLACTIVETEXTUREPROC) wglGetProcAddress("glActiveTexture");
  bindBuffer = (PFNGLBINDBUFFERARBPROC) wglGetProcAddress("glBindBuffer");
  bufferData = (PFNGLBUFFERDATAARBPROC) wglGetProcAddress("glBufferData");
  deleteBuffers = (PFNGLDELETEBUFFERSARBPROC) wglGetProcAddress("glDeleteBuffers");
  createShader = (PFNGLCREATESHADERPROC) wglGetProcAddress("glCreateShader");
  shaderSource = (PFNGLSHADERSOURCEPROC) wglGetProcAddress("glShaderSource");
  compileShader = (PFNGLCOMPILESHADERPROC) wglGetProcAddress("glCompileShader");
  getShaderiv = (PFNGLGETSHADERIVPROC) wglGetProcAddress("glGetShaderiv");
  attachShader = (PFNGLATTACHSHADERPROC) wglGetProcAddress("glAttachShader");
  linkProgram = (PFNGLLINKPROGRAMPROC) wglGetProcAddress("glLinkProgram");
  getProgramiv = (PFNGLGETPROGRAMIVPROC) wglGetProcAddress("glGetProgramiv");
  getAttribLocation = (PFNGLGETATTRIBLOCATIONPROC) wglGetProcAddress("glGetAttribLocation");
  getUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) wglGetProcAddress("glGetUniformLocation");
  enableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) wglGetProcAddress("glEnableVertexAttribArray");
  disableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) wglGetProcAddress("glDisableVertexAttribArray");
  vertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) wglGetProcAddress("glVertexAttribPointer");
  uniform1f = (PFNGLUNIFORM1FPROC) wglGetProcAddress("glUniform1f");
  uniform1i = (PFNGLUNIFORM1IPROC) wglGetProcAddress("glUniform1i");
  uniform3f = (PFNGLUNIFORM3FPROC) wglGetProcAddress("glUniform3f");
  isShader = (PFNGLISSHADERPROC) wglGetProcAddress("glIsShader");
  getShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) wglGetProcAddress("glGetShaderInfoLog");
  isProgram = (PFNGLISPROGRAMPROC) wglGetProcAddress("glIsProgram");
  getProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) wglGetProcAddress("glGetProgramInfoLog");
}



/** Prints the shader program linking log (error message).
 * @param program The shader program to output. */
void ShaderRenderer::PrintProgramLog(GLuint program) {
  if (isProgram(program)) {
    int infoLogLength = 0;
    int maxLength = infoLogLength;
    getProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
    char* infoLog = new char[maxLength];
    getProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
    if (infoLogLength > 0) printf("%s\n", infoLog);
    delete[] infoLog;
  }
  else printf("Name %d is not a program!\n", program);
}



/** Prints the shader compile log (error message).
 * @param shader The shader to output.
 * @param isVtx 'true' on vertex shader, 'false' on fragment shader. */
void ShaderRenderer::PrintShaderLog(GLuint shader, bool isVtx) {
  printf("Unable to compile %s shader %d!\n", (isVtx ? "vertex" : "fragment"), shader);
  if (isShader(shader)) {  // Make sure name is shader.
    int infoLogLength = 0;
    int maxLength = infoLogLength;
    getShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    char* infoLog = new char[maxLength];
    getShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
    if (infoLogLength > 0) printf("%s\n", infoLog);
    delete[] infoLog;
  }
  else printf("Name %d is not a shader!\n", shader);
}



/** Create a new shader renderer with the default shader. */
ShaderRenderer::ShaderRenderer() {
  InitExtensions();
  const char* vDef = "attribute vec3 vert; attribute vec2 vertTexCoord; varying vec2 fragTexCoord; void main(void) { fragTexCoord = vertTexCoord; gl_Position = gl_ModelViewProjectionMatrix * vec4(vert, 1);}";
  const char* fDef = "uniform sampler2D tex; varying vec2 fragTexCoord; void main() {gl_FragColor = texture(tex, fragTexCoord);}";
  shaderProgram = BuildShaderProgram(vDef, fDef);
  if (shaderProgram != 0) {
    SetupShader();
    printf("[ShaderRenderer] Initialized with default shader.\n");
  }
}



/** Create a shader renderer with the specified shaders.
 * @param vsFile Vertex shader file.
 * @param fsFile Fragment shader file. */
ShaderRenderer::ShaderRenderer(const char* vsFile, const char* fsFile) {
  std::string vs = readShaderFromFile(vsFile);
  std::string fs = readShaderFromFile(fsFile);
  if (vs != "" && fs != "") {
    InitExtensions();
    shaderProgram = BuildShaderProgram(vs.c_str(), fs.c_str());   
    if (shaderProgram != 0) {
      SetupShader();
      printf("[ShaderRenderer] Initialized with shaders '%s', '%s'.\n", vsFile, fsFile);
    }
  }
}



/** Create a vertex buffer object (VBO) for shader rendering.
 * @params Linear VNTI arrays, their respective sizes and return array (uint[4]).
 * @return Pointer to vertex buffer object. */
VertexBufferObject* ShaderRenderer::CreateVBO(float* vtx, float* nor, float* tex,
  unsigned int* idx, int nrVtx, int nrNor, int nrTex, int nrIdx) {

  // Create the buffers.
  VertexBufferObject* vbo = new VertexBufferObject();
  genBuffers(1, &vbo->vBuf);
  genBuffers(1, &vbo->nBuf);
  genBuffers(1, &vbo->tBuf);
  genBuffers(1, &vbo->iBuf);

  // Bind and fill the OpenGL buffers consecutively.
  bindBuffer(GL_ARRAY_BUFFER, vbo->vBuf);
  bufferData(GL_ARRAY_BUFFER, nrVtx * sizeof(float), vtx, GL_STATIC_DRAW);
  bindBuffer(GL_ARRAY_BUFFER, vbo->nBuf);
  bufferData(GL_ARRAY_BUFFER, nrNor * sizeof(float), nor, GL_STATIC_DRAW);
  bindBuffer(GL_ARRAY_BUFFER, vbo->tBuf);
  bufferData(GL_ARRAY_BUFFER, nrTex * sizeof(float), tex, GL_STATIC_DRAW);
  bindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo->iBuf);
  bufferData(GL_ELEMENT_ARRAY_BUFFER, nrIdx * sizeof(GLuint), idx, GL_STATIC_DRAW);
  return vbo;
}



/** Create an VBO for our model structure.
 * @param mdl The model to create a VBO for. */
void ShaderRenderer::CreateModelVBO(Model* mdl) {
  
  // Calculate array sizes.
  int nrVtx = mdl->Vertices.size() * 3;
  int nrNor = mdl->Normals.size() * 3;
  int nrTex = mdl->UVs.size() * 2;
  int nrIdx = mdl->Indices.size();

  // Create temporary (straight) arrays.
  float* vtx = new float[nrVtx];
  float* nor = new float[nrNor];
  float* tex = new float[nrTex];
  uint*  idx = new uint[nrIdx];

  // Copy Float3(), Float2()... stuff to these arrays.
  std::vector<Float3>& v = mdl->Vertices;
  std::vector<Float3>& n = mdl->Normals;
  std::vector<Float2>& t = mdl->UVs;
  for (int i = 0; i < nrVtx / 3; i ++) { vtx[i * 3] = v[i].X;  vtx[i * 3 + 1] = v[i].Y;  vtx[i * 3 + 2] = v[i].Z; }
  for (int i = 0; i < nrNor / 3; i ++) { nor[i * 3] = n[i].X;  nor[i * 3 + 1] = n[i].Y;  nor[i * 3 + 2] = n[i].Z; }
  for (int i = 0; i < nrTex / 2; i ++) { tex[i * 2] = t[i].X;  tex[i * 2 + 1] = t[i].Y; }
  for (int i = 0; i < nrIdx; i ++) { idx[i] = (uint) mdl->Indices[i]; }

  // Bind and fill the OpenGL buffers consecutively.
  mdl->VBO = CreateVBO(vtx, nor, tex, idx, nrVtx, nrNor, nrTex, nrIdx);

  // Remove the temporary arrays.
  delete vtx;
  delete nor;
  delete tex;
  delete idx;
}


uint wgtAttr, bneAttr;


/** Set up the shader by obtaining the addresses and enabling the attibute pointers. */
void ShaderRenderer::SetupShader() { 
  posAttr = getAttribLocation(shaderProgram, "vert"); enableVertexAttribArray(posAttr);
  texAttr = getAttribLocation(shaderProgram, "vertTexCoord"); enableVertexAttribArray(texAttr);
  wgtAttr = getAttribLocation(shaderProgram, "weights"); enableVertexAttribArray(wgtAttr);
  bneAttr = getAttribLocation(shaderProgram, "bones"); enableVertexAttribArray(bneAttr);

  /*
  // Get the attributes.
  posAttr = getAttribLocation(shaderProgram, "posAttr"); enableVertexAttribArray(posAttr);
  norAttr = getAttribLocation(shaderProgram, "norAttr"); enableVertexAttribArray(norAttr); 
  texAttr = getAttribLocation(shaderProgram, "texAttr"); enableVertexAttribArray(texAttr);

  // Get the uniform locations.
  pMatrixUniform = getUniformLocation(shaderProgram, "uPMatrix");
  mvMatrixUniform = getUniformLocation(shaderProgram, "uMVMatrix");
  nMatrixUniform = getUniformLocation(shaderProgram, "uNMatrix");
  samplerUniform = getUniformLocation(shaderProgram, "uSampler");
  materialShininessUniform = getUniformLocation(shaderProgram, "uMaterialShininess");
  showSpecularHighlightsUniform = getUniformLocation(shaderProgram, "uShowSpecularHighlights");
  useTexturesUniform = getUniformLocation(shaderProgram, "uUseTextures");
  useLightingUniform = getUniformLocation(shaderProgram, "uUseLighting");
  ambientColorUniform = getUniformLocation(shaderProgram, "uAmbientColor");
  pointLightingLocationUniform = getUniformLocation(shaderProgram, "uPointLightingLocation");
  pointLightingSpecularColorUniform = getUniformLocation(shaderProgram, "uPointLightingSpecularColor");
  pointLightingDiffuseColorUniform = getUniformLocation(shaderProgram, "uPointLightingDiffuseColor");
  
  uniform1i(useLightingUniform, true);
  uniform1i(samplerUniform, 0);
  uniform1i(useTexturesUniform, true);
  uniform1i(showSpecularHighlightsUniform, true);
  
  uniform3f(pointLightingLocationUniform, 80, 40, 10);
  uniform3f(ambientColorUniform, 0.6f, 0.3f, 0.3f);
  uniform3f(pointLightingSpecularColorUniform, 1.0f, 1.0f, 1.0f);
  uniform3f(pointLightingDiffuseColorUniform, 0.8f, 0.8f, 0.8f);    
  uniform1f(materialShininessUniform, 1.0f);
  */
}



/** Enable or disable the shader renderer. */
void ShaderRenderer::SetActive(bool enabled) {
  if (enabled) {
    useProgram(shaderProgram);
    activeTexture(GL_TEXTURE0);
    uniform1i(getAttribLocation(shaderProgram, "tex"), 0);
  }
  else { // Unbind texture and the program.
    glBindTexture(GL_TEXTURE_2D, 0);
    useProgram(NULL);
  }
}



/** Bind a vertex buffer object to the GPU buffers.
 * @param vbo VBO to bind. */
void ShaderRenderer::BindVBO(VertexBufferObject* vbo) {
  bindBuffer(GL_ARRAY_BUFFER, vbo->vBuf);  // Load vertex buffer.
  vertexAttribPointer(posAttr, 3, GL_FLOAT, false, 0, 0);
  bindBuffer(GL_ARRAY_BUFFER, vbo->nBuf);   // Load normal vector buffer.
  vertexAttribPointer(norAttr, 3, GL_FLOAT, false, 0, 0);
  bindBuffer(GL_ARRAY_BUFFER, vbo->tBuf);  // Load texture coordinate buffer.
  vertexAttribPointer(texAttr, 2, GL_FLOAT, false, 0, 0);
  bindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo->iBuf); // Load vertex index buffer.
}



/** Draw a part of a bound VBO with the given texture.
 * @param offset Start index offset for this call.
 * @param length Number of indices to use for drawing call.
 * @param texture Index to texture (GPU buffer). */
void ShaderRenderer::Draw(int offset, int length, GLuint texture) {
  glBindTexture(GL_TEXTURE_2D, texture);
  glDrawElements(GL_TRIANGLES, length, GL_UNSIGNED_INT, (void*) (offset * sizeof(GLuint)));
}
