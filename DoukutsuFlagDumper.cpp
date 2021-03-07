// Doukutsu flag dumper
// Dumps a massive list of flags and (almost) everywhere that they are used
// (almost as in, it won't account for flag jumps in Credit.tsc, or <DNP and <DNA commands in Head.tsc, or a couple of other weird unusual cases)

#include <iostream>
#include <iomanip>
#include <string>
#include <stdexcept>
#include "FlagScraper.h"
#include "PXEReader.h"

void printUsage(const char* name);
void outputFlags(const FlagScraper::UsagesArray& flags);

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printUsage(argv[0]);
		return 0;
	}

	std::string dataDir{argv[1]};
	bool showFlag0 = false;
	if (argc >= 3 && std::string{argv[2]} == "--show-flag-0")
		showFlag0 = true;

	try
	{
		FlagScraper fs{dataDir, showFlag0};
		outputFlags(fs.process());
	}
	catch (std::runtime_error& re)
	{
		std::cout << re.what() << "\n\n";
		printUsage(argv[0]);
		return 0;
	}
}

void outputFlags(const FlagScraper::UsagesArray& flags)
{
	std::cout << std::setfill('0') << std::right;
	for (unsigned i = 0; i < flags.size(); ++i)
	{
		if (!flags[i].empty())
		{
			std::cout << "Flag " << i << '\n';
			for (const auto& usage : flags[i])
			{
				std::cout << '\t' << usage.mapName << ": event " << usage.eventNum << ' ';

				using ut = decltype(usage.usageType);
				switch (usage.usageType)
				{
				case ut::Flag_Plus:
					std::cout << "<FL+";
					break;
				case ut::Flag_Minus:
					std::cout << "<FL-";
					break;
				case ut::Flag_Jump:
					std::cout << "<FLJ";
					break;
				case ut::DNP:
					std::cout << "<DNP" << std::setw(4) << usage.param;
					break;
				case ut::DNA:
					std::cout << "<DNA" << std::setw(4) << usage.param;
					break;
				case ut::Appear_Set:
					std::cout << "appear on flag set (type: " << usage.param << "; draw order: " << usage.param2 << ')';
					break;
				case ut::Appear_Clear:
					std::cout << "disappear on flag set (type: " << usage.param << "; draw order: " << usage.param2 << ')';
					break;
				case ut::Flag_ID:
					std::cout << "has flag ID (type: " << usage.param << "; draw order: " << usage.param2 << ')';
					break;
				}
				std::cout << '\n';
			}
		}
	}
}

void printUsage(const char* name)
{
	std::cout << "Usage: " << name << " <CS /data directory> [options]\n";
	std::cout << "Options:\n"
		"\t--show-flag-0\tShows usage of flag 0000 (but why?)\n";
}
