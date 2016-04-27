#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

namespace utils {
	
	bool isNumber(const std::string& str);
	float genRand();
	float genRand(float v);
	float genRand(float a, float b);
	bool segmentSegmentIntersect(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d, float *tab, float *tcd, bool segmentOnly, glm::vec2& intPoint);

	void extractEdges(const cv::Mat& img, std::vector<std::pair<glm::vec2, glm::vec2>>& edges);
	void cleanEdges(std::vector<std::pair<glm::vec2, glm::vec2>>& edges, float maxLineGap, float theta);
	void cleanContours(std::vector<std::pair<glm::vec2, glm::vec2>>& edges, float maxLineGap, float theta);
}
