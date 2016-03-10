#include "RoofGableOperator.h"
#include "CGA.h"
#include "Shape.h"
#include "Utils.h"

namespace cga {

	RoofGableOperator::RoofGableOperator(const std::string& angle) {
		this->name = "roofGable";
		this->angle = angle;

		if (!utils::isNumber(angle)) {
			this->params.push_back(angle);
		}
	}

	boost::shared_ptr<Shape> RoofGableOperator::apply(boost::shared_ptr<Shape>& shape, const Grammar& grammar, std::list<boost::shared_ptr<Shape> >& stack) {
		float actual_angle = grammar.evalFloat(angle, shape);
		return shape->roofGable(shape->_name, actual_angle);
	}

}