#ifndef TOOL_FUNCTIONS_CPP
#define TOOL_FUNCTIONS_CPP
#include "tool_functions.h"


template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

string numToStr(real32 value, int32 sf)
{
    stringstream iss;
    if(sf > 0)
    {
        iss << fixed << setprecision(sf) << value;
    }else{
        iss << value;
    }
    return iss.str();
}

string numToStr(int32 value)
{
    stringstream iss;
    iss << value;
    return iss.str();
}

string numToStr(int64 value)
{
    stringstream iss;
    iss << value;
    return iss.str();
}

string numToStr(uint32 value)
{
    stringstream iss;
    iss << value;
    return iss.str();
}

string numToStr(uint64 value)
{
    stringstream iss;
    iss << value;
    return iss.str();
}

string binaryToStr(byte value)
{
    bitset<8> b(value);
    return b.to_string();
}

string variableToStr(sf::Vector2u value)
{
    return "("+numToStr((int32)value.x)+", "+numToStr((int32)value.y)+")";
}

string variableToStr(sf::Vector2f value)
{
    return "("+numToStr((real32)value.x, 2)+", "+numToStr((real32)value.y, 2)+")";
}

string variableToStr(sf::Vector2i value)
{
    return "("+numToStr((int32)value.x)+", "+numToStr((int32)value.y)+")";
}

string variableToStr(sf::IntRect value)
{
    return "("+numToStr((int32)value.top)+", "+numToStr((int32)value.left)+", "+numToStr((int32)value.width)+", "+numToStr((int32)value.height)+")";
}

string variableToStr(sf::FloatRect value)
{
    return "("+numToStr((real32)value.top)+", "+numToStr((real32)value.left)+", "+numToStr((real32)value.width)+", "+numToStr((real32)value.height)+")";
}

string variableToStr(sf::Color value)
{
    return "rgb("+numToStr((int32)value.r)+", "+numToStr((int32)value.g)+", "+numToStr((int32)value.b)+")";
}

string variableToStr(uint32 value)
{
    return numToStr((int32)value);
}

string variableToStr(real32 value)
{
    return numToStr(value);
}

string variableToStr(int32 value)
{
    return numToStr(value);
}

string variableToStr(int64 value)
{
    return numToStr(value);
}

string variableToStr(uint64 value)
{
    return numToStr(value);
}

string variableToStr(bool value)
{
    return value ? "True" : "False";
}

bool isNumber(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}


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

bool createLinearJsonFromMemory(Json::Value &root, byte* source, string format)
{
    vector<string> data_format = split(format, ';');
	const string data_token   	= "%";
	const string token_string 	= "s";
	const string token_bool		= "b";
	const string token_int		= "i";
	const string token_uint		= "u";
	const string token_int64	= "I";
	const string token_uint64	= "U";
	const string token_float	= "f";
	const string token_double	= "d";

	const string token_colour 	= "c";

	const string token_byteSkip	= "x";

	uint32 byte_offset = 0;

	for(uint32 i = 0; i < data_format.size(); i++)
	{
		size_t data_split = data_format[i].find_first_of(data_token);

		if(data_split != string::npos)
		{
			string type_string = data_format[i].substr(0, data_split);
			string name_string  = data_format[i].substr(data_split+data_token.length());

			// Update
			if(root.isMember(name_string))
            {
                //TODO(Connor): Implement Update Linear Json Functionality
            // Create
            }else{

                if(type_string==token_string)
                {
                    root[name_string] = *((string*)(source+byte_offset));
                    byte_offset += sizeof(string);
                }else if(type_string==token_bool)
                {
                    root[name_string] = *((bool*)(source+byte_offset));
                    byte_offset += sizeof(bool);
                }else if(type_string==token_int)
                {
                    root[name_string] = *((Json::Value::Int*)(source+byte_offset));
                    byte_offset += sizeof(int16);
                }else if(type_string==token_uint)
                {
                    root[name_string] = *((Json::Value::UInt*)(source+byte_offset));
                    byte_offset += sizeof(uint16);
                }else if(type_string==token_int64)
                {
                    root[name_string] = *((Json::Value::Int64*)(source+byte_offset));
                    byte_offset += sizeof(Json::Value::Int64);
                }else if(type_string==token_uint64)
                {
                    root[name_string] = *((Json::Value::UInt64*)(source+byte_offset));
                    byte_offset += sizeof(Json::Value::UInt64);
                }else if(type_string==token_float)
                {
                    root[name_string] = *((real32*)(source+byte_offset));
                    byte_offset += sizeof(real32);
                }else if(type_string==token_double)
                {
                    root[name_string] = *((real64*)(source+byte_offset));
                    byte_offset += sizeof(real64);
                }else if(type_string==token_colour)
                {
                    Json::Value vec(Json::arrayValue);
                    vec.append( ((sf::Color*)(source+byte_offset))->r );
                    vec.append( ((sf::Color*)(source+byte_offset))->g );
                    vec.append( ((sf::Color*)(source+byte_offset))->b );
                    vec.append( ((sf::Color*)(source+byte_offset))->a );
                    root[name_string] = vec;

                    byte_offset += sizeof(sf::Color);
                }else if(type_string==token_byteSkip)
                {
                    if(isNumber(name_string))
					{
						byte_offset += str2uint(name_string);
					}else{
						cout << "Error: Byteskip failed because '" << name_string << "' is not a number" << endl;
						return false;
					}
                }else{
                    cout << "Error: Member '" << name_string << "' does not exist in the Json::Value" << endl;
                    return false;
                }

            }
		}
	}
}

