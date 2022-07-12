#pragma once

#include <stdexcept>

#include "DataSource.hpp"





namespace Dxf::Parser
{





/** Exception that is thrown when an error is encountered while parsing lines from a datasource.
Contains details about the error encountered. */
class Error:
	public std::runtime_error
{
	using Super = std::runtime_error;

	/** The line number where the error was detected. */
	unsigned mLineNumber;

	/** The detailed error message. */
	std::string mMessage;


public:

	Error(unsigned aLineNumber, std::string && aMessage):
		Super("Dxf::Parser::Error"),
		mLineNumber(aLineNumber),
		mMessage(std::move(aMessage))
	{
	}

	/** Returns the line number where the error was detected. */
	unsigned lineNumber() const { return mLineNumber; }

	/** Returns the message stored in the exception. */
	const std::string & message() const { return mMessage; }
};





/** Extracts individual lines from input data source.
The input is expected to be either LF- or CRLF-separated; CR-only is NOT supported.
Switching from CRLF to LF and back in the middle IS supported and auto-detected.
Buffers some of the data so that the data source isn't called too often. */
class LineExtractor
{
public:
	/** Creates a new instance tied to the specified datasource. */
	LineExtractor(DataSource && aDataSource);

	// Disable copy- and move-constructors:
	LineExtractor(const LineExtractor & aOther) = delete;
	LineExtractor(LineExtractor && aOther) = delete;


	/** Returns the next line of input data from the data source.
	Throws an exception on error, either from the DataSource itself or a Dxf::Util::LineError. */
	std::string getNextLine();

	/** Returns the current line number.
	Useful when reporting errors. */
	unsigned currentLineNum() const { return mCurrentLineNum; }


protected:

	/** The data source that provides the data when there's not enough in the buffer. */
	DataSource mDataSource;

	/** The buffer for data read from the data source. */
	std::vector<char> mBuffer;

	/** The current position in the mBuffer where the next line will start. */
	size_t mCurPos;

	/** The position in mBuffer one after the last valid data byte. */
	size_t mDataEnd;

	/** The line-counter of the input data. Used mainly for error reporting. */
	unsigned mCurrentLineNum;

	/** Set to true if the last data read operation signalled an EOF
	Further attempts at reads throw an exception. */
	bool mIsEof;


	/** Attempts to read in more data from the data source.
	Updates mIsEof and throws appropriate exceptions on read-errors.
	If the buffer has way too much unprocessed data in it, throws an error (invalid data format). */
	void readMoreData();
};





}  // namespace Dxf::Parser
