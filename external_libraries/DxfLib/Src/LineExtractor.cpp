#include "LineExtractor.hpp"

#include <cstring>





namespace Dxf::Parser
{





namespace
{
	/** The maximum size mBuffer is allowed to grow to.
	If a line is not found within this space, the file is considered invalid. */
	static const size_t MAX_BUFFER_SIZE = 4 * 1024 * 1024;
}





LineExtractor::LineExtractor(DataSource && aDataSource):
	mDataSource(std::move(aDataSource)),
	mCurPos(0),
	mDataEnd(0),
	mCurrentLineNum(1),
	mIsEof(false)
{
	mBuffer.resize(1000);
	readMoreData();
}





std::string LineExtractor::getNextLine()
{
	// Search for the newline in the buffered data:
	for (size_t i = mCurPos; i < mDataEnd; ++i)
	{
		if (mBuffer[i] != '\n')
		{
			continue;
		}
		// Found a newline, return the string:
		auto oldPos = mCurPos;
		mCurPos = i + 1;
		mCurrentLineNum += 1;
		size_t skipCr = 0;
		if ((i > oldPos) && (mBuffer[i - 1] == '\r'))  // If the last char is a CR, remove it
		{
			skipCr = 1;
		}
		return std::string(&mBuffer.front() + oldPos, i - oldPos - skipCr);
	}

	// There is no newline in the buffer, read more data and re-try:
	readMoreData();
	return getNextLine();
}





void LineExtractor::readMoreData()
{
	// If we're reading past an EOF, throw an exception:
	if (mIsEof)
	{
		throw Dxf::Parser::Error(mCurrentLineNum, "End of file reached.");
	}

	// If there's too much wasted space, compact the buffer:
	if (mCurPos > mBuffer.size() / 2)
	{
		std::memmove(&mBuffer.front(), &mBuffer.front() + mCurPos, mDataEnd - mCurPos);
		mDataEnd -= mCurPos;
		mCurPos = 0;
	}

	// If there's not much space left in the buffer, enlarge the buffer:
	if (mDataEnd > 2 * mBuffer.size() / 3)
	{
		if (mBuffer.size() * 2 > MAX_BUFFER_SIZE)
		{
			throw Dxf::Parser::Error(mCurrentLineNum, "Line too long, doesn't fit the buffer");
		}
		mBuffer.resize(mBuffer.size() * 2);
	}

	// Read the bytes from the datasource:
	auto numBytesRead = mDataSource(&mBuffer.front() + mDataEnd, mBuffer.size() - mDataEnd);
	if (numBytesRead == 0)
	{
		mIsEof = true;
		return;
	}
	mDataEnd += numBytesRead;
}





}  // namespace Dxf::Parser
