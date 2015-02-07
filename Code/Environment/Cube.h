#pragma once

#include <Visualization/Model3D.h>


class Cube : public Model3D {

  protected:


  public:

    Cube(Float3 position) {
      Position = Vector (position.X, position.Y, position.Z);      
      Movement = Vector(0,0,0);
      Acceleration = Vector(0,0,0);
      
      _renderingMode = Model3D::MESH;


      // Bottom
      _triangles.push_back(new Geometry(new Vertex(Float3(-0.5f,-0.5f,-0.5f)), new Vertex(Float3(0.5f,-0.5f,-0.5f)), new Vertex(Float3(0.5f,0.5f,-0.5f))));
      _triangles.push_back(new Geometry(new Vertex(Float3(-0.5f,-0.5f,-0.5f)), new Vertex(Float3(-0.5f,0.5f,-0.5f)), new Vertex(Float3(0.5f,0.5f,-0.5f))));
      
      // Top
      _triangles.push_back(new Geometry(new Vertex(Float3(-0.5f,-0.5f,0.5f)), new Vertex(Float3(0.5f,-0.5f,0.5f)), new Vertex(Float3(0.5f,0.5f,0.5f))));
      _triangles.push_back(new Geometry(new Vertex(Float3(-0.5f,-0.5f,0.5f)), new Vertex(Float3(-0.5f,0.5f,0.5f)), new Vertex(Float3(0.5f,0.5f,0.5f))));

      // Front
      _triangles.push_back(new Geometry(new Vertex(Float3(-0.5f,-0.5f,-0.5f)), new Vertex(Float3(0.5f,-0.5f,-0.5f)), new Vertex(Float3(0.5f,-0.5f,0.5f))));
      _triangles.push_back(new Geometry(new Vertex(Float3(-0.5f,-0.5f,-0.5f)), new Vertex(Float3(-0.5f,-0.5f,0.5f)), new Vertex(Float3(0.5f,-0.5f,0.5f))));

      // Back
      _triangles.push_back(new Geometry(new Vertex(Float3(-0.5f,0.5f,-0.5f)), new Vertex(Float3(0.5f,0.5f,-0.5f)), new Vertex(Float3(0.5f,0.5f,0.5f))));
      _triangles.push_back(new Geometry(new Vertex(Float3(-0.5f,0.5f,-0.5f)), new Vertex(Float3(-0.5f,0.5f,0.5f)), new Vertex(Float3(0.5f,0.5f,0.5f))));

      // Left
      _triangles.push_back(new Geometry(new Vertex(Float3(-0.5f,-0.5f,-0.5f)), new Vertex(Float3(-0.5f,0.5f,-0.5f)), new Vertex(Float3(-0.5f,0.5f,0.5f))));
      _triangles.push_back(new Geometry(new Vertex(Float3(-0.5f,-0.5f,-0.5f)), new Vertex(Float3(-0.5f,-0.5f,0.5f)), new Vertex(Float3(-0.5f,0.5f,0.5f))));

      // Right
      _triangles.push_back(new Geometry(new Vertex(Float3(0.5f,-0.5f,-0.5f)), new Vertex(Float3(0.5f,0.5f,-0.5f)), new Vertex(Float3(0.5f,0.5f,0.5f))));
      _triangles.push_back(new Geometry(new Vertex(Float3(0.5f,-0.5f,-0.5f)), new Vertex(Float3(0.5f,-0.5f,0.5f)), new Vertex(Float3(0.5f,0.5f,0.5f))));
    }
};
