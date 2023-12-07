//@file hwdescriptioncache.cpp
//@author Margherita Rufi
//@version 1.0 2023-11-26
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

//@brief saveNbWaysDataCache
// This function saves the number of ways of the data cache selected by the user
// in the variable "data_ways". It is called in two cases, both from the
// cachewidget.cpp file:
// 1. Before the launch of Ripes GUI, to get the default number of ways.
// 2. After the launch of the GUI, any time the user changes the number of ways
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

void saveInstrCacheSettings(std::shared_ptr<Ripes::CacheSim> cacheSimPtr) {
  instrWays = cacheSimPtr->getWaysBits();
  instrLines = cacheSimPtr->getLineBits();
  instrBlocks = cacheSimPtr->getBlockBits();
  instrWriteAllocPolicy = cacheSimPtr->getWriteAllocPolicy();
  instrWritePolicy = cacheSimPtr->getWritePolicy();
  instrReplPolicy = cacheSimPtr->getReplacementPolicy();
  valueInstrUpdated = true;
}

/*void saveDataCacheSimPointer(std::shared_ptr<Ripes::CacheSim> m_cacheSim){
  dataCachePointer = m_cacheSim;
}

void saveInstrCacheSimPointer(std::shared_ptr<Ripes::CacheSim> m_cacheSim){
  instrCachePointer = m_cacheSim;
}*/

/*void saveReplPolicyDataCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr) {
  data_ways = cacheSimPtr->getWaysBits();
}*/

//@brief saveNbLinesDataCache
// This function saves the number of lines of the data cache selected by the
// user in the variable "data_lines". It is called in two cases, both from the
// cachewidget.cpp file:
// 1. Before the launch of Ripes GUI, to get the default number of lines.
// 2. After the launch of the GUI, any time the user changes the number of lines
// of the data cache.
//
//@param std::shared_ptr<Ripes::CacheSim> cacheSimPtr: pointer to the data cache
// simulator
//@return void
/*void saveNbLinesDataCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr) {
  data_lines = cacheSimPtr->getLineBits();
}

//@brief saveNbBlocksDataCache
// This function saves the number of blocks of the data cache selected by the
// user in the variable "data_blocks". It is called in two cases, both from the
// cachewidget.cpp file:
// 1. Before the launch of Ripes GUI, to get the default number of blocks.
// 2. After the launch of the GUI, any time the user changes the number of
// blocks of the data cache.
//
//@param std::shared_ptr<Ripes::CacheSim> cacheSimPtr: pointer to the data cache
// simulator
//@return void
void saveNbBlocksDataCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr) {
data_blocks = cacheSimPtr->getBlockBits();
}

//@brief saveNbWaysInstrCache
// This function saves the number of ways of the instruction cache selected by
// the user in the variable "instr_ways". It is called in two cases, both from
// the cachewidget.cpp file:
// 1. Before the launch of Ripes GUI, to get the default number of ways.
// 2. After the launch of the GUI, any time the user changes the number of ways
// of the instruction cache.
//
//@param std::shared_ptr<Ripes::CacheSim> cacheSimPtr: pointer to the
// instruction cache simulator
//@return void
void saveNbWaysInstrCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr) {
instr_ways = cacheSimPtr->getWaysBits();
}

//@brief saveNbLinesInstrCache
// This function saves the number of lines of the instruction cache selected by
// the user in the variable "instr_lines". It is called in two cases, both from
// the cachewidget.cpp file:
// 1. Before the launch of Ripes GUI, to get the default number of lines.
// 2. After the launch of the GUI, any time the user changes the number of lines
// of the instruction cache.
//
//@param std::shared_ptr<Ripes::CacheSim> cacheSimPtr: pointer to the
// instruction cache simulator
//@return void
void saveNbLinesInstrCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr) {
instr_lines = cacheSimPtr->getLineBits();
}

//@brief saveNbBlocksInstrCache
// This function saves the number of blocks of the instruction cache selected by
// the user in the variable "instr_blocks". It is called in two cases, both from
// the cachewidget.cpp file:
// 1. Before the launch of Ripes GUI, to get the default number of blocks.
// 2. After the launch of the GUI, any time the user changes the number of
// blocks of the instruction cache.
//
//@param std::shared_ptr<Ripes::CacheSim> cacheSimPtr: pointer to the
// instruction cache simulator
//@return void
void saveNbBlocksInstrCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr) {
instr_blocks = cacheSimPtr->getBlockBits();
}*/

