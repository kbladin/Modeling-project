#include "user_input.h"

float readFloat(std::string msg){
	float f = 0.0f;
	std::string input = "";
	while (true) {
		std::cout << msg << std::endl;
		std::getline(std::cin, input);

		// This code converts from string to number safely.
		std::stringstream myStream(input);
		if (myStream >> f)
	 		break;
		std::cout << "Bad input, try again" << std::endl;
	}

	return f;
}
