#pragma once

#include <string>
#include <vector>

struct Entity
{
	int type;
	int eventNum;
	int flagNum;
	unsigned int flags;
	int drawOrder; // Just in case
};

class PXEReader
{
	std::vector<Entity> entities;

public:
	PXEReader() = default;
	PXEReader(const std::string& fileName);

	template <typename Func>
	std::vector<Entity> getEntities(Func condition) const
	{
		std::vector<Entity> result;
		for (const Entity& e : entities)
			if (condition(e))
				result.push_back(e);
		return result;
	}

	std::vector<Entity>::const_iterator begin() const { return entities.begin(); }
	std::vector<Entity>::const_iterator end() const { return entities.end(); }
};
