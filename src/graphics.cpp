#include "graphics.h"

SceneWidget::SceneWidget(QWidget *parent) :
	QGLWidget(QGLFormat(QGL::SampleBuffers), parent) {}

void SceneWidget::set_camera(const aruco::CameraParameters &camera) {
	camera_ = camera;
}

void SceneWidget::initializeGL() {
	glClearColor(0, 0, 0, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_SMOOTH);
}

void SceneWidget::paintGL() {
	// Abort painting if the camera is invalid.
	if (!camera_.isValid()) {
		return;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Reset the matrix and mode.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Use the camera's projection matrix for the scene.
	double projection_matrix[16];
	cv::Size widget_size(this->size().width(), this->size().height());
	camera_.glGetProjectionMatrix(widget_size, widget_size, projection_matrix,
			0.05, 10);
	glLoadMatrixd(projection_matrix);

	for (auto it=markers_.begin(); it != markers_.end(); ++it) {
		// Set the matrix mode and reset the matrix to the default identity matrix.
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// Calculate and use the marker's matrix.
		// TODO.
		double modelview_matrix[16];
		it->glGetModelViewMatrix(modelview_matrix);
		glLoadMatrixd(modelview_matrix);

		// Draw the marker's model.
		// TODO.
		DrawCube();
	}
}

void SceneWidget::resizeGL(int width, int height) {
	 int side = qMin(width, height);
	glViewport((width - side) / 2, (height - side) / 2, side, side);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-2, +2, -2, +2, 1.0, 15.0);
	glMatrixMode(GL_MODELVIEW);
}

void SceneWidget::set_markers(const std::vector<aruco::Marker> &markers) {
	markers_ = markers;
}

void SceneWidget::DrawCube() {
	glPushMatrix();

	glBegin(GL_POLYGON);
	qglColor(Qt::red);
	glVertex3f( -0.5, -0.5, -0.5);
	glVertex3f( -0.5,  0.5, -0.5);
	glVertex3f(  0.5,  0.5, -0.5);
	glVertex3f(  0.5, -0.5, -0.5);
	glEnd();

	glBegin(GL_POLYGON);
	qglColor(Qt::green);
	glVertex3f(  0.5, -0.5, 0.5 );
	glVertex3f(  0.5,  0.5, 0.5 );
	glVertex3f( -0.5,  0.5, 0.5 );
	glVertex3f( -0.5, -0.5, 0.5 );
	glEnd();
	 
	glBegin(GL_POLYGON);
	qglColor(Qt::blue);
	glVertex3f( 0.5, -0.5, -0.5 );
	glVertex3f( 0.5,  0.5, -0.5 );
	glVertex3f( 0.5,  0.5,  0.5 );
	glVertex3f( 0.5, -0.5,  0.5 );
	glEnd();
	 
	glBegin(GL_POLYGON);
	qglColor(Qt::yellow);
	glVertex3f( -0.5, -0.5,  0.5 );
	glVertex3f( -0.5,  0.5,  0.5 );
	glVertex3f( -0.5,  0.5, -0.5 );
	glVertex3f( -0.5, -0.5, -0.5 );
	glEnd();
	 
	glBegin(GL_POLYGON);
	qglColor(Qt::magenta);
	glVertex3f(  0.5,  0.5,  0.5 );
	glVertex3f(  0.5,  0.5, -0.5 );
	glVertex3f( -0.5,  0.5, -0.5 );
	glVertex3f( -0.5,  0.5,  0.5 );
	glEnd();
	 
	glBegin(GL_POLYGON);
	qglColor(Qt::cyan);
	glVertex3f(  0.5, -0.5, -0.5 );
	glVertex3f(  0.5, -0.5,  0.5 );
	glVertex3f( -0.5, -0.5,  0.5 );
	glVertex3f( -0.5, -0.5, -0.5 );
	glEnd();

	glPopMatrix();
}
