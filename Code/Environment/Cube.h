#pragma once

#include <Data/Object3D.h>
#include <Data/Model3D.h>


class Cube : public Object3D {

  public:

    Cube(Float3 position) {
      Position = Vector (position.X, position.Y, position.Z);      
      Movement = Vector(0,0,0);
      Acceleration = Vector(0,0,0);
      Model = new Model3D();

      Model->RenderingMode = Model3D::MESH;


      // Bottom
      Model->Triangles.push_back(new Geometry(new Vertex(Float3(-0.5f,-0.5f,-0.5f)), new Vertex(Float3(0.5f,-0.5f,-0.5f)), new Vertex(Float3(0.5f,0.5f,-0.5f))));
      Model->Triangles.push_back(new Geometry(new Vertex(Float3(-0.5f,-0.5f,-0.5f)), new Vertex(Float3(-0.5f,0.5f,-0.5f)), new Vertex(Float3(0.5f,0.5f,-0.5f))));
      
      // Top
      Model->Triangles.push_back(new Geometry(new Vertex(Float3(-0.5f,-0.5f,0.5f)), new Vertex(Float3(0.5f,-0.5f,0.5f)), new Vertex(Float3(0.5f,0.5f,0.5f))));
      Model->Triangles.push_back(new Geometry(new Vertex(Float3(-0.5f,-0.5f,0.5f)), new Vertex(Float3(-0.5f,0.5f,0.5f)), new Vertex(Float3(0.5f,0.5f,0.5f))));

      // Front
      Model->Triangles.push_back(new Geometry(new Vertex(Float3(-0.5f,-0.5f,-0.5f)), new Vertex(Float3(0.5f,-0.5f,-0.5f)), new Vertex(Float3(0.5f,-0.5f,0.5f))));
      Model->Triangles.push_back(new Geometry(new Vertex(Float3(-0.5f,-0.5f,-0.5f)), new Vertex(Float3(-0.5f,-0.5f,0.5f)), new Vertex(Float3(0.5f,-0.5f,0.5f))));

      // Back
      Model->Triangles.push_back(new Geometry(new Vertex(Float3(-0.5f,0.5f,-0.5f)), new Vertex(Float3(0.5f,0.5f,-0.5f)), new Vertex(Float3(0.5f,0.5f,0.5f))));
      Model->Triangles.push_back(new Geometry(new Vertex(Float3(-0.5f,0.5f,-0.5f)), new Vertex(Float3(-0.5f,0.5f,0.5f)), new Vertex(Float3(0.5f,0.5f,0.5f))));

      // Left
      Model->Triangles.push_back(new Geometry(new Vertex(Float3(-0.5f,-0.5f,-0.5f)), new Vertex(Float3(-0.5f,0.5f,-0.5f)), new Vertex(Float3(-0.5f,0.5f,0.5f))));
      Model->Triangles.push_back(new Geometry(new Vertex(Float3(-0.5f,-0.5f,-0.5f)), new Vertex(Float3(-0.5f,-0.5f,0.5f)), new Vertex(Float3(-0.5f,0.5f,0.5f))));

      // Right
      Model->Triangles.push_back(new Geometry(new Vertex(Float3(0.5f,-0.5f,-0.5f)), new Vertex(Float3(0.5f,0.5f,-0.5f)), new Vertex(Float3(0.5f,0.5f,0.5f))));
      Model->Triangles.push_back(new Geometry(new Vertex(Float3(0.5f,-0.5f,-0.5f)), new Vertex(Float3(0.5f,-0.5f,0.5f)), new Vertex(Float3(0.5f,0.5f,0.5f))));
    }
};
