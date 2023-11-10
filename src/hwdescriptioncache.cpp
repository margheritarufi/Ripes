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


void saveNbWaysDataCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr) {
  data_ways = cacheSimPtr->getWaysBits();
  qDebug() << "Number of ways in data cache:" << data_ways;
}

void saveNbLinesDataCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr) {
  data_lines = cacheSimPtr->getLineBits();
  qDebug() << "Number of lines in data cache:" << data_lines;
}

void saveNbBlocksDataCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr) {
  data_blocks = cacheSimPtr->getBlockBits();
  qDebug() << "Number of blocks in data cache:" << data_lines;
}

void saveNbWaysInstrCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr) {
  instr_ways = cacheSimPtr->getWaysBits();
  qDebug() << "Nb of ways of instruction cache:" << instr_ways;
}

void saveNbLinesInstrCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr) {
  instr_lines = cacheSimPtr->getLineBits();
  qDebug() << "Nb of lines of instruction cache:" << instr_lines;
}

void saveNbBlocksInstrCache(std::shared_ptr<Ripes::CacheSim> cacheSimPtr) {
  instr_blocks = cacheSimPtr->getBlockBits();
  qDebug() << "Nb of blocks of instruction cache:" << instr_blocks;
}

void writeCacheSettings(std::shared_ptr<std::ofstream> file) {
  writeWaysDataCache(file);
  writeLinesDataCache(file);
  writeBlocksDataCache(file);
  writeWaysInstrCache(file);
  writeLinesInstrCache(file);
  writeBlocksInstrCache(file);
}

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

void writeWaysInstrCache(std::shared_ptr<std::ofstream> file) {
  if (file->is_open()) {
    (*file) << "`define " << std::left << std::setw(20) << "WAYS_INSTR_CACHE "
            << std::pow(2, instr_ways) << std::right << std::setw(35)
            << " //Possible values: any power of 2 until 2^10" << std::endl;
    std::cout << "Number of ways in the instr cache successfully written in "
                 "params.vh."
              << std::endl;
  } else {
    std::cerr << "Ripes couldn't open params.vh to write the number of ways in "
                 "the instr cache"
              << std::endl;
  }
}

void writeLinesInstrCache(std::shared_ptr<std::ofstream> file) {
  if (file->is_open()) {
    (*file) << "`define " << std::left << std::setw(20) << "LINES_INSTR_CACHE "
            << std::pow(2, instr_lines) << std::right << std::setw(35)
            << " //Possible values: any power of 2 until 2^10" << std::endl;
    std::cout << "Number of lines in the instr cache successfully written in "
                 "params.vh."
              << std::endl;
  } else {
    std::cerr << "Ripes couldn't open params.vh to write the number of lines "
                 "in the instr cache"
              << std::endl;
  }
}

void writeBlocksInstrCache(std::shared_ptr<std::ofstream> file) {
  if (file->is_open()) {
    (*file) << "`define " << std::left << std::setw(20) << "BLOCKS_INSTR_CACHE "
            << std::pow(2, instr_blocks) << std::right << std::setw(35)
            << " //Possible values: any power of 2 until 2^10" << std::endl;
    std::cout << "Number of blocks in the instr cache successfully written in "
                 "params.vh."
              << std::endl;
  } else {
    std::cerr << "Ripes couldn't open params.vh to write the number of lines "
                 "in the instr cache"
              << std::endl;
  }
}