//@brief writeCacheSettings
// This function is called in the function "downloadFiles" in the file
// "hwdescription.cpp" and writes the parameters of the processor's cache in the
// file "params.vh".
//
//@param std::shared_ptr<std::ofstream> file: pointer to the file "params.vh"
//@return void
void writeCacheSettings(std::shared_ptr<std::ofstream> file) {
  /*dataWays = dataCachePointer->getWaysBits();
  dataLines = dataCachePointer->getLineBits();
  dataBlocks = dataCachePointer->getBlockBits();
  dataWriteAllocPolicy = dataCachePointer->getWriteAllocPolicy();*/
  //The WriteAllocatePolicy type assigns 0 to "Write Allocate" and 1 to "No Write ALlocate".
  //It makes more sense to switch them.
  //The first if is needed to avoid reverse value if save*CacheSettings() functions weren't called
  if (valueDataUpdated == true){
    if (dataWriteAllocPolicy == 0){
      tempWriteAllocPolicy = 1;
    } else if (dataWriteAllocPolicy == 1){
      tempWriteAllocPolicy = 0;
    }
    dataWriteAllocPolicy = tempWriteAllocPolicy;
    //Put back valueUpdated to false
    valueDataUpdated = false;
  }
  /*dataWritePolicy = dataCachePointer->getWritePolicy();
  dataReplPolicy = dataCachePointer->getReplacementPolicy();
  instrWays = instrCachePointer->getWaysBits();
  instrLines = instrCachePointer->getLineBits();
  instrBlocks = instrCachePointer->getBlockBits();
  instrWriteAllocPolicy = instrCachePointer->getWriteAllocPolicy();*/
  if (valueInstrUpdated == true){
    if (instrWriteAllocPolicy == 0){
      tempWriteAllocPolicy = 1;
    } else if (instrWriteAllocPolicy == 1){
      tempWriteAllocPolicy = 0;
    }
    instrWriteAllocPolicy = tempWriteAllocPolicy;
    //Put back valueUpdated to false
    valueInstrUpdated = false;
  }


  /*instrWritePolicy = instrCachePointer->getWritePolicy();
  instrReplPolicy = instrCachePointer->getReplacementPolicy();*/
  if (file->is_open()) {
    (*file) << "\n" << std::endl;
    printVerilogDefine(file, "WAYS_DATA_CACHE", std::pow(2, dataWays),
                       "//Possible values: any power of 2 until 2^10");
    printVerilogDefine(file, "LINES_DATA_CACHE", std::pow(2, dataLines),
                       "//Possible values: any power of 2 until 2^10");
    printVerilogDefine(file, "BLOCKS_DATA_CACHE", std::pow(2, dataBlocks),
                       "//Possible values: any power of 2 until 2^10");
    printVerilogDefine(file, "WR_ALLOC_POLICY_DCACHE", dataWriteAllocPolicy,
                       "//Possible values: 0 (No Write Allocate), 1 (Write Allocate)");
    printVerilogDefine(file, "WR_POLICY_DCACHE", dataWritePolicy,
                       "//Possible values: 0 (Write Through), 1 (Write Back)");
    printVerilogDefine(file, "REPL_POLICY_DCACHE", dataReplPolicy,
                       "//Possible values: 0 (Random), 1 (LRU)");
    printVerilogDefine(file, "WAYS_INSTR_CACHE", std::pow(2, instrWays),
                       "//Possible values: any power of 2 until 2^10");
    printVerilogDefine(file, "LINES_INSTR_CACHE", std::pow(2, instrLines),
                       "//Possible values: any power of 2 until 2^10");
    printVerilogDefine(file, "BLOCKS_INSTR_CACHE", std::pow(2, instrBlocks),
                       "//Possible values: any power of 2 until 2^10");
    printVerilogDefine(file, "WR_ALLOC_POLICY_ICACHE", instrWriteAllocPolicy,
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
