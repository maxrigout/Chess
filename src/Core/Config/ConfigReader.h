#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <exception>
#include <iostream>

// just simple key-value pair for now...
class ConfigReader
{
private:
	class Property
	{
	public:
		Property(const std::string& value, const std::unordered_map<std::string, Property>& children = {})
		: m_value(value), m_childProperties(children)
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
				throw std::runtime_error("unable to locate property 1.: " + key);
			}
		}
	private:
		std::string m_value;
		std::unordered_map<std::string, Property> m_childProperties;
	};

	class File
	{
	public:
		File(const std::unordered_map<std::string, Property>& props)
		: m_properties{ props }
		{
			std::cout << "File constructor\n";
			std::cout << "props.size()        == " << props.size() << std::endl;
			std::cout << "m_properties.size() == " << m_properties.size() << std::endl;
		}
		File(const File& other)
		: m_properties{ other.m_properties }
		{
			std::cout << "File copy contructor\n";
			std::cout << "other.m_properties.size() == " << other.m_properties.size() << std::endl;
			std::cout << "m_properties.size()       == " << m_properties.size() << std::endl;
		}

		const Property& operator[](const std::string& key) const 
		{
			try 
			{
				return m_properties.at(key);
			}
			catch (...)
			{
				throw std::runtime_error("unable to locate property 2.: " + key);
			}
		}
	private:
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
			ite = s_files.emplace(fileName, file).first;
		}
		return ite->second;
	}


private:

	ConfigReader() {}

	const File& GetFile_Impl(const std::string& fileName)
	{
		auto ite = s_files.find(fileName);
		if (ite == s_files.end())
		{
			// load the file
			File file = ParseFile(fileName);
			ite = s_files.emplace(fileName, file).first;
		}
		return ite->second;
	}

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
		while (getline(f, line))
		{
			std::string key, value;
			std::stringstream ss;
			ss << line;

			ss >> key >> value;
			key = key.substr(0, key.length() - 1);
			std::cout << "key: " << key << " - value: -" << value << "-" << std::endl;
			if (!value.empty())
			{
				Property property(value);
				fileData.emplace(key, property);
				continue;
			}
		}

		return File(fileData);
	}

	// std::unordered_map<std::string, File> m_files;
	static std::unordered_map<std::string, File> s_files;
};