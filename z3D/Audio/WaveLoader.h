
#include "predecl.h"

namespace z3D
{
	namespace Audio
	{
		class WaveLoader: protected RiffReader
		{
		public:
			static bool					load(const void* ptr,unsigned long length,unsigned short& channels,unsigned short& sample_bits,unsigned long& samples_per_sec,void*& data,unsigned long& data_size);
			static bool					load(const void* ptr,unsigned long length,unsigned short& channels,unsigned short& sample_bits,unsigned long& samples_per_sec,vector<char>& data);
		};
	};
};

