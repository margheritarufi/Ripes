#include "cachesim/cachesim.h"
#include <QWidget>

void saveNbWaysDataCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr);
void saveNbLinesDataCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr);
void saveNbBlocksDataCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr);
void saveNbWaysInstrCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr);
void saveNbLinesInstrCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr);
void saveNbBlocksInstrCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr);
void writeCacheSettings(std::shared_ptr<std::ofstream> file);
