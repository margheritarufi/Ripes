#include "hwdescription.h"
#include <QFileDialog>
#include <QDebug>

namespace Ripes{

void OpenFileDialog(){
  QString defaultFileName = "Ripes_params.vh";
  QString QselectedDirectory = QFileDialog::getSaveFileName(nullptr, "Select destination folder", QDir::homePath() + "/" + defaultFileName, "Verilog header file (*.vh);;All Files (*)");
  if (!QselectedDirectory.isEmpty()) {
    qDebug() << "Selected folder: " << QselectedDirectory;
    //QString fileName = "params.vh";
    //QString QcompleteDirectory = selectedDirectory + "/" + fileName;
    std::string selectedDirectory = QselectedDirectory.toStdString();
    std::ofstream file(selectedDirectory);

    if (file.is_open()) {
      file << "//These are automatically-generated parameters. The designer cannot change them because they depend on the user's choice of the processor" << std::endl;
      file.close();
      std::cout << "'params.vh' file created successfully." << std::endl;
    } else {
      std::cerr << "Ripes couldn't open 'params.vh' file." << std::endl;
    }
  }else{
    std::cerr << "No folder chosen." << std::endl;
  }

}

/*HwDescription::HwDescription(QObject* parent) : QObject(parent) {}

void HwDescription::OpenFileDialog(){
  // Creare una finestra di dialogo per la selezione della directory
  QString selectedDirectory = QFileDialog::getExistingDirectory(nullptr, "Select destination folder", QDir::homePath(), QFileDialog::ShowDirsOnly);

  if (!selectedDirectory.isEmpty()) {
    qDebug() << "Selected folder: " << selectedDirectory;
  }
}*/
}
