#pragma once
#include "Core/Logger.h"

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <stack>
#include <vector>
#include <exception>
#include <utility>

// just simple key-value pair for now...
class ConfigReader
{
public:
	class Property
	{
	public:
		Property() = default;
		Property(const std::string& canonicalName, const std::string& name, const std::string& value,
                 const std::unordered_map<std::string, Property>& children = {})
		: m_canonicalName(canonicalName), m_name(name), m_value(value), m_childProperties(children)
		{}
		bool HasValue() const { return !m_value.empty(); }
		bool HasChildren() const { return !m_childProperties.empty(); }

		const std::string& GetStringValue() const { return m_value; }
		int GetIntValue() const { return atoi(m_value.c_str()); }
		float GetFloatValue() const { return atof(m_value.c_str()); }

		const std::string& GetName() const { return m_name; }

		const Property& operator[](const std::string& key) const
		{ 
			try 
			{
				return m_childProperties.at(key);
			}
			catch (...)
			{
				LOG_FATAL("unable to locate child property <", key, "> of ", m_name);
				throw std::runtime_error("unable to locate child property <" + key + "> of " + m_name);
			}
		}

		std::string Dump() const
		{
			std::string out = m_name + (m_childProperties.empty()? ": " + m_value : "\n");
			for (const auto& [key, child] : m_childProperties)
			{
				out += child.Dump();
				out += "\n";
			}
			return out;
		}

	private:
		std::string m_canonicalName;
		std::string m_name;
		std::string m_value;
		std::unordered_map<std::string, Property> m_childProperties;

		friend class ConfigReader;
	};
	
public:

	static const Property& ReadYaml(const std::string& fileName) 
	{
		auto ite = s_files.find(fileName);
		if (ite == s_files.end())
		{
			// load the file
			Property file = ParseYamlFile(fileName);
			auto [existing_ite, insert_successful] = s_files.emplace(fileName, file);
			if (!insert_successful)
			{
				LOG_ERROR("unable to cache the file: ", fileName);
			}
			ite = existing_ite;
		}
		return ite->second;
	}

	static void DumpConfig(const std::string& fileName)
	{
		Property file = ReadYaml(fileName);
		std::string msg = fileName + '\n' + file.Dump();
		LOG_DEBUG(msg);
	}


private:
	using PropertyMap = std::unordered_map<std::string, Property>;

	ConfigReader() {}

	static Property ParseYamlFile(const std::string& fileName)
	{
		std::fstream f(fileName);
		if (!f.is_open())
		{
			throw std::runtime_error("cannot open file: " + fileName);
		}

		// read the entire file
		std::vector<std::string> lines;
		{
			std::string l;
			while (getline(f, l))
			{
				lines.push_back(l);
			}
		}

		// add an empty line if the file doesn't contain one
		if (!lines.back().empty())
		{
			lines.emplace_back("");
		}

		PropertyMap properties;

		for (int i = 0; i < lines.size() - 1; ++i)
		{
			Property p = ParseYamlProperty("", lines, i);
			properties.emplace(p.GetName(), p);
		}

		return Property(fileName, "", "" , properties);
	}

	// need to include the '.' in the parent name
	static Property ParseYamlProperty(const std::string& parentName, const std::vector<std::string>& lines, int& startIndex)
	{
		const std::string& line = lines[startIndex];
		auto [key, value] = ExtractPropertyInfo(line);

		if (!value.empty())
		{
			return Property(parentName + key, key, value);
		}

		int indent = CountIndent(line);

		if (IsPropertyGroup(value, indent, lines[startIndex + 1]))
		{
			int currentIndent = CountIndent(lines[startIndex + 1]);
			PropertyMap children;
			while (currentIndent - indent == 1 && startIndex < lines.size() - 1)
			{
				Property childProperty = ParseYamlProperty(parentName + key + '.', lines, ++startIndex);
				children.emplace(childProperty.GetName(), childProperty);
				currentIndent = CountIndent(lines[startIndex + 1]);
			}
			return Property(parentName + key, key, "", children);
		}
		throw std::runtime_error("bad yml file!");
	}

	static std::string GetName(const std::vector<std::string>& tree)
	{
		if (tree.size() == 0) 
			return "";
		
		std::string out;
		for (const auto& name : tree)
		{
			out += ("." + name);
		}
		return out.substr(1);
	}

	static bool IsPropertyGroup(const std::string& value, int indent, const std::string& nextLine)
	{
		return value.empty() && CountIndent(nextLine) == indent + 1;
	}

	static std::pair<std::string, std::string> ExtractPropertyInfo(const std::string& line)
	{
		std::string key, value;
		std::stringstream ss;
		ss << line;

		ss >> key >> value;
		key = key.substr(0, key.length() - 1);
		return std::make_pair(key, value);
	}

	static int CountIndent(const std::string& line)
	{
		std::string sep = "  ";
		return line.find_first_not_of(sep) / sep.length();
	}

	static std::unordered_map<std::string, Property> s_files;
};

#ifdef CONFIG_READER_IMPL
#undef CONFIG_READER_IMPL
std::unordered_map<std::string, ConfigReader::Property> ConfigReader::s_files;
#endif