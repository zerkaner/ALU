#pragma once

#include <Visualization/Model3D.h>


class Cube : public Model3D {

  protected:

    Float3 GetPosition() { 
      //Position.X += 0.05;
      //Position.Y += 0.1;
      return Position;  
    }

    //TODO Mittelpunktsproblem. Eiert beim Rotieren um Außenpunkt, nicht um die Mitte!
    //TODO Position müßte -/+ verrechnet werden!
    Float2 GetOrientation() { 
      Direction.X -= 0.2;
      Direction.Y += 2;
      return Direction; 
    }

  public:

    Float3 Position;
    Float2 Direction;

    Cube(Float3 position) {
      Position = position;
      _renderingMode = Model3D::DIRECT;

      // Bottom
      _triangles.push_back(new Geometry(new Vertex(Float3(0,0,0)), new Vertex(Float3(1,0,0)), new Vertex(Float3(1,1,0))));
      _triangles.push_back(new Geometry(new Vertex(Float3(0,0,0)), new Vertex(Float3(0,1,0)), new Vertex(Float3(1,1,0))));
      
      // Top
      _triangles.push_back(new Geometry(new Vertex(Float3(0,0,1)), new Vertex(Float3(1,0,1)), new Vertex(Float3(1,1,1))));
      _triangles.push_back(new Geometry(new Vertex(Float3(0,0,1)), new Vertex(Float3(0,1,1)), new Vertex(Float3(1,1,1))));

      // Front
      _triangles.push_back(new Geometry(new Vertex(Float3(0,0,0)), new Vertex(Float3(1,0,0)), new Vertex(Float3(1,0,1))));
      _triangles.push_back(new Geometry(new Vertex(Float3(0,0,0)), new Vertex(Float3(0,0,1)), new Vertex(Float3(1,0,1))));

      // Back
      _triangles.push_back(new Geometry(new Vertex(Float3(0,1,0)), new Vertex(Float3(1,1,0)), new Vertex(Float3(1,1,1))));
      _triangles.push_back(new Geometry(new Vertex(Float3(0,1,0)), new Vertex(Float3(0,1,1)), new Vertex(Float3(1,1,1))));

      // Left
      _triangles.push_back(new Geometry(new Vertex(Float3(0,0,0)), new Vertex(Float3(0,1,0)), new Vertex(Float3(0,1,1))));
      _triangles.push_back(new Geometry(new Vertex(Float3(0,0,0)), new Vertex(Float3(0,0,1)), new Vertex(Float3(0,1,1))));

      // Right
      _triangles.push_back(new Geometry(new Vertex(Float3(1,0,0)), new Vertex(Float3(1,1,0)), new Vertex(Float3(1,1,1))));
      _triangles.push_back(new Geometry(new Vertex(Float3(1,0,0)), new Vertex(Float3(1,0,1)), new Vertex(Float3(1,1,1))));
    }
};
