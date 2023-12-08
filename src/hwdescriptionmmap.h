//@file hwdescriptionmmap.h
//@author Margherita Rufi
//@version 1.0 2023-11-26
//@brief This file contains the declaration of the functions of
// hwdescriptionmmap.cpp.

#include <iomanip>
#include <memory>

std::shared_ptr<std::ofstream> createMemoryMapFile();
void writeMemoryMap(std::shared_ptr<std::ofstream> file);
