// DxfParser.cpp

// Implements the Dxf::Parser class representing the DXF file format parser

#include "DxfParser.hpp"
#include <iostream>
//#include "fmt/format.h"





namespace Dxf::Parser
{




namespace
{

/** Returns true if the two strings are the same, ignoring their case.
Optimized for comparing to a lower-case string constant. */
static bool isSameStringIgnoreCase(const std::string & aStr1, const char * aLowerStr2)
{
	auto len = aStr1.length();
	for (size_t i = 0; i < len; ++i)
	{
		if (tolower(aStr1[i]) != aLowerStr2[i])
		{
			return false;
		}
	}
	return (aLowerStr2[len] == 0);
}

}





class Parser
{
	/** The LineExtractor that reads the data source and splits it into individual lines. */
	LineExtractor mLineExtractor;

	/** The drawing into which the parsed data is applied. */
	std::shared_ptr<Drawing> mDrawing;





	/** Unified entrypoint for when an error occurs.
	Put a debugger breakpoint here in order to debug mysterious errors. */
	[[noreturn]] void throwError(std::string && aMessage)
	{
		throw Error(mLineExtractor.currentLineNum(), std::move(aMessage));
	}





	/** Returns a copy of the string with the whitespace removed from the front and end. */
	std::string trimWhitespace(const std::string & aStr)
	{
		size_t len = aStr.length();
		size_t start = 0;
		while (start < len)
		{
			if (static_cast<unsigned char>(aStr[start]) > 32)
			{
				break;
			}
			++start;
		}
		if (start == len)
		{
			return "";
		}

		size_t end = len;
		while (end >= start)
		{
			if (static_cast<unsigned char>(aStr[end]) > 32)
			{
				break;
			}
			--end;
		}

		return aStr.substr(start, end - start + 1);
	}





	/** Parses any integer type.
	Throws an Error upon invalid input. */
	template <class T>
	T stringToInt(const std::string & aStr)
	{
		if (aStr.empty())
		{
			throwError("invalid number: <empty string>");
		}

		size_t i = 0;
		bool isPositive = true;
		T result = 0;
		if (aStr[0] == '+')
		{
			i++;
		}
		else if (aStr[0] == '-')
		{
			i++;
			isPositive = false;
		}
		if (isPositive)
		{
			for (size_t size = aStr.size(); i < size; i++)
			{
				if ((aStr[i] < '0') || (aStr[i] > '9'))
				{
					throwError("Invalid character in number: \"{}\"");
				}
				if (std::numeric_limits<T>::max() / 10 < result)
				{
					throwError("Number is too large.");
				}
				result *= 10;
				T digit = static_cast<T>(aStr[i] - '0');
				if (std::numeric_limits<T>::max() - digit < result)
				{
					throwError("Number is too large.");
				}
				result += digit;
			}
		}
		else
		{
			// Unsigned result cannot be signed!
			if (!std::numeric_limits<T>::is_signed)
			{
				throwError("Unexpected negative number");
			}

			for (size_t size = aStr.size(); i < size; i++)
			{
				if ((aStr[i] < '0') || (aStr[i] > '9'))
				{
					throwError("Invalid character in number: \"{}\"");
				}
				if (std::numeric_limits<T>::min() / 10 > result)
				{
					throwError("Number is too large.");
				}
				result *= 10;
				T digit = static_cast<T>(aStr[i] - '0');
				if (std::numeric_limits<T>::min() + digit > result)
				{
					throwError("Number is too large.");
				}
				result -= digit;
			}
		}
		return result;
	}




	/** Parses the specified string into a double-precision floating point number.
	Throws an Error upon invalid input. */
	double stringToDouble(const std::string & aStr)
	{
		if (aStr.empty())
		{
			throwError("invalid number: <empty string>");
		}

		try
		{
			auto res = std::stod(aStr);
			return res;
		}
		catch (const std::exception & exc)
		{
			std::cout<<"Got "<<exc.what()<<std::endl;
			throwError("Cannot parse number");
		}
	}





