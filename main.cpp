// main.cpp

// Project specific includes

#include "MakeString.h"

// Standard includes

// Boost

#include <boost/asio.hpp>
#include <boost/current_function.hpp>
#include <boost/detail/lightweight_main.hpp>
#include <boost/regex.hpp>

// Standard Library

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <set>
#include <string>
#include <utility>

namespace
{
	typedef std::set<std::string> mySet;
	mySet result;
}

struct Settings
{
	const std::string filename = "file.txt";                                           // C++11 required here
	std::string url;
	std::string host;
	std::string get;
	boost::asio::ip::tcp::iostream s;
} appSettings;

void error(const std::string& message)
{
	std::cerr << message << '\n';
	std::exit(EXIT_FAILURE);
}

void parseCmdLine(int argc, char* argv[])
{
	if (argc != 2)
	{
		error("*** Wrong number of arguments!");
	}

	appSettings.url = argv[1];

	std::size_t pos = appSettings.url.find('/');
	if (pos == std::string::npos)
	{
		appSettings.host = appSettings.url;
		appSettings.get = "/";
	}
	else
	{
		appSettings.host = appSettings.url.substr(0, pos);
		appSettings.get = appSettings.url.substr(pos);
	}
}

void printSettings()
{
	std::cout	<< std::setw(6) << std::left << "url: " << appSettings.url << '\n'
				<< std::setw(6) << std::left << "host: " << appSettings.host << '\n'
				<< std::setw(6) << std::left << "get: " << appSettings.get << '\n';
}

void connect()
{
	appSettings.s.connect(appSettings.host, "http");
	if (!appSettings.s)
	{
		error("*** Can't connect");
	}

	appSettings.s << "GET " << appSettings.get << " HTTP/1.0\r\n";
	appSettings.s << "Host: " << appSettings.host;
	appSettings.s << "\r\n\r\n" << std::flush;
}

void writeFile()
{
	try
	{
		std::ofstream f;
		f.exceptions(std::ios_base::failbit | std::ios_base::badbit);
		f.open(appSettings.filename);
		f << appSettings.s.rdbuf();
	}
	catch (const std::ios_base::failure& e)
	{
		error(MakeString() << "In function " << BOOST_CURRENT_FUNCTION << ": \n An error occurred while opening / writing data to file: " << e.what());
	}
}

void parseData()
{
	std::ifstream f(appSettings.filename);
	if (!f)
	{
		error("Can't open file");
	}

	std::string data((std::istream_iterator<char>(f)), std::istream_iterator<char>()); // Don't forget about parentheses!

	try
	{
		boost::regex regEx("([a-zA-Z0-9_\\.]+)@(([a-zA-Z0-9]+\\.)+[a-zA-Z]{2,4})");    // May throw boost::bad_expression
		boost::smatch results;

		std::string::const_iterator cur = data.begin();
		std::string::const_iterator end = data.end();
		while (boost::regex_search(cur, end, results, regEx))                          // May throw std::runtime_error
		{
			std::pair<mySet::iterator, bool> ret = result.insert(results[0]);
			if (ret.second == false)
			{
				std::cout << "Element " << results[0] << "already exists\n";
			}

			cur = results[1].second;
		}
	}
	catch (const std::runtime_error& e)
	{
		error(MakeString() << "In function " << BOOST_CURRENT_FUNCTION << ": \n An error occurred while parsing data: " << e.what());
	}
}

void printResults()
{
	std::cout << "==========================Results============================== \n";
	for (const auto& i : result)
	{
		std::cout << i << '\n';
	}
    /*
     *  We can also use algorithm from STL for non-C++11 compatible compilers:
     *	std::copy(result.begin(), result.end(), std::ostream_iterator<std::string>(std::cout, "\n"));
     */
}

int cpp_main(int argc, char* argv[])
{
	parseCmdLine(argc, argv);
	printSettings();
	connect();
	writeFile();
	parseData();
	printResults();
}