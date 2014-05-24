#ifndef __GRAPHICS_H_
#define __GRAPHICS_H_

#include <QGLWidget>
#include <QtOpenGL>

#include <aruco/aruco.h>

#include <memory>
#include <vector>

#include "boardwrap.h"
#include "scene.h"

class SceneWidget : public QGLWidget {
  Q_OBJECT
 protected:
  // OpenGL methods for scene rendering.
  void initializeGL();
  void paintGL();
  void resizeGL(int width, int height);

  // Camera to use to get the projection matrix.
  aruco::CameraParameters camera_;

  // Scene to use for drawing models.
  std::shared_ptr<Scene> scene_;

  // The markers detected during the last frame.
  std::vector<aruco::Marker> markers_;

 public:
  explicit SceneWidget(QWidget *parent = nullptr);

  void SetMarkers(const std::vector<aruco::Marker> &markers);
  void SetCamera(const aruco::CameraParameters &camera);
  void SetScene(std::shared_ptr<Scene> scene);
};

#endif  // __GRAPHICS_H_
