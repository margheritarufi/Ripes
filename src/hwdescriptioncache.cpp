//@file hwdescriptioncache.cpp
//@author Margherita Rufi
//@version 1.1 2023-12-08
//@brief This file contains the functions used to write the parameters of the
// processor'a cache in the params.vh file. The functions are called in the
// function "downloadFiles" in the file "hwdescription.cpp".
#include "hwdescriptioncache.h"
#include "hwdescription.h"

#include <QGraphicsScene>
#include <QGraphicsView>

#include <QDebug>
#include <cmath>
#include <iomanip>
#include <memory>

int dataWays;
int dataLines;
int dataBlocks;
int dataWriteAllocPolicy;
int tempWriteAllocPolicy;
int dataWritePolicy;
int dataReplPolicy;
int instrWays;
int instrLines;
int instrBlocks;
int instrWriteAllocPolicy;
int instrWritePolicy;
int instrReplPolicy;
bool valueDataUpdated;
bool valueInstrUpdated;
std::shared_ptr<Ripes::CacheSim> dataCachePointer = nullptr;
std::shared_ptr<Ripes::CacheSim> instrCachePointer = nullptr;

//@brief saveDataCacheSettings
// This function saves the settings of the data cache selected by the user.
// It is called in two cases, both from the cachewidget.cpp file:
// 1. Before the launch of Ripes GUI, to get the default settingss.
// 2. After the launch of the GUI, any time the user changes the settings
// of the data cache.
//
//@param std::shared_ptr<Ripes::CacheSim> cacheSimPtr: pointer to the data cache
// simulator
//@return void
void saveDataCacheSettings(std::shared_ptr<Ripes::CacheSim> cacheSimPtr) {
  dataWays = cacheSimPtr->getWaysBits();
  dataLines = cacheSimPtr->getLineBits();
  dataBlocks = cacheSimPtr->getBlockBits();
  dataWriteAllocPolicy = cacheSimPtr->getWriteAllocPolicy();
  dataWritePolicy = cacheSimPtr->getWritePolicy();
  dataReplPolicy = cacheSimPtr->getReplacementPolicy();
  valueDataUpdated = true;
}

//@brief saveDataCacheSettings
// This function saves the settings of the instruction cache selected by the
// user. It is called in two cases, both from the cachewidget.cpp file:
// 1. Before the launch of Ripes GUI, to get the default settingss.
// 2. After the launch of the GUI, any time the user changes the settings
// of the instruction cache.
//
//@param std::shared_ptr<Ripes::CacheSim> cacheSimPtr: pointer to the data cache
// simulator
//@return void
void saveInstrCacheSettings(std::shared_ptr<Ripes::CacheSim> cacheSimPtr) {
  instrWays = cacheSimPtr->getWaysBits();
  instrLines = cacheSimPtr->getLineBits();
  instrBlocks = cacheSimPtr->getBlockBits();
  instrWriteAllocPolicy = cacheSimPtr->getWriteAllocPolicy();
  instrWritePolicy = cacheSimPtr->getWritePolicy();
  instrReplPolicy = cacheSimPtr->getReplacementPolicy();
  valueInstrUpdated = true;
}

//@brief writeCacheSettings
// This function is called in the function "downloadFiles" in the file
// "hwdescription.cpp" and writes the parameters of the processor's cache in the
// file "ripes_params.vh".
//
//@param std::shared_ptr<std::ofstream> file: pointer to the file "params.vh"
//@return void
void writeCacheSettings(std::shared_ptr<std::ofstream> file) {
  // The WriteAllocatePolicy type assigns 0 to "Write Allocate" and 1 to "No
  // Write ALlocate". It makes more sense to switch them. The first if-clause is
  // needed to avoid reverse value if save**CacheSettings() functions weren't
  // called
  if (valueDataUpdated == true) {
    if (dataWriteAllocPolicy == 0) {
      tempWriteAllocPolicy = 1;
    } else if (dataWriteAllocPolicy == 1) {
      tempWriteAllocPolicy = 0;
    }
    dataWriteAllocPolicy = tempWriteAllocPolicy;
    // Put back valueUpdated to false
    valueDataUpdated = false;
  }
  if (valueInstrUpdated == true) {
    if (instrWriteAllocPolicy == 0) {
      tempWriteAllocPolicy = 1;
    } else if (instrWriteAllocPolicy == 1) {
      tempWriteAllocPolicy = 0;
    }
    instrWriteAllocPolicy = tempWriteAllocPolicy;
    // Put back valueUpdated to false
    valueInstrUpdated = false;
  }

  if (file->is_open()) {
    (*file) << "\n" << std::endl;
    printVerilogDefine(file, "WAYS_DATA_CACHE", std::pow(2, dataWays),
                       "//Possible values: any power of 2 until 2^10");
    printVerilogDefine(file, "LINES_DATA_CACHE", std::pow(2, dataLines),
                       "//Possible values: any power of 2 until 2^10");
    printVerilogDefine(file, "WORDSLINE_DATA_CACHE", std::pow(2, dataBlocks),
                       "//Possible values: any power of 2 until 2^10");
    printVerilogDefine(
        file, "WR_ALLOC_POLICY_DCACHE", dataWriteAllocPolicy,
        "//Possible values: 0 (No Write Allocate), 1 (Write Allocate)");
    printVerilogDefine(file, "WR_POLICY_DCACHE", dataWritePolicy,
                       "//Possible values: 0 (Write Through), 1 (Write Back)");
    printVerilogDefine(file, "REPL_POLICY_DCACHE", dataReplPolicy,
                       "//Possible values: 0 (Random), 1 (LRU)");
    printVerilogDefine(file, "WAYS_INSTR_CACHE", std::pow(2, instrWays),
                       "//Possible values: any power of 2 until 2^10");
    printVerilogDefine(file, "LINES_INSTR_CACHE", std::pow(2, instrLines),
                       "//Possible values: any power of 2 until 2^10");
    printVerilogDefine(file, "WORDSLINE_INSTR_CACHE", std::pow(2, instrBlocks),
                       "//Possible values: any power of 2 until 2^10");
    printVerilogDefine(
        file, "WR_ALLOC_POLICY_ICACHE", instrWriteAllocPolicy,
        "//Possible values: 0 (No Write Allocate), 1 (Write Allocate)");
    printVerilogDefine(file, "WR_POLICY_ICACHE", instrWritePolicy,
                       "//Possible values: 0 (Write Through), 1 (Write Back)");
    printVerilogDefine(file, "REPL_POLICY_ICACHE", instrReplPolicy,
                       "//Possible values: 0 (Random), 1 (LRU)");
    sendOutputStream("Cache settings", paramsFileName);
  } else {
    sendErrorStream("Cache settings", paramsFileName);
  }
}
