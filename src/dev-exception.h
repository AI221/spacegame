/*!
 * Defines an exception which isn't supposed to be caught; it is a mistake that should generally be fixed 
 * during development (i.e. not due to filesystem/environment factors)
 */

#pragma once

#include <exception>

struct GE_DevException : std::exception
{
	GE_DevException(const char* str)
	{
		this->str = str;
	}
	const char* what() const noexcept { return str; }
	private:
		const char* str;
};

