#include "GLWidget3D.h"
#include "MainWindow.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <map>
#include "GLUtils.h"
#include <QDir>
#include <QTextStream>
#include <iostream>
#include <QProcess>
#include "Utils.h"
#include "MCMC.h"
#include "GrammarParser.h"
#include "CGA.h"
#include "Rectangle.h"
#include "MCMCConfigDialog.h"
#include <fstream>
#include <opencv2/core.hpp>
#include <QFileDialog>

#ifndef M_PI
#define	M_PI	3.141592653
#endif

ViewpointExample::ViewpointExample(const std::vector<float>& param_values, std::vector<std::pair<glm::vec2, glm::vec2>>& contour) {
	this->param_values = param_values;
	this->contour = contour;
	this->ambiguous = false;
}

bool ViewpointExample::isSimilar(const ViewpointExample& other) {
	if (contour.size() != other.contour.size()) return false;

	// compute the bounding box
	glutils::BoundingBox bbox1;
	for (int i = 0; i < contour.size(); ++i) {
		bbox1.addPoint(contour[i].first);
	}
	glutils::BoundingBox bbox2;
	for (int i = 0; i < other.contour.size(); ++i) {
		bbox2.addPoint(other.contour[i].first);
	}

	// compute the transform of the other to make it same size as bbox1
	float scale_x = bbox1.sx() / bbox2.sx();
	float scale_y = bbox1.sy() / bbox2.sy();
	float trans_x = bbox1.minPt.x - bbox2.minPt.x * scale_x;
	float trans_y = bbox1.minPt.y - bbox2.minPt.y * scale_y;

	float min_diff = std::numeric_limits<float>::max();

	for (int i = 0; i < other.contour.size(); ++i) {
		float diff = 0.0f;

		for (int k = 0; k < contour.size(); ++k) {
			// compute the corresponding coordinates of the vertex of the other
			glm::vec2 p2;
			p2.x = other.contour.at((i + k) % contour.size()).first.x * scale_x + trans_x;
			p2.y = other.contour.at((i + k) % contour.size()).first.y * scale_y + trans_y;

			diff += glm::length(contour[k].first - p2);
		}

		if (diff < min_diff) {
			min_diff = diff;
		}
	}

	if (min_diff / contour.size() / bbox1.sx() < 0.09) return true;
	else return false;
}

Viewpoint::Viewpoint(int snippet_id, const Camera& camera) {
	this->snippet_id = snippet_id;
	this->camera = camera;
	this->ambiguous = false;
}

