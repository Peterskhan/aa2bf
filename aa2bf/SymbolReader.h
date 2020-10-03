#pragma once

// Standard includes
#include <iostream>
#include <cstdint>
#include <vector>

// RapidJSON includes
#include "rapidjson/document.h"

// Project includes
#include "Symbol.h"

// Customizable pixel codes
constexpr char pixel = '#';
constexpr char space = ' ';

class SymbolReader {
public:
	SymbolReader(std::size_t width, std::size_t heigth, std::size_t paddingHorizontal, std::size_t paddingVertical, 
		         rapidjson::Value::Array symbols);

	std::size_t getByteStreamSize() const { return byteStream.size(); }
	std::size_t getSymbolsCount() const { return symbols.size(); }

	bool hasErrors() const { return errorCount != 0; }
	bool hasWarnings() const { return warningCount != 0; }
	std::size_t getErrorCount() const { return errorCount; }
	std::size_t getWarningCount() const { return warningCount; }

	void printByteStreamData(std::ostream& out) const;
	void printSymbolDescriptions(std::ostream& out) const;
	void printParsedSymbols(std::ostream& out) const;
private:

	bool getBitAtOffset(std::size_t offset) const;

	// Font defaults
	std::size_t defaultWidth;
	std::size_t defaultHeight;
	std::size_t defaultPaddingHorizontal;
	std::size_t defaultPaddingVertical;

	// Bytestream construction data
	std::size_t  currentOffset;
	std::uint8_t currentByte;
	std::uint8_t currentBitMask;

	// Output data
	std::vector<std::uint8_t> byteStream;
	std::vector<Symbol>       symbols;

	// Status data
	std::size_t errorCount;
	std::size_t warningCount;
};