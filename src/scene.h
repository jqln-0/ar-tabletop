#ifndef __SCENE_H_
#define __SCENE_H_

#include <QSettings>
#include <QtOpenGL>
#include <QFileInfo>

#include <aruco/aruco.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <unordered_map>

class Model {
 protected:
  bool valid_;
  const aiScene *scene_;

 public:
  Model();
  Model(Assimp::Importer &importer, const string &filename);
  void Render();
	void RenderNode(const aiNode *node);
	void ApplyMaterial(size_t index);
  bool IsValid();
};

class Scene {
 protected:
  aruco::BoardConfiguration board_;
  std::unordered_map<int, Model> models_;
  Assimp::Importer importer_;

	std::string RelativeTo(QString scene_filename, QString filename);

 public:
	Scene(QString filename);
	void DrawModel(int id);
};

#endif  // __SCENE_H_
