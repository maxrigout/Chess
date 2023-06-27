#pragma once
#include "Core/Logger.h"

#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <stack>
#include <vector>
#include <exception>
#include <utility>

// just simple key-value pair for now...
class ConfigReader
{
private:
	class Property
	{
	public:
		Property() {}
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
		{}

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
	using PropertyMap = std::unordered_map<std::string, Property>;

	ConfigReader() {}

	static File ParseFile(const std::string& fileName)
	{
		std::fstream f(fileName);
		if (!f.is_open())
		{
			throw std::runtime_error("cannot open file: " + fileName);
		}

		// read the entire file
		std::vector<std::string> lines;
		std::string l;
		while (getline(f, l))
		{
			lines.push_back(l);
		}

		// add an empty line if the file doesn't contain one
		if (!lines.back().empty())
		{
			lines.emplace_back("");
		}

		PropertyMap fileData;
		std::stack<std::pair<std::string, PropertyMap>> propertiesStack;

		int currentIndent = 0;
		for (int i = 0; i < lines.size() - 1; i++)
		{
			const std::string& line = lines[i];
			int indent = CountIndent(line);
			auto [key, value] = ExtractPropertyInfo(line);

			// if we find a parent... (ie value.empty() and next line is indented by 1 more)
			if (IsPropertyGroup(value, indent, lines[i + 1]))
			{
				std::string parentName;
				if (!propertiesStack.empty())
					parentName = propertiesStack.top().first + ".";
				propertiesStack.emplace();
				propertiesStack.top().first = parentName + key;
				// prematurely set the new indent...
				currentIndent = indent;
				continue;
			}

			// if the indent is 0, we can add the property
			if (indent == 0)
			{
				fileData.emplace(key, Property(key, value));
			}
			// if the indent is the same as currentIndent, we can add the property to the child properties
			else if (indent == currentIndent)
			{
				std::string name = propertiesStack.top().first + "." + key;
				propertiesStack.top().second.emplace(key, Property(name, value));
				continue;
			}
			else if (indent < currentIndent)
			{
				// if the indent decreases, we create the parent
				fileData.emplace(propertiesStack.top().first, Property(propertiesStack.top().first, "", propertiesStack.top().second));
				propertiesStack.pop();
			}
		}

		return File(fileName, fileData);
	}
/*
	static File ParseFile2(const std::string& fileName)
	{
		std::ifstream f(fileName);
		if (!f.is_open())
		{
			throw std::runtime_error("cannot open file: " + fileName);
		}

		PropertyMap fileData;

		int currentIndent = 0;
		std::vector<std::string> nameHierarchy;

		// read the entire file
		std::vector<std::string> lines;
		std::string l;
		while (getline(f, l))
		{
			lines.push_back(l);
		}


		for (const auto& line : lines)
		{
			std::string key, value;
			std::stringstream ss;
			ss << line;

			ss >> key >> value;
			key = key.substr(0, key.length() - 1);
			LOG_DEBUG("key: " + key + " - value: -" + value + "-" );
			
			Property property(GetName(nameHierarchy) + key, value);
			fileData.emplace(key, property);
		}

		return File(fileName, fileData);
	}

	static Property ParseLine2(std::vector<std::string>::iterator& linesIte, std::vector<std::string>::iterator& end, int indent, const std::string& parentName, PropertyMap& children)
	{
		if (linesIte == end)
			return;
		std::string line = *linesIte, key, value;
		int currentIndent = CountIndent(line);
		
		// extract the key and the value from the line
		std::stringstream ss;
		ss << line;

		ss >> key >> value;
		key = key.substr(0, key.length() - 1);
		// if there's no value, then we have a new "group"
		//    create a new parent
		//    call ParseLine with that parent
		if (currentIndent > indent)
		{
			// we found a new parent
			++linesIte;
			ParseLine2(linesIte, end, currentIndent, parentName + "." + key, children);
			return Property(key, "", children);
		}
		if (currentIndent < indent)
		{
			// 
		}

		// if there is a value
		//    add the value to the parent
		// if the indent has changed
		//    create a new parent (indent greater)
		//    return (indent less)
		return Property();
	}
*/
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
		return line.find_first_not_of("\t");
	}

	static std::unordered_map<std::string, File> s_files;
};

#ifdef CONFIG_READER_IMPL
#undef CONFIG_READER_IMPL
std::unordered_map<std::string, ConfigReader::File> ConfigReader::s_files;
#endif