GLWidget3D::GLWidget3D(MainWindow *parent) : QGLWidget(QGLFormat(QGL::SampleBuffers)) {
	this->mainWin = parent;
	shiftPressed = false;
	dragging = false;

	// This is necessary to prevent the screen overdrawn by OpenGL
	setAutoFillBackground(false);

	// 光源位置をセット
	// ShadowMappingは平行光源を使っている。この位置から原点方向を平行光源の方向とする。
	light_dir = glm::normalize(glm::vec3(-4, -5, -8));

	// シャドウマップ用のmodel/view/projection行列を作成
	glm::mat4 light_pMatrix = glm::ortho<float>(-50, 50, -50, 50, 0.1, 200);
	glm::mat4 light_mvMatrix = glm::lookAt(-light_dir * 50.0f, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	light_mvpMatrix = light_pMatrix * light_mvMatrix;
}

void GLWidget3D::updateStatusBar() {
	QString msg = QString("xrot: %1, yrot: %2, zrot: %3, pos: (%4, %5, %6)").arg(camera.xrot).arg(camera.yrot).arg(camera.zrot).arg(camera.pos.x).arg(camera.pos.y).arg(camera.pos.z);
	mainWin->statusBar()->showMessage(msg);
}

void GLWidget3D::loadImage(const std::string& filename) {
	QImage img;
	img.load(filename.c_str());
	bgImage = img.scaled(width(), height());

	cv::Mat m = cv::Mat(bgImage.height(), bgImage.width(), CV_8UC4, bgImage.bits(), bgImage.bytesPerLine()).clone();
	detectEdges(m);

	update();
}

void GLWidget3D::detectEdges(const cv::Mat& img) {
	cv::Mat grayImage;

	cv::cvtColor(img, grayImage, CV_BGR2GRAY);
	cv::blur(grayImage, grayImage, cv::Size(3, 3));
	cv::Canny(grayImage, grayImage, 50, 200, 3);
	cv::imwrite("test.png", grayImage);

	std::vector<cv::Vec4i> lines;
	HoughLinesP(grayImage, lines, 1, CV_PI / 180, 50, 100, 20);

	cv::Mat test(grayImage.size(), grayImage.type(), cv::Scalar(255));
	for (size_t i = 0; i < lines.size(); i++) {
		cv::line(test, cv::Point(lines[i][0], lines[i][1]), cv::Point(lines[i][2], lines[i][3]), cv::Scalar(0), 3, CV_AA);
	}
	cv::imwrite("test2.png", test);
}

void GLWidget3D::saveContour(const std::string& filename) {
	/*
	std::ofstream out(filename);
	for (auto v_line : v_lines) {
		out << v_line.first.x << "\t" << v_line.first.y << "\t" << v_line.second.x << "\t" << v_line.second.y << "\n";
	}
	out.close();
	*/

	cv::Mat img(height(), width(), CV_8UC3, cv::Scalar(255, 255, 255));
	for (auto line : v_lines) {
		cv::line(img, cv::Point(line.first.x, line.first.y), cv::Point(line.second.x, line.second.y), cv::Scalar(0, 0, 0), 1);
	}
	cv::imwrite(filename, img);
}

void GLWidget3D::extractCameraParameters() {
	std::vector<glm::vec2> vp = computeVanishingPoints(v_lines);

	cv::Mat K = extractCameraIntrinsicParameters(vp);
	cv::Mat R33 = extractCameraExtrinsicParameters(vp, K);

	cv::Mat R;
	cv::Rodrigues(R33, R);

	std::cout << "R33: " << std::endl;
	std::cout << R33 << std::endl;
	std::cout << "R: " << std::endl;
	std::cout << R << std::endl;
}

cv::Mat GLWidget3D::extractCameraIntrinsicParameters(std::vector<glm::vec2>& vp) {
	vp[0] = glm::vec2(1344.35, 543.089);
	vp[1] = glm::vec2(-186.516, 558.918);
	vp[2] = glm::vec2(500.968, -2966.13);

	cv::Mat_<double> A(3, 4);
	A(0, 0) = vp[0].x * vp[1].x + vp[0].y * vp[1].y;
	A(0, 1) = vp[1].x + vp[0].x;
	A(0, 2) = vp[1].y + vp[0].y;
	A(0, 3) = 1;

	A(1, 0) = vp[0].x * vp[2].x + vp[0].y * vp[2].y;
	A(1, 1) = vp[2].x + vp[0].x;
	A(1, 2) = vp[2].y + vp[0].y;
	A(1, 3) = 1;

	A(2, 0) = vp[2].x * vp[1].x + vp[2].y * vp[1].y;
	A(2, 1) = vp[1].x + vp[2].x;
	A(2, 2) = vp[1].y + vp[2].y;
	A(2, 3) = 1;


	cv::Mat dst;
	cv::SVD::solveZ(A, dst);

	cv::Mat_<double> W(3, 3);

	W(0, 0) = dst.at<double>(0, 0);
	W(0, 1) = 0;
	W(0, 2) = dst.at<double>(1, 0);

	W(1, 0) = 0;
	W(1, 1) = dst.at<double>(0, 0);
	W(1, 2) = dst.at<double>(2, 0);

	W(2, 0) = dst.at<double>(1, 0);
	W(2, 1) = dst.at<double>(2, 0);
	W(2, 2) = dst.at<double>(3, 0);

	if (W(2, 2) < 0) W = -W;
	std::cout << W << std::endl;



	cv::Mat chol = W.clone();

	if (Cholesky(chol.ptr<double>(), chol.step, chol.cols, 0, 0, 0)) {
		cv::Mat diagElem = chol.diag();

		for (int e = 0; e < diagElem.rows; ++e)	{
			float elem = diagElem.at<double>(e);
			chol.row(e) *= elem;
			chol.at<double>(e, e) = 1.0f / elem;
		}
	}
	chol.at<double>(2, 0) = 0;
	chol.at<double>(2, 1) = 0;

	std::cout << "chol:" << std::endl;
	std::cout << chol << std::endl;

	cv::Mat K = chol.inv(cv::DECOMP_SVD);
	K.at<double>(2, 2) = 1;
	std::cout << "K:" << std::endl;
	std::cout << K << std::endl;

	return K;
}

cv::Mat GLWidget3D::extractCameraExtrinsicParameters(std::vector<glm::vec2>& vp, const cv::Mat& K) {
	cv::Mat invK = K.inv(cv::DECOMP_SVD);

	cv::Mat_<double> v(3, 3);
	v(0, 0) = vp[0].x;
	v(1, 0) = vp[0].y;
	v(2, 0) = 1;
	v(0, 1) = vp[1].x;
	v(1, 1) = vp[1].y;
	v(2, 1) = 1;
	v(0, 2) = vp[2].x;
	v(1, 2) = vp[2].y;
	v(2, 2) = 1;


	cv::Mat R = invK * v;
	double lambda1 = sqrt(R.at<double>(0, 0) * R.at<double>(0, 0) + R.at<double>(1, 0) * R.at<double>(1, 0) + R.at<double>(2, 0) * R.at<double>(2, 0));
	double lambda2 = sqrt(R.at<double>(0, 1) * R.at<double>(0, 1) + R.at<double>(1, 1) * R.at<double>(1, 1) + R.at<double>(2, 1) * R.at<double>(2, 1));
	double lambda3 = sqrt(R.at<double>(0, 2) * R.at<double>(0, 2) + R.at<double>(1, 2) * R.at<double>(1, 2) + R.at<double>(2, 2) * R.at<double>(2, 2));

	R.at<double>(0, 0) /= lambda1;
	R.at<double>(1, 0) /= lambda1;
	R.at<double>(2, 0) /= lambda1;
	R.at<double>(0, 1) /= lambda2;
	R.at<double>(1, 1) /= lambda2;
	R.at<double>(2, 1) /= lambda2;
	R.at<double>(0, 2) /= lambda3;
	R.at<double>(1, 2) /= lambda3;
	R.at<double>(2, 2) /= lambda3;

	return R;
}

std::vector<glm::vec2> GLWidget3D::computeVanishingPoints(const std::vector<std::pair<glm::vec2, glm::vec2> >& v_lines) {
	std::vector<glm::vec2> vp(3);

	// 最も左の縦の線と、最も右の縦の線
	std::pair<glm::vec2, glm::vec2> left_vline = getLeftmostVLine(v_lines);
	std::pair<glm::vec2, glm::vec2> right_vline = getRightmostVLine(v_lines);

	float tab, tcd;
	utils::segmentSegmentIntersect(left_vline.first, left_vline.second, right_vline.first, right_vline.second, &tab, &tcd, false, vp[2]);

	std::vector<std::pair<glm::vec2, glm::vec2> > left_hlines = getLeftmostHLines(v_lines);
	std::vector<std::pair<glm::vec2, glm::vec2> > right_hlines = getRightmostHLines(v_lines);

	utils::segmentSegmentIntersect(left_hlines[0].first, left_hlines[0].second, left_hlines[1].first, left_hlines[1].second, &tab, &tcd, false, vp[1]);
	utils::segmentSegmentIntersect(right_hlines[0].first, right_hlines[0].second, right_hlines[1].first, right_hlines[1].second, &tab, &tcd, false, vp[0]);

	std::cout << "-----------------------------------------------" << std::endl;
	std::cout << "(" << vp[0].x << "," << vp[0].y << ")" << std::endl;
	std::cout << "(" << vp[1].x << "," << vp[1].y << ")" << std::endl;
	std::cout << "(" << vp[2].x << "," << vp[2].y << ")" << std::endl;

	return vp;
}

std::vector<std::pair<glm::vec2, glm::vec2> > GLWidget3D::getLeftmostHLines(const std::vector<std::pair<glm::vec2, glm::vec2> >& lines) {
	float min_x1 = std::numeric_limits<float>::max();
	float min_x2 = std::numeric_limits<float>::max();
	std::vector<std::pair<glm::vec2, glm::vec2> > ret_lines(2);

	for (auto line : lines) {
		if (fabs(line.first.x - line.second.x) > fabs(line.first.y - line.second.y) && fabs(line.first.x - line.second.x) > 50) {
			if (std::min(line.first.x, line.second.x) < min_x1) {
				min_x2 = min_x1;
				ret_lines[1] = ret_lines[0];
				min_x1 = std::min(line.first.x, line.second.x);
				ret_lines[0] = line;
			}
			else if (std::min(line.first.x, line.second.x) < min_x2) {
				min_x2 = std::min(line.first.x, line.second.x);
				ret_lines[1] = line;
			}
		}
	}

	return ret_lines;
}

std::vector<std::pair<glm::vec2, glm::vec2> > GLWidget3D::getRightmostHLines(const std::vector<std::pair<glm::vec2, glm::vec2> >& lines) {
	float max_x1 = -std::numeric_limits<float>::max();
	float max_x2 = -std::numeric_limits<float>::max();
	std::vector<std::pair<glm::vec2, glm::vec2> > ret_lines(2);

	for (auto line : lines) {
		if (fabs(line.first.x - line.second.x) > fabs(line.first.y - line.second.y) && fabs(line.first.x - line.second.x) > 50) {
			if (std::max(line.first.x, line.second.x) > max_x1) {
				max_x2 = max_x1;
				ret_lines[1] = ret_lines[0];
				max_x1 = std::max(line.first.x, line.second.x);
				ret_lines[0] = line;
			}
			else if (std::max(line.first.x, line.second.x) > max_x2) {
				max_x2 = std::max(line.first.x, line.second.x);
				ret_lines[1] = line;
			}
		}
	}

	return ret_lines;
}

std::pair<glm::vec2, glm::vec2> GLWidget3D::getLeftmostVLine(const std::vector<std::pair<glm::vec2, glm::vec2> >& lines) {
	float min_x = std::numeric_limits<float>::max();
	std::pair<glm::vec2, glm::vec2> ret_line;

	for (auto line : lines) {
		if (fabs(line.first.x - line.second.x) < fabs(line.first.y - line.second.y)) {
			if (std::min(line.first.x, line.second.x) < min_x) {
				min_x = std::min(line.first.x, line.second.x);
				ret_line = line;
			}
		}
	}

	return ret_line;
}

std::pair<glm::vec2, glm::vec2> GLWidget3D::getRightmostVLine(const std::vector<std::pair<glm::vec2, glm::vec2> >& lines) {
	float max_x = -std::numeric_limits<float>::max();
	std::pair<glm::vec2, glm::vec2> ret_line;

	for (auto line : lines) {
		if (fabs(line.first.x - line.second.x) < fabs(line.first.y - line.second.y)) {
			if (std::max(line.first.x, line.second.x) > max_x) {
				max_x = std::max(line.first.x, line.second.x);
				ret_line = line;
			}
		}
	}

	return ret_line;
}

void GLWidget3D::extractContourVectors(std::vector<std::pair<glm::vec2, glm::vec2>>& edges) {
	QImage img = this->grabFrameBuffer();
	cv::Mat mat = cv::Mat(img.height(), img.width(), CV_8UC4, img.bits(), img.bytesPerLine()).clone();
	cv::cvtColor(mat, mat, CV_BGRA2GRAY);

#if 0
	cv::imwrite("contour0.png", mat);
#endif

	cv::threshold(mat, mat, 128, 255, cv::THRESH_BINARY_INV);
	
	std::vector<cv::Vec4i> lines;
	cv::HoughLinesP(mat, lines, 1, CV_PI / 180, 10, 10, 10);

	// HoughLinesの結果を、edgesリストにコピー
	edges.resize(lines.size());
	for (int i = 0; i < lines.size(); ++i) {
		edges[i].first = glm::vec2(lines[i][0], lines[i][1]);
		edges[i].second = glm::vec2(lines[i][2], lines[i][3]);
	}

#if 0
	cv::Mat result(mat.size(), CV_8UC3, cv::Scalar(255, 255, 255));
	std::cout << "contour lines (" << edges.size() << "): " << std::endl;
	for (int i = 0; i < edges.size(); ++i) {
		std::cout << "(" << edges[i].first.x << "," << edges[i].first.y << ") - (" << edges[i].second.x << "," << edges[i].second.y << ")" << std::endl;
		cv::Scalar color(rand() % 250, rand() % 250, rand() % 250);
		cv::circle(result, cv::Point(edges[i].first.x, edges[i].first.y), 6, color, -1);
		cv::circle(result, cv::Point(edges[i].second.x, edges[i].second.y), 6, color, -1);
		cv::line(result, cv::Point(edges[i].first.x, edges[i].first.y), cv::Point(edges[i].second.x, edges[i].second.y), color, 2);
	}
	std::cout << std::endl;
	cv::imwrite("contour.png", result);
#endif

	utils::cleanEdges(edges, 20, 5.0 / 180.0 * M_PI);
#if 0
	cv::Mat result2(mat.size(), CV_8UC3, cv::Scalar(255, 255, 255));
	for (int i = 0; i < edges.size(); ++i) {
		std::cout << "(" << edges[i].first.x << "," << edges[i].first.y << ") - (" << edges[i].second.x << "," << edges[i].second.y << ")" << std::endl;
		cv::line(result2, cv::Point(edges[i].first.x, edges[i].first.y), cv::Point(edges[i].second.x, edges[i].second.y), cv::Scalar(rand() % 250, rand() % 250, rand() % 250), 3);
	}
	std::cout << std::endl;
	cv::imwrite("contour2.png", result2);
#endif

	utils::cleanContours(edges, 80, 10.0 / 180.0 * M_PI);
#if 0
	cv::Mat result3(mat.size(), CV_8UC3, cv::Scalar(255, 255, 255));
	for (int i = 0; i < edges.size(); ++i) {
		std::cout << "(" << edges[i].first.x << "," << edges[i].first.y << ") - (" << edges[i].second.x << "," << edges[i].second.y << ")" << std::endl;
		cv::Scalar color(rand() % 250, rand() % 250, rand() % 250);
		cv::circle(result3, cv::Point(edges[i].first.x, edges[i].first.y), 6, color, -1);
		cv::circle(result3, cv::Point(edges[i].second.x, edges[i].second.y), 6, color, -1);
		cv::line(result3, cv::Point(edges[i].first.x, edges[i].first.y), cv::Point(edges[i].second.x, edges[i].second.y), color, 2);
	}
	std::cout << std::endl;
	cv::imwrite("contour3.png", result3);
#endif






}



/**
 * This event handler is called when the mouse press events occur.
 */
void GLWidget3D::mousePressEvent(QMouseEvent *e) {
	if (e->buttons() & Qt::RightButton) {
		camera.mousePress(e->x(), e->y());
	}
	else if (e->button() & Qt::LeftButton) {
		lastPos = glm::vec2(e->x(), e->y());
		curPos = glm::vec2(e->x(), e->y());
		dragging = true;
	}
}

/**
 * This event handler is called when the mouse release events occur.
 */
void GLWidget3D::mouseReleaseEvent(QMouseEvent *e) {
	if (e->button() == Qt::LeftButton) {
		v_lines.push_back(std::make_pair(lastPos, curPos));
		dragging = false;
	}
}

/**
 * This event handler is called when the mouse move events occur.
 */
void GLWidget3D::mouseMoveEvent(QMouseEvent *e) {
	if (e->buttons() & Qt::RightButton) { // Rotate
		if (shiftPressed) { // Move
			camera.move(e->x(), e->y());
		}
		else {
			camera.rotate(e->x(), e->y());
		}
	}
	else if (e->buttons() & Qt::LeftButton) {
		curPos = glm::vec2(e->x(), e->y());
	}
	updateStatusBar();
	update();
}

void GLWidget3D::wheelEvent(QWheelEvent* e) {
	camera.zoom(e->delta() * 0.01);
	updateStatusBar();
	update();
}

/**
 * This function is called once before the first call to paintGL() or resizeGL().
 */
void GLWidget3D::initializeGL() {
	// init glew
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
	}

	if (glewIsSupported("GL_VERSION_4_2"))
		printf("Ready for OpenGL 4.2\n");
	else {
		printf("OpenGL 4.2 not supported\n");
		exit(1);
	}
	const GLubyte* text = glGetString(GL_VERSION);
	printf("VERSION: %s\n", text);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_3D);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glDisable(GL_TEXTURE_3D);

	glEnable(GL_TEXTURE_2D_ARRAY);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glDisable(GL_TEXTURE_2D_ARRAY);

	////////////////////////////////
	renderManager.init("", "", "", true, 8192);
	renderManager.resize(this->width(), this->height());
	renderManager.renderingMode = RenderManager::RENDERING_MODE_LINE;

	glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "tex0"), 0);//tex0: 0

	fixCamera();

	std::vector<Vertex> vertices;
	glutils::drawBox(30, 30, 15, glm::vec4(1, 1, 1, 1), glm::translate(glm::rotate(glm::mat4(), -(float)M_PI * 0.5f, glm::vec3(1, 0, 0)), glm::vec3(0, 0, 7.5)), vertices);
	glutils::drawGrid(30, 30, 2, glm::vec4(0, 0, 1, 1), glm::vec4(0.95, 0.95, 1, 1), glm::rotate(glm::mat4(), -(float)M_PI * 0.5f, glm::vec3(1, 0, 0)), vertices);
	renderManager.addObject("box", "", vertices, true);

	updateStatusBar();
}

