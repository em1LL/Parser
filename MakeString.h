// MakeString.h

#ifndef _MAKESTRING_H_
#define _MAKESTRING_H_

// Standard includes

// Standard Library

#include <sstream>
#include <string>

class MakeString
{
	public:
		template <typename T>
		MakeString& operator<<(const T& arg)
		{
			m_stream << arg;
			return *this;
		}
		operator std::string() const
		{
			return m_stream.str();
		}

	protected:
		std::ostringstream m_stream;
};

#endif // _MAKESTRING_H_