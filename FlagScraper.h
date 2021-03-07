#pragma once

#include <string>
#include <vector>
#include <array>
#include <utility>
#include "PXEReader.h"

class FlagScraper
{
public:
	struct FlagUsage
	{
		enum class UsageType { Flag_Plus, Flag_Minus, Flag_Jump, DNP, DNA, Appear_Set, Appear_Clear, Flag_ID };
		std::string mapName;
		int eventNum;
		int param;
		int param2;
		UsageType usageType;

		FlagUsage() = default;
		FlagUsage(std::string m, int e, int p, int p2, UsageType u) : mapName(std::move(m)), eventNum(e), param(p), param2(p2), usageType(u) {}
	};

	static constexpr int NumFlags = 8000;
	using UsagesArray = std::array<std::vector<FlagUsage>, NumFlags>;
private:
	UsagesArray flagUsages;
	std::string dataDir;
	bool procFlag0;

	void processTSCFile(const std::string& filePath, const PXEReader* entities);
	void processMap(const std::string& pxePath, const std::string& tscPath);

	void setUsage(int flag, FlagUsage fu);
public:
	FlagScraper(const std::string& dir, bool showFlag0);

	const UsagesArray& process();
};
