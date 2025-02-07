#include "ioperipheraltab.h"
#include "ui_ioperipheraltab.h"

#include "editor/csyntaxhighlighter.h"
#include "fonts.h"
#include "io/iobase.h"
#include "io/iomanager.h"
#include "io/periphparammodel.h"
#include "io/registermapmodel.h"
#include "processorhandler.h"

namespace Ripes {

IOPeripheralTab::IOPeripheralTab(QWidget *parent, IOBase *peripheral)
    : QWidget(parent), m_ui(new Ui::IOPeripheralTab), m_peripheral(peripheral) {
  m_ui->setupUi(this);
  m_ui->exports->setFont(QFont(Fonts::monospace, 11));

  m_ui->description->setText(m_peripheral->description());
  m_ui->registerMapView->setModel(new RegisterMapModel(peripheral, this));
  m_ui->registerMapView->horizontalHeader()->setStretchLastSection(true);
  m_ui->registerMapView->resizeColumnsToContents();

  if (peripheral->parameters().size() == 0) {
    m_ui->parameterGroupBox->hide();
  } else {
    m_ui->parameterView->setModel(new PeriphParamModel(peripheral, this));
    m_ui->parameterView->horizontalHeader()->setStretchLastSection(true);
    m_ui->parameterView->resizeColumnsToContents();
    m_ui->parameterView->setItemDelegateForColumn(
        PeriphParamModel::Value, new PeriphParamDelegate(this));
  }

  // Disable parameter modification during processor running
  connect(ProcessorHandler::get(), &ProcessorHandler::runStarted, this,
          [=] { m_ui->parameterView->setEnabled(false); });
  connect(ProcessorHandler::get(), &ProcessorHandler::runFinished, this,
          [=] { m_ui->parameterView->setEnabled(true); });

  updateExportsInfo();

  connect(peripheral, &IOBase::paramsChanged, this,
          &IOPeripheralTab::updateExportsInfo);

  //rufi
  //connect(peripheral, &IOBase::paramsChanged, this, [=]{
        // &IOPeripheralTab::updateExportsInfo;
        //const auto &paramsiotab = peripheral->parameters();
        // qDebug() << "just for breakpoint";
        // });

  //rufi
  //Each time a peripheral is set up, the code passes form here before launching the GUI.
  //So if there are 4 peripherals it stops here 4 times in a row.
  //I think here you get only default values of params and regs
  /*auto symbols = IOManager::get().assemblerSymbolsForPeriph(m_peripheral);
  const auto &reg = m_peripheral->registers(); //you get the list of registers (like hundreds of leds values)
  const auto &params = m_peripheral->parameters(); //you get height, width, size or other parameters proper of each peripheral*/
}

void IOPeripheralTab::updateExportsInfo() {
  m_ui->exports->clear();
  auto symbols = IOManager::get().assemblerSymbolsForPeriph(m_peripheral);
  for (const auto &symbol : symbols) {
    m_ui->exports->appendPlainText("#define " + symbol.first.v + " " + "(0x" +
                                   QString::number(symbol.second, 16) + ")");
  }
  CSyntaxHighlighter(m_ui->exports->document()).rehighlight();
}

IOPeripheralTab::~IOPeripheralTab() { delete m_ui; }

} // namespace Ripes
