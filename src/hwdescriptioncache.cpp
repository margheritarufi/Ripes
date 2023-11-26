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

int data_ways;
int data_lines;
int data_blocks;
int instr_ways;
int instr_lines;
int instr_blocks;

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
void saveNbWaysDataCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr) {
  data_ways = cacheSimPtr->getWaysBits();
}

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
void saveNbLinesDataCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr) {
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
}

//@brief writeCacheSettings
// This function is called in the function "downloadFiles" in the file
// "hwdescription.cpp" and writes the parameters of the processor's cache in the
// file "params.vh".
//
//@param std::shared_ptr<std::ofstream> file: pointer to the file "params.vh"
//@return void
void writeCacheSettings(std::shared_ptr<std::ofstream> file) {
  /*writeWaysDataCache(file);
  writeLinesDataCache(file);
  writeBlocksDataCache(file);
  writeWaysInstrCache(file);
  writeLinesInstrCache(file);
  writeBlocksInstrCache(file);*/

  if (file->is_open()) {
    (*file) << "\n" << std::endl;
    printVerilogDefine(file, "WAYS_DATA_CACHE", std::pow(2, data_ways),
                       "//Possible values: any power of 2 until 2^10");
    printVerilogDefine(file, "LINES_DATA_CACHE", std::pow(2, data_lines),
                       "//Possible values: any power of 2 until 2^10");
    printVerilogDefine(file, "BLOCKS_DATA_CACHE", std::pow(2, data_blocks),
                       "//Possible values: any power of 2 until 2^10");
    printVerilogDefine(file, "WAYS_INSTR_CACHE", std::pow(2, instr_ways),
                       "//Possible values: any power of 2 until 2^10");
    printVerilogDefine(file, "LINES_INSTR_CACHE", std::pow(2, instr_lines),
                       "//Possible values: any power of 2 until 2^10");
    printVerilogDefine(file, "BLOCKS_INSTR_CACHE", std::pow(2, instr_blocks),
                       "//Possible values: any power of 2 until 2^10");
    sendOutputStream("Cache settings", paramsFileName);
  } else {
    sendErrorStream("Cache settings", paramsFileName);
  }
}
