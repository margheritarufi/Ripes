//@file hwdescriptioncache.cpp
//@author Margherita Rufi
//@version 1.0 2023-11-13
//@brief This file contains the functions used to write the parameters of the
// processor'a cache in the params.vh file. The functions are called in the
// function "downloadFiles" in the file "hwdescription.cpp".
#include "hwdescriptioncache.h"

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
// of the data cache in the tab "Data Cache" of the window "Cache".
//
//@param std::shared_ptr<Ripes::CacheSim> cacheSimPtr: pointer to the data cache
// simulator
//@return void
void saveNbWaysDataCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr) {
  data_ways = cacheSimPtr->getWaysBits();
  qDebug() << "Number of ways in data cache:" << data_ways;
}

//@brief saveNbLinesDataCache
// This function saves the number of lines of the data cache selected by the
// user in the variable "data_lines". It is called in two cases, both from the
// cachewidget.cpp file:
// 1. Before the launch of Ripes GUI, to get the default number of lines.
// 2. After the launch of the GUI, any time the user changes the number of lines
// of the data cache in the tab "Data Cache" of the window "Cache".
//
//@param std::shared_ptr<Ripes::CacheSim> cacheSimPtr: pointer to the data cache
// simulator
//@return void
void saveNbLinesDataCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr) {
  data_lines = cacheSimPtr->getLineBits();
  qDebug() << "Number of lines in data cache:" << data_lines;
}

//@brief saveNbBlocksDataCache
// This function saves the number of blocks of the data cache selected by the
// user in the variable "data_blocks". It is called in two cases, both from the
// cachewidget.cpp file:
// 1. Before the launch of Ripes GUI, to get the default number of blocks.
// 2. After the launch of the GUI, any time the user changes the number of
// blocks of the data cache in the tab "Data Cache" of the window "Cache".
//
//@param std::shared_ptr<Ripes::CacheSim> cacheSimPtr: pointer to the data cache
// simulator
//@return void
void saveNbBlocksDataCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr) {
  data_blocks = cacheSimPtr->getBlockBits();
  qDebug() << "Number of blocks in data cache:" << data_blocks;
}

//@brief saveNbWaysInstrCache
// This function saves the number of ways of the instruction cache selected by
// the user in the variable "instr_ways". It is called in two cases, both from
// the cachewidget.cpp file:
// 1. Before the launch of Ripes GUI, to get the default number of ways.
// 2. After the launch of the GUI, any time the user changes the number of ways
// of the instruction cache in the tab "Instruction Cache" of the window
// "Cache".
//
//@param std::shared_ptr<Ripes::CacheSim> cacheSimPtr: pointer to the
// instruction cache simulator
//@return void
void saveNbWaysInstrCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr) {
  instr_ways = cacheSimPtr->getWaysBits();
  qDebug() << "Nb of ways of instruction cache:" << instr_ways;
}

//@brief saveNbLinesInstrCache
// This function saves the number of lines of the instruction cache selected by
// the user in the variable "instr_lines". It is called in two cases, both from
// the cachewidget.cpp file:
// 1. Before the launch of Ripes GUI, to get the default number of lines.
// 2. After the launch of the GUI, any time the user changes the number of lines
// of the instruction cache in the tab "Instruction Cache" of the window
// "Cache".
//
//@param std::shared_ptr<Ripes::CacheSim> cacheSimPtr: pointer to the
// instruction cache simulator
//@return void
void saveNbLinesInstrCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr) {
  instr_lines = cacheSimPtr->getLineBits();
  qDebug() << "Nb of lines of instruction cache:" << instr_lines;
}

//@brief saveNbBlocksInstrCache
// This function saves the number of blocks of the instruction cache selected by
// the user in the variable "instr_blocks". It is called in two cases, both from
// the cachewidget.cpp file:
// 1. Before the launch of Ripes GUI, to get the default number of blocks.
// 2. After the launch of the GUI, any time the user changes the number of
// blocks of the instruction cache in the tab "Instruction Cache" of the window
// "Cache".
//
//@param std::shared_ptr<Ripes::CacheSim> cacheSimPtr: pointer to the
// instruction cache simulator
//@return void
void saveNbBlocksInstrCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr) {
  instr_blocks = cacheSimPtr->getBlockBits();
  qDebug() << "Nb of blocks of instruction cache:" << instr_blocks;
}

//@brief writeCacheSettings
// This function is called in the function "downloadFiles" in the file
// "hwdescription.cpp" and writes the parameters of the processor's cache in the
// file "params.vh".
//
//@param std::shared_ptr<std::ofstream> file: pointer to the file "params.vh"
//@return void
void writeCacheSettings(std::shared_ptr<std::ofstream> file) {
  writeWaysDataCache(file);
  writeLinesDataCache(file);
  writeBlocksDataCache(file);
  writeWaysInstrCache(file);
  writeLinesInstrCache(file);
  writeBlocksInstrCache(file);
}

