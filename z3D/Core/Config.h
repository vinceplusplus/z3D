
#pragma once

#include "predecl.h"
#include "AnyValue.h"

namespace z3D
{
	namespace Core
	{
		class Config
		{
		private:
			wcs								_filepath;
			map<wcs, AnyValue>				_key_value_pair;
			size_t							_modified;
		public:
			Config(const wcs& filepath, bool lower_id = false);
			virtual ~Config();
		public:
			Config&				operator=(const Config& other);
		public:
			static Config*		fromWCS(const wcs& source, bool lower_id = false);
			static Config*		fromUTF8(const mbs& source, bool lower_id = false);
		public:
			void				flush();
		public:
			const wcs			getWString(const wcs& key) const;
			int32_t				getInt32(const wcs& key) const;
			uint32_t			getUInt32(const wcs& key) const;
			int64_t				getInt64(const wcs& key) const;
			uint64_t			getUInt64(const wcs& key) const;
			float				getFloat(const wcs& key) const;
			double				getDouble(const wcs& key) const;
		public:
			void				setWString(const wcs& key, const wcs& value);
			void				setInt32(const wcs& key, int32_t value);
			void				setUInt32(const wcs& key, uint32_t value);
			void				setInt64(const wcs& key, int64_t value);
			void				setUInt64(const wcs& key, uint64_t value);
			void				setFloat(const wcs& key, float value);
			void				setDouble(const wcs& key, double value);
		public:
			void				getWString(const wcs& key, wcs& value) const;
			void				getInt32(const wcs& key, int32_t& value) const;
			void				getUInt32(const wcs& key, uint32_t& value) const;
			void				getInt64(const wcs& key, int64_t& value) const;
			void				getUInt64(const wcs& key, uint64_t& value) const;
			void				getFloat(const wcs& key, float& value) const;
			void				getDouble(const wcs& key, double& value) const;
		public:
			void				remove(const wcs& key);
			void				clear();
		private:
			void				read(const mbs& config_utf8, bool lower_id);
		public:
			void				readFromWCS(const wcs& source, bool lower_id);
			void				readFromUTF8(const mbs& source, bool lower_id);
		public:
			bool				exists(const wcs& key) const;
			map<wcs,AnyValue>	toMap() const;
			wcs					toWCS() const;
		public:
			const wcs&			filepath() const;
		public:
			void				setFilepath(const wcs& filepath);
		};
	};
};

