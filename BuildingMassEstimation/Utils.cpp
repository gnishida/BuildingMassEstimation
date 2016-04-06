#include "Utils.h"
#include <regex>
#include <sstream>

namespace utils {

	const float FLOAT_TOL = 1e-6f;

	bool isNumber(const std::string& str) {
		std::regex e("^-?\\d*\\.?\\d+");
		if (std::regex_match(str, e)) return true;
		else return false;
	}

	float genRand() {
		return (float)(rand() % 1000) / 1000.0f;
	}

	float genRand(float v) {
		return genRand() * v;
	}

	float genRand(float a, float b) {
		return genRand(b - a) + a;
	}

	/**
	* Computes the intersection between two line segments on the XY plane.
	* Segments must intersect within their extents for the intersection to be valid. z coordinate is ignored.
	*
	* @param a one end of the first line
	* @param b another end of the first line
	* @param c one end of the second line
	* @param d another end of the second line
	* @param tab
	* @param tcd
	* @param segmentOnly
	* @param intPoint	the intersection
	* @return true if two lines intersect / false otherwise
	**/
	bool segmentSegmentIntersect(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d, float *tab, float *tcd, bool segmentOnly, glm::vec2& intPoint) {
		glm::vec2 u = b - a;
		glm::vec2 v = d - c;

		if (glm::length(u) < FLOAT_TOL || glm::length(u) < FLOAT_TOL) {
			return false;
		}

		float numer = v.x*(c.y - a.y) + v.y*(a.x - c.x);
		float denom = u.y*v.x - u.x*v.y;

		if (denom == 0.0f)  {
			// they are parallel
			*tab = 0.0f;
			*tcd = 0.0f;
			return false;
		}

		float t0 = numer / denom;

		glm::vec2 ipt = a + t0*u;
		glm::vec2 tmp = ipt - c;
		float t1;
		if (glm::dot(tmp, v) > 0.0f) {
			t1 = tmp.length() / v.length();
		}
		else {
			t1 = -1.0f * tmp.length() / v.length();
		}

		//Check if intersection is within segments
		if (segmentOnly && !((t0 >= FLOAT_TOL) && (t0 <= 1.0f - FLOAT_TOL) && (t1 >= FLOAT_TOL) && (t1 <= 1.0f - FLOAT_TOL))){
			return false;
		}

		*tab = t0;
		*tcd = t1;
		glm::vec2 dirVec = b - a;

		intPoint = a + (*tab)*dirVec;

		return true;
	}
}