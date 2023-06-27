#pragma once
#include "Core/Logger.h"

#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <exception>

// just simple key-value pair for now...
class ConfigReader
{
private:
	class Property
	{
	public:
		Property(const std::string& name, const std::string& value,
			const std::unordered_map<std::string, Property>& children = {})
		: m_name(name), m_value(value), m_childProperties(children)
		{}
		bool HasValue() const { return !m_value.empty(); }
		bool HasChildren() const { return !m_childProperties.empty(); }

		const std::string& GetStringValue() const { return m_value; }
		int GetIntValue() const { return atoi(m_value.c_str()); }
		float GetFloatValue() const { return atof(m_value.c_str()); }

		const Property& operator[](const std::string& key) const
		{ 
			try 
			{
				return m_childProperties.at(key);
			}
			catch (...)
			{
				throw std::runtime_error("unable to locate child property <" + key + "> of " + m_name);
			}
		}

		std::string Dump() const
		{
			std::string out = m_name + (m_childProperties.empty()? "" : ": " + m_value);
			for (const auto& [key, child] : m_childProperties)
			{
				out += child.Dump();
				out += "\n";
			}
			return out;
		}
	private:
		std::string m_name;
		std::string m_value;
		std::unordered_map<std::string, Property> m_childProperties;
	};

	class File
	{
	public:
		File(const std::string name, const std::unordered_map<std::string, Property>& props)
		: m_name(name), m_properties(props)
		{
			LOG_DEBUG("File constructor");
			LOG_DEBUG("props.size()        == " + std::to_string(props.size()));
			LOG_DEBUG("m_properties.size() == " + std::to_string(m_properties.size()));
		}
		File(const File& other)
		: m_name(other.m_name), m_properties(other.m_properties)
		{
			LOG_DEBUG("File copy contructor");
			LOG_DEBUG("other.m_properties.size() == " + std::to_string(other.m_properties.size()));
			LOG_DEBUG("m_properties.size()       == " + std::to_string(m_properties.size()));
		}

		const std::string& GetName() const { return m_name; }
		std::string DumpProperties() const
		{
			std::string out;
			for (const auto& [key, prop] : m_properties)
			{
				out += prop.Dump();
				out += "\n";
			}
			return out;
		}

		const Property& operator[](const std::string& key) const 
		{
			try 
			{
				return m_properties.at(key);
			}
			catch (...)
			{
				throw std::runtime_error("unable to locate property <" + key + "> of file " + m_name);
			}
		}
	private:
		std::string m_name;
		std::unordered_map<std::string, Property> m_properties;
	};

public:

	static ConfigReader GetInstance()
	{
		static ConfigReader instance;
		return instance;
	}

	static const File& GetFile(const std::string& fileName) 
	{ 
		auto ite = s_files.find(fileName);
		if (ite == s_files.end())
		{
			// load the file
			File file = ParseFile(fileName);
			auto [existing_ite, insert_successful] = s_files.emplace(fileName, file);
			if (!insert_successful)
			{
				LOG_ERROR("unable to cache the file: " + fileName);
			}
			ite = existing_ite;
		}
		return ite->second;
	}

	static void DumpConfig(const std::string& fileName)
	{
		File file = GetFile(fileName);
		std::string msg = fileName + '\n' + file.DumpProperties();
		LOG_DEBUG(msg);
	}


private:

	ConfigReader() {}

	static File ParseFile(const std::string& fileName)
	{
		std::ifstream f(fileName);
		if (!f.is_open())
		{
			throw std::runtime_error("cannot open file: " + fileName);
		}

		std::unordered_map<std::string, Property> fileData;
		std::unordered_map<std::string, Property> propertiesData;

		int currentIndent = 0;
		std::string line;
		std::vector<std::string> nameHierarchy;
		while (getline(f, line))
		{
			std::string key, value;
			std::stringstream ss;
			ss << line;

			int indent = CountIndent(line);
			while(indent < currentIndent)
			{
				nameHierarchy.pop_back();
				--currentIndent;
			}

			ss >> key >> value;
			key = key.substr(0, key.length() - 1);
			LOG_DEBUG("key: " + key + " - value: -" + value + "-" );
			if (!value.empty())
			{
				Property property(GetName(nameHierarchy) + key, value);
				fileData.emplace(key, property);
				continue;
			}
			// TODO: need a stack
			nameHierarchy.push_back(key);
			// this is the only time when we would have a new indentation
			propertiesData = {};
		}

		return File(fileName, fileData);
	}

	static std::string GetName(const std::vector<std::string>& tree)
	{
		std::string out;
		for (const auto& name : tree)
		{
			out += name + ".";
		}
		return out;
	}

	static int CountIndent(const std::string& line)
	{
		return line.find_first_not_of("\t");
	}

	static std::unordered_map<std::string, File> s_files;
};

#ifdef CONFIG_READER_IMPL
#undef CONFIG_READER_IMPL
std::unordered_map<std::string, ConfigReader::File> ConfigReader::s_files;
#endif