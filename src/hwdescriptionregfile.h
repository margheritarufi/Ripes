//@file hwdescriptionregfile.h
//@author Margherita Rufi
//@version 1.0 2023-11-26
//@brief This file contains the declaration of the functions of
//hwdescriptionregfile.cpp.

#include <iomanip>
#include <memory>

std::shared_ptr<std::ofstream> createRegsFile();
void writeRegsInitialValues(std::shared_ptr<std::ofstream> file);
std::string getAliasRegName(int i);
