#include "SymbolReader.h"
#include <algorithm>
#include <iomanip>

SymbolReader::SymbolReader(std::size_t width,
						   std::size_t height,
						   std::size_t paddingHorizontal,
						   std::size_t paddingVertical,
						   rapidjson::Value::Array symbols)
	: defaultWidth(width),
	  defaultHeight(height),
	  defaultPaddingHorizontal(paddingHorizontal),
	  defaultPaddingVertical(paddingVertical),
	  currentOffset(0),
	  currentByte(0),
	  currentBitMask(128),
	  errorCount(0),
	  warningCount(0)
{

	// Parsing JSON symbols
	for (auto it = symbols.begin(); it != symbols.end(); it++) {

		bool statusOK = true;
		std::string name;
		std::size_t width = defaultWidth;
		std::size_t height = defaultHeight;
		std::size_t paddingHorizontal = defaultPaddingHorizontal;
		std::size_t paddingVertical = defaultPaddingVertical;

		// Parsing symbol members
		for (auto& member : it->GetObject()) {

			// Checking symbol specific parameters
			if (member.name == "name")              name = member.value.GetString();
			if (member.name == "width")             width = member.value.GetInt();
			if (member.name == "height")            height = member.value.GetInt();
			if (member.name == "paddingHorizontal") paddingHorizontal = member.value.GetInt();
			if (member.name == "paddingVertical")   paddingVertical = member.value.GetInt();

			// Parsing symbol data
			if (member.name == "data") {

				// Printing status message
				std::cout << "Parsing symbol (\"" << name << "\")..." << std::endl;

				// Extracting data array
				auto data = member.value.GetArray();

				// Checking data height 
				if (data.Size() > height) {
					std::cout << "Error (\"" << name << "\"): Height mismatch - expecting " 
						      << height << " rows, got " << data.Size()
						      << std::endl;
					errorCount++;
					statusOK = false;
				}

				// Parsing each row of data array
				for (auto dataIt = data.begin(); dataIt != data.end(); dataIt++) {

					// Extracting row string
					std::string row = dataIt->GetString();

					// Checking data width
					if (row.length() != width) {
						std::cout << "Error (\"" << name << "\"): Width mismatch  - expecting " 
							      << width << " columns, got " << row.length() 
							      << std::endl;
						errorCount++;
						statusOK = false;
					}

					// Generating bytestream
					for (std::size_t i = 0; i < row.length(); i++) {

						// Setting next bit according to input
						if (row[i] == pixel) currentByte |= currentBitMask;

						// Shifting bitmask 
						currentBitMask = currentBitMask >> 1;

						// Flushing byte when ready
						if (currentBitMask == 0) {
							byteStream.push_back(currentByte);
							currentByte = 0;
							currentBitMask = 128;
						}
					}
				}
			}
		}

		// Saving symbol descriptor
		if(statusOK) this->symbols.push_back(Symbol(name, currentOffset, width, height, paddingHorizontal, paddingVertical));

		// Incrementing current offset
		currentOffset += width * height;
	}

	// Flushing optional remaining byte to bytestream
	if (currentBitMask != 128) byteStream.push_back(currentByte);

	// Sorting symbols into ASCII table order
	std::cout << "Sorting symbols... " << std::endl;
	std::sort(this->symbols.begin(), this->symbols.end(), symbolCompare);
}

void SymbolReader::printByteStreamData(std::ostream& out) const {

	// Printing macro definition for placing array into PROGMEM
	out << "#define AA2BF_PROGMEM     // TODO: Place attribute that defines program memory storage \n\n";

	// Printing array header
	out << "uint8_t data[] = { \n\t";

	// Counter for databytes printed
	std::size_t dataCounter = 0;

	// Priting array data
	for (auto it = byteStream.begin(); it != byteStream.end(); it++, dataCounter++) {

		// Printing next databyte
		out << "0x" << std::setfill('0') << std::setw(2) << std::hex	// Preparing stream for hexadecimals
			<< (unsigned int)(*it)										// Printing byte in hexadecimal format
			<< ((it + 1) != byteStream.end() ? ", " : " ");				// Optionally printing commas until the last byte

	 // Inserting newline every 4 databytes printed
		if (dataCounter % 4 == 3 && (it + 1) != byteStream.end()) {
			out << "\n\t";
		}
	}

	// Printing end of array definition
	out << "\n} AA2BF_PROGMEM; \n\n";
}

void SymbolReader::printSymbolDescriptions(std::ostream& out) const {

	// Preparing stream for decimals
	out << std::dec;

	// Printing array header
	out << "SymbolDescriptor descriptors[] = { \n\t";

	// Printing symbol descriptors
	for (auto it = symbols.begin(); it != symbols.end(); it++) {

		// Printing next descriptor
		out << "{ " << std::setfill(' ')
			<< std::setw(4)  << it->getOffset()            << ", "
			<< std::setw(4)  << it->getWidth()             << ", "
			<< std::setw(4)  << it->getHeight()            << ", "
			<< std::setw(4)  << it->getPaddingHorizontal() << ", "
			<< std::setw(4)  << it->getPaddingVertical()   << " }" << ((it + 1) != symbols.end() ? ", // \"" : "  // \"")
		    << it->getName() << "\""
			<< ((it + 1) != symbols.end() ? "\n\t" : " ");
	}  

	// Printing end of array definition
	out << "\n} AA2BF_PROGMEM; \n\n";
}

bool SymbolReader::getBitAtOffset(std::size_t offset) const {
	std::size_t byteIndex = offset / 8;
	std::size_t bitIndex = offset % 8;

	return byteStream[byteIndex] & (1 << (7 - bitIndex));
}

void SymbolReader::printParsedSymbols(std::ostream& out) const {
	
	// Iterating symbol descriptors
	for (auto it = symbols.begin(); it != symbols.end(); it++) {

		// Calculating bytestream offset
		std::size_t offset = it->getOffset();

		// Printing symbol information
		std::cout << "Symbol: " << "\"" << it->getName() << "\"" << " (" << it->getWidth() << ", " << it->getHeight()
				  << ", " << it->getPaddingHorizontal() << ", " << it->getPaddingVertical() << "):" 
				  << std::endl << std::endl;

		// Printing top boundary
		std::cout << std::string(it->getWidth() + 2 * it->getPaddingHorizontal() + 2, '*') << std::endl;
		for (std::size_t i = 0; i < it->getPaddingVertical(); i++) {
			std::cout << "*" << std::string(it->getWidth() + it->getPaddingHorizontal() * 2, ' ') << "*" << std::endl;
		}

		// Printing symbol 
		for (std::size_t i = 0; i < it->getHeight(); i++) {

			// Printing row beginning
			std::cout << "*" << std::string(it->getPaddingHorizontal(), ' ');

			for (std::size_t j = 0; j < it->getWidth(); j++) {

				// Printing the next pixel
				if (getBitAtOffset(offset++)) std::cout << "#";
				else                          std::cout << " ";
			}

			// Printing row ending
			std::cout << std::string(it->getPaddingHorizontal(), ' ') << "*" << std::endl;
		}

		// Printing bottom boundary
		for (std::size_t i = 0; i < it->getPaddingVertical(); i++) {
			std::cout << "*" << std::string(it->getWidth() + it->getPaddingHorizontal() * 2, ' ') << "*" << std::endl;
		}
		std::cout << std::string(it->getWidth() + 2 * it->getPaddingHorizontal() + 2, '*') << std::endl;
		std::cout << std::endl << std::endl;
	}
}