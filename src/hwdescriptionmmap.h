#include <iomanip>
#include <memory>

std::shared_ptr<std::ofstream> createMemoryMapFile();
void writeMemoryMap(std::shared_ptr<std::ofstream> file);
