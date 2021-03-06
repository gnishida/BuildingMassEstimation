﻿#include "GLWidget3D.h"
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
#include <fstream>
#include <opencv2/core.hpp>
#include <QFileDialog>

#ifndef M_PI
#define	M_PI	3.141592653
#endif

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
	cga.generateGeometry(faces, true);
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
void GLWidget3D::generateTrainingData(const QString& cga_dir, const QString& out_dir, int numSamples, int image_width, int image_height, bool grayscale, bool centering3D, bool centering, bool centeringNoise, float centeringNoiseMax, bool meanGeneration, int cameraType, float cameraDistanceBase, float cameraHeight, int xrotMin, int xrotMax, int yrotMin, int yrotMax, int fovMin, int fovMax, bool modifyImage, int lineWidthMin, int lineWidthMax, bool edgeNoise, float edgeNoiseMax) {
	if (QDir(out_dir).exists()) {
		std::cout << "Clearning output directory..." << std::endl;
		QDir(out_dir).removeRecursively();
		std::cout << "Done." << std::endl;
	}
	QDir().mkpath(out_dir);

	std::cout << "Training images are being generated with following parameters:" << std::endl;
	std::cout << "  cga_dir: " << cga_dir.toUtf8().constData() << std::endl;
	std::cout << "  out_dir: " << out_dir.toUtf8().constData() << std::endl;
	std::cout << "  #Samples: " << numSamples << std::endl;
	std::cout << "  image width: " << image_width << std::endl;
	std::cout << "  image height: " << image_height << std::endl;
	std::cout << "  grayscale: " << (grayscale ? "true" : "false") << std::endl;
	std::cout << "  centering 3D: " << (centering3D ? "true" : "false") << std::endl;
	std::cout << "  mean generation: " << (meanGeneration ? "true" : "false") << std::endl;
	std::cout << "  centering: " << (centering ? "true" : "false") << std::endl;
	std::cout << "  centering noise: " << (centeringNoise ? "true" : "false") << std::endl;
	std::cout << "  centering noise max: " << centeringNoiseMax << std::endl;
	std::cout << "  camera type: " << (cameraType == 0 ? "street view" : "aerial view") << std::endl;
	std::cout << "  camera distance base: " << cameraDistanceBase << std::endl;
	std::cout << "  camera height: " << cameraHeight << std::endl;
	std::cout << "  x rot: " << xrotMin << " - " << xrotMax << std::endl;
	std::cout << "  y rot: " << yrotMin << " - " << yrotMax << std::endl;
	std::cout << "  FOV: " << fovMin << " - " << fovMax << std::endl;
	std::cout << "  modify image: " << (modifyImage ? "true" : "false") << std::endl;
	std::cout << "  line width: " << lineWidthMin << " - " << lineWidthMax << std::endl;
	std::cout << "  edge noise: " << (edgeNoise ? "true" : "false") << std::endl;
	std::cout << "  edge noise max: " << edgeNoiseMax << std::endl;
	std::cout << std::endl;

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

		QString out_dir_for_snippet = out_dir + "/" + QString("%1").arg(i + 1, 2, 10, QChar('0'));	// images/01, images/02, ...
		if (!QDir(out_dir_for_snippet).exists()) QDir().mkdir(out_dir_for_snippet);

		QFile file(out_dir_for_snippet + "/parameters.txt");
		if (!file.open(QIODevice::WriteOnly)) {
			std::cerr << "Cannot open file for writing: " << qPrintable(file.errorString()) << std::endl;
			return;
		}

		// show the progress
		printf("Grammar: %s", fileInfoList[i].baseName().toUtf8().constData());

		QTextStream out(&file);

		cv::Mat meanMat;
		if (grayscale) {
			meanMat = cv::Mat(image_height, image_width, CV_64F, cv::Scalar(0));
		}
		else {
			meanMat = cv::Mat(image_height, image_width, CV_64FC3, cv::Scalar(0, 0, 0));
		}
		
		cga::CGA cga;

		cga::Grammar grammar;
		cga.modelMat = glm::rotate(glm::mat4(), -(float)M_PI * 0.5f, glm::vec3(1, 0, 0));
		cga::parseGrammar(fileInfoList[i].absoluteFilePath().toUtf8().constData(), grammar);

		// rotate the camera around y axis within the range
		for (int yrot = yrotMin; yrot <= yrotMax; ++yrot) {
			// rotate the camera around x axis within the range
			for (int xrot = xrotMin; xrot <= xrotMax; ++xrot) {
				for (int fov = fovMin; fov <= fovMax; ++fov) {
					float cameraDistance = cameraDistanceBase / tanf((float)fov * 0.5 / 180.0f * M_PI);

					camera.xrot = xrot;
					camera.yrot = yrot;
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
						cga.generateGeometry(faces, centering3D);
						renderManager.addFaces(faces, true);

						// render 2d image
						render();
						QImage img = grabFrameBuffer();
						cv::Mat mat = cv::Mat(img.height(), img.width(), CV_8UC4, img.bits(), img.bytesPerLine()).clone();

						// translate the image
						if (centering) {
							if (!moveCenter(mat)) continue;

							if (centeringNoise) {
								int shift_x = round(utils::genRand(-width() * centeringNoiseMax * 0.01f, width() * centeringNoiseMax * 0.01f));
								int shift_y = round(utils::genRand(-height() * centeringNoiseMax * 0.01f, height() * centeringNoiseMax * 0.01f));
								translateImage(mat, mat, shift_x, shift_y);
							}
						}

						if (modifyImage) {
							// extract contour vectors
							std::vector<std::pair<glm::vec2, glm::vec2>> contour;
							utils::extractEdges(mat, contour);

							// add noise
							if (edgeNoise) {
								for (int ci = 0; ci < contour.size(); ++ci) {
									contour[ci].first.x += round(utils::genRand(-width() * edgeNoiseMax * 0.01f, width() * edgeNoiseMax * 0.01f));
									contour[ci].first.y += round(utils::genRand(-height() * edgeNoiseMax * 0.01f, height() * edgeNoiseMax * 0.01f));
									contour[ci].second.x += round(utils::genRand(-width() * edgeNoiseMax * 0.01f, width() * edgeNoiseMax * 0.01f));
									contour[ci].second.y += round(utils::genRand(-height() * edgeNoiseMax * 0.01f, height() * edgeNoiseMax * 0.01f));
								}
							}

							// 画像を縮小
							glm::vec2 scale((float)image_width / width(), (float)image_height / height());
							for (int ci = 0; ci < contour.size(); ++ci) {
								contour[ci].first.x *= scale.x;
								contour[ci].first.y *= scale.y;
								contour[ci].second.x *= scale.x;
								contour[ci].second.y *= scale.y;
							}

							// generate the rendered image
							cv::Scalar color;
							if (grayscale) {
								mat = cv::Mat(image_height, image_width, CV_8U, cv::Scalar(255));
								color = cv::Scalar(0);
							}
							else {
								mat = cv::Mat(image_height, image_width, CV_8UC3, cv::Scalar(255, 255, 255));
								color = cv::Scalar(0, 0, 0);
							}
							for (int ci = 0; ci < contour.size(); ++ci) {
								int lineWidth = utils::genIntRand(lineWidthMin, lineWidthMax);
								cv::line(mat, cv::Point(contour[ci].first.x, contour[ci].first.y), cv::Point(contour[ci].second.x, contour[ci].second.y), color, lineWidth, cv::LINE_AA);
							}
						}
						else {
							// 画像を縮小
							cv::resize(mat, mat, cv::Size(image_width, image_height));
							cv::threshold(mat, mat, 250, 255, CV_THRESH_BINARY);

							// generate the rendered image
							if (grayscale) {
								cv::cvtColor(mat, mat, cv::COLOR_BGR2GRAY);
							}
						}

						// create the subfolder
						int subfolder_idx = count / 100000;
						QString subfolder_name = QString("%1").arg(subfolder_idx, 3, 10, QChar('0'));
						QString subfolder_path = out_dir_for_snippet + "/" + subfolder_name;
						if (!QDir(subfolder_path).exists()) QDir().mkdir(subfolder_path);	// images/01/000, images/01/001, ...

						// save an image to a file
						QString filename = subfolder_path + "/" + QString("%1.png").arg(count, 6, 10, QChar('0'));
						cv::imwrite(filename.toUtf8().constData(), mat);

						// update the mean image
						if (meanGeneration) {
							for (int r = 0; r < mat.rows; ++r) {
								for (int c = 0; c < mat.cols; ++c) {
									if (grayscale) {
										meanMat.at<double>(r, c) += mat.at<unsigned char>(r, c);
									}
									else {
										cv::Vec3b color = mat.at<cv::Vec3b>(r, c);
										cv::Vec3d total = meanMat.at<cv::Vec3d>(r, c);
										total[0] += color[0];
										total[1] += color[1];
										total[2] += color[2];
										meanMat.at<cv::Vec3d>(r, c) = total;
									}
								}
							}
						}

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

		if (meanGeneration) {
			meanMat /= (float)count;
			QString filename = out_dir + "\\" + fileInfoList[i].baseName() + "\\mean.png";
			cv::imwrite(filename.toUtf8().constData(), meanMat);
		}
	}

	resize(origWidth, origHeight);
	resizeGL(origWidth, origHeight);

	std::cout << "Training images were successfully generated." << std::endl;
}

void GLWidget3D::visualizePredictedData(const QString& cga_dir, const QString& out_dir, bool centering3D, int cameraType, float cameraDistanceBase, float cameraHeight, int xrotMin, int xrotMax, int yrotMin, int yrotMax, int fovMin, int fovMax) {
	int origWidth = width();
	int origHeight = height();
	resize(512, 512);
	resizeGL(512, 512);

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
			int dummy;
			QString line_imgname = in_test.readLine();
			QStringList imgnames = line_imgname.split(" ");
			sscanf(imgnames[0].toUtf8().constData(), "%03d/image_%06d.png", &dummy, &img_id);

			if (img_id == 2245) {
				int xxx = 0;
			}

			std::vector<float> true_param = true_param_values[img_id];

			// 真のカメラパラメータをセット
			if (xrotMin != xrotMax && yrotMin != yrotMax) {
				camera.xrot = xrotMin + (xrotMax - xrotMin) * true_param[0];
				camera.yrot = yrotMin + (yrotMax - yrotMin) * true_param[1];

				true_param.erase(true_param.begin());
				true_param.erase(true_param.begin());
			}
			else {
				camera.xrot = xrotMin;
				camera.yrot = yrotMin;
			}
			camera.zrot = 0.0f;
			if (fovMin != fovMax) {
				camera.fovy = fovMin + true_param[0] * (fovMax - fovMin);
				true_param.erase(true_param.begin());
			}
			else {
				camera.fovy = fovMin;
			}
			float cameraDistance = cameraDistanceBase / tanf(camera.fovy * 0.5 / 180.0f * M_PI);
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
			
			// 真のパラメータをセット
			cga.setParamValues(grammar, true_param);

			// set axiom
			cga::Rectangle* start = new cga::Rectangle("Start", "", glm::translate(glm::rotate(glm::mat4(), -3.141592f * 0.5f, glm::vec3(1, 0, 0)), glm::vec3(0, 0, 0)), glm::mat4(), 0, 0, glm::vec3(1, 1, 1));
			cga.stack.push_back(boost::shared_ptr<cga::Shape>(start));

			// generate 3d model
			renderManager.removeObjects();
			cga.derive(grammar, true);
			std::vector<boost::shared_ptr<glutils::Face> > faces;
			cga.generateGeometry(faces, centering3D);
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
			if (xrotMin != xrotMax && yrotMin != yrotMax) {
				camera.xrot = xrotMin + (xrotMax - xrotMin) * param_values[0];
				camera.yrot = yrotMin + (yrotMax - yrotMin) * param_values[1];
				param_values.erase(param_values.begin());
				param_values.erase(param_values.begin());
			}
			else {
				camera.xrot = xrotMin;
				camera.yrot = yrotMin;
			}
			camera.zrot = 0.0f;
			if (fovMin != fovMax) {
				camera.fovy = fovMin + param_values[0] * (fovMax - fovMin);
				param_values.erase(param_values.begin());
			}
			cameraDistance = cameraDistanceBase / tanf(camera.fovy * 0.5 / 180.0f * M_PI);
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

			// predictdパラメータをセット
			cga.setParamValues(grammar, param_values);

			// set axiom
			cga::Rectangle* start2 = new cga::Rectangle("Start", "", glm::translate(glm::rotate(glm::mat4(), -3.141592f * 0.5f, glm::vec3(1, 0, 0)), glm::vec3(0, 0, 0)), glm::mat4(), 0, 0, glm::vec3(1, 1, 1));
			cga.stack.push_back(boost::shared_ptr<cga::Shape>(start2));

			// generate 3d model
			renderManager.removeObjects();
			cga.derive(grammar, true);
			std::vector<boost::shared_ptr<glutils::Face> > faces2;
			cga.generateGeometry(faces2, centering3D);
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

	resize(origWidth, origHeight);
	resizeGL(origWidth, origHeight);
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

void GLWidget3D::translateImage(cv::Mat source, cv::Mat& target, int shift_x, int shift_y) {
	target = cv::Mat(source.size(), source.type(), cv::Scalar(255, 255, 255, 255));
	
	for (int r = 0; r < target.rows; ++r) {
		for (int c = 0; c < target.cols; ++c) {
			if (c - shift_x < 0 || c - shift_x >= source.cols) continue;
			else if (r - shift_y < 0 || r - shift_y >= source.rows) continue;
			else {
				cv::Vec4b color = source.at<cv::Vec4b>(r - shift_y, c - shift_x);
				target.at<cv::Vec4b>(r, c) = color;
			}
		}
	}
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