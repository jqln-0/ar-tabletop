#ifndef __GRAPHICS_H_
#define __GRAPHICS_H_

#include <aruco/aruco.h>
#include <QGLWidget>
#include <QtOpenGL>
#include <vector>

class SceneWidget : public QGLWidget {
	Q_OBJECT
	protected:
		// OpenGL methods for scene drawing.
		void initializeGL();
		void paintGL();
		void resizeGL(int width, int height);

		// Scene data.
		aruco::CameraParameters camera_;

		// Marker and object data.
		std::vector<aruco::Marker> markers_;

	public:
		explicit SceneWidget(QWidget *parent = nullptr);

		void set_markers(const std::vector<aruco::Marker> &markers);
		void set_camera(const aruco::CameraParameters &camera);

		void DrawCube();
};

#endif // __GRAPHICS_H_
