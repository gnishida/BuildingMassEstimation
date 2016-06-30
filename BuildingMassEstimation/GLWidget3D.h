#pragma once

#include <glew.h>
#include "Shader.h"
#include "Vertex.h"
#include <QGLWidget>
#include <QMouseEvent>
#include <QTimer>
#include "Camera.h"
#include "ShadowMapping.h"
#include "RenderManager.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class MainWindow;

class GLWidget3D : public QGLWidget {
public:
	GLWidget3D(MainWindow *parent = 0);

	void updateStatusBar();
	void drawScene();
	void render();
	void loadCGA(const std::string& cga_filename);
	void generateTrainingData(const QString& cga_dir, const QString& out_dir, int numSamples, int image_width, int image_height, bool grayscale, bool centering3D, bool centering, bool centeringNoise, float centeringNoiseMax, bool meanGeneration, int cameraType, float cameraDistanceBase, float cameraHeight, int xrotMin, int xrotMax, int yrotMin, int yrotMax, int fovMin, int fovMax, bool modifyImage, int lineWidthMin, int lineWidthMax, bool edgeNoise, float edgeNoiseMax);
	void visualizePredictedData(const QString& cga_dir, const QString& out_dir, bool centering3D, int cameraType, float cameraDistanceBase, float cameraHeight, int xrotMin, int xrotMax, int yrotMin, int yrotMax, int fovMin, int fovMax);
	bool moveCenter(cv::Mat& img);
	void translateImage(cv::Mat source, cv::Mat& target, int shift_x, int shift_y);

	void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);

protected:
	void initializeGL();
	void resizeGL(int width, int height);
	//void paintGL();
	void paintEvent(QPaintEvent* e);
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void wheelEvent(QWheelEvent* e);

public:
	static enum { RENDERING_MODE_REGULAR = 0, RENDERING_MODE_LINE };

public:
	MainWindow* mainWin;
	Camera camera;
	glm::vec3 light_dir;
	glm::mat4 light_mvpMatrix;
	bool shiftPressed;

	RenderManager renderManager;
	QImage bgImage;
	glm::vec2 lastPos;
	glm::vec2 curPos;
	std::vector<std::pair<glm::vec2, glm::vec2> > v_lines;
	bool dragging;

};

