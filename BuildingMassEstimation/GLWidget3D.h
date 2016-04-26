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

class ViewpointExample {
public:
	std::vector<float> param_values;
	std::vector<std::pair<glm::vec2, glm::vec2>> contour;
	bool ambiguous;

public:
	ViewpointExample(const std::vector<float>& param_values, std::vector<std::pair<glm::vec2, glm::vec2>>& contour);

	bool isSimilar(const ViewpointExample& other);
};

class Viewpoint {
public:
	int snippet_id;
	Camera camera;
	bool ambiguous;
	std::vector<ViewpointExample> examples;

public:
	Viewpoint(int snippet_id, const Camera& camera);

	//bool isSimilar(const Viewpoint& other);
};

class GLWidget3D : public QGLWidget {
public:
	GLWidget3D(MainWindow *parent = 0);

	void updateStatusBar();
	void loadImage(const std::string& filename);
	void detectEdges(const cv::Mat& img);
	void saveContour(const std::string& filename);
	void extractCameraParameters();
	cv::Mat extractCameraIntrinsicParameters(std::vector<glm::vec2>& vp);
	cv::Mat extractCameraExtrinsicParameters(std::vector<glm::vec2>& vp, const cv::Mat& K);
	std::vector<glm::vec2> computeVanishingPoints(const std::vector<std::pair<glm::vec2, glm::vec2> >& v_lines);
	void extractContourVectors(std::vector<std::pair<glm::vec2, glm::vec2>>& edges);
	std::vector<std::pair<glm::vec2, glm::vec2> > getLeftmostHLines(const std::vector<std::pair<glm::vec2, glm::vec2> >& lines);
	std::vector<std::pair<glm::vec2, glm::vec2> > getRightmostHLines(const std::vector<std::pair<glm::vec2, glm::vec2> >& lines);
	std::pair<glm::vec2, glm::vec2> getLeftmostVLine(const std::vector<std::pair<glm::vec2, glm::vec2> >& lines);
	std::pair<glm::vec2, glm::vec2> getRightmostVLine(const std::vector<std::pair<glm::vec2, glm::vec2> >& lines);
	void drawScene();
	void render();
	void loadCGA(const std::string& cga_filename);
	void generateTrainingData(const QString& cga_dir, const QString& out_dir, int numSamples, int image_width, int image_height, bool grayscale, bool centering, int cameraType, float cameraDistanceBase, float cameraHeight, int xrotMin, int xrotMax, int yrotMin, int yrotMax, int fovMin, int fovMax);
	void generateTrainingDataWithoutAmgiousViewpoints(const QString& cga_dir, const QString& out_dir, int numSamples, int image_width, int image_height, bool grayscale, bool centering, int xrotMin, int xrotMax, int yrotMin, int yrotMax);
	void visualizePredictedData(const QString& cga_dir, const QString& out_dir, int cameraType, float cameraDistanceBase, float cameraHeight, int xrotMin, int xrotMax, int yrotMin, int yrotMax, int fovMin, int fovMax);
	void runMCMC(const std::string& cga_filename, const std::string& target_filename, int numIterations);
	void runMCMCAll(const std::string& cga_dir, int numIterations);
	bool moveCenter(cv::Mat& img);
	void fixCamera();

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

