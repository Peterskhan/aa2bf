#pragma once

// Project includes
#include <string>

class Symbol {
public:
	// Constructor
	Symbol(std::string name, std::size_t offset, std::size_t width, std::size_t height, 
		   std::size_t paddingHorizontal, std::size_t paddingVertical);

	// Property getters
	std::string getName() const              { return name; }
	std::size_t getOffset() const            { return offset; }
	std::size_t getWidth() const             { return width; }
	std::size_t getHeight() const            { return height; }
	std::size_t getPaddingHorizontal() const { return paddingHorizontal; }
	std::size_t getPaddingVertical() const   { return paddingVertical; }

private:
	std::string name;
	std::size_t offset;
	std::size_t width;
	std::size_t height;
	std::size_t paddingHorizontal;
	std::size_t paddingVertical;
};

// Compare function for comparing symbols
bool symbolCompare(const Symbol& lhs, const Symbol& rhs);