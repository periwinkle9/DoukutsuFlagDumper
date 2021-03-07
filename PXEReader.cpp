#include "PXEReader.h"
#include <fstream>
#include <utility>

PXEReader::PXEReader(const std::string& fileName)
{
	std::ifstream ifs(fileName, std::ios::binary);
	ifs.ignore(4); // Ignore "PXE" header
	int count;

	// Get number of entities
	ifs.read(reinterpret_cast<char*>(&count), 4);

	for (int i = 0; i < count; ++i)
	{
		Entity entity{};

		ifs.ignore(4); // Ignore x and y positions
		ifs.read(reinterpret_cast<char*>(&entity.flagNum), 2);
		ifs.read(reinterpret_cast<char*>(&entity.eventNum), 2);
		ifs.read(reinterpret_cast<char*>(&entity.type), 2);
		ifs.read(reinterpret_cast<char*>(&entity.flags), 2);
		entity.drawOrder = i;

		entities.push_back(std::move(entity));
	}
}
