//@file hwdescriptionioperiph.h
//@author Margherita Rufi
//@version 1.0 2023-11-26
//@brief This file contains the declaration of the functions of
//hwdescriptionioperiph.cpp.

#include <iomanip>
#include <memory>

void writeNbPeriph(std::shared_ptr<std::ofstream> file);
void writePeriphParams(std::shared_ptr<std::ofstream> file);