/**
 * This function is called whenever the widget has been resized.
 */
void GLWidget3D::resizeGL(int width, int height) {
	height = height ? height : 1;
	glViewport(0, 0, width, height);
	camera.updatePMatrix(width, height);

	renderManager.resize(width, height);
}

/**
 * This function is called whenever the widget needs to be painted.
 */
void GLWidget3D::paintEvent(QPaintEvent* e) {
	// OpenGLで描画
	makeCurrent();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	
	render();

	// REMOVE
	glActiveTexture(GL_TEXTURE0);
	
	// OpenGLの設定を元に戻す
	glShadeModel(GL_FLAT);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	// draw sketch
	QPainter painter(this);
	painter.setOpacity(0.6);
	painter.drawImage(0, 0, bgImage);

	painter.setPen(QPen(Qt::black, 3));
	for (auto v_line : v_lines) {
		painter.drawLine(v_line.first.x, v_line.first.y, v_line.second.x, v_line.second.y);
	}
	if (dragging) {
		painter.drawLine(lastPos.x, lastPos.y, curPos.x, curPos.y);
	}
	painter.end();

	glEnable(GL_DEPTH_TEST);
}

/**
 * Draw the scene.
 */
void GLWidget3D::drawScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(true);

	renderManager.renderAll();
}

void GLWidget3D::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// PASS 1: Render to texture
	glUseProgram(renderManager.programs["pass1"]);

	glBindFramebuffer(GL_FRAMEBUFFER, renderManager.fragDataFB);
	glClearColor(0.95, 0.95, 0.95, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderManager.fragDataTex[0], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, renderManager.fragDataTex[1], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, renderManager.fragDataTex[2], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, renderManager.fragDataTex[3], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, renderManager.fragDepthTex, 0);

	// Set the list of draw buffers.
	GLenum DrawBuffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, DrawBuffers); // "3" is the size of DrawBuffers
	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("+ERROR: GL_FRAMEBUFFER_COMPLETE false\n");
		exit(0);
	}

	glUniformMatrix4fv(glGetUniformLocation(renderManager.programs["pass1"], "mvpMatrix"), 1, false, &camera.mvpMatrix[0][0]);
	glUniform3f(glGetUniformLocation(renderManager.programs["pass1"], "lightDir"), light_dir.x, light_dir.y, light_dir.z);
	glUniformMatrix4fv(glGetUniformLocation(renderManager.programs["pass1"], "light_mvpMatrix"), 1, false, &light_mvpMatrix[0][0]);

	glUniform1i(glGetUniformLocation(renderManager.programs["pass1"], "shadowMap"), 6);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, renderManager.shadow.textureDepth);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	drawScene();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// PASS 2: Create AO
	if (renderManager.renderingMode == RenderManager::RENDERING_MODE_SSAO) {
		glUseProgram(renderManager.programs["ssao"]);
		glBindFramebuffer(GL_FRAMEBUFFER, renderManager.fragDataFB_AO);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderManager.fragAOTex, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, renderManager.fragDepthTex_AO, 0);
		GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Always check that our framebuffer is ok
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			printf("++ERROR: GL_FRAMEBUFFER_COMPLETE false\n");
			exit(0);
		}

		glDisable(GL_DEPTH_TEST);
		glDepthFunc(GL_ALWAYS);

		glUniform2f(glGetUniformLocation(renderManager.programs["ssao"], "pixelSize"), 2.0f / this->width(), 2.0f / this->height());

		glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "tex0"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[0]);

		glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "tex1"), 2);
		glActiveTexture(GL_TEXTURE2);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[1]);

		glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "tex2"), 3);
		glActiveTexture(GL_TEXTURE3);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[2]);

		glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "depthTex"), 8);
		glActiveTexture(GL_TEXTURE8);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDepthTex);

		glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "noiseTex"), 7);
		glActiveTexture(GL_TEXTURE7);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragNoiseTex);

		{
			glUniformMatrix4fv(glGetUniformLocation(renderManager.programs["ssao"], "mvpMatrix"), 1, false, &camera.mvpMatrix[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(renderManager.programs["ssao"], "pMatrix"), 1, false, &camera.pMatrix[0][0]);
		}

		glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "uKernelSize"), renderManager.uKernelSize);
		glUniform3fv(glGetUniformLocation(renderManager.programs["ssao"], "uKernelOffsets"), renderManager.uKernelOffsets.size(), (const GLfloat*)renderManager.uKernelOffsets.data());

		glUniform1f(glGetUniformLocation(renderManager.programs["ssao"], "uPower"), renderManager.uPower);
		glUniform1f(glGetUniformLocation(renderManager.programs["ssao"], "uRadius"), renderManager.uRadius);

		glBindVertexArray(renderManager.secondPassVAO);

		glDrawArrays(GL_QUADS, 0, 4);
		glBindVertexArray(0);
		glDepthFunc(GL_LEQUAL);
	}
	else if (renderManager.renderingMode == RenderManager::RENDERING_MODE_LINE || renderManager.renderingMode == RenderManager::RENDERING_MODE_HATCHING) {
		glUseProgram(renderManager.programs["line"]);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);
		glDepthFunc(GL_ALWAYS);

		glUniform2f(glGetUniformLocation(renderManager.programs["line"], "pixelSize"), 1.0f / this->width(), 1.0f / this->height());
		glUniformMatrix4fv(glGetUniformLocation(renderManager.programs["line"], "pMatrix"), 1, false, &camera.pMatrix[0][0]);
		if (renderManager.renderingMode == RenderManager::RENDERING_MODE_LINE) {
			glUniform1i(glGetUniformLocation(renderManager.programs["line"], "useHatching"), 0);
		}
		else {
			glUniform1i(glGetUniformLocation(renderManager.programs["line"], "useHatching"), 1);
		}

		glUniform1i(glGetUniformLocation(renderManager.programs["line"], "tex0"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[0]);

		glUniform1i(glGetUniformLocation(renderManager.programs["line"], "tex1"), 2);
		glActiveTexture(GL_TEXTURE2);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[1]);

		glUniform1i(glGetUniformLocation(renderManager.programs["line"], "tex2"), 3);
		glActiveTexture(GL_TEXTURE3);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[2]);

		glUniform1i(glGetUniformLocation(renderManager.programs["line"], "tex3"), 4);
		glActiveTexture(GL_TEXTURE4);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[3]);

		glUniform1i(glGetUniformLocation(renderManager.programs["line"], "depthTex"), 8);
		glActiveTexture(GL_TEXTURE8);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDepthTex);

		glUniform1i(glGetUniformLocation(renderManager.programs["line"], "hatchingTexture"), 5);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_3D, renderManager.hatchingTextures);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glBindVertexArray(renderManager.secondPassVAO);

		glDrawArrays(GL_QUADS, 0, 4);
		glBindVertexArray(0);
		glDepthFunc(GL_LEQUAL);
	}
	else if (renderManager.renderingMode == RenderManager::RENDERING_MODE_CONTOUR) {
		glUseProgram(renderManager.programs["contour"]);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);
		glDepthFunc(GL_ALWAYS);

		glUniform2f(glGetUniformLocation(renderManager.programs["contour"], "pixelSize"), 1.0f / this->width(), 1.0f / this->height());

		glUniform1i(glGetUniformLocation(renderManager.programs["contour"], "depthTex"), 8);
		glActiveTexture(GL_TEXTURE8);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDepthTex);

		glBindVertexArray(renderManager.secondPassVAO);

		glDrawArrays(GL_QUADS, 0, 4);
		glBindVertexArray(0);
		glDepthFunc(GL_LEQUAL);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Blur

	if (renderManager.renderingMode == RenderManager::RENDERING_MODE_BASIC || renderManager.renderingMode == RenderManager::RENDERING_MODE_SSAO) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		qglClearColor(QColor(0xFF, 0xFF, 0xFF));
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);
		glDepthFunc(GL_ALWAYS);

		glUseProgram(renderManager.programs["blur"]);
		glUniform2f(glGetUniformLocation(renderManager.programs["blur"], "pixelSize"), 2.0f / this->width(), 2.0f / this->height());
		//printf("pixelSize loc %d\n", glGetUniformLocation(vboRenderManager.programs["blur"], "pixelSize"));

		glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "tex0"), 1);//COLOR
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[0]);

		glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "tex1"), 2);//NORMAL
		glActiveTexture(GL_TEXTURE2);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[1]);

		/*glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "tex2"), 3);
		glActiveTexture(GL_TEXTURE3);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[2]);*/

		glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "depthTex"), 8);
		glActiveTexture(GL_TEXTURE8);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDepthTex);

		glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "tex3"), 4);//AO
		glActiveTexture(GL_TEXTURE4);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragAOTex);

		if (renderManager.renderingMode == RenderManager::RENDERING_MODE_SSAO) {
			glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "ssao_used"), 1); // ssao used
		}
		else {
			glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "ssao_used"), 0); // no ssao
		}

		glBindVertexArray(renderManager.secondPassVAO);

		glDrawArrays(GL_QUADS, 0, 4);
		glBindVertexArray(0);
		glDepthFunc(GL_LEQUAL);

	}

	// REMOVE
	glActiveTexture(GL_TEXTURE0);
}

