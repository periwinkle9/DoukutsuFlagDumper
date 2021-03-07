#include "TSCReader.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <utility>
namespace fs = std::filesystem;

int getNumber(const char* str)
{
	int ret = 0;
	int pos = 1000;
	for (int i = 0; i < 4 && *str != '\0'; ++i)
	{
		ret += (*str - '0') * pos;
		++str;
		pos /= 10;
	}
	return ret;
}

TSCReader::EventIterator::EventIterator(const char* buf) :
	event{}
{
	event.end = buf;
	findNextEvent();
}

void TSCReader::EventIterator::findNextEvent()
{
	const char* ptr = event.end;
	if (ptr != nullptr)
	{
		Event e{};
		while (*ptr != '#' && *ptr != '\0')
			++ptr;
		if (*ptr == '#') // Found event
		{
			e.id = getNumber(++ptr);
			// Advance to newline
			while (*ptr != '\n' && *ptr != '\0') // ...but check for EOF just in case
				++ptr;
			if (*ptr != '\0')
			{
				e.start = ++ptr;
				// Find start of next event (yes, I'm too lazy to search for <END or whatnot)
				while (*ptr != '#' && *ptr != '\0')
					++ptr;
				e.end = ptr;
			}
		}

		event = std::move(e);
	}
}

Event TSCReader::EventIterator::operator*() const
{
	return event;
}
auto TSCReader::EventIterator::operator++() -> EventIterator&
{
	findNextEvent();
	return *this;
}


TSCReader::TSCReader(const std::string& fileName)
{
	// Read entire file
	{
		std::ifstream ifs(fileName, std::ios::binary);
		std::ostringstream oss;
		oss << ifs.rdbuf();
		contents = oss.str();
	}
	decrypt();
}

auto TSCReader::begin() const -> EventIterator
{
	return EventIterator{contents.data()};
}

auto TSCReader::end() const -> EventIterator
{
	return EventIterator{};
}

void TSCReader::decrypt()
{
	// Get middle byte
	unsigned char offset = contents[contents.size() / 2];
	if (offset == 0)
		offset = 7;

	for (std::string::size_type i = 0; i < contents.size(); ++i)
	{
		if (i != contents.size() / 2)
			contents[i] = static_cast<char>(static_cast<unsigned char>(contents[i]) - offset);
	}
}