	/** Reads the next group code and value from the stream.
	Assumes that mStream converts CRLF to LF upon reading. */
	std::pair<int, std::string> readNext()
	{
		auto groupCodeStr = mLineExtractor.getNextLine();
		auto isWhiteSpace = [](const char aChar)
		{
			return ((aChar == ' ') || (aChar == '\t'));
		};
		groupCodeStr.erase(std::remove_if(groupCodeStr.begin(), groupCodeStr.end(), isWhiteSpace), groupCodeStr.end());  // Remove any whitespace from the line
		auto groupCode = stringToInt<int>(groupCodeStr);
		auto value = mLineExtractor.getNextLine();
		return {groupCode, value};
	}





	void parseHeaderSection()
	{
		return skipUntilSectionEnd();
	}





	void parseClassesSection()
	{
		return skipUntilSectionEnd();
	}





	/** Skips input data from mStream until a section end {0, ENDSEC} is encountered. */
	void skipUntilSectionEnd()
	{
		for (;;)
		{
			auto [groupCode, value] = readNext();
			if (groupCode == 0)
			{
				if (isSameStringIgnoreCase(value, "endsec"))
				{
					return;
				}
			}
		}  // for (;;)
	}





	/** Parses the entire TABLES section of the DXF data.
	Finishes after encountering the {0, ENDSEC} pair. */
	void parseTablesSection()
	{
		for (;;)
		{
			auto [groupCode, value] = readNext();
			switch (groupCode)
			{
				case 0:
				{
					if (isSameStringIgnoreCase(value, "endsec"))
					{
						return;
					}
					else if (isSameStringIgnoreCase(value, "table"))
					{
						parseSingleTable();
					}
					else
					{
						throwError("Unexpected item in TABLES section");
					}
					break;
				}
				default:
				{
					throwError("Unexpected group code.");
				}
			}
		}
	}




	/** Parses a single table out of the TABLES section of the DXF data.
	Finishes after encountering the {0, ENDTAB} pair*/
	void parseSingleTable()
	{
		for (;;)
		{
			auto [groupCode, value] = readNext();
			switch (groupCode)
			{
				case 0:
				{
					if (isSameStringIgnoreCase(value, "endtab"))
					{
						return;
					}
					break;
				}
				case 2:
				{
					if (isSameStringIgnoreCase(value, "layer"))
					{
						return parseLayerTable();
					}
					// No error on unknown values, we skip tables other than layer
					break;
				}
			}
		}
	}





	/** Parses the LAYER table contents.
	Finishes after encountering the {0, ENDTAB} pair. */
	void parseLayerTable()
	{
		std::shared_ptr<Layer> currentLayer;
		for (;;)
		{
			auto [groupCode, value] = readNext();
			switch (groupCode)
			{
				case 0:
				{
					if (isSameStringIgnoreCase(value, "endtab"))
					{
						return;
					}
					else if (isSameStringIgnoreCase(value, "layer"))
					{
						currentLayer = nullptr;
					}
					break;
				}  // case 0
				case 2:
				{
					if (currentLayer != nullptr)
					{
						throwError("Layer entry has a duplicate name (group 2)");
					}
					try
					{
						currentLayer = mDrawing->addLayer(value);
					}
					catch (Dxf::Drawing::LayerAlreadyExists & exc)
					{
						std::cout<<"Got "<<exc.what()<<std::endl;
						throwError("Duplicate layer");
					}
					break;
				}
				case 62:
				{
					if (currentLayer != nullptr)
					{
						currentLayer->setDefaultColor(stringToInt<Color>(trimWhitespace(value)));
					}
					break;
				}
				// Do NOT throw errors on unknown group codes, we ignore a lot of them
			}
		}
	}




	void parseBlocksSection()
	{
		// TODO: Parse the block definitions
		return skipUntilSectionEnd();
	}





