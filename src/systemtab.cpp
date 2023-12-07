#include "systemtab.h"
#include "hwdescription.h"
#include "processorhandler.h"
#include "ui_systemtab.h"

// #include <QGraphicsItem>
// #include <QPushButton>
// #include <QToolBar>
// #include <QVBoxLayout>
// #include <QVector>
// #include <iostream>
// #include <QBrush>
// #include <QGraphicsLineItem>
// #include <QGraphicsTextItem>
// #include <QPen>
// #include <QPropertyAnimation>
// #include <QToolTip>
#include <QWheelEvent>

namespace Ripes {

SystemTab::SystemTab(QToolBar *toolbar, QWidget *parent)
    : RipesTab(toolbar, parent), m_ui(new Ui::SystemTab) {

  m_ui->setupUi(this);

  const Ripes::ProcessorID &processorID = Ripes::ProcessorHandler::getID();

  // Set initial dimensions of the splitter
  int width = this->width(); // Width of the tab window
  m_ui->splitter->setSizes(
      {static_cast<int>(width * 0.8), static_cast<int>(width * 0.2)});

  // Get the current active peripherals
  auto activePeripherals = Ripes::IOManager::get().getPeripherals();
  int nbActivePeripherals = activePeripherals.size();

  // Create the three main, fixed blocks
  // MAYBE CREATE A FUNCTION LIKE SETBLOCK (usable only for these 3 because
  // their pointers already exists, while for the peripherals I cannot tell how
  // many pointers I will need)
  cpuRect = new SystemBlock(
      CPU, processorIDToQString(processorID).toStdString(), startSceneX,
      startSceneY, baseBlockDimension, baseBlockDimension);
  memoryRect =
      new SystemBlock(MEMORY, "Memory",
                      startSceneX + baseSpaceBetweenBlocks + baseBlockDimension,
                      startSceneY, baseBlockDimension, baseBlockDimension);
  busRect =
      new SystemBlock(BUS, "Bus", startSceneX - baseSpaceBetweenBlocks,
                      startSceneY + baseBlockDimension + baseDistanceFromBus,
                      busLength(nbActivePeripherals), baseBusHeight);

  // Create a list of the blocks in the scheme
  blocksVector = {};
  blocksVector.append(cpuRect);
  blocksVector.append(memoryRect);
  blocksVector.append(busRect);

  // Add the blocks of the peripherals, according to the currently active ones
  int spaceOffset = 0;
  for (const Ripes::IOBase *periph : activePeripherals) {
    blocksVector.append(
        new SystemBlock(PERIPHERAL,
                        periph->baseName().toStdString() + " " +
                            std::to_string(periph->getm_id()),
                        startSceneX + baseBlockDimension * spaceOffset +
                            baseSpaceBetweenBlocks * spaceOffset,
                        startSceneY + baseBlockDimension +
                            baseDistanceFromBus * 2 + baseBusHeight,
                        baseBlockDimension, baseBlockDimension));
    spaceOffset++;
  }

  // Create the scene and everything that you want to see in it (blocks, labels
  // and later the arrows) Add the name of each block to the list Add an arrow
  // between each block and the bus
  scene = new QGraphicsScene(this);
  for (const auto &element : blocksVector) {
    scene->addItem(element);
    scene->addItem(&element->getLabel());
    setItemStyles(element, Qt::gray);
    m_ui->myListWidget->addItem(element->getQName());
    if (element->getBlockType() != BUS) {
      connectItemsWithArrow(element, busRect, Qt::red);
    }
  }

  // Create view with the scene, add it to the grid layout and change some
  // settings
  view = new SystemTabView(this);
  view->setScene(scene);
  m_ui->myGridLayout->addWidget(view);
  view->setRenderHint(QPainter::Antialiasing, true);
  view->setRenderHint(QPainter::SmoothPixmapTransform, true);
  view->setDragMode(QGraphicsView::ScrollHandDrag);
  view->setInteractive(true);

  // Do the connection to handle the click on an item of the list and the change
  // of the memory map
  connect(m_ui->myListWidget, &QListWidget::itemClicked, this,
          &SystemTab::onListItemClicked);
  connect(&IOManager::get(), &IOManager::memoryMapChanged, this,
          [=]() { updateSystemTab(); });
}

// these are the items created with "new": you have to delete them when
// deconstructing the Tab. Each Tab is created with "new" in the mainwindow.cpp,
// so somewhere (either it is automatic or it is done in the deconstructor of
// the FancyTabBar) "delete SystemTab" will be called. I HAVE ADDED THE FIRST
// ONES, NOT m_ui!!
SystemTab::~SystemTab() {
  delete scene;
  delete view;
  /*delete cpuRect;
  delete memoryRect;
  delete busRect;*/ //if I uncomment: C:\Users\margh\OneDrive\Desktop\build-RipesTest2-Desktop_Qt_6_5_3_MSVC2019_64bit-Debug\Ripes.exe crashed. Why?
  delete m_ui;
}

void SystemTab::onListItemClicked(QListWidgetItem *item) {
  // When an item of the list is clicked, the corresponding block is selected
  QString itemName = item->text();
  for (const auto &element : blocksVector) {
    if (itemName == element->getQName()) {
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

void SystemTab::updateSystemTab() {
  // must re-call it every time to get the new peripherals (it is a reference,
  // not a pointer)
  auto activePeripherals = Ripes::IOManager::get().getPeripherals();
  int nbActivePeripherals = activePeripherals.size();
  // After the following function all the peripheral blocks are removed from the
  // blocksVector and from the scene The non-peripheral blocks are just updated,
  // not removed from the vector
  deletePeripheralBlocks(nbActivePeripherals);
  // If there is any peripheral, add all of them in the blocksVector
  if (nbActivePeripherals > 0) {
    addNewPeripherals(activePeripherals);
  }
}

void SystemTab::deletePeripheralBlocks(int nbActivePeripherals) {
  // must re-call it every time to get the new ID (it is a reference, not a
  // pointer)
  const Ripes::ProcessorID &processorID = Ripes::ProcessorHandler::getID();
  m_ui->myListWidget->clear();

  // Delete a block from the blocksVector only if the lambda function return
  // "true"
  blocksVector.erase(
      std::remove_if(
          blocksVector.begin(), blocksVector.end(),
          [&](SystemBlock *element) {
            if (element->getBlockType() == Ripes::CPU) {
              QString NewName = processorIDToQString(processorID);
              element->setName(NewName.toStdString());
              element->updateLabel(NewName);
              m_ui->myListWidget->addItem(element->getQName());
              return false;
            } else if (element->getBlockType() == Ripes::BUS) {
              element->setRect(startSceneX - baseSpaceBetweenBlocks,
                               startSceneY + baseBlockDimension +
                                   baseDistanceFromBus,
                               busLength(nbActivePeripherals), baseBusHeight);
              m_ui->myListWidget->addItem(element->getQName());
              return false;
            } else if (element->getBlockType() == Ripes::MEMORY) {
              m_ui->myListWidget->addItem(element->getQName());
              return false;
            } else if (element->getBlockType() == Ripes::PERIPHERAL) {
              scene->removeItem(element);
              delete element; // free memory. It is needed since I have
                              // created the element using "new". Arrow wasn't,
                              // for example.
              return true;
            }
            return false;
          }),
      blocksVector.end());
}

void SystemTab::addNewPeripherals(std::set<IOBase *> &activePeripherals) {
  int spaceOffset = 0;
  for (const Ripes::IOBase *periph : activePeripherals) {
    SystemBlock *newBlock =
        new SystemBlock(PERIPHERAL,
                        periph->baseName().toStdString() + " " +
                            std::to_string(periph->getm_id()),
                        startSceneX + baseBlockDimension * spaceOffset +
                            baseSpaceBetweenBlocks * spaceOffset,
                        startSceneY + baseBlockDimension +
                            baseDistanceFromBus * 2 + baseBusHeight,
                        baseBlockDimension,
                        baseBlockDimension); // maybe put this expression and
                                             // the one of bus in a variable
    blocksVector.append(newBlock);
    spaceOffset++;
    scene->addItem(newBlock);
    scene->addItem(&newBlock->getLabel());
    setItemStyles(newBlock, Qt::gray);
    connectItemsWithArrow(newBlock, busRect, Qt::red);
    m_ui->myListWidget->addItem(newBlock->getQName());
  }
}

int SystemTab::busLength(int nbActivePeripherals) {
  // default value: bus for cpu and memory only
  int busLengthValue = (baseBlockDimension + baseSpaceBetweenBlocks) * 2 +
                       baseSpaceBetweenBlocks;
  if (nbActivePeripherals > 2) {
    busLengthValue =
        (baseBlockDimension + baseSpaceBetweenBlocks) * nbActivePeripherals +
        baseSpaceBetweenBlocks;
  }
  return busLengthValue;
}

void SystemTab::connectItemsWithArrow(SystemBlock *startItem,
                                      SystemBlock *endItem,
                                      const QColor &color) {
  // Two cases: peripheral arrows arrive below the bus, the others above
  if (startItem->getBlockType() == PERIPHERAL) {
    startItem->getArrow().getArrowLine().setLine(
        startItem->sceneBoundingRect().center().x(),
        startItem->sceneBoundingRect().y() - baseDistanceFromBus / 4,
        startItem->sceneBoundingRect().center().x(),
        endItem->sceneBoundingRect().y() +
            endItem->sceneBoundingRect().height() + baseDistanceFromBus / 4);
  } else {
    startItem->getArrow().getArrowLine().setLine(
        startItem->sceneBoundingRect().center().x(),
        startItem->sceneBoundingRect().height() +
            startItem->sceneBoundingRect().y() + baseDistanceFromBus / 4,
        startItem->sceneBoundingRect().center().x(),
        endItem->sceneBoundingRect().y() - baseDistanceFromBus / 4);
  }
  // Set style of the arrow
  startItem->getArrow().getArrowLine().setPen(QPen(color, arrowThickness));

  // Add arrow to the scene
  scene->addItem(&startItem->getArrow().getArrowLine());

  // Tooltip for arrow line
  startItem->getArrow().getArrowLine().setToolTip("Wishbone bus");

  // Add tips to the arrow
  int reverse = 1;
  if (startItem->getBlockType() != PERIPHERAL) {
    reverse = -1;
  }
  // Points of the base of the triangle tip1
  QPointF arrowP1Up = startItem->getArrow().getArrowLine().line().p2() +
                      QPointF(arrowThickness, 0);
  QPointF arrowP2Up = startItem->getArrow().getArrowLine().line().p2() -
                      QPointF(arrowThickness, 0);
  // Points of the base of the triangle tip2
  QPointF arrowP1Down = startItem->getArrow().getArrowLine().line().p1() +
                        QPointF(arrowThickness, 0);
  QPointF arrowP2Down = startItem->getArrow().getArrowLine().line().p1() -
                        QPointF(arrowThickness, 0);

  QPolygonF arrowHeadUp, arrowHeadDown;
  arrowHeadUp.clear();
  arrowHeadDown.clear();
  arrowHeadUp << startItem->getArrow().getArrowLine().line().p2() -
                     QPointF(0, baseDistanceFromBus / 4) * reverse
              << arrowP1Up << arrowP2Up;
  arrowHeadDown << startItem->getArrow().getArrowLine().line().p1() +
                       QPointF(0, baseDistanceFromBus / 4) * reverse
                << arrowP1Down << arrowP2Down;

  // Now use the temporary, scope-limited points/polygons just defined to update
  // the already existing ItemUp and ItemDown (they were created and
  // default-initialized when the block was created and so the m_arrow attribute
  // was created) This avoids:
  // 1. Using "new"
  // 2. Using QPolygonItem ItemUp(arrowHeadUp) that is scope-limited, so will
  // desappear from the scene when the final "}" of this function arrives
  startItem->getArrow().getArrowHeadItemUp().setPolygon(arrowHeadUp);
  startItem->getArrow().getArrowHeadItemDown().setPolygon(arrowHeadDown);
  // Set the color of the tip items
  startItem->getArrow().getArrowHeadItemUp().setBrush(QBrush(Qt::red));
  startItem->getArrow().getArrowHeadItemDown().setBrush(QBrush(Qt::red));

  // Add the QGraphicsPolygonItem to the scene
  scene->addItem(&startItem->getArrow().getArrowHeadItemUp());
  scene->addItem(&startItem->getArrow().getArrowHeadItemDown());
}

/*******SystemTabView********/
SystemTabView::SystemTabView(QWidget *parent) : QGraphicsView(parent) {
  // Constructor needs to be declared in .cpp otherwise link error (external
  // symbol)
}

void SystemTabView::zoomIn() { scale(1.2, 1.2); }

void SystemTabView::zoomOut() { scale(1.0 / 1.2, 1.0 / 1.2); }

void SystemTabView::wheelEvent(
    QWheelEvent
        *event) { // automatically called when wheel is moved (QGraphicsView)
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

SystemBlock::SystemBlock(BlockType type, std::string name, qreal x, qreal y,
                         qreal width, qreal height, QGraphicsItem *parent)
    : QGraphicsRectItem(x, y, width, height, parent), m_type(type),
      m_name(name) {

  // For each instantiated rectangle, set the value of and place the m_lable
  m_label.setPlainText(QString::fromStdString(name));
  labelPosition =
      rect().center() - QPointF(m_label.boundingRect().width() / 2,
                                m_label.boundingRect().height() / 2);
  m_label.setPos(labelPosition);
}

void SystemBlock::updateLabel(const QString &newName) {
  m_label.setPlainText(newName);
  labelPosition =
      rect().center() - QPointF(m_label.boundingRect().width() / 2,
                                m_label.boundingRect().height() / 2);
  m_label.setPos(labelPosition);
}

} // namespace Ripes
