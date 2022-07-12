#include "DataSource.hpp"

#include <istream>
#include <cstring>





namespace Dxf::Parser
{





DataSource dataSourceFromString(std::string && aInput)
{
	class DS
	{
		/** The complete input data. */
		std::string mInput;

		/** The position within mInput where the next read should start. */
		size_t mCurPos;


	public:

		DS(std::string && aInput):
			mInput(aInput),
			mCurPos(0)
		{
		}

		size_t operator ()(char * aDestBuffer, size_t aSize)
		{
			if (mCurPos >= mInput.size())
			{
				return 0;
			}
			auto numLeft = mInput.size() - mCurPos;
			auto numToCopy = std::min(aSize, numLeft);
			std::memcpy(aDestBuffer, mInput.data() + mCurPos, numToCopy);
			mCurPos += numToCopy;
			return numToCopy;
		}
	};

	return DS(std::move(aInput));
}





DataSource dataSourceFromStdStream(std::istream & aStream)
{
	aStream.exceptions(std::istream::failbit | std::istream::badbit);
	return [&aStream](char * aDestBuffer, size_t aSize)
	{
		return aStream.readsome(aDestBuffer, aSize);
	};
}





}  // namespace Dxf::Parser