void GLWidget3D::loadCGA(const std::string& cga_filename) {
	cga::Grammar grammar;
	cga::parseGrammar(cga_filename.c_str(), grammar);

	cga::CGA cga;
	cga.modelMat = glm::rotate(glm::mat4(), -3.1415926f * 0.5f, glm::vec3(1, 0, 0));

	renderManager.removeObjects();

	// set axiom
	cga::Rectangle* start = new cga::Rectangle("Start", "", glm::translate(glm::rotate(glm::mat4(), -3.141592f * 0.5f, glm::vec3(1, 0, 0)), glm::vec3(0, 0, 0)), glm::mat4(), 0, 0, glm::vec3(1, 1, 1));
	cga.stack.push_back(boost::shared_ptr<cga::Shape>(start));

	// generate 3d model
	cga.derive(grammar, true);
	std::vector<boost::shared_ptr<glutils::Face> > faces;
	cga.generateGeometry(faces);
	renderManager.addFaces(faces, true);

	renderManager.updateShadowMap(this, light_dir, light_mvpMatrix);

	// render 2d image
	render();
}

/**
* @param cga_dir			directory of CGA files
* @param out_dir			directory for the output files
* @param numSamples		number of samples per viewpoint
* @param image_width		image width
* @param image_height		image height
* @param grayscale			true if the output file is to be grayscale
* @param cameraTYpe		0 -- street view / 1 -- aerial view
* @param cameraDistance	distance to the camera
* @param cameraHeight		height of the camera
* @param xrotMean			mean of xrot
* @param xrotRange			range of xrot
* @param yrotMean			mean of yrot
* @param yrotRange			range of yrot
* @param fovMin			min of fov
* @param fovMax			max of fov
*/
void GLWidget3D::generateTrainingData(const QString& cga_dir, const QString& out_dir, int numSamples, int image_width, int image_height, bool grayscale, bool centering, int cameraType, float cameraDistance, float cameraHeight, int xrotMin, int xrotMax, int yrotMin, int yrotMax, int fovMin, int fovMax) {
	if (QDir(out_dir).exists()) {
		std::cout << "Clearning output directory..." << std::endl;
		QDir(out_dir).removeRecursively();
		std::cout << "Done." << std::endl;
	}
	QDir().mkpath(out_dir);

	srand(0);
	renderManager.useShadow = false;
	renderManager.renderingMode = RenderManager::RENDERING_MODE_CONTOUR;

	int origWidth = width();
	int origHeight = height();
	resize(512, 512);
	resizeGL(512, 512);

	QDir dir(cga_dir);

	QStringList filters;
	filters << "*.xml";
	QFileInfoList fileInfoList = dir.entryInfoList(filters, QDir::Files | QDir::NoDotAndDotDot);
	for (int i = 0; i < fileInfoList.size(); ++i) {
		int count = 0;

		if (!QDir(out_dir + "\\" + fileInfoList[i].baseName()).exists()) QDir().mkdir(out_dir + "\\" + fileInfoList[i].baseName());

		QFile file(out_dir + "\\" + fileInfoList[i].baseName() + "/parameters.txt");
		if (!file.open(QIODevice::WriteOnly)) {
			std::cerr << "Cannot open file for writing: " << qPrintable(file.errorString()) << std::endl;
			return;
		}

		// show the progress
		printf("Grammar: %s", fileInfoList[i].baseName().toUtf8().constData());

		QTextStream out(&file);

		cga::CGA cga;

		cga::Grammar grammar;
		cga.modelMat = glm::rotate(glm::mat4(), -(float)M_PI * 0.5f, glm::vec3(1, 0, 0));
		cga::parseGrammar(fileInfoList[i].absoluteFilePath().toUtf8().constData(), grammar);

		// rotate the camera around y axis within the range
		for (int yrot = yrotMin; yrot <= yrotMax; ++yrot) {
			camera.yrot = yrot;

			// rotate the camera around x axis within the range
			for (int xrot = xrotMin; xrot <= xrotMax; ++xrot) {
				camera.xrot = xrot;
				camera.zrot = 0;
				if (cameraType == 0) { // street view
					camera.pos.x = 0;
					camera.pos.y = -cameraDistance * sinf(camera.xrot / 180.0f * M_PI) + cameraHeight * cosf(camera.xrot / 180.0f * M_PI);
					camera.pos.z = cameraDistance * cosf(camera.xrot / 180.0f * M_PI) + cameraHeight * sinf(camera.xrot / 180.0f * M_PI);
				}
				else { // aerial view
					camera.pos.x = 0;
					camera.pos.y = cameraHeight;
					camera.pos.z = cameraDistance;
				}

				for (int fov = fovMin; fov <= fovMax; ++fov) {
					camera.fovy = fov;
					camera.updatePMatrix(width(), height());

					// randomly sample N parameter values
					for (int k = 0; k < numSamples; ++k) {
						renderManager.removeObjects();

						std::vector<float> param_values;
						param_values = cga.randomParamValues(grammar);

						// set axiom
						cga::Rectangle* start = new cga::Rectangle("Start", "", glm::translate(glm::rotate(glm::mat4(), -3.141592f * 0.5f, glm::vec3(1, 0, 0)), glm::vec3(0, 0, 0)), glm::mat4(), 0, 0, glm::vec3(1, 1, 1));
						cga.stack.push_back(boost::shared_ptr<cga::Shape>(start));

						// generate 3d model
						cga.derive(grammar, true);
						std::vector<boost::shared_ptr<glutils::Face> > faces;
						cga.generateGeometry(faces);
						renderManager.addFaces(faces, true);

						// render 2d image
						render();
						QImage img = grabFrameBuffer();
						cv::Mat mat = cv::Mat(img.height(), img.width(), CV_8UC4, img.bits(), img.bytesPerLine()).clone();

						// translate the image
						if (centering) {
							if (!moveCenter(mat)) continue;
						}

						// 画像を縮小
						cv::resize(mat, mat, cv::Size(256, 256));
						cv::threshold(mat, mat, 250, 255, CV_THRESH_BINARY);
						if (image_width != 256 || image_height != 256) {
							cv::resize(mat, mat, cv::Size(image_width, image_height));
							cv::threshold(mat, mat, 250, 255, CV_THRESH_BINARY);
						}

						// grayscale
						if (grayscale) {
							cv::cvtColor(mat, mat, CV_BGR2GRAY);
						}

						// create the subfolder
						int subfolder_idx = count / 100000;
						QString subfolder_name = QString("%1").arg(subfolder_idx, 3, 10, QChar('0'));
						if (!QDir(out_dir + "\\" + fileInfoList[i].baseName() + "\\" + subfolder_name).exists()) QDir().mkdir(out_dir + "\\" + fileInfoList[i].baseName() + "\\" + subfolder_name);

						// set filename
						QString filename = out_dir + "\\" + fileInfoList[i].baseName() + "\\" + subfolder_name + "\\" + QString("image_%1.png").arg(count, 6, 10, QChar('0'));
						cv::imwrite(filename.toUtf8().constData(), mat);

						// add camera parameters to the params
						if (fovMin != fovMax) {
							param_values.insert(param_values.begin(), (float)(fov - fovMin) / (fovMax - fovMin));
						}
						if (xrotMin != xrotMax || yrotMin != yrotMax) {
							param_values.insert(param_values.begin(), (float)(yrot - yrotMin) / (yrotMax - yrotMin));
							param_values.insert(param_values.begin(), (float)(xrot - xrotMin) / (xrotMax - xrotMin));
						}

						// write all the param values [xrot, yrot, param1, param2, ...] to the file
						for (int pi = 0; pi < param_values.size(); ++pi) {
							if (pi > 0) {
								out << ",";
							}
							out << param_values[pi];
						}
						out << "\n";

						count++;

						// show the progress
						printf("\rGrammar: %s, count = %d", fileInfoList[i].baseName().toUtf8().constData(), count);
						fflush(stdout);
					}
				}
			}
		}

		file.close();

		// show the progress
		printf("\n");
	}

	resize(origWidth, origHeight);
	resizeGL(origWidth, origHeight);
}

