#include "graphics.h"

SceneWidget::SceneWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent) {}

void SceneWidget::SetCamera(const aruco::CameraParameters &camera) {
  camera_ = camera;
}

void SceneWidget::SetScene(std::shared_ptr<Scene> scene) { scene_ = scene; }

void SceneWidget::SetMarkers(const std::vector<aruco::Marker> &markers) {
  markers_ = markers;
}

void SceneWidget::initializeGL() {
  glClearColor(0, 0, 0, 0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_SMOOTH);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);

  // Setup lighting.
  GLfloat light_position[] = {1, 1, 0, 0};
  GLfloat light_ambient[] = {0.2, 0.2, 0.2, 1};
  GLfloat light_diffuse[] = {1, 1, 1, 1};
  GLfloat light_specular[] = {1, 1, 1, 1};
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
}

void SceneWidget::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Only try to render the scene if we have a valid camera to use.
  if (!camera_.isValid()) {
    return;
  }

  // Reset the matrix and mode.
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // Use the camera's projection matrix for the scene.
  double projection_matrix[16];
  cv::Size widget_size(this->size().width(), this->size().height());
  camera_.glGetProjectionMatrix(widget_size, widget_size, projection_matrix,
                                0.05, 1000);
  glLoadMatrixd(projection_matrix);

  for (auto it = markers_.begin(); it != markers_.end(); ++it) {
    // Set the matrix mode and reset the matrix to the default identity matrix.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Calculate and use the marker's matrix.
    double modelview_matrix[16];
    it->glGetModelViewMatrix(modelview_matrix);
    glLoadMatrixd(modelview_matrix);

    // Draw this marker's model.
    if (scene_) {
      scene_->DrawModel(it->id);
    }

    glPopMatrix();
  }

  // Render the board if given.
  if (scene_->GetBoard().HasBoard()) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    qglColor(Qt::magenta);

    // Set the board's model matrix.
    double modelview_matrix[16];
    scene_->GetBoard().GetBoard().glGetModelViewMatrix(modelview_matrix);
    glLoadMatrixd(modelview_matrix);

    scene_->DrawBoard();

    glPopMatrix();
  }
}

void SceneWidget::resizeGL(int width, int height) {
  // TODO: How much of this is needed? OpenGL is a mystery.
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, this->width(), 0, this->height(), -1.0, 1.0);
  glViewport(0, 0, this->width(), this->height());
}
