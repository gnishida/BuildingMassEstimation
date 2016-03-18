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
#include "MCMCConfigDialog.h"

#ifndef M_PI
#define	M_PI	3.141592653
#endif

GLWidget3D::GLWidget3D(MainWindow *parent) : QGLWidget(QGLFormat(QGL::SampleBuffers)) {
	this->mainWin = parent;
	shiftPressed = false;

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
	camera.mousePress(e->x(), e->y());
}

/**
 * This event handler is called when the mouse release events occur.
 */
void GLWidget3D::mouseReleaseEvent(QMouseEvent *e) {
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
	updateStatusBar();
	updateGL();
}

void GLWidget3D::wheelEvent(QWheelEvent* e) {
	camera.zoom(e->delta());
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
void GLWidget3D::paintGL() {
	render();

	//printf("<<\n");
	//VBOUtil::disaplay_memory_usage();

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

	glm::mat4 modelMat = glm::rotate(glm::mat4(), -(float)M_PI * 0.5f, glm::vec3(1, 0, 0));
	std::vector<Vertex> vertices;

	// set axiom
	cga::Rectangle* start = new cga::Rectangle("Start", "", glm::translate(glm::rotate(glm::mat4(), -3.141592f * 0.5f, glm::vec3(1, 0, 0)), glm::vec3(-0.5, -0.5, 0)), glm::mat4(), 1, 1, glm::vec3(1, 1, 1));
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

void GLWidget3D::generateTrainingData(int image_width, int image_height) {
	QString resultDir = "results/buildings/";

	if (QDir(resultDir).exists()) {
		QDir(resultDir).removeRecursively();
	}
	QDir().mkpath(resultDir);

	srand(0);
	renderManager.useShadow = false;

	int origWidth = width();
	int origHeight = height();
	//resize(512, 512);
	//resizeGL(512, 512);

	// fix camera view direction and position
	fixCamera();

	std::vector<std::string> cga_files;
	cga_files.push_back("../cga/primitives/prim1.xml");
	//cga_files.push_back("../cga/primitives/prim2.xml");

	std::vector<cga::Grammar> grammars;
	for (int i = 0; i < cga_files.size(); ++i) {
		cga::Grammar g;
		cga::parseGrammar(cga_files[i].c_str(), g);
		grammars.push_back(g);
	}

	cga::CGA cga;
	cga.modelMat = glm::rotate(glm::mat4(), -3.1415926f * 0.5f, glm::vec3(1, 0, 0));

	int count = 0;
	for (int k = 0; k < 10; ++k) {
		std::vector<float> param_values;

		renderManager.removeObjects();

		glm::mat4 modelMat = glm::rotate(glm::mat4(), -(float)M_PI * 0.5f, glm::vec3(1, 0, 0));
		std::vector<Vertex> vertices;

		// 1つのビルは、3つのprimitiveで構成されるという前提
		std::vector<glutils::BoundingBox> bboxes;
		for (int p = 0; p < 3; ++p) {
			// primitive type
			int obj_type = rand() % grammars.size();

			// set axiom
			cga::Rectangle* start = new cga::Rectangle("Start", "", glm::translate(glm::rotate(glm::mat4(), -3.141592f * 0.5f, glm::vec3(1, 0, 0)), glm::vec3(-0.5, -0.5, 0)), glm::mat4(), 1, 1, glm::vec3(1, 1, 1));
			cga.stack.push_back(boost::shared_ptr<cga::Shape>(start));

			// generate 3d model
			cga.randomParamValues(grammars[obj_type]);
			cga.derive(grammars[obj_type], true);
			std::vector<boost::shared_ptr<glutils::Face> > faces;
			cga.generateGeometry(faces);

			// set color
			glm::vec4 color(p == 0 ? 1 : 0, p == 1 ? 1 : 0, p == 2 ? 1 : 0, 1);
			for (int fi = 0; fi < faces.size(); ++fi) {
				for (int vi = 0; vi < faces[fi]->vertices.size(); ++vi) {
					faces[fi]->vertices[vi].color = color;
				}
			}

			renderManager.addFaces(faces, false);

			// 2d bounding boxを計算
			glutils::BoundingBox bbox;
			for (auto face : faces) {
				for (auto v : face->vertices) {
					glm::vec4 pp = camera.mvpMatrix * glm::vec4(v.position, 1);
					pp.x = (pp.x / pp.w * 0.5 + 0.5) * width();
					pp.y = (0.5 - pp.y / pp.w * 0.5) * height();
					bbox.addPoint(glm::vec2(pp.x, pp.y));
				}
			}

			bboxes.push_back(bbox);
		}

		//renderManager.updateShadowMap(this, light_dir, light_mvpMatrix);

		// render 2d image with color
		renderManager.renderingMode = RenderManager::RENDERING_MODE_BASIC;

		// render 2d image
		render();
		QImage img = this->grabFrameBuffer();
		/*
		QPainter painter(&img);
		painter.setPen(QPen(Qt::blue, 3, Qt::DotLine));
		for (auto bbox : bboxes) {
			painter.drawRect(bbox.minPt.x - 10, bbox.minPt.y - 10, bbox.maxPt.x - bbox.minPt.x + 20, bbox.maxPt.y - bbox.minPt.y + 20);
			//cv::rectangle(mat, cv::Point(bbox.minPt.x - 10, bbox.minPt.y - 10), cv::Point(bbox.maxPt.x + 10, bbox.maxPt.y + 10), cv::Scalar(255, 0, 0), 2, );
		}
		*/
		cv::Mat mat = cv::Mat(img.height(), img.width(), CV_8UC4, img.bits(), img.bytesPerLine()).clone();
		

		// 実際のピクセルカラーを使って、さらにtightなbounding boxにする
		for (int bi = 0; bi < bboxes.size(); ++bi) {
			glm::vec3 color(bi == 0 ? 255 : 0, bi == 1 ? 255 : 0, bi == 2 ? 255 : 0);

			glm::vec2 minPt = glm::vec2(bboxes[bi].minPt);
			glm::vec2 maxPt = glm::vec2(bboxes[bi].maxPt);

			// X最小値
			bool found = false;
			for (int x = minPt.x; x <= maxPt.x; ++x) {
				for (int y = minPt.y; y <= maxPt.y; ++y) {
					/*
					int B = mat.at<cv::Vec4b>(y, x)[0];
					int G = mat.at<cv::Vec4b>(y, x)[1];
					int R = mat.at<cv::Vec4b>(y, x)[2];

					std::cout << B << "," << G << "," << R << std::endl;
					*/
					if (mat.at<cv::Vec4b>(y, x)[0] == color.b && mat.at<cv::Vec4b>(y, x)[1] == color.g && mat.at<cv::Vec4b>(y, x)[2] == color.r) {
						found = true;
						break;
					}
				}

				if (found) {
					bboxes[bi].minPt.x = x;
					break;
				}
			}

			if (!found) {
				bboxes[bi].maxPt = bboxes[bi].minPt;
				continue;
			}

			// X最大値
			for (int x = maxPt.x; x >= minPt.x; --x) {
				bool found = false;
				for (int y = minPt.y; y <= maxPt.y; ++y) {
					if (mat.at<cv::Vec4b>(y, x)[0] == color.b && mat.at<cv::Vec4b>(y, x)[1] == color.g && mat.at<cv::Vec4b>(y, x)[2] == color.r) {
						found = true;
						break;
					}
				}

				if (found) {
					bboxes[bi].maxPt.x = x;
					break;
				}
			}

			// Y最小値
			for (int y = minPt.y; y <= maxPt.y; ++y) {
				bool found = false;
				for (int x = minPt.x; x <= maxPt.x; ++x) {
					if (mat.at<cv::Vec4b>(y, x)[0] == color.b && mat.at<cv::Vec4b>(y, x)[1] == color.g && mat.at<cv::Vec4b>(y, x)[2] == color.r) {
						found = true;
						break;
					}
				}

				if (found) {
					bboxes[bi].minPt.y = y;
					break;
				}
			}

			// Y最大値
			for (int y = maxPt.y; y >= minPt.y; --y) {
				bool found = false;
				for (int x = minPt.x; x <= maxPt.x; ++x) {
					if (mat.at<cv::Vec4b>(y, x)[0] == color.b && mat.at<cv::Vec4b>(y, x)[1] == color.g && mat.at<cv::Vec4b>(y, x)[2] == color.r) {
						found = true;
						break;
					}
				}

				if (found) {
					bboxes[bi].maxPt.y = y;
					break;
				}
			}
		}
		
		// 小さすぎるbounding boxは除外
		for (int bi = 0; bi < bboxes.size(); ) {
			if (fabs(bboxes[bi].maxPt.x - bboxes[bi].minPt.x) < 10 || fabs(bboxes[bi].maxPt.y - bboxes[bi].minPt.y) < 10) {
				bboxes.erase(bboxes.begin() + bi);
			}
			else {
				bi++;
			}
		}

		// render 2d image with line rendering
		renderManager.renderingMode = RenderManager::RENDERING_MODE_LINE;
		render();
		QImage img2 = this->grabFrameBuffer();

		for (int bi = 0; bi < bboxes.size(); ++bi) {
			//cv::rectangle(mat, cv::Rect(bboxes[bi].minPt.x - 10, bboxes[bi].minPt.y - 10, bboxes[bi].maxPt.x - bboxes[bi].minPt.x + 20, bboxes[bi].maxPt.y - bboxes[bi].minPt.y + 20), cv::Scalar(0, 0, 0), 2);
		}

		QPainter painter(&img2);
		int bi = 0;
		for (auto bbox : bboxes) {
			if (bi == 0) {
				painter.setPen(QPen(Qt::red, 3, Qt::DotLine));
			}
			else if (bi == 1) {
				painter.setPen(QPen(Qt::green, 3, Qt::DotLine));
			}
			else {
				painter.setPen(QPen(Qt::blue, 3, Qt::DotLine));
			}
			painter.drawRect(bbox.minPt.x - 10, bbox.minPt.y - 10, bbox.maxPt.x - bbox.minPt.x + 20, bbox.maxPt.y - bbox.minPt.y + 20);
			//cv::rectangle(mat, cv::Point(bbox.minPt.x - 10, bbox.minPt.y - 10), cv::Point(bbox.maxPt.x + 10, bbox.maxPt.y + 10), cv::Scalar(255, 0, 0), 2, );
			bi++;
		}
		cv::Mat mat2 = cv::Mat(img2.height(), img2.width(), CV_8UC4, img2.bits(), img2.bytesPerLine()).clone();


		// 画像を縮小
		/*
		if (image_width < 512) {
			float scale = 512.0f / width();
			cv::resize(mat, mat, cv::Size(width() * scale, height() * scale));
			cv::threshold(mat, mat, 250, 255, CV_THRESH_BINARY);
		}
		cv::resize(mat, mat, cv::Size(image_width, image_height));
		cv::threshold(mat, mat, 250, 255, CV_THRESH_BINARY);
		*/

		// set filename
		QString filename = resultDir + "/" + QString("image_%1.png").arg(count, 6, 10, QChar('0'));
		cv::imwrite(filename.toUtf8().constData(), mat2);

		count++;
	}

	//resize(origWidth, origHeight);
	//resizeGL(origWidth, origHeight);
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