#include "scene.h"

using std::string;

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

	valid_ = true;
}

bool Model::IsValid() { return valid_; }

void Model::Render() {
	// TODO: Check if there's any additional setup we might need to do (lazy
	// loading textures?)
	RenderNode(scene_->mRootNode);
}

void Model::RenderNode(const aiNode *node) {
	// Remember previous matrix settings.
	glPushMatrix();

	// TODO: Wtf is this meant to do?
	//aiMatrix4x4 matrix = node->mTransformation;
	//glMultMatrixf((float*)&matrix);

	// Draw this node's meshes.
	for (size_t m=0; m < node->mNumMeshes; ++m) {
		const aiMesh *mesh = scene_->mMeshes[node->mMeshes[m]];

		// Apply this mesh's material.
		//ApplyMaterial(mesh->mMaterialIndex);

		// Draw this mesh's faces.
		for (size_t f=0; f < mesh->mNumFaces; ++f ) {
			const aiFace *face = &(mesh->mFaces[f]);

			if (face->mNumIndices != 3) {
				// We only expect triangles, but ignore other face types just in case.
				return;
			}

			glBegin(GL_TRIANGLES);
			for (size_t v=0; v < face->mNumIndices; ++v) {
				size_t index = face->mIndices[v];
				
				// Apply vertex coloring.
				if (mesh->HasVertexColors(index)) {
					aiColor4D *colors = mesh->mColors[index];
					glColor4f(colors->r, colors->g, colors->b, colors->a);
				}

				// Apply face normal.
				if (mesh->HasNormals()) {
					aiVector3D normals = mesh->mNormals[index];
					glNormal3f(normals.x, normals.y, normals.z);
				}

				aiVector3D vertex = mesh->mVertices[index];
				glVertex3f(vertex.x, vertex.y, vertex.z);
			}
			glEnd();
		}
	}

	// Draw any children this node has.
	for (size_t i = 0; i < node->mNumChildren; ++i) {
		RenderNode(node->mChildren[i]);
	}

	glPopMatrix();
}

void Model::ApplyMaterial(size_t index) {
	aiMaterial *material = scene_->mMaterials[index];

	// Default material colors.
	aiColor4D ambient;
	aiColor4D diffuse;
	aiColor4D specular;
	aiColor4D emission;
	
	// Try to get the model values for each component.
	material->Get(AI_MATKEY_COLOR_AMBIENT, &ambient, NULL);
	material->Get(AI_MATKEY_COLOR_DIFFUSE, &diffuse, NULL);
	material->Get(AI_MATKEY_COLOR_SPECULAR, &specular, NULL);
	material->Get(AI_MATKEY_COLOR_EMISSIVE, &emission, NULL);

	// Apply the components.
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (GLfloat*)&ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, (GLfloat*)&diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (GLfloat*)&specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, (GLfloat*)&emission);
}

Scene::Scene(QString filename) {
  QSettings config(filename, QSettings::IniFormat);
  QStringList keys = config.childKeys();

  for (auto it = keys.begin(); it != keys.end(); ++it) {
    if (it->compare("board", Qt::CaseInsensitive) == 0) {
			std::string board_path = RelativeTo(filename, config.value(*it, "").toString());
      board_.readFromFile(config.value("board", "").toString().toStdString());
    } else if (it->compare("background", Qt::CaseInsensitive) == 0) {
      // Set background.
			std::string background_path = RelativeTo(filename, config.value(*it, "").toString());
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
			std::string model_path = RelativeTo(filename, config.value(*it, "").toString());
      Model m(importer_, model_path);
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
