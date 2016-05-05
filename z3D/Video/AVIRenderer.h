
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Video
	{
		class AVIRenderer
		{
		protected:
			SPtr<AVIReader>				_reader;
		protected:
			void*						_internal;
			unsigned long				_last_frame;
		protected:
			static bool					_inited;
			static map<long,long>		_map_fcc_codecid;
		public:
			static void					__init();
			static void					__shutdown();
		protected:
			AVIRenderer();
		public:
			virtual ~AVIRenderer();
		public:
			static AVIRenderer*			load(const wcs& filepath);
			static AVIRenderer*			load(const SPtr<AVIReader>& reader);
		public:
			static AVIRenderer*			loadNoCache(const wcs& filepath);
		public:							// zero-based frame index, return value indicates whether the buffer is updated
			bool						render(unsigned long frame);
			bool						renderAt(REAL time);
		public:
			const void*					get_fipImage() const;
		public:
			const AVIReader&			reader() const;
		public:
			REAL						length() const;
			unsigned long				dx() const;
			unsigned long				dy() const;
		public:
			unsigned long				last_frame() const;
		private:
			static void					rescale_32bit(void* src, unsigned long src_dx, unsigned long src_dy, void* dst, unsigned long dst_dx, unsigned long dst_dy);
			static void					rescale_24bit(void* src, unsigned long src_dx, unsigned long src_dy, void* dst, unsigned long dst_dx, unsigned long dst_dy);
		};
	};
};