	/** Parses the entities from mStream.
	If aParentBlockDef is valid, the entities are stored within that BlockDefinition. */
	void parseEntitiesSection(BlockDefinition * aParentBlockDef)
	{
		PrimitivePtr last;
		PrimitivePtr cur;
		std::string currentCaption;  // Accumulator for text / mtext
		bool isPolylineSequence = false;
		for (;;)
		{
			auto [groupCode, value] = readNext();
			switch (groupCode)
			{
				case 0:
				{
					if (cur != nullptr)
					{
						if (
							(last != nullptr) &&
							(last->mObjectType == otPolyline) &&
							(cur->mObjectType == otVertex) &&
							isPolylineSequence
						)
						{
							std::static_pointer_cast<Polyline>(last)->addVertex(std::move(*std::static_pointer_cast<Vertex>(cur)));
							cur.reset();
						}
						else
						{
							last = cur;
							if (aParentBlockDef != nullptr)
							{
								aParentBlockDef->mObjects.push_back(cur);
							}
						}  // not poly vertex
						cur = nullptr;
					}
					if (isSameStringIgnoreCase(value, "endsec"))
					{
						return;
					}
					if (isSameStringIgnoreCase(value, "endblk"))
					{
						return;
					}
					if (isSameStringIgnoreCase(value, "seqend"))
					{
						isPolylineSequence = false;
					}
					else if (isSameStringIgnoreCase(value, "line"))
					{
						cur = std::make_shared<Line>();
					}
					else if (isSameStringIgnoreCase(value, "polyline"))
					{
						cur = std::make_shared<Polyline>();
						isPolylineSequence = true;
					}
					else if (isSameStringIgnoreCase(value, "vertex"))
					{
						cur = std::make_shared<Vertex>();
					}
					else if (isSameStringIgnoreCase(value, "lwpolyline"))
					{
						cur = std::make_shared<LWPolyline>();
					}
					else if (isSameStringIgnoreCase(value, "text"))
					{
						cur = std::make_shared<Text>();
					}
					else if (isSameStringIgnoreCase(value, "mtext"))
					{
						cur = std::make_shared<Text>();
					}
					else if (isSameStringIgnoreCase(value, "point"))
					{
						cur = std::make_shared<Point>();
					}
					else if (isSameStringIgnoreCase(value, "arc"))
					{
						cur = std::make_shared<Arc>();
					}
					else if (isSameStringIgnoreCase(value, "circle"))
					{
						cur = std::make_shared<Circle>();
					}
					else
					{
						// DEBUG: std::cout << "Unhandled entity: " << value << "\n";
					}
					break;
				}  // case 0

				case 1:
				{
					// text parsing:
					if (cur != nullptr)
					{
						switch (cur->mObjectType)
						{
							case otText:
							{
								currentCaption.append(value);
								const auto text = std::static_pointer_cast<Text>(cur);
								text->mRawText.assign(convertDxfText(currentCaption));
								currentCaption.clear();
								break;
							}
							case otError:
							case otLine:
							case otPolyline:
							case otLWPolyline:
							case otPolygon:
							case otSolid:
							case otCircle:
							case otSimpleEllipse:
							case otHatch:
							case otArc:
							case otBlock:
							case otVertex:
							case otPoint:
							default:
							{
								// do nothing
								break;
							}
						}
					}
					break;
				}  // case 1

				case 3:
				{
					// additional text parsing:
					if (cur != nullptr)
					{
						switch (cur->mObjectType)
						{
							case otText: currentCaption.append(value); break;
							default:
							{
								// do nothing
								break;
							}
						}
					}
					break;
				}  // case 3

				case 8:  // layer
				{
					if (cur != nullptr)
					{
						auto lay = mDrawing->layerByName(value);
						if (lay != nullptr)
						{
							lay->addObject(cur);
						}
					}
					break;
				}

				case 10:
				{
					if (cur == nullptr)
					{
						break;
					}
					switch (cur->mObjectType)
					{
						case otLWPolyline:
						{
							std::static_pointer_cast<LWPolyline>(cur)->addVertex({stringToDouble(value), 0});
							break;
						}
						case otLine:
						case otPoint:
						case otVertex:
						case otText:
						case otCircle:
						case otArc:
						{
							cur->mPos.mX = stringToDouble(value);
							break;
						}
						case otPolyline: break;  // Ignore

						default:
						{
							throwError("Unhandled object type with groupcode 10");
							break;
						}
					}
					break;
				}  // case 10

				case 11:
				{
					if (cur == nullptr)
					{
						break;
					}
					switch (cur->mObjectType)
					{
						case otLine: std::static_pointer_cast<Line>(cur)->mPos2.mX = stringToDouble(value); break;
						default:
						{
							break;
						}
					}
					break;
				}  // case 11

				case 20:
				{
					if (cur == nullptr)
					{
						break;
					}
					switch (cur->mObjectType)
					{
						case otLWPolyline:
						{
							if (!std::static_pointer_cast<LWPolyline>(cur)->mVertices.empty())
							{
								std::static_pointer_cast<LWPolyline>(cur)->mVertices.back().mPos.mY = stringToDouble(value);
							}
							break;
						}
						case otLine:
						case otPoint:
						case otVertex:
						case otText:
						case otCircle:
						case otArc:
						{
							cur->mPos.mY = stringToDouble(value);
							break;
						}
						case otPolyline: break;  // Ignore

						default:
						{
							throwError("Unhandled object type with groupcode 20");
						}
					}
					break;
				}  // case 20

				case 21:
				{
					if (cur == nullptr)
					{
						break;
					}
					switch (cur->mObjectType)
					{
						case otText:
						{
							break;  // Ignore
						}
						case otLine: std::static_pointer_cast<Line>(cur)->mPos2.mY = stringToDouble(value); break;

						default:
						{
							throwError("Unhandled object type with groupcode 21");
							break;
						}
					}
					break;
				}  // case 21

				case 30:
				{
					if (cur == nullptr)
					{
						break;
					}
					switch (cur->mObjectType)
					{
						case otLine:
						case otPoint:
						case otVertex:
						case otText:
						case otCircle:
						case otArc:
						{
							cur->mPos.mZ = stringToDouble(value);
							break;
						}
						case otPolyline: break;  // Ignore

						default:
						{
							throwError("Unhandled object type with groupcode 30: {}");
							break;
						}
					}
					break;
				}  // case 30

				case 31:
				{
					if (cur == nullptr)
					{
						break;
					}
					switch (cur->mObjectType)
					{
						case otText:
						{
							break;  // Ignore
						}
						case otLine: std::static_pointer_cast<Line>(cur)->mPos2.mZ = stringToDouble(value); break;

						default:
						{
							throwError("Unhandled object type with groupcode 31: {}");
							break;
						}
					}
					break;
				}  // case 31

				case 38:
				{
					if (cur == nullptr)
					{
						break;
					}
					switch (cur->mObjectType)
					{
						case otLWPolyline: std::static_pointer_cast<LWPolyline>(cur)->mPos.mZ = stringToDouble(value); break;
						case otPolyline:
						{
							// Silently ignore this "error"
							break;
						}

						case otError:
						case otLine:
						case otPolygon:
						case otSolid:
						case otCircle:
						case otSimpleEllipse:
						case otHatch:
						case otArc:
						case otText:
						case otBlock:
						case otVertex:
						case otPoint:
						default:
						{
							throwError("Unhandled object type with groupcode 38: {}");
						}
					}
					break;
				}  // case 38

				case 40:
				{
					if (cur == nullptr)
					{
						break;
					}
					switch (cur->mObjectType)
					{
						case otCircle:
						{
							std::static_pointer_cast<Circle>(cur)->mRadius = stringToDouble(value);
							break;
						}
						case otArc:
						{
							std::static_pointer_cast<Arc>(cur)->mRadius = stringToDouble(value);
							break;
						}
						default:
						{
							throwError("Unhandled object type with groupcode 40: {}");
						}
					}
					break;
				}  // case 40

				case 42:
				{
					if (cur == nullptr)
					{
						break;
					}
					switch (cur->mObjectType)
					{
						case otVertex:
						{
							std::static_pointer_cast<Vertex>(cur)->mBulge = stringToDouble(value);
							break;
						}
						case otLWPolyline:
						{
							auto polyline = std::static_pointer_cast<LWPolyline>(cur);
							if (!polyline->mVertices.empty())
							{
								polyline->mVertices.back().mBulge = stringToDouble(value);
							}
							break;
						}
						default:
						{
							throwError("Unhandled ob ject type with groupcode 42: {}");
						}
					}
					break;
				}  // case 42

				case 50:
				{
					if (cur == nullptr)
					{
						break;
					}
					switch (cur->mObjectType)
					{
						case otArc:
						{
							std::static_pointer_cast<Arc>(cur)->mStartAngle = stringToDouble(value);
							break;
						}
						default:
						{
							throwError("Unhandled object type with groupcode 50: {}");
						}
					}
					break;
				}  // case 50

				case 51:
				{
					if (cur == nullptr)
					{
						break;
					}
					switch (cur->mObjectType)
					{
						case otArc:
						{
							std::static_pointer_cast<Arc>(cur)->mEndAngle = stringToDouble(value);
							break;
						}
						default:
						{
							throwError("Unhandled object type with groupcode 51: {}");
						}
					}
					break;
				}  // case 51

				case 70:
				{
					if (cur == nullptr)
					{
						break;
					}
					switch (cur->mObjectType)
					{
						case otPolyline:
						{
							std::static_pointer_cast<Polyline>(cur)->mFlags = static_cast<PolylineFlags>(stringToInt<int>(trimWhitespace(value)));
							break;
						}
						case otLWPolyline:
						{
							std::static_pointer_cast<LWPolyline>(cur)->mFlags = static_cast<PolylineFlags>(stringToInt<int>(trimWhitespace(value)));
							break;
						}
						case otVertex: break;  // Ignore

						case otError:
						case otLine:
						case otPolygon:
						case otSolid:
						case otCircle:
						case otSimpleEllipse:
						case otHatch:
						case otArc:
						case otText:
						case otBlock:
						case otPoint:
						default:
						{
							throwError("Unhandled object type with groupcode 70: {}");
						}
					}  // switch (cur->mObjectType)
					break;
				}  // case 70

				default:
				{
					/*
					// TODO: We don't store the extended data yet
					if (cur == nullptr) break;
					if (cur->Extended == nullptr)
					{
						cur->Extended = new ExtendedData();
					}

					setExtendedEntityData(*cur->Extended, groupCode, std::move(value));
					*/
					break;
				}
			}  // switch (mCurrentGroup)
		}  // for (;;)
	}





