#ifndef TOOL_FUNCTIONS_CPP
#define TOOL_FUNCTIONS_CPP
#include "tool_functions.h"

string fastReadFile(string filename)
{
	ifstream file;
	file.open(filename);

	// Return Empty String If File Cannot Open
	if (!file.good())
	{
		cout << "Error: '" << filename << "' could not be opened!" << '\n';
		return "";
	}

	// Allocate Memory Size Needed From File
	string contents;
	file.seekg(0, ios::end);
	contents.resize(file.tellg());

	// Read Contents Of File Into Memory
	file.seekg(0, ios::beg);
	file.read(&contents[0], contents.size());

	// Clean Up And Return Contents
	file.close();
	return(contents);
}

Json::Value readJsonFile(string filename)
{
	// Initalize Parser With File Contents
	Json::Reader parser;
	Json::Value  output;
	// unSerialize File Data
	bool parseSuccess = parser.parse(fastReadFile(filename), output);

	//Return Empty Value If Error Reading
	if (!parseSuccess)
	{
		cout << "Error: '" << filename << "' JSON parse failure! Message: '" << string(parser.getFormattedErrorMessages()) << "'\n";
		return Json::Value();
	}

	// Return Output
	return output;
}

uint32 str2uint(string a)
{
	uint32 output;
	istringstream ss(a);
	ss >> output;
	return output;
}

real32 interpolate(real32 a, real32 b, real32 t)
{
	return a+((b-a)*t);
}

void drawLine(sf::RenderTarget &target, sf::Vector2f p1, sf::Vector2f p2, sf::Color colour)
{
	sf::Vertex line[] =
	{
		sf::Vertex(p1, colour),
		sf::Vertex(p2, colour)
	};

	target.draw(line, 2, sf::Lines);
}

void drawLine(sf::RenderTarget &target, sf::Vector2f p1, sf::Vector2f p2, real32 thickness, sf::Color colour)
{
	real32		 angle  = getAngle(p2, p1);
	real32		 length = getDistance(p1, p2);

	sf::RectangleShape line;
	line.setFillColor(colour);
	line.setSize(sf::Vector2f(length, thickness));
	line.setOrigin(0, thickness/2.0);
	line.setRotation(angle*kDEGREES_PER_RADIAN);
	line.setPosition(p1);

	target.draw(line);
}

#endif /* end of include guard: TOOL_FUNCTIONS_CPP */
