
#include "Constants.h"

#include "SMem.h"

#include "Config.h"

#include "LexScan.h"

#include "Misc.h"

#include "../IO/FileSystem.h"
#include "../IO/MemReader.h"
#include "../IO/MemWriter.h"

using namespace z3D::IO;

namespace z3D
{
	namespace Core
	{
		Config::Config(const wcs& filepath, bool lower_id)
		{
			_modified = false;

			_filepath = filepath;

			if(!filepath.length())
				return;

			mbs config_utf8 = FileSystem::loadUTF8FromAll(filepath);

			read(config_utf8, lower_id);
			_modified = false;
		}
		Config::~Config()
		{
			flush();
		}
		Config&			Config::operator=(const Config& other)
		{
			_filepath = L"";
			_key_value_pair = other._key_value_pair;
			_modified = true;
			return *this;
		}
		Config*			Config::fromWCS(const wcs& source, bool lower_id)
		{
			Config* cfg = new Config(L"");
			cfg->read(WCSTOUTF8(source), lower_id);
			return cfg;
		}
		Config*			Config::fromUTF8(const mbs& source, bool lower_id)
		{
			Config* cfg = new Config(L"");
			cfg->read(source, lower_id);
			return cfg;
		}
		void				Config::flush()
		{
			if(!_modified || !_filepath.length())
				return;
			wcs config;

			config += UNICODE_BYTE_ORDER_MARK;
			config += toWCS();

			MemWriter writer;
			writer.write(config);
			SMem sm = writer.retrieveAll();
			FileSystem::getFileSystem(L"WIN32")->saveFile(_filepath, sm.get(), sm.size());

			_modified = false;
		}
		void				Config::read(const mbs& config_utf8, bool lower_id)
		{
			LexScan cfg_scan(config_utf8);

			int state = 0;
			int ret;

			while((ret = cfg_scan.scan())!=0)
			{
				if(ret == SCN_TOKEN_IDENTIFIER)
					state = 1;
				else if(state == 1 && ret == '=')
					state = 2;
				else if(state == 2 && (ret == SCN_TOKEN_STRING || ret == SCN_TOKEN_INTEGER || ret == SCN_TOKEN_REAL))
				{
					wcs id;
					if(lower_id)
					{
						wchar_t id_buffer[2048];
						wcscpy_s(id_buffer, cfg_scan.getIdentifier().c_str());
						_wcslwr_s(id_buffer);
						id_buffer[_countof(id_buffer) - 1] = 0;
						id = id_buffer;
					}
					switch(ret)
					{
					case SCN_TOKEN_STRING:
						setWString(lower_id ? id : cfg_scan.getIdentifier(), cfg_scan.getValueWString());
						break;
					case SCN_TOKEN_INTEGER:
						setInt64(lower_id ? id : cfg_scan.getIdentifier(), cfg_scan.getValueInt64());
						break;
					case SCN_TOKEN_REAL:
						setDouble(lower_id ? id : cfg_scan.getIdentifier(), cfg_scan.getValueDouble());
						break;
					}
					state = 0;
				}
				else
					state = 0;
			}
		}
		const wcs			Config::getWString(const wcs& key) const
		{
			map<wcs,AnyValue>::const_iterator iter = _key_value_pair.find(key);
			if(iter != _key_value_pair.end())
				return iter->second.getWString();
			return L"";
		}
		int32_t				Config::getInt32(const wcs& key) const
		{
			map<wcs,AnyValue>::const_iterator iter = _key_value_pair.find(key);
			if(iter != _key_value_pair.end())
				return iter->second.getInt32();
			return 0;
		}
		uint32_t			Config::getUInt32(const wcs& key) const
		{
			map<wcs,AnyValue>::const_iterator iter = _key_value_pair.find(key);
			if(iter != _key_value_pair.end())
				return iter->second.getUInt32();
			return 0;
		}
		int64_t				Config::getInt64(const wcs& key) const
		{
			map<wcs,AnyValue>::const_iterator iter = _key_value_pair.find(key);
			if(iter != _key_value_pair.end())
				return iter->second.getInt64();
			return 0;
		}
		uint64_t			Config::getUInt64(const wcs& key) const
		{
			map<wcs,AnyValue>::const_iterator iter = _key_value_pair.find(key);
			if(iter != _key_value_pair.end())
				return iter->second.getUInt64();
			return 0;
		}
		float				Config::getFloat(const wcs& key) const
		{
			map<wcs,AnyValue>::const_iterator iter = _key_value_pair.find(key);
			if(iter != _key_value_pair.end())
				return iter->second.getFloat();
			return 0.0f;
		}
		double				Config::getDouble(const wcs& key) const
		{
			map<wcs,AnyValue>::const_iterator iter = _key_value_pair.find(key);
			if(iter != _key_value_pair.end())
				return iter->second.getDouble();
			return 0.0;
		}
		void					Config::setWString(const wcs& key, const wcs& value)
		{
			_key_value_pair[key].setWString(value);
			_modified = true;
		}
		void					Config::setInt32(const wcs& key, int32_t value)
		{
			_key_value_pair[key].setInt32(value);
			_modified = true;
		}
		void					Config::setUInt32(const wcs& key, uint32_t value)
		{
			_key_value_pair[key].setUInt32(value);
			_modified = true;
		}
		void					Config::setInt64(const wcs& key, int64_t value)
		{
			_key_value_pair[key].setInt64(value);
			_modified = true;
		}
		void					Config::setUInt64(const wcs& key, uint64_t value)
		{
			_key_value_pair[key].setUInt64(value);
			_modified = true;
		}
		void					Config::setFloat(const wcs& key, float value)
		{
			_key_value_pair[key].setFloat(value);
			_modified = true;
		}
		void					Config::setDouble(const wcs& key, double value)
		{
			_key_value_pair[key].setDouble(value);
			_modified = true;
		}
		void					Config::getWString(const wcs& key, wcs& value) const
		{
			map<wcs,AnyValue>::const_iterator iter = _key_value_pair.find(key);
			if(iter != _key_value_pair.end())
				value = iter->second.getWString();
		}
		void					Config::getInt32(const wcs& key, int32_t& value) const
		{
			map<wcs,AnyValue>::const_iterator iter = _key_value_pair.find(key);
			if(iter != _key_value_pair.end())
				value = iter->second.getInt32();
		}
		void					Config::getUInt32(const wcs& key, uint32_t& value) const
		{
			map<wcs,AnyValue>::const_iterator iter = _key_value_pair.find(key);
			if(iter != _key_value_pair.end())
				value = iter->second.getUInt32();
		}
		void					Config::getInt64(const wcs& key, int64_t& value) const
		{
			map<wcs,AnyValue>::const_iterator iter = _key_value_pair.find(key);
			if(iter != _key_value_pair.end())
				value = iter->second.getInt64();
		}
		void					Config::getUInt64(const wcs& key, uint64_t& value) const
		{
			map<wcs,AnyValue>::const_iterator iter = _key_value_pair.find(key);
			if(iter != _key_value_pair.end())
				value = iter->second.getUInt64();
		}
		void					Config::getFloat(const wcs& key, float& value) const
		{
			map<wcs,AnyValue>::const_iterator iter = _key_value_pair.find(key);
			if(iter != _key_value_pair.end())
				value = iter->second.getFloat();
		}
		void					Config::getDouble(const wcs& key, double& value) const
		{
			map<wcs,AnyValue>::const_iterator iter = _key_value_pair.find(key);
			if(iter != _key_value_pair.end())
				value = iter->second.getDouble();
		}
		void					Config::remove(const wcs& key)
		{
			map<wcs,AnyValue>::iterator iter = _key_value_pair.find(key);
			if(iter!=_key_value_pair.end())
			{
				_key_value_pair.erase(iter);
				_modified = true;
			}
		}
		void					Config::clear()
		{
			_key_value_pair.clear();
			_modified = true;
		}
		void					Config::readFromWCS(const wcs& source, bool lower_id)
		{
			read(WCSTOUTF8(source), lower_id);
		}
		void					Config::readFromUTF8(const mbs& source, bool lower_id)
		{
			read(source, lower_id);
		}
		bool					Config::exists(const wcs& key) const
		{
			return _key_value_pair.count(key) ? true : false;
		}
		map<wcs,AnyValue>		Config::toMap() const
		{
			return _key_value_pair;
		}
		wcs						Config::toWCS() const
		{
			wcs config;
			map<wcs, AnyValue>::const_iterator iter;
			map<wcs, AnyValue>::const_iterator end = _key_value_pair.end();
			for(iter = _key_value_pair.begin(); iter != end; ++iter)
			{
				config += iter->first + L" = ";
				switch(iter->second.getType())
				{
				case AnyValue::TYPE_WSTRING:
					config += wcs(L"\"") + ESCSTR(iter->second.getWString()) + L"\"";
					break;
				case AnyValue::TYPE_INT64:
					config += FORMATW(L"%I64d", iter->second.getInt64());
					break;
				case AnyValue::TYPE_DOUBLE:
					config += FORMATW(L"%f", iter->second.getDouble());
					break;
				}
				config += L"\r\n";
			}
			return config;
		}
		const wcs&				Config::filepath() const
		{
			return _filepath;
		}
		void					Config::setFilepath(const wcs& filepath)
		{
			_filepath = filepath;
		}
	};
};