	/*
	// TODO: Process the extended entity data
	void setExtendedEntityData(ExtendedData & extended, int aGroupCode, std::string && aValue)
	{
		switch (aGroupCode)
		{
			case 1000:
			{
				extended.mString = std::move(aValue);
				break;
			}
			case 1001:
			{
				// TODO: Add check that the group is the first one within the extended data and the name is in a reference table
				extended.mApplicationName.assign(mCurrentValue);
				break;
			}
			case 1002:
			{
				// TODO: Add support for blocks
				break;
			}
			case 1003:
			{
				extended.LayerName.assign(mCurrentValue);
				break;
			}
			case 1004:
			{
				// TODO: Add parser of binary data
				break;
			}
			case 1005:
			{
				long value;
				if (StringToInt(mCurrentValue, &value) == FALSE || value < INT_MIN || value > INT_MAX) value = 0u;
				extended.DatabaseHandle = static_cast<int>(value);
				break;
			}
			case 1010:
			{
				extended.X = _tstof(mCurrentValue);
				break;
			}
			case 1020:
			{
				extended.Y = _tstof(mCurrentValue);
				break;
			}
			case 1030:
			{
				extended.Z = _tstof(mCurrentValue);
				break;
			}
			case 1011:
			{
				extended.WorldSpacePositionX = _tstof(mCurrentValue);
				break;
			}
			case 1021:
			{
				extended.WorldSpacePositionY = _tstof(mCurrentValue);
				break;
			}
			case 1031:
			{
				extended.WorldSpacePositionZ = _tstof(mCurrentValue);
				break;
			}
			case 1012:
			{
				extended.WorldSpaceDisplacementX = _tstof(mCurrentValue);
				break;
			}
			case 1022:
			{
				extended.WorldSpaceDisplacementY = _tstof(mCurrentValue);
				break;
			}
			case 1032:
			{
				extended.WorldSpaceDisplacementZ = _tstof(mCurrentValue);
				break;
			}
			case 1013:
			{
				extended.WorldDirectionX = _tstof(mCurrentValue);
				break;
			}
			case 1023:
			{
				extended.WorldDirectionY = _tstof(mCurrentValue);
				break;
			}
			case 1033:
			{
				extended.WorldDirectionZ = _tstof(mCurrentValue);
				break;
			}
			case 1040:
			{
				extended.Real = _tstof(mCurrentValue);
				break;
			}
			case 1041:
			{
				extended.Distance = _tstof(mCurrentValue);
				break;
			}
			case 1042:
			{
				extended.ScaleFactor = _tstof(mCurrentValue);
				break;
			}
			case 1070:
			{
				long value;
				if (StringToInt(mCurrentValue, &value) == FALSE || value < 0 || value > USHRT_MAX) value = 0;
				extended.Integer = static_cast<unsigned short int>(value);
				break;
			}
			case 1071:
			{
				long value;
				if (StringToInt(mCurrentValue, &value) == FALSE || value < INT_MIN || value > INT_MAX) value = 0;
				extended.Long = static_cast<int>(value);
				break;
			}
			default:
			{
				break;
			}
		}
	}
	*/





