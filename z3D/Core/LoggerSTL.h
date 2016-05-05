
#pragma once

#include "predecl.h"
#include "fast_string.h"

namespace z3D
{
	namespace Core
	{
		class LoggerSTL
		{
		private:
			Logger*						_logger;
		public:
			LoggerSTL(Logger* logger);
		public:
			LoggerSTL&				operator<<(const wcs& line);
			LoggerSTL&				operator<<(const mbs& line);
		public:
			void					flush();
		};

		extern Z3D_API LoggerSTL	syslog;
	};
};

