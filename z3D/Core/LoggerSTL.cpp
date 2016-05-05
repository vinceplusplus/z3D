
#include "LoggerSTL.h"
#include "Critical/Logger.h"

namespace z3D
{
	namespace Core
	{
		LoggerSTL::LoggerSTL(Logger* logger)
		{
			_logger = logger;
		}
		LoggerSTL&				LoggerSTL::operator<<(const wcs& line)
		{
			(*_logger)<<line.c_str();
			return *this;
		}
		LoggerSTL&				LoggerSTL::operator<<(const mbs& line)
		{
			(*_logger)<<line.c_str();
			return *this;
		}
		void					LoggerSTL::flush()
		{
			_logger->flush();
		}
	};
};

