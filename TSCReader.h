#pragma once

#include <string>
#include <iterator>

struct Event
{
	int id;
	const char* start;
	const char* end;
};

class TSCReader
{
	std::string contents;

	void decrypt();
public:
	class EventIterator
	{
		Event event;

		void findNextEvent();
	public:
		EventIterator(const char* buf = nullptr);

		Event operator*() const;
		EventIterator& operator++();
		EventIterator operator++(int) { EventIterator old(*this); ++(*this); return old; }

		bool operator==(const EventIterator& other) const { return event.start == other.event.start; }
		bool operator!=(const EventIterator& other) const { return !(*this == other); }
	};

	TSCReader() = default;
	TSCReader(const std::string& fileName);

	EventIterator begin() const;
	EventIterator end() const;

	const std::string& getContents() const { return contents; }
};

int getNumber(const char* str);
