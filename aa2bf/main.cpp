// Standard includes
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstdint>
#include <vector>
#include <iomanip>
#include <cstdlib>
#include <cstdio>

// RapidJSON includes
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

// Project includes
#include "SymbolReader.h"


int main(unsigned int argc, const char **argv) {

	// Customizable parameters
	constexpr std::size_t inputBufferSize = 4096;

	// Printing application header
	std::cout << "AA2BF: ASCII Art to Bitfield    " << std::endl;
	std::cout << "Created by: Peter Gyulai (2020) " << std::endl;
	std::cout << "------------------------------- " << std::endl;
	std::cout << std::endl;

	// Opening JSON file for reading symbols 
	FILE* inputFile = nullptr;
	if (argc == 2) { fopen_s(&inputFile,  argv[1],       "rt"); }
	else           { fopen_s(&inputFile, "symbols.json", "rt"); }
	if(!inputFile) { 
		std::cout << "File error: " << argv[1] << " not found. " << std::endl;
		std::cout << "If the path contains spaces or special characters, "
					 "make sure that it is enclosed within double quotes."
				  << std::endl;
		return 1;
	}
	
	// Constructing reading stream
	char inputBuffer[inputBufferSize];
	rapidjson::FileReadStream stream(inputFile, inputBuffer, inputBufferSize);

	// Parsing JSON document
	rapidjson::Document document;
	document.ParseStream(stream);

	// Checking document header validity
	try {

		// Checking parse errors
		if (document.HasParseError()) {

			auto error = document.GetParseError();
			std::cout << document.GetErrorOffset() << std::endl;
			std::cout << rapidjson::GetParseError_En(document.GetParseError());

			throw "Error: The provided document can not be parsed.";
		}

		// Checking required member presence
		if (!document.HasMember("fontname")) throw "Error: The font has no name.";
		if (!document.HasMember("width")) throw "Error: No default width provided.";
		if (!document.HasMember("height")) throw "Error: No default height provided.";
		if (!document.HasMember("paddingHorizontal")) throw "Error: No default horizontal padding provided.";
		if (!document.HasMember("paddingVertical")) throw "Error: No default vertical padding provided.";
		if (!document.HasMember("symbols")) throw "Error: The provided document does not contain symbols.";
	}
	catch (const char* error) {
		std::cout << error << std::endl;
		return 2;
	}

	// Printing document information
	try {
		std::cout << "Document information: " << std::endl;
		std::cout << "--------------------- " << std::endl << std::endl;

		std::cout << "Font name:                    "; std::cout << document["fontname"].GetString() << std::endl;
		std::cout << "Default width:                "; std::cout << document["width"].GetInt() << std::endl;
		std::cout << "Default height:               "; std::cout << document["height"].GetInt() << std::endl;
		std::cout << "Default vertical padding:     "; std::cout << document["paddingVertical"].GetInt() << std::endl;
		std::cout << "Default horizontal padding:   "; std::cout << document["paddingHorizontal"].GetInt() << std::endl;
		std::cout << "Symbol count:                 "; std::cout << document["symbols"].GetArray().Size() << std::endl;
	}
	catch (const char* error) {
		std::cout << error << std::endl;
		return 3;
	}

	// Exctracting document parameters
	std::string fontname = document["fontname"].GetString();
	std::size_t width = document["width"].GetInt();
	std::size_t height = document["height"].GetInt();
	std::size_t paddingHorizontal = document["paddingVertical"].GetInt();
	std::size_t paddingVertical = document["paddingHorizontal"].GetInt();
	rapidjson::Value::Array symbols = document["symbols"].GetArray();

	// Printing process messages
	std::cout << std::endl << std::endl;
	std::cout << "Parsing status: " << std::endl;
	std::cout << "--------------- " << std::endl;
	std::cout << std::endl;
	std::cout << "Parsing started." << std::endl;

	// Reading symbols from document
	SymbolReader reader(width, height, paddingHorizontal, paddingVertical, symbols);
	std::cout << "Parsing finished." << std::endl << std::endl;
	std::cout << "Warnings:        " << reader.getWarningCount() << std::endl;
	std::cout << "Errors:          " << reader.getErrorCount() << std::endl;

	// Printing end statistics
	std::cout << std::endl << std::endl;
	std::cout << "Statistics: " << std::endl;
	std::cout << "----------- " << std::endl;
	std::cout << std::endl;

	std::cout << "Symbols parsed:     " << reader.getSymbolsCount() << " / " 
		      << document["symbols"].GetArray().Size() << std::endl;
	std::cout << "Bytestream size:    " << reader.getByteStreamSize() << " bytes" << std::endl;
	std::cout << "Total memory usage: "
		      << reader.getByteStreamSize() +
		         reader.getSymbolsCount() * (4 * sizeof(std::uint8_t) + sizeof(std::size_t))
		      << " bytes" << std::endl;

	// Printing code output
	std::cout << std::endl << std::endl;
	std::cout << "Code output: " << std::endl;
	std::cout << "------------ " << std::endl;
	std::cout << std::endl;

	if (reader.hasErrors()) {

		// Printing error message
		std::cout << "Code output generation failed due to errors during the parsing process." << std::endl
			      << "Check parsing status messages for the list of warnings and errors.     " << std::endl
				  << std::endl;

		// Printing parsed symbols for self-check
		std::string response;
		std::cout << "Do you want to check the generated bytestream? (y/n)";
		std::cin >> response;
		if (response == "y") reader.printParsedSymbols(std::cout);

		return 4;
	}
	else {

		// Generating code output
		reader.printByteStreamData(std::cout);
		reader.printSymbolDescriptions(std::cout);

		// Printing parsed symbols for self-check
		std::string response;
		std::cout << "Do you want to check the generated bytestream? (y/n)";
		std::cin >> response;
		std::cout << std::endl << std::endl;
		if (response == "y") reader.printParsedSymbols(std::cout);
	}

	return 0;
}