	void parseObjectsSection()
	{
		return skipUntilSectionEnd();
	}





	/*
	// TODO: Multiline text formatting
	void MText::RemoveFormatting()
	{
		// AutoCAD DXF - Format Multiline Text in an Alternate Text Editor

		std::string unformatted;
		unsigned int bracesNestedLevel = 0u;
		bool isCodeParameter = false;

		for (std::string::const_iterator i = Caption.begin(); i != Caption.end(); ++i)
		{
			if (isCodeParameter)
			{
				if (*i == ';')
				{
					isCodeParameter = false;
				}

				continue;
			}
			else if (*i == '{')
			{
				++bracesNestedLevel;
				continue;
			}
			else if (*i == '}')
			{
				// If the level is 0, skip it and tolerate this fault
				if (bracesNestedLevel > 0)
				{
					--bracesNestedLevel;
				}

				continue;
			}
			else if (*i != '\\')
			{
				unformatted.push_back(*i);
				continue;
			}

			++i;
			if (i == Caption.end())
			{
				unformatted.push_back('\\');
				break;
			}

			switch (*i)
			{
				case 'L':  // Turns overline on
				case 'l':  // Turns overline off
				case 'O':  // Turns underline on
				case 'o':  // Turns underline off
				case 'K':  // Turns strike-through on
				case 'k':  // Turns strike-through off
					// Skip
					break;
				case '~':  // Inserts a nonbreaking space
					unformatted.push_back(' ');
					break;
				case '\\':  // Inserts a backslash
					unformatted.push_back('\\');
					break;
				case '{':  // Inserts an opening brace
					unformatted.push_back('{');
					break;
				case '}':  // Inserts a closing brace
					unformatted.push_back('}');
					break;
				case 'P':  // Ends paragraph
					unformatted.push_back(' ');
					break;
				case 'X':  // Paragraph wrap on the dimension line (only in dimensions)
				case 'C':  // Changes to the specified color
				case 'F':  // Changes to the specified font file
				case 'f':  // -||-
				case 'H':  // Changes to the text height specified in drawing units or
								// Changes the text height to a multiple of the current text height
				case 'S':  // Stacks the subsequent text at the /, #, or ^ symbol
				case 'T':  // Adjusts the space between characters
				case 'Q':  // Changes obliquing angle
				case 'W':  // Changes width factor to produce wide text
				case 'A':  // Sets the alignment value; valid values: 0, 1, 2 (bottom, center, top)
					isCodeParameter = true;
					break;
				case 'p':
					if ((*(i + 1u) == 'x') && (*(i + 2u) == 'i'))
					{
						// \pxi format code
						isCodeParameter = true;
					}
					else
					{
						unformatted.push_back('\\');
						unformatted.push_back(*i);
					}
					break;
				case 'U':
					if (*(i + 1u) == '+')
					{
						++i;

						unsigned int wideCharacter = 0u;
						for (size_t byteCounter = 0u; byteCounter < 4u; ++byteCounter)
						{
							++i;
							if (i != Caption.end())
							{
								wideCharacter <<= 4u;

								if ((*i >= '0') && (*i <= '9'))
								{
									wideCharacter += (*i - '0');
								}
								else if ((*i >= 'a') && (*i <= 'f'))
								{
									wideCharacter += (*i - 'a') + 10u;
								}
								else if ((*i >= 'A') && (*i <= 'F'))
								{
									wideCharacter += (*i - 'A') + 10u;
								}
							}
							else
							{
								break;
							}
						}

						if (wideCharacter < 0x7fu)
						{
							// The first 128 characters are the same for ANSI and UNICODE
							unformatted.push_back(static_cast<TCHAR>(wideCharacter));
						}
						else
						{
							WString wideString;
							wideString.push_back(static_cast<wchar_t>(wideCharacter));
							// It converts character from UNICODE to ANSI of current codepage
							const std::string & utf8String(UnicodeToText(wideString));
							unformatted.append(utf8String);
						}
						break;
					}
					// Fall through
				default:  // Unrecognized code
					unformatted.push_back('\\');
					unformatted.push_back(*i);
					break;
			}
		}

		Caption.assign(unformatted);
	}
	*/





