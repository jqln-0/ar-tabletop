#ifndef __SCENE_H_
#define __SCENE_H_

#include <QSettings>
#include <QtOpenGL>
#include <QFileInfo>

#include <aruco/aruco.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

enum PLYState {
  kInitial,
  kHeader,
  kVertices,
  kFaces,
};

class Vertex {
 public:
  // Vertex position.
  float x, y, z;
  // Normal direction.
  float nx, ny, nz;
  // Color.
  float r, g, b;
};

class Face {
 public:
  std::vector<size_t> vertices;
};

class Model {
 protected:
  bool valid_;
  std::vector<Vertex> vertices_;
  std::vector<Face> faces_;

 public:
  Model();
  Model(const string &filename);
  void Render();
  bool IsValid();
};

class Scene {
 protected:
  aruco::BoardConfiguration board_;
  std::unordered_map<int, Model> models_;

  std::string RelativeTo(QString scene_filename, QString filename);

 public:
  Scene(QString filename);
  void DrawModel(int id);
};

#endif  // __SCENE_H_
