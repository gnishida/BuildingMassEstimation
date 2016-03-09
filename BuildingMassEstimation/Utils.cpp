#include "Utils.h"
#include <regex>
#include <sstream>

namespace utils {

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
}