bool saveLinearJsonFileFromMemory(string filename, byte* source, string format)
{
    Json::Value root;

    if(!createLinearJsonFromMemory(root, source, format))
    {
        return false;
    }

    Json::StyledWriter writer;

    std::ofstream file_id;
    file_id.open(filename);

    file_id << writer.write(root);

    file_id.close();

    return true;
}

bool loadLinearJsonIntoMemory(Json::Value &root, byte* dest, string format, bool haltIfMemberDoesNotExist)
{
	vector<string> data_format = split(format, ';');
	const string data_token   	= "%";
	const string token_string 	= "s";
	const string token_bool		= "b";
	const string token_int		= "i";
	const string token_uint		= "u";
	const string token_int64	= "I";
	const string token_uint64	= "U";
	const string token_float	= "f";
	const string token_double	= "d";

	const string token_colour 	= "c";

	const string token_byteSkip	= "x";

	uint32 byte_offset = 0;

	for(uint32 i = 0; i < data_format.size(); i++)
	{
		size_t data_split = data_format[i].find_first_of(data_token);

		if(data_split != string::npos)
		{
			string type_string = data_format[i].substr(0, data_split);
			string name_string  = data_format[i].substr(data_split+data_token.length());

			if(root.isMember(name_string))
			{
				if(type_string==token_string)
				{
					if(root[name_string].isString())
					{
						string data = root[name_string].asString();
						*((string*)(dest+byte_offset)) = data;
					}else if(haltIfMemberDoesNotExist)
					{
						cout << "Error: Member '" << name_string << "' is not type 'String' in the Json::Value" << endl;
						return false;
					}
					byte_offset += sizeof(string);
				}else if(type_string==token_bool)
				{
					if(root[name_string].isBool() || root[name_string].isConvertibleTo(Json::booleanValue))
					{
						bool data = root[name_string].asBool();
						*((bool*)(dest+byte_offset)) = data;
					}else if(haltIfMemberDoesNotExist)
					{
						cout << "Error: Member '" << name_string << "' is not type 'Boolean' in the Json::Value" << endl;
						return false;
					}
					byte_offset += sizeof(bool);
				}else if(type_string==token_double)
				{
					if(root[name_string].isDouble() || root[name_string].isConvertibleTo(Json::realValue))
					{
						real64 data = root[name_string].asDouble();
						*((real64*)(dest+byte_offset)) = data;
					}else if(haltIfMemberDoesNotExist)
					{
						cout << "Error: Member '" << name_string << "' is not type 'Double' in the Json::Value" << endl;
						return false;
					}
					byte_offset += sizeof(real64);
				}else if(type_string==token_float)
				{
					if(root[name_string].isDouble() || root[name_string].isConvertibleTo(Json::realValue))
					{
						real32 data = root[name_string].asFloat();
						*((real32*)(dest+byte_offset)) = data;
					}else if(haltIfMemberDoesNotExist)
					{
						cout << "Error: Member '" << name_string << "' is not type 'Float' in the Json::Value" << endl;
						return false;
					}
					byte_offset += sizeof(real32);
				}else if(type_string==token_int)
				{
					if(root[name_string].isInt() || root[name_string].isConvertibleTo(Json::intValue))
					{
						int32 data = root[name_string].asInt();
						*((int32*)(dest+byte_offset)) = data;
					}else if(haltIfMemberDoesNotExist)
					{
						cout << "Error: Member '" << name_string << "' is not type 'Int32' in the Json::Value" << endl;
						return false;
					}
					byte_offset += sizeof(int32);
				}else if(type_string==token_int64)
				{
					if(root[name_string].isInt64() || root[name_string].isConvertibleTo(Json::intValue))
					{
						int64 data = root[name_string].asInt64();
						*((int64*)(dest+byte_offset)) = data;
					}else if(haltIfMemberDoesNotExist)
					{
						cout << "Error: Member '" << name_string << "' is not type 'Int64' in the Json::Value" << endl;
						return false;
					}
					byte_offset += sizeof(int64);
				}else if(type_string==token_uint)
				{
					if(root[name_string].isUInt() || root[name_string].isConvertibleTo(Json::uintValue))
					{
						uint32 data = root[name_string].asUInt();
						*((uint32*)(dest+byte_offset)) = data;
					}else if(haltIfMemberDoesNotExist)
					{
						cout << "Error: Member '" << name_string << "' is not type 'UInt32' in the Json::Value" << endl;
						return false;
					}
					byte_offset += sizeof(uint32);
				}else if(type_string==token_uint64)
				{
					if(root[name_string].isUInt64() || root[name_string].isConvertibleTo(Json::uintValue))
					{
						uint64 data = root[name_string].asUInt64();
						*((uint64*)(dest+byte_offset)) = data;
					}else if(haltIfMemberDoesNotExist)
					{
						cout << "Error: Member '" << name_string << "' is not type 'UInt64' in the Json::Value" << endl;
						return false;
					}
					byte_offset += sizeof(uint64);
				}else if(type_string==token_colour)
				{
				    if(root[name_string].isArray())
                    {
                        if(root[name_string].size() >= 3 && root[name_string].size() <= 4)
                        {
                            bool data_safe=true;
                            sf::Color data;

                            if(data_safe && (root[name_string][0].isUInt() || root[name_string][0].isConvertibleTo(Json::uintValue)) && root[name_string][0].asUInt() < 256)
                            {
                                data.r = root[name_string][0].asUInt();
                            }else{
                                data_safe = false;
                            }

                            if(data_safe && (root[name_string][1].isUInt() || root[name_string][1].isConvertibleTo(Json::uintValue)) && root[name_string][1].asUInt() < 256)
                            {
                                data.g = root[name_string][1].asUInt();
                            }else{
                                data_safe = false;
                            }

                            if(data_safe && (root[name_string][2].isUInt() || root[name_string][2].isConvertibleTo(Json::uintValue)) && root[name_string][2].asUInt() < 256)
                            {
                                data.b = root[name_string][2].asUInt();
                            }else{
                                data_safe = false;
                            }

                            if(root[name_string].size() >= 4)
                            {
                                if(data_safe && (root[name_string][3].isUInt() || root[name_string][3].isConvertibleTo(Json::uintValue)) && root[name_string][3].asUInt() < 256)
                                {
                                    data.a = root[name_string][3].asUInt();
                                }else{
                                    data_safe = false;
                                }
                            }

                            if (data_safe)
                            {
                                *((sf::Color*)(dest+byte_offset)) = data;
                            }else if(haltIfMemberDoesNotExist)
                            {
                                cout << "Error: Member '" << name_string << "' has an incorrect types of values to be type 'Colour'([r,g,b,a]) in the Json::Value" << endl;
                                return false;
                            }
                        }else if(haltIfMemberDoesNotExist)
                        {
                            cout << "Error: Member '" << name_string << "' has an incorrect number of values to be type 'Colour'([r,g,b,a]) in the Json::Value" << endl;
                            return false;
                        }
                    }else if(haltIfMemberDoesNotExist)
					{
						cout << "Error: Member '" << name_string << "' is not type 'Colour'([r,g,b,a]) in the Json::Value" << endl;
						return false;
					}
					byte_offset += sizeof(sf::Color);
                }else if(token_string==token_byteSkip)
				{
					if(isNumber(name_string))
					{
						byte_offset += str2uint(name_string);
					}else{
						cout << "Error: Byteskip failed because '" << name_string << "' is not a number" << endl;
						return false;
					}
				}

			}else if(haltIfMemberDoesNotExist)
			{
				cout << "Error: Member '" << name_string << "' does not exist in the Json::Value" << endl;
				return false;
			}

		}else{
			// TODO(Connor): Logging
			cout << "Error: While loading json into memory, data format member '" << data_format[i] << "' missing data token '" << data_token << "'" << endl;
			return false;
		}
	}

	return true;
}

uint32 str2uint(string a)
{
	uint32 output;
	istringstream ss(a);
	ss >> output;
	return output;
}

bool str2char(string &str, char* dest, uint32 buffer_size)
{
    uint32 buffer_end = min(buffer_size-1,(uint32)str.length());
    str.copy(dest, buffer_end);
    dest[buffer_end]='\0';
    return (str.length() < buffer_size);
}

real32 interpolate(real32 a, real32 b, real32 t)
{
	return a+((b-a)*t);
}

sf::Color interpolate(sf::Color a, sf::Color b, real32 t)
{
	return sf::Color((byte)interpolate((real32)a.r, (real32)b.r, t), (byte)interpolate((real32)a.g, (real32)b.g, t), (byte)interpolate((real32)a.b, (real32)b.b, t), (byte)interpolate((real32)a.a, (real32)b.a, t));
}

real32 precentDiff(real32 min, real32 max, real32 x)
{
	return (x-min)/(max-min);
}

string whitespace(uint32 size)
{
	string output;
	for(uint32 i=0; i < size; i++)
	{
		output+=" ";
	}
	return output;
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