void GLWidget3D::generateTrainingDataWithoutAmgiousViewpoints(const QString& cga_dir, const QString& out_dir, int numSamples, int image_width, int image_height, bool grayscale, bool centering, float xrotMin, float xrotMax, float yrotMin, float yrotMax) {
	if (QDir(out_dir).exists()) {
		std::cout << "Clearning output directory..." << std::endl;
		QDir(out_dir).removeRecursively();
		std::cout << "Done." << std::endl;
	}
	QDir().mkpath(out_dir);

	QString ambiguousDir = "results\\ambiguous\\";
	if (QDir(ambiguousDir).exists()) {
		std::cout << "Clearning ambiguous directory..." << std::endl;
		QDir(ambiguousDir).removeRecursively();
		std::cout << "Done." << std::endl;
	}
	QDir().mkpath(ambiguousDir);
	
	srand(0);
	renderManager.useShadow = false;
	renderManager.renderingMode = RenderManager::RENDERING_MODE_CONTOUR;

	int origWidth = width();
	int origHeight = height();
	resize(512, 512);
	resizeGL(512, 512);

	// 各snippet, viewpointについて、good/badを保存する
	std::vector<std::vector<Viewpoint>> viewpoints;

	// fix camera view direction and position
	fixCamera();

	QDir dir(cga_dir);

	std::vector<cga::Grammar> grammars;

	QStringList filters;
	filters << "*.xml";
	QFileInfoList fileInfoList = dir.entryInfoList(filters, QDir::Files | QDir::NoDotAndDotDot);
	
	std::cout << "Extracting contour lines..." << std::endl;
	for (int i = 0; i < fileInfoList.size(); ++i) {
		viewpoints.push_back(std::vector<Viewpoint>());

		cga::CGA cga;

		cga::Grammar grammar;
		cga.modelMat = glm::rotate(glm::mat4(), -(float)M_PI * 0.5f, glm::vec3(1, 0, 0));
		cga::parseGrammar(fileInfoList[i].absoluteFilePath().toUtf8().constData(), grammar);
		grammars.push_back(grammar);
		
		// rotate the camera around y axis within the range
		for (int yrot = yrotMin; yrot <= yrotMax; ++yrot) {
			camera.yrot = yrot;

			// rotate the camera around x axis within the range
			for (int xrot = xrotMin; xrot <= xrotMax; ++xrot) {
				camera.xrot = xrot;
				camera.updateMVPMatrix();

				viewpoints.back().push_back(Viewpoint(i, camera));

				// randomly sample N parameter values
				for (int k = 0; k < numSamples; ++k) {
					renderManager.removeObjects();

					std::vector<float> param_values = cga.randomParamValues(grammar);

					// set axiom
					cga::Rectangle* start = new cga::Rectangle("Start", "", glm::translate(glm::rotate(glm::mat4(), -3.141592f * 0.5f, glm::vec3(1, 0, 0)), glm::vec3(0, 0, 0)), glm::mat4(), 0, 0, glm::vec3(1, 1, 1));
					cga.stack.push_back(boost::shared_ptr<cga::Shape>(start));

					// generate 3d model
					cga.derive(grammar, true);
					std::vector<boost::shared_ptr<glutils::Face> > faces;
					cga.generateGeometry(faces);
					renderManager.addFaces(faces, true);

					// render 2d image
					render();

					// extract contour edges
					std::vector<std::pair<glm::vec2, glm::vec2>> contour;
					extractContourVectors(contour);

					viewpoints.back().back().examples.push_back(ViewpointExample(param_values, contour));
				}
			}
		}
	}
	std::cout << "Done." << std::endl;

	// check the ambiguity
	int xx_count = 0;

	std::cout << "Checking ambiguous cases..." << std::endl;
	renderManager.renderingMode = RenderManager::RENDERING_MODE_LINE;
	for (int i = 0; i < viewpoints.size(); ++i) {
		for (int j = 0; j < viewpoints[i].size(); ++j) {
			for (int k = 0; k < viewpoints[i][j].examples.size(); ++k) {

				for (int i2 = 0; i2 < viewpoints.size(); ++i2) {
					if (i2 == i) continue;

					for (int j2 = 0; j2 < viewpoints[i2].size(); ++j2) {
						if (viewpoints[i][j].ambiguous && viewpoints[i2][j2].ambiguous) continue;

						int ambigous_count = 0;
						for (int k2 = 0; k2 < viewpoints[i2][j2].examples.size(); ++k2) {

							////////////////////////////////// DEBUG //////////////////////////////////////
							/*
							cv::Mat img1(height(), width(), CV_8UC3, cv::Scalar(255, 255, 255));
							for (int p = 0; p < viewpoints[i][j].examples[k].contour.size(); ++p) {
								cv::line(img1, cv::Point(viewpoints[i][j].examples[k].contour[p].first.x, viewpoints[i][j].examples[k].contour[p].first.y), cv::Point(viewpoints[i][j].examples[k].contour[p].second.x, viewpoints[i][j].examples[k].contour[p].second.y), cv::Scalar(rand() % 200, rand() % 200, rand() % 200), 3);
							}
							cv::imwrite("image1.png", img1);
							cv::Mat img2(height(), width(), CV_8UC3, cv::Scalar(255, 255, 255));
							for (int p = 0; p < viewpoints[i2][j2].examples[k2].contour.size(); ++p) {
								cv::line(img2, cv::Point(viewpoints[i2][j2].examples[k2].contour[p].first.x, viewpoints[i2][j2].examples[k2].contour[p].first.y), cv::Point(viewpoints[i2][j2].examples[k2].contour[p].second.x, viewpoints[i2][j2].examples[k2].contour[p].second.y), cv::Scalar(rand() % 200, rand() % 200, rand() % 200), 3);
							}
							cv::imwrite("image2.png", img2);
							*/
							////////////////////////////////// DEBUG //////////////////////////////////////


							if (viewpoints[i][j].examples[k].isSimilar(viewpoints[i2][j2].examples[k2])) {
								////////////////////////////////// DEBUG //////////////////////////////////////
								/*
								std::cout << "Amibigous:" << std::endl;
								std::cout << "Snippet: " << i << ", xrot=" << viewpoints[i][j].camera.xrot << ", yrot=" << viewpoints[i][j].camera.yrot << std::endl;
								std::cout << "Snippet: " << i2 << ", xrot=" << viewpoints[i2][j2].camera.xrot << ", yrot=" << viewpoints[i2][j2].camera.yrot << std::endl;
								std::cout << std::endl;

								cv::Mat img1(height(), width(), CV_8UC3, cv::Scalar(255, 255, 255));
								for (int p = 0; p < viewpoints[i][j].examples[k].contour.size(); ++p) {
									cv::line(img1, cv::Point(viewpoints[i][j].examples[k].contour[p].first.x, viewpoints[i][j].examples[k].contour[p].first.y), cv::Point(viewpoints[i][j].examples[k].contour[p].second.x, viewpoints[i][j].examples[k].contour[p].second.y), cv::Scalar(rand() % 200, rand() % 200, rand() % 200), 3);
								}
								QString n = QString("results/image_%1_1.png").arg(xx_count);
								cv::imwrite(n.toUtf8().constData(), img1);
								cv::Mat img2(height(), width(), CV_8UC3, cv::Scalar(255, 255, 255));
								for (int p = 0; p < viewpoints[i2][j2].examples[k2].contour.size(); ++p) {
									cv::line(img2, cv::Point(viewpoints[i2][j2].examples[k2].contour[p].first.x, viewpoints[i2][j2].examples[k2].contour[p].first.y), cv::Point(viewpoints[i2][j2].examples[k2].contour[p].second.x, viewpoints[i2][j2].examples[k2].contour[p].second.y), cv::Scalar(rand() % 200, rand() % 200, rand() % 200), 3);
								}
								n = QString("results/image_%1_2.png").arg(xx_count++);
								cv::imwrite(n.toUtf8().constData(), img2);
								*/
								////////////////////////////////// DEBUG //////////////////////////////////////

								if (rand() % 100 == 0) {
									cga::CGA cga;
									cga.modelMat = glm::rotate(glm::mat4(), -(float)M_PI * 0.5f, glm::vec3(1, 0, 0));

									// 似ている１つ目の画像を保存
									{
										camera.xrot = viewpoints[i][j].camera.xrot;
										camera.yrot = viewpoints[i][j].camera.yrot;
										camera.updateMVPMatrix();

										renderManager.removeObjects();

										cga.setParamValues(grammars[i], viewpoints[i][j].examples[k].param_values);

										// set axiom
										cga::Rectangle* start = new cga::Rectangle("Start", "", glm::translate(glm::rotate(glm::mat4(), -3.141592f * 0.5f, glm::vec3(1, 0, 0)), glm::vec3(0, 0, 0)), glm::mat4(), 0, 0, glm::vec3(1, 1, 1));
										cga.stack.push_back(boost::shared_ptr<cga::Shape>(start));

										// generate 3d model
										cga.derive(grammars[i], true);
										std::vector<boost::shared_ptr<glutils::Face> > faces;
										cga.generateGeometry(faces);
										renderManager.addFaces(faces, true);

										// render 2d image
										render();

										QImage img = grabFrameBuffer();
										cv::Mat mat = cv::Mat(img.height(), img.width(), CV_8UC4, img.bits(), img.bytesPerLine()).clone();

										// make the line light blue
										for (int r = 0; r < mat.rows; ++r) {
											for (int c = 0; c < mat.cols; ++c) {
												cv::Vec4b color = mat.at<cv::Vec4b>(r, c);
												if (color[0] < 200 && color[1] < 200 && color[2] < 200) {
													color[0] = 255;
													color[1] = 128;
													color[2] = 128;
												}
												mat.at<cv::Vec4b>(r, c) = color;
											}
										}

										// draw contour lines
										for (int p = 0; p < viewpoints[i][j].examples[k].contour.size(); ++p) {
											cv::line(mat, cv::Point(viewpoints[i][j].examples[k].contour[p].first.x, viewpoints[i][j].examples[k].contour[p].first.y), cv::Point(viewpoints[i][j].examples[k].contour[p].second.x, viewpoints[i][j].examples[k].contour[p].second.y), cv::Scalar(0, 0, 0, 255), 1);
										}
																				
										QString n = QString(ambiguousDir + "image_%1_%2.png").arg(xx_count).arg(i + 1);
										cv::imwrite(n.toUtf8().constData(), mat);
									}

									// 似ている２つ目の画像を保存
									{
										camera.xrot = viewpoints[i2][j2].camera.xrot;
										camera.yrot = viewpoints[i2][j2].camera.yrot;
										camera.updateMVPMatrix();

										renderManager.removeObjects();

										cga.setParamValues(grammars[i2], viewpoints[i2][j2].examples[k2].param_values);

										// set axiom
										cga::Rectangle* start = new cga::Rectangle("Start", "", glm::translate(glm::rotate(glm::mat4(), -3.141592f * 0.5f, glm::vec3(1, 0, 0)), glm::vec3(0, 0, 0)), glm::mat4(), 0, 0, glm::vec3(1, 1, 1));
										cga.stack.push_back(boost::shared_ptr<cga::Shape>(start));

										// generate 3d model
										cga.derive(grammars[i2], true);
										std::vector<boost::shared_ptr<glutils::Face> > faces;
										cga.generateGeometry(faces);
										renderManager.addFaces(faces, true);

										// render 2d image
										render();

										QImage img = grabFrameBuffer();
										cv::Mat mat = cv::Mat(img.height(), img.width(), CV_8UC4, img.bits(), img.bytesPerLine()).clone();

										// make the line light blue
										for (int r = 0; r < mat.rows; ++r) {
											for (int c = 0; c < mat.cols; ++c) {
												cv::Vec4b color = mat.at<cv::Vec4b>(r, c);
												if (color[0] < 200 && color[1] < 200 && color[2] < 200) {
													color[0] = 255;
													color[1] = 128;
													color[2] = 128;
												}
												mat.at<cv::Vec4b>(r, c) = color;
											}
										}

										// draw contour lines
										for (int p = 0; p < viewpoints[i2][j2].examples[k2].contour.size(); ++p) {
											cv::line(mat, cv::Point(viewpoints[i2][j2].examples[k2].contour[p].first.x, viewpoints[i2][j2].examples[k2].contour[p].first.y), cv::Point(viewpoints[i2][j2].examples[k2].contour[p].second.x, viewpoints[i2][j2].examples[k2].contour[p].second.y), cv::Scalar(0, 0, 0, 255), 1);
										}
										
										QString n = QString(ambiguousDir + "image_%1_%2.png").arg(xx_count++).arg(i2 + 1);
										cv::imwrite(n.toUtf8().constData(), mat);
									}

								}

								viewpoints[i][j].examples[k].ambiguous = true;
								viewpoints[i2][j2].examples[k2].ambiguous = true;

								ambigous_count++;
							}
						}

						if (ambigous_count > viewpoints[i2][j2].examples.size() * 0.5f) {
							viewpoints[i][j].ambiguous = true;
							viewpoints[i2][j2].ambiguous = true;
						}
					}
				}
			}
		}
	}
	std::cout << "Done." << std::endl;

	// obtain the minimum number of images among classes
	int min_numSamples = std::numeric_limits<int>::max();
	for (int i = 0; i < viewpoints.size(); ++i) {
		int count = 0;
		for (int j = 0; j < viewpoints[i].size(); ++j) {
			for (int k = 0; k < viewpoints[i][j].examples.size(); ++k) {
				if (!viewpoints[i][j].examples[k].ambiguous) count++;
			}
		}

		if (count < min_numSamples) min_numSamples = count;
	}
	std::cout << "Min #samples: " << min_numSamples << std::endl;
	if (min_numSamples < 200) {
		std::cout << "At least 200 samples are required." << std::endl;
		min_numSamples = 200;
	}

	// augumenting training images
	std::cout << "Augmenting training images..." << std::endl;
	for (int i = 0; i < viewpoints.size(); ++i) {
		int count = 0;
		for (int j = 0; j < viewpoints[i].size(); ++j) {
			for (int k = 0; k < viewpoints[i][j].examples.size(); ++k) {
				if (!viewpoints[i][j].examples[k].ambiguous) count++;
			}
		}

		if (count >= min_numSamples) continue;

		for (int loop = 0; loop < min_numSamples + 100 - count; ++loop) {
			int viewpoint_idx = rand() % viewpoints[i].size();
			int example_idx = rand() % viewpoints[i][viewpoint_idx].examples.size();

			std::vector<float> param_values = viewpoints[i][viewpoint_idx].examples[example_idx].param_values;
			for (int k = 0; k < param_values.size(); ++k) {
				float val = param_values[k] + utils::genRand(-0.1, 0.1);
				val = std::min(1.0f, std::max(0.0f, val));

				param_values[k] = val;
			}

			viewpoints[i][viewpoint_idx].examples.push_back(ViewpointExample(param_values, viewpoints[i][viewpoint_idx].examples[example_idx].contour));
		}
	}

	// generate training images
	std::cout << "Generating training images..." << std::endl;
	renderManager.renderingMode = RenderManager::RENDERING_MODE_CONTOUR;
	for (int i = 0; i < viewpoints.size(); ++i) {
		if (!QDir(out_dir + "\\" + fileInfoList[i].baseName()).exists()) QDir().mkdir(out_dir + "\\" + fileInfoList[i].baseName());

		int count = 0;

		for (int j = 0; j < viewpoints[i].size(); ++j) {
			//if (viewpoints[i][j].ambiguous) continue;

			camera = viewpoints[i][j].camera;
			camera.updateMVPMatrix();

			for (int k = 0; k < viewpoints[i][j].examples.size(); ++k) {
				if (viewpoints[i][j].examples[k].ambiguous) continue;

				cga::CGA cga;
				cga.modelMat = glm::rotate(glm::mat4(), -(float)M_PI * 0.5f, glm::vec3(1, 0, 0));

				renderManager.removeObjects();

				// set parameter values
				cga.setParamValues(grammars[i], viewpoints[i][j].examples[k].param_values);

				// set axiom
				cga::Rectangle* start = new cga::Rectangle("Start", "", glm::translate(glm::rotate(glm::mat4(), -3.141592f * 0.5f, glm::vec3(1, 0, 0)), glm::vec3(0, 0, 0)), glm::mat4(), 0, 0, glm::vec3(1, 1, 1));
				cga.stack.push_back(boost::shared_ptr<cga::Shape>(start));

				// generate 3d model
				cga.derive(grammars[i], true);
				std::vector<boost::shared_ptr<glutils::Face> > faces;
				cga.generateGeometry(faces);
				renderManager.addFaces(faces, true);

				// render 2d image
				render();

				QImage img = grabFrameBuffer();
				cv::Mat mat = cv::Mat(img.height(), img.width(), CV_8UC4, img.bits(), img.bytesPerLine()).clone();

				// translate the image
				if (centering) {
					if (!moveCenter(mat)) continue;
				}

				// 画像を縮小
				cv::resize(mat, mat, cv::Size(256, 256));
				cv::threshold(mat, mat, 250, 255, CV_THRESH_BINARY);
				if (image_width != 256 || image_height != 256) {
					cv::resize(mat, mat, cv::Size(image_width, image_height));
					cv::threshold(mat, mat, 250, 255, CV_THRESH_BINARY);
				}

				// grayscale
				if (grayscale) {
					cv::cvtColor(mat, mat, CV_BGR2GRAY);
				}

				// set filename
				QString filename = out_dir + "\\" + fileInfoList[i].baseName() + "\\" + QString("image_%1.png").arg(count, 6, 10, QChar('0'));
				cv::imwrite(filename.toUtf8().constData(), mat);

				count++;
			}
		}
	}
	std::cout << "Done." << std::endl;

	resize(origWidth, origHeight);
	resizeGL(origWidth, origHeight);

}

