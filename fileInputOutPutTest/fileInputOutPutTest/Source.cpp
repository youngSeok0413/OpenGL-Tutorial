#include <iostream>
#include <fstream>
#include <sstream>

static std::string ParseFile(const std::string& filePath) {

	std::fstream fs(filePath);
	std::string line;
	std::stringstream ss;

	while (std::getline(fs, line)) {
		ss << line << '\n';
	}

	return ss.str();
}

int main() {
	std::cout << ParseFile("res/shader/test.txt") << std::endl;
	return 0;
}

//???????????????????!!