#include "Symbol.h"

Symbol::Symbol(std::string name,
			   std::size_t offset,
			   std::size_t width,
			   std::size_t height,
			   std::size_t paddingHorizontal,
			   std::size_t paddingVertical)
	: name(name),
	  offset(offset),
	  width(width),
	  height(height),
	  paddingHorizontal(paddingHorizontal),
	  paddingVertical(paddingVertical)
{}

bool symbolCompare(const Symbol& lhs, const Symbol& rhs) {
	if (lhs.getName().length() == 1 && rhs.getName().length() == 1) {
		return lhs.getName()[0] < rhs.getName()[0];
	}
	else if (lhs.getName().length() == 1) return true;
	else if (rhs.getName().length() == 1) return false;
	else return lhs.getName() < rhs.getName();
}