void GLWidget3D::visualizePredictedDataWithFixedView(const QString& cga_dir, const QString& out_dir, int cameraType, float cameraDistance, float cameraHeight, float xrot, float yrot, float fov) {
	// fix camera view direction and position
	camera.xrot = xrot;
	camera.yrot = yrot;
	camera.zrot = 0;
	camera.fovy = fov;
	if (cameraType == 0) { // street view
		camera.pos.x = 0;
		camera.pos.y = -cameraDistance * sinf(camera.xrot / 180.0f * M_PI) + cameraHeight * cosf(camera.xrot / 180.0f * M_PI);
		camera.pos.z = cameraDistance * cosf(camera.xrot / 180.0f * M_PI) + cameraHeight * sinf(camera.xrot / 180.0f * M_PI);
	}
	else { // aerial view
		camera.pos.x = 0;
		camera.pos.y = cameraHeight;
		camera.pos.z = cameraDistance;
	}
	camera.updatePMatrix(width(), height());

	for (int i = 1; i < 30; ++i) { 
		QFile predicted_results_file(QString("prediction\\predicted_results_%1.txt").arg(i));
		if (!predicted_results_file.exists()) continue;
		if (!predicted_results_file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

		QFile test_file(QString("prediction\\test_%1.txt").arg(i, 2, 10, QChar('0')));
		if (!test_file.exists()) continue;
		if (!test_file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

		// load ground truth parameter values
		std::vector<std::vector<float>> true_param_values;
		QFile true_file(QString(out_dir + "\\contour_%1\\parameters.txt").arg(i, 2, 10, QChar('0')));
		if (!true_file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
		QTextStream in_true(&true_file);
		while (true) {
			QString line = in_true.readLine();
			if (line.isEmpty()) break;

			true_param_values.push_back(std::vector<float>());
			QStringList values = line.split(",");
			for (auto value : values) {
				true_param_values.back().push_back(value.toFloat());
			}
		}


		// setup CGA
		QString cga_file = QString(cga_dir + "\\contour_%1.xml").arg(i, 2, 10, QChar('0'));
		cga::CGA cga;
		cga::Grammar grammar;
		cga.modelMat = glm::rotate(glm::mat4(), -(float)M_PI * 0.5f, glm::vec3(1, 0, 0));
		cga::parseGrammar(cga_file.toUtf8().constData(), grammar);

		QTextStream in_predicted_results(&predicted_results_file);
		QTextStream in_test(&test_file);

		while (true) {
			// read param values
			QString line_values = in_predicted_results.readLine();
			if (line_values.isEmpty()) break;
			QStringList values = line_values.split(",");

			std::vector<float> param_values;
			for (auto value : values) {
				param_values.push_back(value.toFloat());
			}


			// 画像IDを取得
			int img_id;
			QString line_imgname = in_test.readLine();
			QStringList imgnames = line_imgname.split(" ");
			sscanf(imgnames[0].toUtf8().constData(), "image_%06d.png", &img_id);

			// 真のパラメータをセット
			cga.setParamValues(grammar, true_param_values[img_id]);

			// set axiom
			cga::Rectangle* start = new cga::Rectangle("Start", "", glm::translate(glm::rotate(glm::mat4(), -3.141592f * 0.5f, glm::vec3(1, 0, 0)), glm::vec3(0, 0, 0)), glm::mat4(), 0, 0, glm::vec3(1, 1, 1));
			cga.stack.push_back(boost::shared_ptr<cga::Shape>(start));

			// generate 3d model
			renderManager.removeObjects();
			cga.derive(grammar, true);
			std::vector<boost::shared_ptr<glutils::Face> > faces;
			cga.generateGeometry(faces);
			renderManager.addFaces(faces, true);

			// render 2d image
			render();
			QImage img = grabFrameBuffer();
			cv::Mat mat = cv::Mat(img.height(), img.width(), CV_8UC4, img.bits(), img.bytesPerLine()).clone();

			// 画像を縮小
			cv::resize(mat, mat, cv::Size(512, 512));
			cv::threshold(mat, mat, 250, 255, CV_THRESH_BINARY);
			cv::resize(mat, mat, cv::Size(256, 256));
			cv::threshold(mat, mat, 250, 255, CV_THRESH_BINARY);

			// predictdパラメータをセット
			cga.setParamValues(grammar, param_values);

			// set axiom
			cga::Rectangle* start2 = new cga::Rectangle("Start", "", glm::translate(glm::rotate(glm::mat4(), -3.141592f * 0.5f, glm::vec3(1, 0, 0)), glm::vec3(0, 0, 0)), glm::mat4(), 0, 0, glm::vec3(1, 1, 1));
			cga.stack.push_back(boost::shared_ptr<cga::Shape>(start2));

			// generate 3d model
			renderManager.removeObjects();
			cga.derive(grammar, true);
			std::vector<boost::shared_ptr<glutils::Face> > faces2;
			cga.generateGeometry(faces2);
			renderManager.addFaces(faces2, true);

			// render 2d image
			render();
			QImage img2 = grabFrameBuffer();
			cv::Mat mat2 = cv::Mat(img2.height(), img2.width(), CV_8UC4, img2.bits(), img2.bytesPerLine()).clone();

			// 画像を縮小
			cv::resize(mat2, mat2, cv::Size(512, 512));
			cv::threshold(mat2, mat2, 250, 255, CV_THRESH_BINARY);
			cv::resize(mat2, mat2, cv::Size(256, 256));
			cv::threshold(mat2, mat2, 250, 255, CV_THRESH_BINARY);

			// make the line blue
			for (int r = 0; r < mat2.rows; ++r) {
				for (int c = 0; c < mat2.cols; ++c) {
					cv::Vec4b color = mat2.at<cv::Vec4b>(r, c);
					color[0] = 255;
					mat2.at<cv::Vec4b>(r, c) = color;
				}
			}

			// blend
			cv::Mat mat3;
			cv::addWeighted(mat, 0.5, mat2, 0.5, 0.0, mat3);

			// save
			QString output_dir = QString("prediction\\contour_%1\\").arg(i, 2, 10, QChar('0'));
			if (!QDir(output_dir).exists()) QDir().mkpath(output_dir);
			QString output_filename = output_dir + QString("predicted_%1.png").arg(img_id, 6, 10, QChar('0'));
			cv::imwrite(output_filename.toUtf8().constData(), mat3);
		}
	}
}

void GLWidget3D::visualizePredictedDataWithRotation(const QString& cga_dir, const QString& out_dir, int cameraType, float cameraDistance, float cameraHeight, float xrotMin, float xrotMax, float yrotMin, float yrotMax, float fovMin, float fovMax) {
	for (int i = 1; i < 30; ++i) {
		QFile predicted_results_file(QString("prediction\\predicted_results_%1.txt").arg(i));
		if (!predicted_results_file.exists()) continue;
		if (!predicted_results_file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

		QFile test_file(QString("prediction\\test_%1.txt").arg(i, 2, 10, QChar('0')));
		if (!test_file.exists()) continue;
		if (!test_file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

		// load ground truth parameter values
		std::vector<std::vector<float>> true_param_values;
		QFile true_file(QString(out_dir + "\\contour_%1\\parameters.txt").arg(i, 2, 10, QChar('0')));
		if (!true_file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
		QTextStream in_true(&true_file);
		while (true) {
			QString line = in_true.readLine();
			if (line.isEmpty()) break;

			true_param_values.push_back(std::vector<float>());
			QStringList values = line.split(",");
			for (auto value : values) {
				true_param_values.back().push_back(value.toFloat());
			}
		}

		// setup CGA
		QString cga_file = QString(cga_dir + "\\contour_%1.xml").arg(i, 2, 10, QChar('0'));
		cga::CGA cga;
		cga::Grammar grammar;
		cga.modelMat = glm::rotate(glm::mat4(), -(float)M_PI * 0.5f, glm::vec3(1, 0, 0));
		cga::parseGrammar(cga_file.toUtf8().constData(), grammar);

		QTextStream in_predicted_results(&predicted_results_file);
		QTextStream in_test(&test_file);

		while (true) {
			// read param values
			QString line_values = in_predicted_results.readLine();
			if (line_values.isEmpty()) break;
			QStringList values = line_values.split(",");

			std::vector<float> param_values;
			for (auto value : values) {
				param_values.push_back(value.toFloat());
			}


			// 画像IDを取得
			int img_id;
			QString line_imgname = in_test.readLine();
			QStringList imgnames = line_imgname.split(" ");
			sscanf(imgnames[0].toUtf8().constData(), "image_%06d.png", &img_id);

			// 真のカメラパラメータをセット
			camera.xrot = xrotMin + (xrotMax - xrotMin) * true_param_values[img_id][0];
			camera.yrot = yrotMin + (yrotMax - yrotMin) * true_param_values[img_id][1];
			camera.zrot = 0.0f;
			camera.fovy = fovMin + true_param_values[img_id][2] * (fovMax - fovMin);
			if (cameraType == 0) { // street view
				camera.pos.x = 0;
				camera.pos.y = -cameraDistance * sinf(camera.xrot / 180.0f * M_PI) + cameraHeight * cosf(camera.xrot / 180.0f * M_PI);
				camera.pos.z = cameraDistance * cosf(camera.xrot / 180.0f * M_PI) + cameraHeight * sinf(camera.xrot / 180.0f * M_PI);
			}
			else { // aerial view
				camera.pos.x = 0;
				camera.pos.y = cameraHeight;
				camera.pos.z = cameraDistance;
			}
			camera.updatePMatrix(width(), height());
			
			// カメラパラメータを削除
			std::vector<float> true_param = true_param_values[img_id];
			true_param.erase(true_param.begin());
			true_param.erase(true_param.begin());
			true_param.erase(true_param.begin());

			// 真のパラメータをセット
			cga.setParamValues(grammar, true_param);

			// set axiom
			cga::Rectangle* start = new cga::Rectangle("Start", "", glm::translate(glm::rotate(glm::mat4(), -3.141592f * 0.5f, glm::vec3(1, 0, 0)), glm::vec3(0, 0, 0)), glm::mat4(), 0, 0, glm::vec3(1, 1, 1));
			cga.stack.push_back(boost::shared_ptr<cga::Shape>(start));

			// generate 3d model
			renderManager.removeObjects();
			cga.derive(grammar, true);
			std::vector<boost::shared_ptr<glutils::Face> > faces;
			cga.generateGeometry(faces);
			renderManager.addFaces(faces, true);

			// render 2d image
			render();
			QImage img = grabFrameBuffer();
			cv::Mat mat = cv::Mat(img.height(), img.width(), CV_8UC4, img.bits(), img.bytesPerLine()).clone();

			// 画像を縮小
			cv::resize(mat, mat, cv::Size(512, 512));
			cv::threshold(mat, mat, 250, 255, CV_THRESH_BINARY);
			cv::resize(mat, mat, cv::Size(256, 256));
			cv::threshold(mat, mat, 250, 255, CV_THRESH_BINARY);

			// predictedカメラパラメータをセット
			camera.xrot = xrotMin + (xrotMax - xrotMin) * param_values[0];
			camera.yrot = yrotMin + (yrotMax - yrotMin) * param_values[1];
			camera.zrot = 0.0f;
			camera.fovy = fovMin + param_values[2] * (fovMax - fovMin);
			if (cameraType == 0) { // street view
				camera.pos.x = 0;
				camera.pos.y = -cameraDistance * sinf(camera.xrot / 180.0f * M_PI) + cameraHeight * cosf(camera.xrot / 180.0f * M_PI);
				camera.pos.z = cameraDistance * cosf(camera.xrot / 180.0f * M_PI) + cameraHeight * sinf(camera.xrot / 180.0f * M_PI);
			}
			else { // aerial view
				camera.pos.x = 0;
				camera.pos.y = cameraHeight;
				camera.pos.z = cameraDistance;
			}
			camera.updatePMatrix(width(), height());

			// カメラパラメータを削除
			param_values.erase(param_values.begin());
			param_values.erase(param_values.begin());
			param_values.erase(param_values.begin());

			// predictdパラメータをセット
			cga.setParamValues(grammar, param_values);

			// set axiom
			cga::Rectangle* start2 = new cga::Rectangle("Start", "", glm::translate(glm::rotate(glm::mat4(), -3.141592f * 0.5f, glm::vec3(1, 0, 0)), glm::vec3(0, 0, 0)), glm::mat4(), 0, 0, glm::vec3(1, 1, 1));
			cga.stack.push_back(boost::shared_ptr<cga::Shape>(start2));

			// generate 3d model
			renderManager.removeObjects();
			cga.derive(grammar, true);
			std::vector<boost::shared_ptr<glutils::Face> > faces2;
			cga.generateGeometry(faces2);
			renderManager.addFaces(faces2, true);

			// render 2d image
			render();
			QImage img2 = grabFrameBuffer();
			cv::Mat mat2 = cv::Mat(img2.height(), img2.width(), CV_8UC4, img2.bits(), img2.bytesPerLine()).clone();

			// 画像を縮小
			cv::resize(mat2, mat2, cv::Size(512, 512));
			cv::threshold(mat2, mat2, 250, 255, CV_THRESH_BINARY);
			cv::resize(mat2, mat2, cv::Size(256, 256));
			cv::threshold(mat2, mat2, 250, 255, CV_THRESH_BINARY);

			// make the line blue
			for (int r = 0; r < mat2.rows; ++r) {
				for (int c = 0; c < mat2.cols; ++c) {
					cv::Vec4b color = mat2.at<cv::Vec4b>(r, c);
					color[0] = 255;
					mat2.at<cv::Vec4b>(r, c) = color;
				}
			}

			// blend
			cv::Mat mat3;
			cv::addWeighted(mat, 0.5, mat2, 0.5, 0.0, mat3);

			// save
			QString output_dir = QString("prediction\\contour_%1\\").arg(i, 2, 10, QChar('0'));
			if (!QDir(output_dir).exists()) QDir().mkpath(output_dir);
			QString output_filename = output_dir + QString("predicted_%1.png").arg(img_id, 6, 10, QChar('0'));
			cv::imwrite(output_filename.toUtf8().constData(), mat3);
		}
	}
}

void GLWidget3D::runMCMC(const std::string& cga_filename, const std::string& target_filename, int numIterations) {
	fixCamera();

	QImage input(target_filename.c_str());

	cga::Grammar grammar;
	cga::parseGrammar(cga_filename.c_str(), grammar);

	mcmc::MCMC mcmc(input, this, grammar);
	mcmc.run(numIterations);
}

void GLWidget3D::runMCMCAll(const std::string& cga_dir, int numIterations) {
	QDir dir(cga_dir.c_str());
	dir.setNameFilters(QStringList("*.xml"));
	dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

	QStringList fileList = dir.entryList();
	for (int i = 0; i < fileList.count(); ++i) {
		renderManager.renderingMode = RenderManager::RENDERING_MODE_CONTOUR;

		int index = fileList[i].lastIndexOf(".");
		QString target_filename = fileList[i].left(index) + ".png";

		runMCMC((dir.absolutePath() + "/" + fileList[i]).toUtf8().constData(), (dir.absolutePath() + "/" + target_filename).toUtf8().constData(), numIterations);

		QString result_contour_filename = QString("result_%1_contour.png").arg(i + 1);
		QString result_contour_old_filename = QString("result_%1.png").arg(numIterations);
		QFile::copy("results_mcmc/" + result_contour_old_filename, "results/" + result_contour_filename);

		QString result_text_filename = QString("result_%1.txt").arg(i + 1);
		QFile::copy("results_mcmc/result.txt", "results/" + result_text_filename);

		renderManager.renderingMode = RenderManager::RENDERING_MODE_LINE;
		render();
		QString result_image_filename = QString("result_%1.png").arg(i + 1);
		grabFrameBuffer().save("results/" + result_image_filename);
	}
}

bool GLWidget3D::moveCenter(cv::Mat& img) {
	bool scan_r = false;
	int min_r = -1;
	int max_r = -1;
	for (int r = 0; r < img.rows; ++r) {
		for (int c = 0; c < img.cols; ++c) {
			cv::Vec4b color = img.at<cv::Vec4b>(r, c);

			if (color[0] < 100 && color[1] < 100 && color[2] < 100) {
				if (!scan_r) {
					min_r = r;
					max_r = r;
					scan_r = true;
				}
				else {
					max_r = r;
				}
			}
		}
	}

	bool scan_c = false;
	int min_c = -1;
	int max_c = -1;
	for (int c = 0; c < img.rows; ++c) {
		for (int r = 0; r < img.cols; ++r) {
			cv::Vec4b color = img.at<cv::Vec4b>(r, c);

			if (color[0] < 100 && color[1] < 100 && color[2] < 100) {
				if (!scan_c) {
					min_c = c;
					max_c = c;
					scan_c = true;
				}
				else {
					max_c = c;
				}
			}
		}
	}

	// if there is no image, cancel the translation.
	if (min_r == -1 || min_c == -1) return false;

	// if the image is not strictly inside the canvas, cancel the translation.
	if (min_r == 0 || min_c == 0 || max_r == img.rows - 1 || max_c == img.cols - 1) return false;

	cv::Mat tmp = img.clone();
	img = cv::Mat(img.size(), img.type(), cv::Vec4b(255, 255, 255, 255));

	// translate the image
	int offset_c = img.cols * 0.5 - (min_c + max_c) * 0.5;
	int offset_r= img.rows * 0.5 - (min_r + max_r) * 0.5;
	for (int r = 0; r < img.rows; ++r) {
		for (int c = 0; c < img.cols; ++c) {
			if (c + offset_c < 0 || c + offset_c >= img.cols) continue;
			if (r + offset_r < 0 || r + offset_r >= img.rows) continue;

			img.at<cv::Vec4b>(r + offset_r, c + offset_c) = tmp.at<cv::Vec4b>(r, c);
		}
	}

	return true;
}

/**
 * fix camera view direction and position
 */
void GLWidget3D::fixCamera() {
	/*
	camera.xrot = 20.0f;
	camera.yrot = 30.0f;
	camera.zrot = 0.0f;
	camera.pos = glm::vec3(0, 12, 60);
	camera.updateMVPMatrix();
	*/
	camera.xrot = 0.0f;
	camera.yrot = 30.0f;
	camera.zrot = 0.0f;
	camera.pos = glm::vec3(0, 7.5, 40);
	camera.updateMVPMatrix();

	// photo 6
	/*
	camera.fovy = 52.0f;
	camera.xrot = -14.0f;
	camera.yrot = 43.0f;
	camera.zrot = 0.0f;
	camera.pos = glm::vec3(2, 4.6, 13);
	camera.updateMVPMatrix();
	*/

	// 2016/3/21
	camera.xrot = 20.0f;
	camera.yrot = 30.0f;
	camera.zrot = 0.0f;
	camera.pos = glm::vec3(0, 10, 50);
	camera.updateMVPMatrix();

	// good viewpoint 2016/3/29
	camera.xrot = 20.0f;
	camera.yrot = 30.0f;
	camera.zrot = 0.0f;
	camera.pos = glm::vec3(0, 10, 70);
	camera.updateMVPMatrix();

	// poor viewpoint 2016/4/7
	/*
	camera.xrot = 0.0f;
	camera.yrot = 0.0f;
	camera.zrot = 0.0f;
	camera.pos = glm::vec3(0, 10, 70);
	camera.updateMVPMatrix();
	*/
}

void GLWidget3D::keyPressEvent(QKeyEvent *e) {
	shiftPressed = false;

	switch (e->key()) {
	case Qt::Key_Shift:
		shiftPressed = true;
		break;
	default:
		break;
	}
}

void GLWidget3D::keyReleaseEvent(QKeyEvent* e) {
	shiftPressed = false;
}