//@brief writeWaysDataCache
// This function writes the number of ways of the data cache in the file
// "params.vh". It uses the value saved in the variable "data_ways".
//
//@param std::shared_ptr<std::ofstream> file: pointer to the file "params.vh"
//@return void
void writeWaysDataCache(std::shared_ptr<std::ofstream> file) {
  if (file->is_open()) {
    (*file) << "\n`define " << std::left << std::setw(20) << "WAYS_DATA_CACHE "
            << std::pow(2, data_ways) << std::right << std::setw(45)
            << " //Possible values: any power of 2 until 2^10" << std::endl;
    std::cout
        << "Number of ways in the data cache successfully written in params.vh."
        << std::endl;
  } else {
    std::cerr << "Ripes couldn't open params.vh to write the number of ways in "
                 "the data cache"
              << std::endl;
  }
}

//@brief writeLinesDataCache
// This function writes the number of lines of the data cache in the file
// "params.vh". It uses the value saved in the variable "data_lines".
//
//@param std::shared_ptr<std::ofstream> file: pointer to the file "params.vh"
//@return void
void writeLinesDataCache(std::shared_ptr<std::ofstream> file) {
  if (file->is_open()) {
    (*file) << "`define " << std::left << std::setw(20) << "LINES_DATA_CACHE "
            << std::pow(2, data_lines) << std::right << std::setw(35)
            << " //Possible values: any power of 2 until 2^10" << std::endl;
    std::cout << "Number of lines in the data cache successfully written in "
                 "params.vh."
              << std::endl;
  } else {
    std::cerr << "Ripes couldn't open params.vh to write the number of lines "
                 "in the data cache"
              << std::endl;
  }
}

//@brief writeBlocksDataCache
// This function writes the number of blocks of the data cache in the file
// "params.vh". It uses the value saved in the variable "data_blocks".
//
//@param std::shared_ptr<std::ofstream> file: pointer to the file "params.vh"
//@return void
void writeBlocksDataCache(std::shared_ptr<std::ofstream> file) {
  if (file->is_open()) {
    (*file) << "`define " << std::left << std::setw(20) << "BLOCKS_DATA_CACHE "
            << std::pow(2, data_blocks) << std::right << std::setw(35)
            << " //Possible values: any power of 2 until 2^10" << std::endl;
    std::cout << "Number of blocks in the data cache successfully written in "
                 "params.vh."
              << std::endl;
  } else {
    std::cerr << "Ripes couldn't open params.vh to write the number of lines "
                 "in the data cache"
              << std::endl;
  }
}

//@brief writeWaysInstrCache
// This function writes the number of ways of the instruction cache in the file
// "params.vh". It uses the value saved in the variable "instr_ways".
//
//@param std::shared_ptr<std::ofstream> file: pointer to the file "params.vh"
//@return void
void writeWaysInstrCache(std::shared_ptr<std::ofstream> file) {
  if (file->is_open()) {
    (*file) << "`define " << std::left << std::setw(20) << "WAYS_INSTR_CACHE "
            << std::pow(2, instr_ways) << std::right << std::setw(35)
            << " //Possible values: any power of 2 until 2^10" << std::endl;
    std::cout
        << "Number of ways in the instruction cache successfully written in "
           "params.vh."
        << std::endl;
  } else {
    std::cerr << "Ripes couldn't open params.vh to write the number of ways in "
                 "the instruction cache"
              << std::endl;
  }
}

//@brief writeLinesInstrCache
// This function writes the number of lines of the instruction cache in the file
// "params.vh". It uses the value saved in the variable "instr_lines".
//
//@param std::shared_ptr<std::ofstream> file: pointer to the file "params.vh"
//@return void
void writeLinesInstrCache(std::shared_ptr<std::ofstream> file) {
  if (file->is_open()) {
    (*file) << "`define " << std::left << std::setw(20) << "LINES_INSTR_CACHE "
            << std::pow(2, instr_lines) << std::right << std::setw(35)
            << " //Possible values: any power of 2 until 2^10" << std::endl;
    std::cout
        << "Number of lines in the instruction cache successfully written in "
           "params.vh."
        << std::endl;
  } else {
    std::cerr << "Ripes couldn't open params.vh to write the number of lines "
                 "in the instruction cache"
              << std::endl;
  }
}

//@brief writeBlocksInstrCache
// This function writes the number of blocks of the instruction cache in the
// file "params.vh". It uses the value saved in the variable "instr_blocks".
//
//@param std::shared_ptr<std::ofstream> file: pointer to the file "params.vh"
//@return void
void writeBlocksInstrCache(std::shared_ptr<std::ofstream> file) {
  if (file->is_open()) {
    (*file) << "`define " << std::left << std::setw(20) << "BLOCKS_INSTR_CACHE "
            << std::pow(2, instr_blocks) << std::right << std::setw(35)
            << " //Possible values: any power of 2 until 2^10" << std::endl;
    std::cout
        << "Number of blocks in the instruction cache successfully written in "
           "params.vh."
        << std::endl;
  } else {
    std::cerr << "Ripes couldn't open params.vh to write the number of lines "
                 "in the instruction cache"
              << std::endl;
  }

  file->close(); // to delete when add new code!!!!!
}
