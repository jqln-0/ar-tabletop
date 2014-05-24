#include "scene.h"

using std::string;

Model::Model() : valid_(false) {}

Model::Model(const string &filename) : valid_(false) {
  // This is a really bad, really hacky model importer. It reads only ply files
  // (and even then only such files generated by Blender containing a very
  // strict set of features).
  // I would've/should've/could've used another external library for this, but:
  //	- assimp, the gold standard for model importing, is terrible and doesn't
  //		seem to support vertex colors properly.
  //	- The reference .PLY reader leaks memory like a sieve,
  //	- Other libraries are complicated and weirdly designed. I just want to
  // read
  //		a model, damnit!

  // Try to open the source file.
  ifstream f;
  f.open(filename);
  if (!f.is_open()) {
    std::cout << "Failed to open model file " << filename << "\n";
    return;
  }

  // Begin parsing line by line.
  PLYState state = kInitial;
  string line;
  size_t num_vertices, num_faces;
  while (getline(f, line)) {
    stringstream line_str(line);
    string token;
    switch (state) {
      case kInitial:
        // Check the file starts with the requisite magic number.
        if (line != "ply") {
          std::cout << "Only models in .ply format are supported.\n";
          return;
        }
        state = kHeader;
        break;
      case kHeader:
        // We only care about 3 things in the header: format of the body, number
        // of
        // vertices and number of faces.
        line_str >> token;
        if (token == "format" && line != "format ascii 1.0") {
          std::cout << "Only ascii-formatted .ply models supported.\n";
          return;
        }
        if (token == "element") {
          int amount;
          line_str >> token;
          line_str >> amount;
          if (token == "vertex") {
            num_vertices = amount;
          } else if (token == "face") {
            num_faces = amount;
          }
        }

        if (line == "end_header") {
          state = kVertices;
        }
        break;
      case kVertices:
        if (num_vertices > 0) {
          Vertex v;
          int r, g, b;
          line_str >> v.x >> v.y >> v.z >> v.nx >> v.ny >> v.nz >> r >> g >> b;
          // Scale colors from 0 to 1.
          v.r = float(r) / 255.0;
          v.g = float(g) / 255.0;
          v.b = float(b) / 255.0;
          vertices_.push_back(v);
          --num_vertices;
        }
        if (num_vertices == 0) {
          state = kFaces;
        }
        break;
      case kFaces:
        if (num_faces > 0) {
          Face face;
          int num;
          line_str >> num;
          while (num > 0) {
            size_t index;
            line_str >> index;
            face.vertices.push_back(index);
            --num;
          }
          faces_.push_back(face);
          --num_faces;
        }
        break;
    }
  }
  f.close();
  valid_ = true;
}

bool Model::IsValid() const { return valid_; }

void Model::Render() {
  // Remember previous matrix settings.
  glPushMatrix();

  // Iterate over every face.
  for (auto f = faces_.begin(); f != faces_.end(); ++f) {

    // Work out what drawing mode to use.
    size_t num_vertices = f->vertices.size();
    GLenum face_type;
    switch (num_vertices) {
      case 1:
        face_type = GL_POINTS;
        break;
      case 2:
        face_type = GL_LINES;
        break;
      case 3:
        face_type = GL_TRIANGLES;
        break;
      case 4:
        face_type = GL_QUADS;
        break;
      default:
        face_type = GL_POLYGON;
        break;
    }

    glBegin(face_type);
    // Draw every vertex.
    for (auto v = f->vertices.begin(); v != f->vertices.end(); ++v) {
      Vertex vertex = vertices_[*v];
      glColor3f(vertex.r, vertex.g, vertex.b);
      glNormal3f(vertex.nx, vertex.ny, vertex.nz);
      glVertex3f(vertex.x, vertex.y, vertex.z);
    }
    glEnd();
  }

  glPopMatrix();
}

Scene::Scene(QString filename) : board_okay_(false) {
  QSettings config(filename, QSettings::IniFormat);
  QStringList keys = config.childKeys();

  for (auto it = keys.begin(); it != keys.end(); ++it) {
    if (it->compare("board", Qt::CaseInsensitive) == 0) {
      // Try to set the board.
      std::string board_path =
          RelativeTo(filename, config.value(*it, "").toString());
      board_okay_ = false;
      try {
        board_.readFromFile(board_path);
        board_okay_ = true;
      }
      catch (cv::Exception e) {
        std::cout << "Failed to load board: " << e.what() << "\n";
      }
    } else if (it->compare("background", Qt::CaseInsensitive) == 0) {
      // Set background.
      std::string background_path =
          RelativeTo(filename, config.value(*it, "").toString());
      background_ = QImage(background_path.c_str());
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

      // Do the model loading.
      std::string model_path =
          RelativeTo(filename, config.value(*it, "").toString());
      Model m(model_path);
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

std::string Scene::RelativeTo(QString scene_filename, QString filename) {
  QFileInfo info(scene_filename);
  QString path = info.dir().path() + QDir::separator() + filename;
  return path.toStdString();
}

void Scene::DrawModel(int id) {
  if (models_.count(id) == 1) {
    models_[id].Render();
  }
}

void Scene::DrawBoard() {
  glDisable(GL_LIGHTING);
  glColor3f(0.5, 0, 0);
  glBegin(GL_QUADS);
  glVertex3f(5, -0.5, -5);
  glVertex3f(5, -0.5, 5);
  glVertex3f(-5, -0.5, 5);
  glVertex3f(-5, -0.5, -5);
  glEnd();
  glEnable(GL_LIGHTING);
}

bool Scene::HasBackground() const { return !background_.isNull(); }

const QImage &Scene::background() const { return background_; }

bool Scene::HasBoard() const { return board_okay_; }

aruco::BoardConfiguration Scene::board() const { return board_; }
