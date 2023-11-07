#include "cachesim/cachesim.h"
#include "processors/RISC-V/rv_memory.h"
#include <QWidget>

void saveNbDataWays(std::shared_ptr<Ripes::CacheSim> cacheSimPtr);
void saveNbDataLines(std::shared_ptr<Ripes::CacheSim> cacheSimPtr);
void saveNbInstrWays(std::shared_ptr<Ripes::CacheSim> cacheSimPtr);
void writeDataCacheSettings(std::shared_ptr<std::ofstream> file);
void writeWaysDataCache(std::shared_ptr<std::ofstream> file);
void writeLinesDataCache(std::shared_ptr<std::ofstream> file);
