#include "cachesim/cachesim.h"
#include <QWidget>

void saveNbWaysDataCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr);
void saveNbLinesDataCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr);
void saveNbBlocksDataCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr);
void saveNbWaysInstrCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr);
void saveNbLinesInstrCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr);
void saveNbBlocksInstrCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr);
void writeCacheSettings(std::shared_ptr<std::ofstream> file);
void writeWaysDataCache(std::shared_ptr<std::ofstream> file);
void writeLinesDataCache(std::shared_ptr<std::ofstream> file);
void writeBlocksDataCache(std::shared_ptr<std::ofstream> file);
void writeWaysInstrCache(std::shared_ptr<std::ofstream> file);
void writeLinesInstrCache(std::shared_ptr<std::ofstream> file);
void writeBlocksInstrCache(std::shared_ptr<std::ofstream> file);
