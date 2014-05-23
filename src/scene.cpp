#include "scene.h"

Model::Model() : valid_(false) {}

Model::Model(Assimp::Importer &importer, const string &filename)
    : valid_(false),
      scene_(importer.ReadFile(
          filename, aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals |
                        aiProcess_JoinIdenticalVertices |
                        aiProcess_OptimizeMeshes | aiProcess_SortByPType |
                        aiProcess_Triangulate)) {
  // Check the loading worked.
  if (!scene_) {
    std::cout << importer.GetErrorString() << "\n";
    return;
  }
}

bool Model::IsValid() { return valid_; }

Scene::Scene(string filename) {
  QSettings config(filename.c_str(), QSettings::IniFormat);
  QStringList keys = config.childKeys();

  for (auto it = keys.begin(); it != keys.end(); ++it) {
    if (it->compare("board", Qt::CaseInsensitive)) {
      board_.readFromFile(config.value("board", "").toString().toStdString());
    } else if (it->compare("background", Qt::CaseInsensitive)) {
      // Set background.
    } else if (it->startsWith("id", Qt::CaseInsensitive) && it->size() > 2) {
      // Get the id of the markers.
      QString idRaw = it->right(it->size() - 2);
      bool success;
      int id = idRaw.toInt(&success, 10);

      // Check parsing worked.
      if (!success) {
        std::cout << "Could not parse id: " << idRaw.toStdString() << "\n";
        continue;
      }

      // Check id is in valid range.
      if (id < 0 || id > 1023) {
        std::cout << "id must be in range 0 <= id <= 1023, got " << id << "\n";
        continue;
      }

      // Try to load the model specified.
      Model m(importer_, config.value(*it, "").toString().toStdString());
      if (!m.IsValid()) {
        std::cout << "Error loading model.\n";
        continue;
      }

      // Record the model.
      models_[id] = m;
    } else {
      std::cout << "Unknown scene option: " << it->toStdString() << "\n";
    }
  }
}
