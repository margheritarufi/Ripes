#include "systemtab.h"
#include "ui_systemtab.h"
//#include "iomanager.h" in .h
#include "processorhandler.h"
#include "hwdescription.h"

#include <QGraphicsItem>
#include <QPushButton>
#include <QToolBar>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <QVBoxLayout>
#include <iostream>
#include <QListWidgetItem>
#include <QVector>




#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QPen>
#include <QBrush>
#include <QPropertyAnimation>
#include <QToolTip>
#include <QWheelEvent>
//


namespace Ripes {

//QVector<SystemBlock*> blocksVector; // = {};
// Get the list of all active peripherals
//auto activePeripherals = Ripes::IOManager::get().getPeripherals(); //Declaration of reference variable requires initialization
//int nbActivePeripherals;
//auto processorID = Ripes::ProcessorHandler::getID();

SystemBlock::SystemBlock(BlockType type, std::string name, qreal x, qreal y, qreal width,
                         qreal height, QGraphicsItem *parent)
    : QGraphicsRectItem(x, y, width, height, parent), m_type(type), m_name(name) {

  //For each instantiated rectangle, create and place a lable
  m_label = new QGraphicsTextItem(QString::fromStdString(name), this);
  m_label->setPos(x, y);
  cpuLabelPosition = rect().center() - QPointF(m_label->boundingRect().width() / 2, m_label->boundingRect().height() / 2);
  m_label->setPos(cpuLabelPosition);
}

void SystemBlock::updateLabel(const QString &newName) {
  m_label->setPlainText(newName);
}

SystemTab::SystemTab(QToolBar *toolbar, QWidget *parent)
    : RipesTab(toolbar, parent), m_ui(new Ui::SystemTab) {

  m_ui->setupUi(this);

  const Ripes::ProcessorID &processorID = Ripes::ProcessorHandler::getID();

  // Set initial dimensions of the splitter
  int width = this->width();  // Width of the tab window
  m_ui->splitter->setSizes({static_cast<int>(width * 0.8), static_cast<int>(width * 0.2)});

  //Get the current active peripherals
  auto activePeripherals = Ripes::IOManager::get().getPeripherals(); //it's a set: not ordered, not accessible by indexes
  int nbActivePeripherals = activePeripherals.size();

  //Create the three main, fixed blocks
  cpuRect = new SystemBlock(CPU, processorIDToQString(processorID).toStdString(), startSceneX, startSceneY, baseBlockDimension, baseBlockDimension);
  memoryRect = new SystemBlock(MEMORY, "Memory", startSceneX + baseSpaceBetweenBlocks + baseBlockDimension, startSceneY, baseBlockDimension, baseBlockDimension);
  busRect = new SystemBlock(BUS, "Bus", startSceneX - baseSpaceBetweenBlocks, startSceneY + baseBlockDimension + baseDistanceFromBus, (baseBlockDimension + baseSpaceBetweenBlocks) * nbActivePeripherals + baseSpaceBetweenBlocks, baseBusHeight);

  //Create a list of the blocks in the scheme
  blocksVector ={};
  blocksVector.append(cpuRect); // Adjust the dimensions as needed
  blocksVector.append(memoryRect); // Adjust the dimensions as needed
  blocksVector.append(busRect); // Adjust the dimensions as needed

         // For each active peripheral, check its base name and increment the
         // corresponding counter
  /*for (int i=0; i<nbActivePeripherals; i++) {
    std::set<IOBase*>::iterator it = activePeripherals.begin();
    blocksVector.append(new QGraphicsRectItem(*it->baseName(), 0+50*i+15*i, 125, 50, 50));
    ++it; // Avanza all'elemento successivo
  }*/

  // Add the blocks of the peripherals, according to the ones that are currently active
  int spaceOffset = 0;
  for(const Ripes::IOBase *periph : activePeripherals){
    blocksVector.append(new SystemBlock(PERIPHERAL, periph->baseName().toStdString() + " " + std::to_string(periph->getm_id()), startSceneX+baseBlockDimension*spaceOffset+baseSpaceBetweenBlocks*spaceOffset, startSceneY + baseBlockDimension + baseDistanceFromBus*2 + baseBusHeight, baseBlockDimension, baseBlockDimension));
    spaceOffset++;
  }

  // Add all the blocks to the scene and the corresponding name to the list
  scene = new QGraphicsScene(this);
  for(const auto& element : blocksVector){
    scene->addItem(element);
    setItemStyles(element, Qt::gray);
    m_ui->myListWidget->addItem(element->getQName());
    if (element->getBlockType() != BUS){
      connectItemsWithArrow(element, busRect, Qt::red);
    }
  }

  // Create view with the scene, add it to the grid layout and change some settings
  view = new SystemTabView(this);
  view->setScene(scene);
  m_ui->myGridLayout->addWidget(view);
  view->setRenderHint(QPainter::Antialiasing, true);
  view->setRenderHint(QPainter::SmoothPixmapTransform, true);
  view->setDragMode(QGraphicsView::ScrollHandDrag);
  view->setInteractive(true);

  //Do the connection to handle the click on an item of the list and the change of the memory map
  connect(m_ui->myListWidget, &QListWidget::itemClicked, this, &SystemTab::onListItemClicked);
  connect(&IOManager::get(), &IOManager::memoryMapChanged, this, &SystemTab::updateSystemTab);
}

SystemTab::~SystemTab() { delete m_ui; }

void SystemTab::onListItemClicked(QListWidgetItem *item) {
  // When an item of the list is clicked, the corresponding block is selected
  QString itemName = item->text();
  for(const auto& element : blocksVector){
    if(itemName == element->getQName()){
      element->setSelected(true);
    } else {
      element->setSelected(false);
    }
  }
}

void SystemTab::setItemStyles(SystemBlock *item, const QColor &color) {
  // Set colors and styles for items
  item->setBrush(QBrush(color));
  item->setPen(QPen(Qt::black));
  item->setFlag(QGraphicsItem::ItemIsSelectable, true);
  item->setAcceptHoverEvents(true);
}

void SystemTab::updateSystemTab(){
  /*QVector<int> indexesToDelete = {};
  int index = 0;


  for(const auto& element : blocksVector){
    if(element->getBlockType() == Ripes::CPU){
      element -> setName(processorIDToQString(thisID).toStdString());
      //qDebug() << element->getQName().toStdString();
    }
    if(element->getBlockType() == Ripes::BUS){
      element -> setRect(-10, 75, 65 * nbActivePeripherals + 20, 25);
      //qDebug() << element->getQName().toStdString();
    }
    if(element->getBlockType() == Ripes::PERIPHERAL){
      //qDebug() << element->getQName().toStdString() << "will be removed removed";
      //blocksVector.removeOne(element);
      indexesToDelete.append(index);
      //delete element; //free memory
    }
    index++;
  }

  //from the last to the first element of indexestodelete because they are in increasing order and will make deletion crash
  for(int i=indexesToDelete.size()-1; i>=0; i--){
    scene->removeItem(blocksVector[indexesToDelete[i]]);
    delete blocksVector[indexesToDelete[i]];
    blocksVector.remove(indexesToDelete[i]);
  }*/

  auto activePeripherals = Ripes::IOManager::get().getPeripherals(); //must re-call it every time to get the new peripherals (it is a reference, not a pointer)
  int nbActivePeripherals = activePeripherals.size();
  deletePeripheralBlocks(nbActivePeripherals); //after this function all the blocks are removed from the blocksVector and from the scene
  if(nbActivePeripherals > 0){
    addNewPeripherals(activePeripherals);
  }


  /*if (scene) {
    //scene->clear(); this will somehow invalidate the rects too...
    for(const auto& block : blocksVector){
      //Add only the peripherals because the cpu, memory and bus are already (and always) there.
      if(block->getBlockType() == Ripes::PERIPHERAL){
        scene->addItem(block);
        setItemStyles(block, Qt::gray);
        m_ui->myListWidget->addItem(block->getQName());
      }
    }
  } else {
    std::cerr << "Error: scene is null." << std::endl;
  }*/
}

void SystemTab::deletePeripheralBlocks(int nbActivePeripherals) {  //DELETE ALSO ARROW!! Maybe include it in the SystemBlock class?
  const Ripes::ProcessorID &processorID = Ripes::ProcessorHandler::getID(); //must re-call it every time to get the new ID (it is a reference, not a pointer)
  m_ui->myListWidget->clear();

  blocksVector.erase(std::remove_if(blocksVector.begin(), blocksVector.end(),
    [&](SystemBlock* element) {
      if (element->getBlockType() == Ripes::CPU) {
        QString NewName = processorIDToQString(processorID);
        element->setName(NewName.toStdString());
        element->updateLabel(NewName);
        m_ui->myListWidget->addItem(element->getQName());
        return false;
      } else if (element->getBlockType() == Ripes::BUS) {
        element->setRect(startSceneX - baseSpaceBetweenBlocks, startSceneY + baseBlockDimension + baseDistanceFromBus, (baseBlockDimension + baseSpaceBetweenBlocks) * nbActivePeripherals + baseSpaceBetweenBlocks, baseBusHeight);
        m_ui->myListWidget->addItem(element->getQName());
        return false;
      } else if (element->getBlockType() == Ripes::MEMORY) {
        m_ui->myListWidget->addItem(element->getQName());
        return false;
      } else if (element->getBlockType() == Ripes::PERIPHERAL) {
        scene->removeItem(element);
        delete element; // free memory (optional)
        return true;
      }
      return false;
      }),
      blocksVector.end());
}

void SystemTab::addNewPeripherals(std::set<IOBase*>& activePeripherals){
  int spaceOffset = 0;
  for(const Ripes::IOBase *periph : activePeripherals){
    SystemBlock* newBlock = new SystemBlock(PERIPHERAL, periph->baseName().toStdString() + " " + std::to_string(periph->getm_id()), startSceneX+baseBlockDimension*spaceOffset+baseSpaceBetweenBlocks*spaceOffset, startSceneY + baseBlockDimension + baseDistanceFromBus*2 + baseBusHeight, baseBlockDimension, baseBlockDimension); //maybe put this expression and the one of bus in a variable
    blocksVector.append(newBlock);
    spaceOffset++;
    scene->addItem(newBlock);
    setItemStyles(newBlock, Qt::gray);
    connectItemsWithArrow(newBlock, busRect, Qt::red);
    m_ui->myListWidget->addItem(newBlock->getQName());
  }
}

void SystemTab::connectItemsWithArrow(SystemBlock *startItem, SystemBlock *endItem, const QColor &color) {
  // Create arrow line
  QGraphicsLineItem *arrowLine;
  if(startItem->getBlockType() == PERIPHERAL){
    arrowLine = new QGraphicsLineItem(startItem->sceneBoundingRect().center().x(),
                                      startItem->sceneBoundingRect().y() - baseDistanceFromBus/4,
                                      startItem->sceneBoundingRect().center().x(),
                                      endItem->sceneBoundingRect().y() + endItem->sceneBoundingRect().height() + baseDistanceFromBus/4);
  } else {
    arrowLine = new QGraphicsLineItem(startItem->sceneBoundingRect().center().x(),
                                      startItem->sceneBoundingRect().height() + startItem->sceneBoundingRect().y() + baseDistanceFromBus/4,
                                      startItem->sceneBoundingRect().center().x(),
                                      endItem->sceneBoundingRect().y() - baseDistanceFromBus/4);
  }
  arrowLine->setPen(QPen(color, 2));
  // Set pen for the arrow line
  QPen pen(color, 4); // Set the line thickness to 4 (adjust as needed)
  pen.setCapStyle(Qt::FlatCap); // Set round line cap for smoother ends
  arrowLine->setPen(pen);
  scene->addItem(arrowLine);

  // Tooltip for arrow line
  arrowLine->setToolTip("Wishbone bus");

  //Qt Forum
  QPainter* painter = new QPainter();

  painter->setRenderHint(QPainter::Antialiasing, true);

  //qreal arrowSize = 5; // size of head
  painter->setPen(Qt::NoPen);
  painter->setBrush(Qt::red);

  int reverse = 1;
  if(startItem->getBlockType() != PERIPHERAL){
    reverse = -1;
  }

  //double angleUp = std::atan2(arrowLine->line().dy(), -arrowLine->line().dx());
  QPointF arrowP1Up = arrowLine->line().p2() + QPointF(5, 0); //QPointF(sin(angleUp + M_PI / 3) * arrowSize, cos(angleUp + M_PI / 3) * arrowSize * reverse);
  QPointF arrowP2Up = arrowLine->line().p2() - QPointF(5, 0); //+QPointF(sin(angleUp + M_PI - M_PI / 3) * arrowSize, cos(angleUp + M_PI - M_PI / 3) * arrowSize * reverse);


  //double angleDown = std::atan2(-arrowLine->line().dy(), arrowLine->line().dx());
  QPointF arrowP1Down = arrowLine->line().p1() + QPointF(5, 0);//+ QPointF(sin(angleDown + M_PI / 3) * arrowSize, cos(angleDown + M_PI / 3) * arrowSize * reverse);
  QPointF arrowP2Down = arrowLine->line().p1() - QPointF(5, 0); // QPointF(sin(angleDown + M_PI - M_PI / 3) * arrowSize, cos(angleDown + M_PI - M_PI / 3) * arrowSize * reverse);

  QPolygonF arrowHeadUp, arrowHeadDown;
  arrowHeadUp.clear();
  arrowHeadDown.clear();
  arrowHeadUp << arrowLine->line().p2() - QPointF(0, baseDistanceFromBus/4)*reverse << arrowP1Up << arrowP2Up;
  arrowHeadDown << arrowLine->line().p1() + QPointF(0, baseDistanceFromBus/4)*reverse << arrowP1Down << arrowP2Down;
  painter->drawLine(arrowLine->line());
  painter->drawPolygon(arrowHeadUp);
  painter->drawPolygon(arrowHeadDown);

  QGraphicsPolygonItem *arrowHeadItemUp = new QGraphicsPolygonItem(arrowHeadUp);
  QGraphicsPolygonItem *arrowHeadItemDown = new QGraphicsPolygonItem(arrowHeadDown);

  arrowHeadItemUp->setPen(QColorConstants::Red);
  arrowHeadItemUp->setBrush(QBrush(Qt::red));
  arrowHeadItemDown->setPen(QColorConstants::Red);
  arrowHeadItemDown->setBrush(QBrush(Qt::red));

  scene->addItem(arrowHeadItemUp);
  scene->addItem(arrowHeadItemDown);

}

//ADD MEMORY MAP IN THE LIST!


/*******SystemTabView********/
SystemTabView::SystemTabView(QWidget *parent)
    : QGraphicsView(parent) {
  // Constructor needs to be declared in .cpp otherwise link error (external symbol)
}

void SystemTabView::zoomIn() {
  scale(1.2, 1.2);
}

void SystemTabView::zoomOut() {
  scale(1.0 / 1.2, 1.0 / 1.2);
}

void SystemTabView::wheelEvent(QWheelEvent *event) { //automatically called when wheel is moved (QGraphicsView)
  if (event->modifiers() & Qt::ControlModifier) {
    if (event->angleDelta().y() > 0)
      zoomIn();
    else
      zoomOut();
    event->accept();
  } else {
    QGraphicsView::wheelEvent(event);
  }
}

} // namespace Ripes
