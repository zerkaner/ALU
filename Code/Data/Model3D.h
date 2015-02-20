#pragma once
#include <vector>
struct Geoset;
struct Material;


/** 3D model class that defines the .m4 model format. */
class Model3D {

  private:

    /** Clears all current geosets (for deletion or model reload). */
    void ClearGeosets();

  public:

    /** An enumeration of available rendering modes. */
    enum RenderingMode {OFF, POINTS, MESH, DIRECT, VBO};

    RenderingMode RenderingMode;    // Current rendering mode.
    std::vector<Geoset*> Geosets;   // List with all geosets.
    Material* _materials;           // Material pointer.

    /** Create an empty model. */
    Model3D();


    /** Create a model loaded from a file.
     * @param filepath The file to load. */
    Model3D(const char* filepath);


    /** Destructor, frees all allocated memory. */
    ~Model3D();


    /** Reads a model from a given file.
     * @param filepath Path to the model file. */
    void LoadFile(const char* filepath);


    /** Writes the model data to a .m4 file. 
     * @param filepath Path to output file. */
    void WriteFile(const char* filepath);


    /** Scale the model's vertices. 
     * @param factor The factor to scale. */
    void ScaleModel(float factor);


    /** (Re-)Calculate surface normals. */
    void CalculateNormals();

    
    /** Outputs the model properties to console or text file. 
     * @param fileOutput If set to 'true', detailed output will be written to a text file.
     * @param filename Name of the output file [default: output.txt]. */
    void Echo(bool fileOutput = false, const char* filename = "output.txt");
};
