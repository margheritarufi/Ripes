#include "hwdescriptioncache.h"
#include "cachesim/cachewidget.h"

#include <QGraphicsScene>
#include <QGraphicsView>

#include <QDebug>
#include <iomanip>
#include <cmath>

int data_ways;
int data_lines;

void saveNbDataWays(std::shared_ptr<Ripes::CacheSim> cacheSimPtr){
  data_ways = cacheSimPtr->getWaysBits();
  qDebug() << "Number of ways in data cache:" << data_ways;
}

void saveNbDataLines(std::shared_ptr<Ripes::CacheSim> cacheSimPtr){
  data_lines = cacheSimPtr->getLineBits();
  qDebug() << "Number of ways in data cache:" << data_lines;
}

void saveNbInstrWays(std::shared_ptr<Ripes::CacheSim> cacheSimPtr){
  int ways = cacheSimPtr->getWaysBits();
  qDebug() << "Nb of ways of instruction cache:" << ways;
}

void writeDataCacheSettings(std::shared_ptr<std::ofstream> file){
  writeWaysDataCache(file);
  writeLinesDataCache(file);
}

void writeWaysDataCache(std::shared_ptr<std::ofstream> file){
  if (file->is_open()) {
    (*file) << "`define " << std::left << std::setw(15) << "WAYS_DATACACHE " << std::pow(2, data_ways) << std::right << std::setw(35) << " //Possible values: any power of 2 until 2^10" << std::endl;
    std::cout << "Number of ways in the data cache successfully written in params.vh." << std::endl;
  } else {
    std::cerr << "Ripes couldn't open params.vh to write the number of ways in the data cache" << std::endl;
  }
}

void writeLinesDataCache(std::shared_ptr<std::ofstream> file){
  if (file->is_open()) {
    (*file) << "`define " << std::left << std::setw(15) << "LINES_DATACACHE " << std::pow(2, data_lines) << std::right << std::setw(35) << " //Possible values: any power of 2 until 2^10" << std::endl;
    std::cout << "Number of lines in the data cache successfully written in params.vh." << std::endl;
  } else {
    std::cerr << "Ripes couldn't open params.vh to write the number of lines in the data cache" << std::endl;
  }
}
