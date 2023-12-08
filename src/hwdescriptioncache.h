//@file hwdescriptioncache.h
//@author Margherita Rufi
//@version 1.1 2023-12-08
//@brief This file contains the declaration of the functions used to save the
//cache settings in a file.

#include "cachesim/cachesim.h"
#include <QWidget>
#include <memory>

void saveDataCacheSettings(std::shared_ptr<Ripes::CacheSim> cacheSimPtr);
void saveInstrCacheSettings(std::shared_ptr<Ripes::CacheSim> cacheSimPtr);

void writeCacheSettings(std::shared_ptr<std::ofstream> file);
