#include "FlagScraper.h"
#include <iostream>
#include <stdexcept>
#include <filesystem>
#include <utility>
#include "TSCReader.h"
#include "PXEReader.h"
namespace fs = std::filesystem;

FlagScraper::FlagScraper(const std::string& dir, bool showFlag0) :
	flagUsages(), dataDir(dir), procFlag0(showFlag0)
{
	fs::path dataPath{dataDir};
	if (!fs::is_directory(dataPath))
		throw std::runtime_error("invalid data path");
}

void FlagScraper::setUsage(int flag, FlagUsage fu)
{
	if (flag < 0 || flag >= NumFlags)
	{
		std::cerr << "WARNING: Detected usage of out-of-bounds flag " << flag
			<< "!\nThis flag usage will be ignored.\n";
	}
	else
	{
		flagUsages.at(flag).push_back(std::move(fu));
	}
}

auto FlagScraper::process() -> const UsagesArray&
{
	fs::path dataPath{dataDir};

	// Process loose .tsc files first
	for (const fs::directory_entry& dirEntry : fs::directory_iterator(dataPath))
	{
		auto path = dirEntry.path();
		if (dirEntry.is_regular_file() && path.extension() == ".tsc" && path.stem() != "Credit")
			processTSCFile(path.string(), nullptr);
	}

	// Now go into Stage directory
	fs::path stagePath = dataPath / "Stage";
	if (!fs::is_directory(stagePath))
		throw std::runtime_error("Stage folder not found!");

	for (const fs::directory_entry& dirEntry : fs::directory_iterator(stagePath))
	{
		auto path = dirEntry.path();
		if (dirEntry.is_regular_file() && path.extension() == ".pxe")
		{
			std::string pxePath = path.string();
			path.replace_extension(".tsc");
			std::string tscPath = path.string();
			processMap(pxePath, tscPath);
		}
	}

	return flagUsages;
}

void FlagScraper::processTSCFile(const std::string& filePath, const PXEReader* entities)
{
	TSCReader reader{filePath};
	const std::string mapName = fs::path{filePath}.stem().string();

	for (const Event& event : reader)
	{
		int eventNum = event.id;

		for (const char* ch = event.start; ch != event.end; ++ch)
		{
			if (*ch == '<')
			{
				if (*(ch + 1) == 'F' && *(ch + 2) == 'L')
				{
					switch (*(ch + 3))
					{
					case '+':
						setUsage(getNumber(ch + 4), FlagUsage(mapName, eventNum, 0, 0, FlagUsage::UsageType::Flag_Plus));
						break;
					case '-':
						setUsage(getNumber(ch + 4), FlagUsage(mapName, eventNum, 0, 0, FlagUsage::UsageType::Flag_Minus));
						break;
					case 'J':
						setUsage(getNumber(ch + 4), FlagUsage(mapName, eventNum, 0, 0, FlagUsage::UsageType::Flag_Jump));
						break;
					}
				}
				else if (*(ch + 1) == 'D' && *(ch + 2) == 'N' && entities != nullptr)
				{
					if (*(ch + 3) == 'P')
					{
						int delEvent = getNumber(ch + 4);
						std::vector<Entity> npcs = entities->getEntities([delEvent](const Entity& e) { return e.eventNum == delEvent; });
						for (const Entity& e : npcs)
							if (e.flagNum != 0 || procFlag0)
								setUsage(e.flagNum, FlagUsage(mapName, eventNum, delEvent, 0, FlagUsage::UsageType::DNP));
					}
					else if (*(ch + 3) == 'A')
					{
						int delType = getNumber(ch + 4);
						std::vector<Entity> npcs = entities->getEntities([delType](const Entity& e) { return e.type == delType; });
						bool flags[NumFlags] = {};
						for (const Entity& e : npcs)
							if ((e.flagNum != 0 || procFlag0) && e.flagNum >= 0 && e.flagNum < NumFlags && !flags[e.flagNum])
							{
								setUsage(e.flagNum, FlagUsage(mapName, eventNum, delType, 0, FlagUsage::UsageType::DNA));
								flags[e.flagNum] = true;
							}
					}
				}
			}
		}
	}
}

void FlagScraper::processMap(const std::string& pxePath, const std::string& tscPath)
{
	PXEReader npcs{pxePath};
	const std::string mapName = fs::path{pxePath}.stem().string();

	// Report TSC events first
	processTSCFile(tscPath, &npcs);

	// Now do NPCs
	for (const Entity& npc : npcs)
	{
		if (npc.flagNum != 0 || (npc.flags & 0x4800) != 0 || procFlag0)
		{
			if (npc.flags & 0x4800)
			{
				if (npc.flags & 0x800) // Appear when flag set
					setUsage(npc.flagNum, FlagUsage(mapName, npc.eventNum, npc.type, npc.drawOrder, FlagUsage::UsageType::Appear_Set));
				else // Disppear when flag set
					setUsage(npc.flagNum, FlagUsage(mapName, npc.eventNum, npc.type, npc.drawOrder, FlagUsage::UsageType::Appear_Clear));
			}
			else
				setUsage(npc.flagNum, FlagUsage(mapName, npc.eventNum, npc.type, npc.drawOrder, FlagUsage::UsageType::Flag_ID));
		}
	}
}
