#ifndef __SCENE_H_
#define __SCENE_H_

#include <QFileInfo>
#include <QImage>
#include <QSettings>
#include <QtOpenGL>

#include <aruco/aruco.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "boardwrap.h"

enum PLYState {
  kInitial,
  kHeader,
  kVertices,
  kFaces,
};

class Vertex {
 public:
  // Vertex positions.
  float x, y, z;
  // Normal directions.
  float nx, ny, nz;
  // Color components.
  float r, g, b;
};

class Face {
 public:
  std::vector<size_t> vertices;
};

class Model {
 protected:
  // Specifies whether or not this model is valid, ie. whether or not the model
  // loading worked.
  bool valid_;

  std::vector<Vertex> vertices_;
  std::vector<Face> faces_;

 public:
  Model();
  Model(const string &filename);
  void Render();
  bool IsValid() const;
};

class Scene {
 protected:
  Board board_;
  std::unordered_map<int, Model> models_;

  // Background image for each frame.
  QImage background_;

  // Image to display on the scene's board.
  QImage board_image_;

  // Manipulates 'filename' to be a path relative to the parent directory of
  // 'scene_filename'.
  std::string RelativeTo(const QString &scene_filename,
                         const QString &filename) const;

 public:
  Scene(const QString &filename);
  void DrawModel(int id);
  void DrawBoard();

  Board GetBoard() const;
  void SetBoard(Board board);
  const QImage &GetBackground() const;
  const QImage &GetBoardImage() const;
};

#endif  // __SCENE_H_
