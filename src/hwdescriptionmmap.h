#include <iomanip>

std::shared_ptr<std::ofstream> createMemoryMapFile();
void writeMemoryMap(std::shared_ptr<std::ofstream> file);
