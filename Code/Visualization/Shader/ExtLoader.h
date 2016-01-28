#pragma once
#include <SDL_opengl.h>
#include <stdio.h>


// Definition of our functions (common names). The PFNPROC-typedefs are in the SDL library.
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLUNIFORM1FPROC glUniform1f;
PFNGLUNIFORM1IPROC glUniform1i;
PFNGLUNIFORM3FPROC glUniform3f;
PFNGLISSHADERPROC glIsShader;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLISPROGRAMPROC glIsProgram;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;


/** Initializes the OpenGL extentions. */
void initExtensions() {
  glUseProgram = (PFNGLUSEPROGRAMPROC) wglGetProcAddress("glUseProgram");
  glCreateProgram = (PFNGLCREATEPROGRAMPROC) wglGetProcAddress("glCreateProgram");
  glDeleteProgram = (PFNGLDELETEPROGRAMPROC) wglGetProcAddress("glDeleteProgram");
  glGenBuffers = (PFNGLGENBUFFERSARBPROC) wglGetProcAddress("glGenBuffers");
  glBindBuffer = (PFNGLBINDBUFFERARBPROC) wglGetProcAddress("glBindBuffer");
  glBufferData = (PFNGLBUFFERDATAARBPROC) wglGetProcAddress("glBufferData");
  glDeleteBuffers = (PFNGLDELETEBUFFERSARBPROC) wglGetProcAddress("glDeleteBuffers"); 
  glCreateShader = (PFNGLCREATESHADERPROC) wglGetProcAddress("glCreateShader");
  glShaderSource = (PFNGLSHADERSOURCEPROC) wglGetProcAddress("glShaderSource");
  glCompileShader = (PFNGLCOMPILESHADERPROC) wglGetProcAddress("glCompileShader");
  glGetShaderiv = (PFNGLGETSHADERIVPROC) wglGetProcAddress("glGetShaderiv");
  glAttachShader = (PFNGLATTACHSHADERPROC) wglGetProcAddress("glAttachShader");
  glLinkProgram = (PFNGLLINKPROGRAMPROC) wglGetProcAddress("glLinkProgram");
  glGetProgramiv = (PFNGLGETPROGRAMIVPROC) wglGetProcAddress("glGetProgramiv");
  glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC) wglGetProcAddress("glGetAttribLocation");
  glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) wglGetProcAddress("glGetUniformLocation");
  glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) wglGetProcAddress("glEnableVertexAttribArray");
  glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) wglGetProcAddress("glDisableVertexAttribArray");
  glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) wglGetProcAddress("glVertexAttribPointer");
  glUniform1f = (PFNGLUNIFORM1FPROC) wglGetProcAddress("glUniform1");
  glUniform1i = (PFNGLUNIFORM1IPROC) wglGetProcAddress("glUniform1i");
  glUniform3f = (PFNGLUNIFORM3FPROC) wglGetProcAddress("glUniform3f");
  glIsShader = (PFNGLISSHADERPROC) wglGetProcAddress("glIsShader");
  glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) wglGetProcAddress("glGetShaderInfoLog");
  glIsProgram = (PFNGLISPROGRAMPROC) wglGetProcAddress("glIsProgram");
  glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) wglGetProcAddress("glGetProgramInfoLog");
}


void printShaderLog(GLuint shader, bool isVtx) {
  printf("Unable to compile %s shader %d!\n", (isVtx? "vertex" : "fragment"), shader);
  if (glIsShader(shader)) {  // Make sure name is shader.
    int infoLogLength = 0;
    int maxLength = infoLogLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    char* infoLog = new char[maxLength]; 
    glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
    if (infoLogLength > 0) printf("%s\n", infoLog);
    delete[] infoLog;
  }
  else printf("Name %d is not a shader!\n", shader);
}


void printProgramLog(GLuint program) {
  if (glIsProgram(program)) {
    int infoLogLength = 0;
    int maxLength = infoLogLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
    char* infoLog = new char[maxLength];
    glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
    if (infoLogLength > 0) printf("%s\n", infoLog);
    delete[] infoLog;
  }
  else printf("Name %d is not a program!\n", program);
}


/** Builds an OpenGL shader program.
 * @param vss Vertex shader source (string array). 
 * @param fss Fragment shader source (string array). 
 * @return Program ID (positive number), or 0 in case of an error. */
GLuint BuildShaderProgram(const char** vss, const char** fss) {
  GLuint gProgramID = glCreateProgram();  // Generate program.

  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); // Create vertex shader.
  glShaderSource(vertexShader, 1, vss, NULL);             // Set vertex source.
  glCompileShader(vertexShader);                          // Compile vertex source.

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // Create fragment shader.
  glShaderSource(fragmentShader, 1, fss, NULL);               // Set fragment source.  
  glCompileShader(fragmentShader);                            // Compile fragment source.

  // Check for success. Abort and print debug message in case of an error.
  int vCompiled = 0, fCompiled = 0;
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vCompiled);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fCompiled);
  if (vCompiled != 1 || fCompiled != 1) {
    if (vCompiled != 1) printShaderLog(vertexShader, true);
    if (fCompiled != 1) printShaderLog(fragmentShader, false);
    return 0;
  }

  // Attach single shaders to shader program and link program together.
  glAttachShader(gProgramID, vertexShader);
  glAttachShader(gProgramID, fragmentShader);
  glLinkProgram(gProgramID);

  // Check for linking errors.
  int programSuccess = 0;
  glGetProgramiv(gProgramID, GL_LINK_STATUS, &programSuccess);
  if (programSuccess != 1) {
    printf("Error linking program %d!\n", gProgramID);
    printProgramLog(gProgramID);
    return 0;
  }

  // All instructions succeeded. Return program ID!
  return gProgramID;
}