	/** Processes the unicode inserts (\U+xxxx) in the input string into UTF-8 representation. */
	std::string convertDxfText(const std::string & aInput)
	{
		std::string res;
		auto len = aInput.size();
		res.reserve(len);
		for (size_t i = 0; i < len; ++i)
		{
			switch (aInput[i])
			{
				case '\\':
				{
				}
				default:
				{
					res.push_back(aInput[i]);
				}
			}
		}
		return res;
	}




public:

	Parser(DataSource && aDataSource):
		mLineExtractor(std::move(aDataSource)),
		mDrawing(new Drawing)
	{
	}





	/** Parses the data from mLineExtractor into mDrawing. */
	void parse(bool aShouldContinueAfterLayerList)
	{
		for (;;)
		{
			auto [groupCode, value] = readNext();
			switch (groupCode)
			{
				case 0:
				{
					if (isSameStringIgnoreCase(value, "eof"))
					{
						// All done.
						return;
					}
					break;
				}  // case 0

				case 2:
				{
					if (isSameStringIgnoreCase(value, "header"))
					{
						parseHeaderSection();
					}
					else if (isSameStringIgnoreCase(value, "classes"))
					{
						parseClassesSection();
					}
					else if (isSameStringIgnoreCase(value, "tables"))
					{
						parseTablesSection();
						if (!aShouldContinueAfterLayerList)
						{
							return;
						}
					}
					else if (isSameStringIgnoreCase(value, "blocks"))
					{
						parseBlocksSection();
					}
					else if (isSameStringIgnoreCase(value, "entities"))
					{
						parseEntitiesSection(nullptr);
					}
					else if (isSameStringIgnoreCase(value, "objects"))
					{
						parseObjectsSection();
					}
					break;
				}  // case 2
				default:
				{
					break;
				}
			}  // switch groupCode
		}  // forever
	}





	/** Returns the contained drawing. */
	const std::shared_ptr<Drawing> & drawing() const
	{
		return mDrawing;
	}
};





std::shared_ptr<Drawing> parse(DataSource && aDataSource)
{
	Parser parser(std::move(aDataSource));
	parser.parse(true);
	return parser.drawing();
}





std::vector<std::string> parseLayerList(DataSource && aDataSource)
{
	Parser parser(std::move(aDataSource));
	parser.parse(false);
	std::vector<std::string> res;
	for (const auto & lay: parser.drawing()->layers())
	{
		res.push_back(lay->name());
	}
	return res;
}





}  // namespace Dxf::Parser
