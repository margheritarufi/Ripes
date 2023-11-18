#include <iomanip>

std::shared_ptr<std::ofstream> createRegsFile();
void writeRegsInitialValues(std::shared_ptr<std::ofstream> file);
std::string getAliasRegName(int i);