GLuint vtxBuf, norBuf, texBuf, idxBuf;
int vertexPositionAttribute, vertexNormalAttribute, textureCoordAttribute;


bool testFkt() {
  
  const char* v1[] = {"#version 140\nin vec2 LVertexPos2D; void main() { gl_Position = vec4( LVertexPos2D.x, LVertexPos2D.y, 0, 1 ); }"};
  const char* f1[] = {"#version 140\nout vec4 LFragment; void main() { LFragment = vec4( 1.0, 1.0, 1.0, 1.0 ); }"};
  const char* v2[] = {"#version 140\nattribute vec3 aVertexPosition; attribute vec3 aVertexNormal; attribute vec2 aTextureCoord; uniform mat4 uMVMatrix; uniform mat4 uPMatrix; uniform mat3 uNMatrix; varying vec2 vTextureCoord; varying vec3 vTransformedNormal; varying vec4 vPosition; void main(void) { vPosition = uMVMatrix * vec4(aVertexPosition, 1.0); gl_Position = uPMatrix * vPosition; vTextureCoord = aTextureCoord; vTransformedNormal = uNMatrix * aVertexNormal; }"};
  const char* f2[] = {"#version 140\nprecision mediump float; varying vec2 vTextureCoord; varying vec3 vTransformedNormal; varying vec4 vPosition; uniform float uMaterialShininess; uniform bool uShowSpecularHighlights; uniform bool uUseLighting; uniform bool uUseTextures; uniform vec3 uAmbientColor; uniform vec3 uPointLightingLocation; uniform vec3 uPointLightingSpecularColor; uniform vec3 uPointLightingDiffuseColor; uniform sampler2D uSampler; void main(void) { vec3 lightWeighting; if (!uUseLighting) { lightWeighting = vec3(1.0, 1.0, 1.0); } else { vec3 lightDirection = normalize(uPointLightingLocation - vPosition.xyz); vec3 normal = normalize(vTransformedNormal); float specularLightWeighting = 0.0; if (uShowSpecularHighlights) { vec3 eyeDirection = normalize(-vPosition.xyz); vec3 reflectionDirection = reflect(-lightDirection, normal); specularLightWeighting = pow(max(dot(reflectionDirection, eyeDirection), 0.0), uMaterialShininess); } float diffuseLightWeighting = max(dot(normal, lightDirection), 0.0); lightWeighting = uAmbientColor + uPointLightingSpecularColor * specularLightWeighting + uPointLightingDiffuseColor * diffuseLightWeighting; } vec4 fragmentColor; if (uUseTextures) { fragmentColor = texture2D(uSampler, vec2(vTextureCoord.s, vTextureCoord.t)); } else { fragmentColor = vec4(1.0, 1.0, 1.0, 1.0); } gl_FragColor = vec4(fragmentColor.rgb * lightWeighting, fragmentColor.a); if (gl_FragColor.a < 0.46) discard; }"};  

  GLuint prog = BuildShaderProgram(v2, f2);


  // Set up cube shaders.
  int pMatrixUniform, mvMatrixUniform, nMatrixUniform, samplerUniform, materialShininessUniform, showSpecularHighlightsUniform,
  useTexturesUniform, useLightingUniform, ambientColorUniform, pointLightingLocationUniform,
  pointLightingSpecularColorUniform, pointLightingDiffuseColorUniform;

  vertexPositionAttribute = glGetAttribLocation(prog, "aVertexPosition");
  glEnableVertexAttribArray(vertexPositionAttribute);
  vertexNormalAttribute = glGetAttribLocation(prog, "aVertexNormal");
  glEnableVertexAttribArray(vertexNormalAttribute);
  textureCoordAttribute = glGetAttribLocation(prog, "aTextureCoord");
  glEnableVertexAttribArray(textureCoordAttribute);
  pMatrixUniform = glGetUniformLocation(prog, "uPMatrix");
  mvMatrixUniform = glGetUniformLocation(prog, "uMVMatrix");
  nMatrixUniform = glGetUniformLocation(prog, "uNMatrix");
  samplerUniform = glGetUniformLocation(prog, "uSampler");
  materialShininessUniform = glGetUniformLocation(prog, "uMaterialShininess");
  showSpecularHighlightsUniform = glGetUniformLocation(prog, "uShowSpecularHighlights");
  useTexturesUniform = glGetUniformLocation(prog, "uUseTextures");
  useLightingUniform = glGetUniformLocation(prog, "uUseLighting");
  ambientColorUniform = glGetUniformLocation(prog, "uAmbientColor");
  pointLightingLocationUniform = glGetUniformLocation(prog, "uPointLightingLocation");
  pointLightingSpecularColorUniform = glGetUniformLocation(prog, "uPointLightingSpecularColor");
  pointLightingDiffuseColorUniform = glGetUniformLocation(prog, "uPointLightingDiffuseColor");

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
