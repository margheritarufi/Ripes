//@file systemtab.cpp
//@author Margherita Rufi
//@version 1.0 2023-12-08
//@brief This file contains the implementation of the SystemTab class, the
//SystemTabView class the SystemBlock class and the SystemArrow class.

#include "systemtab.h"
#include "hwdescription.h"
#include "processorhandler.h"
#include "ui_systemtab.h"
#include <QWheelEvent>

namespace Ripes {

/*******SystemTab********/

// @brief: constructor of the SystemTab class
// This constructor:
// 1. Adjusts settings of the Tab
// 2. Creates the three main blocks (cpu, main memory and bus)
// 3. Creates the peripherals blocks
// 4. Creates the scene and the view
// 5. Fills the list widget
//
// @param: QToolBar *toolbar, QWidget *parent
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

  // Set tooltip for memory block with the address range
  memoryRect->setToolTip("Address range: x00000000 - xefffffff");

  // Create a list of the blocks in the scheme and add the three main ones
  blocksVector = {};
  blocksVector.append(cpuRect);
  blocksVector.append(memoryRect);
  blocksVector.append(busRect);

  // Add the active peripherals to the list of blocks
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
                        baseBlockDimension, baseBlockDimension);
    blocksVector.append(newBlock);

    // Set tooltip for each peripheral block with the address range
    const Ripes::MemoryMap *myMemMap = &Ripes::IOManager::get().memoryMap();
    for (const auto &entry : *myMemMap) {
      if (entry.second.name == newBlock->getQName()) {
        newBlock->setToolTip(
            "Address range: x" + QString::number(entry.second.startAddr, 16) +
            " - x" +
            QString::number(entry.second.startAddr + entry.second.size - 1,
                            16));
        break;
      }
    }
    spaceOffset++;
  }

  // Create the scene and everything that you want to see in it (blocks, labels
  // and later the arrows). Add the name of each block to the list widget. Add
  // an arrow between each block and the bus (two for the CPU).
  scene = new QGraphicsScene(this);
  for (const auto &element : blocksVector) {
    scene->addItem(element);
    scene->addItem(&element->getLabel());
    setItemStyles(element, Qt::gray);
    m_ui->myListWidget->addItem(element->getQName());
    if (element->getBlockType() != BUS) {
      connectItemsWithArrow(element, busRect, Qt::red);
      if (element->getBlockType() == CPU) {
        connectItemsWithSecondArrow(element, busRect, Qt::red);
      }
    }
  }

  // Create the view with the scene, add it to the grid layout and change some
  // settings
  view = new SystemTabView(this);
  view->setScene(scene);
  m_ui->myGridLayout->addWidget(view);
  view->setRenderHint(QPainter::Antialiasing, true);
  view->setRenderHint(QPainter::SmoothPixmapTransform, true);
  view->setDragMode(QGraphicsView::ScrollHandDrag);
  view->setInteractive(true);

  // Connection to handle the click on an item of the list widget
  connect(m_ui->myListWidget, &QListWidget::itemClicked, this,
          &SystemTab::onListItemClicked);
  // Connection to handle the change of the memory map
  connect(&IOManager::get(), &IOManager::memoryMapChanged, this,
          [=]() { updateSystemTab(); });
}

//@brief: destructor of the SystemTab class
SystemTab::~SystemTab() {
  delete scene;
  delete view;
  /*delete cpuRect;
  delete memoryRect;
  delete busRect;*/ //if I uncomment: \build-RipesTest2-Desktop_Qt_6_5_3_MSVC2019_64bit-Debug\Ripes.exe crashed. Why?
  delete m_ui;
}

//@brief: onListItemClicked
// This function is called when an item of the list widget is clicked. It
// selects the corresponding block in the scene.
//
// @param: QListWidgetItem *item with the item of the list widget that has been
// clicked
// @return: void
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

//@brief: setItemStyles
// This function sets the color and the style of the blocks.
//
// @param: SystemBlock *item with the block to be styled,
// @param: const QColor &color with the color of the block
// @return: void
void SystemTab::setItemStyles(SystemBlock *item, const QColor &color) {
  // Set colors and styles for items
  item->setBrush(QBrush(color));
  item->setPen(QPen(Qt::black));
  item->setFlag(QGraphicsItem::ItemIsSelectable, true);
  item->setAcceptHoverEvents(true);
}

//@brief: updateSystemTab
// This function is called when the memory map is changed. It updates the
// scheme.
//
// @param: void
// @return: void
void SystemTab::updateSystemTab() {
  auto activePeripherals = Ripes::IOManager::get().getPeripherals();
  int nbActivePeripherals = activePeripherals.size();
  // After the following function all the peripheral blocks are removed from the
  // blocksVector and from the scene. The non-peripheral blocks are just
  // updated, not removed from the vector.
  deletePeripheralBlocks(nbActivePeripherals);
  // If there is any active peripheral, add all it in the blocksVector
  if (nbActivePeripherals > 0) {
    addNewPeripherals(activePeripherals);
  }
}

//@brief: deletePeripheralBlocks
// This function deletes the peripheral blocks from the blocksVector and from
// the scene. The non-peripheral blocks are just updated, not removed from the
// vector.
//
// @param: int nbActivePeripherals with the number of active peripherals
// @return: void
void SystemTab::deletePeripheralBlocks(int nbActivePeripherals) {
  const Ripes::ProcessorID &processorID = Ripes::ProcessorHandler::getID();
  m_ui->myListWidget->clear();

  // Delete a block from the blocksVector only if the lambda function returns
  // "true" (i.d. if the block is a peripheral)
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
              delete element; // free memory. It is needed since the element was
                              // created using "new".
              return true;
            }
            return false;
          }),
      blocksVector.end());
}

//@brief: addNewPeripherals
// This function adds the new peripheral blocks to the blocksVector and to the
// scene.
//
// @param: std::set<IOBase *> &activePeripherals with the set of active
// peripherals
// @return: void
void SystemTab::addNewPeripherals(std::set<IOBase *> &activePeripherals) {
  int spaceOffset = 0;
  for (const Ripes::IOBase *periph : activePeripherals) {
    // Create peripheral block
    SystemBlock *newBlock =
        new SystemBlock(PERIPHERAL,
                        periph->baseName().toStdString() + " " +
                            std::to_string(periph->getm_id()),
                        startSceneX + baseBlockDimension * spaceOffset +
                            baseSpaceBetweenBlocks * spaceOffset,
                        startSceneY + baseBlockDimension +
                            baseDistanceFromBus * 2 + baseBusHeight,
                        baseBlockDimension, baseBlockDimension);
    blocksVector.append(newBlock);
    spaceOffset++;

    // Set tooltip for each peripheral block with the address range
    const Ripes::MemoryMap *myMemMap = &Ripes::IOManager::get().memoryMap();
    for (const auto &entry : *myMemMap) {
      if (entry.second.name == newBlock->getQName()) {
        newBlock->setToolTip(
            "Address range: x" + QString::number(entry.second.startAddr, 16) +
            " - x" +
            QString::number(entry.second.startAddr + entry.second.size - 1,
                            16));
        break;
      }
    }

    // Add the new block to the scene and to the list widget
    scene->addItem(newBlock);
    scene->addItem(&newBlock->getLabel());
    setItemStyles(newBlock, Qt::gray);
    connectItemsWithArrow(newBlock, busRect, Qt::red);
    m_ui->myListWidget->addItem(newBlock->getQName());
  }
}

//@brief: busLength
// This function calculates the length of the bus.
//
// @param: int nbActivePeripherals with the number of active peripherals
// @return: int busLengthValue with the length of the bus
int SystemTab::busLength(int nbActivePeripherals) {
  // Default value: bus for cpu and memory only
  int busLengthValue = (baseBlockDimension + baseSpaceBetweenBlocks) * 2 +
                       baseSpaceBetweenBlocks;
  if (nbActivePeripherals > 2) {
    busLengthValue =
        (baseBlockDimension + baseSpaceBetweenBlocks) * nbActivePeripherals +
        baseSpaceBetweenBlocks;
  }
  return busLengthValue;
}

//@brief: connectItemsWithArrow
// This function connects two blocks with an arrow.
//
// @param: SystemBlock *startItem with the block from which the arrow starts,
// @param: SystemBlock *endItem with the block in which the arrow ends,
// @param: const QColor &color with the color of the arrow
// @return: void
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

  // Add tooltip for arrow line
  if (startItem->getBlockType() == CPU) {
    startItem->getArrow().getArrowLine().setToolTip(
        "Wishbone bus signals for instructions");
  } else {
    startItem->getArrow().getArrowLine().setToolTip("Wishbone bus signals");
  }

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

  // Create triangles for the tips of the arrow
  QPolygonF arrowHeadUp, arrowHeadDown;
  arrowHeadUp.clear();
  arrowHeadDown.clear();
  arrowHeadUp << startItem->getArrow().getArrowLine().line().p2() -
                     QPointF(0, baseDistanceFromBus / 4) * reverse
              << arrowP1Up << arrowP2Up;
  arrowHeadDown << startItem->getArrow().getArrowLine().line().p1() +
                       QPointF(0, baseDistanceFromBus / 4) * reverse
                << arrowP1Down << arrowP2Down;

  // Set the arrow tips ITEMS with the triangles created before
  startItem->getArrow().getArrowHeadItemUp().setPolygon(arrowHeadUp);
  startItem->getArrow().getArrowHeadItemDown().setPolygon(arrowHeadDown);
  startItem->getArrow().getArrowHeadItemUp().setBrush(QBrush(Qt::red));
  startItem->getArrow().getArrowHeadItemDown().setBrush(QBrush(Qt::red));

  // Add the QGraphicsPolygonItem to the scene
  scene->addItem(&startItem->getArrow().getArrowHeadItemUp());
  scene->addItem(&startItem->getArrow().getArrowHeadItemDown());
}

//@brief: connectItemsWithSecondArrow
// This function connects two blocks with a second arrow.
//
// @param: SystemBlock *startItem with the block from which the arrow starts,
// @param: SystemBlock *endItem with the block in which the arrow ends,
// @param: const QColor &color with the color of the arrow
// @return: void
void SystemTab::connectItemsWithSecondArrow(SystemBlock *startItem,
                                            SystemBlock *endItem,
                                            const QColor &color) {
  // Two cases: peripheral arrows arrive below the bus, the others above
  if (startItem->getBlockType() == PERIPHERAL) {
    startItem->getSecondArrow().getArrowLine().setLine(
        startItem->sceneBoundingRect().center().x() + baseSpaceBetweenBlocks,
        startItem->sceneBoundingRect().y() - baseDistanceFromBus / 4,
        startItem->sceneBoundingRect().center().x() + baseSpaceBetweenBlocks,
        endItem->sceneBoundingRect().y() +
            endItem->sceneBoundingRect().height() + baseDistanceFromBus / 4);
  } else {
    startItem->getSecondArrow().getArrowLine().setLine(
        startItem->sceneBoundingRect().center().x() + baseSpaceBetweenBlocks,
        startItem->sceneBoundingRect().height() +
            startItem->sceneBoundingRect().y() + baseDistanceFromBus / 4,
        startItem->sceneBoundingRect().center().x() + baseSpaceBetweenBlocks,
        endItem->sceneBoundingRect().y() - baseDistanceFromBus / 4);
  }
  // Set style of the arrow
  startItem->getSecondArrow().getArrowLine().setPen(
      QPen(color, arrowThickness));

  // Add arrow to the scene
  scene->addItem(&startItem->getSecondArrow().getArrowLine());

  // Tooltip for arrow line
  if (startItem->getBlockType() == CPU) {
    startItem->getSecondArrow().getArrowLine().setToolTip(
        "Wishbone bus signals for data");
  } else {
    startItem->getSecondArrow().getArrowLine().setToolTip(
        "Wishbone signals bus");
  }

  int reverse = 1;
  if (startItem->getBlockType() != PERIPHERAL) {
    reverse = -1;
  }
  // Points of the base of the triangle tip1
  QPointF arrowP1Up = startItem->getSecondArrow().getArrowLine().line().p2() +
                      QPointF(arrowThickness, 0);
  QPointF arrowP2Up = startItem->getSecondArrow().getArrowLine().line().p2() -
                      QPointF(arrowThickness, 0);
  // Points of the base of the triangle tip2
  QPointF arrowP1Down = startItem->getSecondArrow().getArrowLine().line().p1() +
                        QPointF(arrowThickness, 0);
  QPointF arrowP2Down = startItem->getSecondArrow().getArrowLine().line().p1() -
                        QPointF(arrowThickness, 0);

  // Create triangles for the tips of the arrow
  QPolygonF arrowHeadUp, arrowHeadDown;
  arrowHeadUp.clear();
  arrowHeadDown.clear();
  arrowHeadUp << startItem->getSecondArrow().getArrowLine().line().p2() -
                     QPointF(0, baseDistanceFromBus / 4) * reverse
              << arrowP1Up << arrowP2Up;
  arrowHeadDown << startItem->getSecondArrow().getArrowLine().line().p1() +
                       QPointF(0, baseDistanceFromBus / 4) * reverse
                << arrowP1Down << arrowP2Down;

  // Set the arrow tips ITEMS with the triangles created before
  startItem->getSecondArrow().getArrowHeadItemUp().setPolygon(arrowHeadUp);
  startItem->getSecondArrow().getArrowHeadItemDown().setPolygon(arrowHeadDown);
  startItem->getSecondArrow().getArrowHeadItemUp().setBrush(QBrush(Qt::red));
  startItem->getSecondArrow().getArrowHeadItemDown().setBrush(QBrush(Qt::red));

  // Add the QGraphicsPolygonItem to the scene
  scene->addItem(&startItem->getSecondArrow().getArrowHeadItemUp());
  scene->addItem(&startItem->getSecondArrow().getArrowHeadItemDown());
}

/*******SystemTabView********/

// @brief: constructor of the SystemTabView class
//
// @param: QWidget *parent
SystemTabView::SystemTabView(QWidget *parent) : QGraphicsView(parent) {}

//@brief: zoomIn
// This function zooms in the scene.
void SystemTabView::zoomIn() { scale(1.2, 1.2); }

//@brief: zoomOut
// This function zooms out the scene.
void SystemTabView::zoomOut() { scale(1.0 / 1.2, 1.0 / 1.2); }

//@brief: wheelEvent
// This function is called when the mouse wheel is moved. It zooms in or out
// depending on the direction of the movement. It is automatically called when
// wheel is moved (QGraphicsView)
//
// @param: QWheelEvent *event
// @return: void
void SystemTabView::wheelEvent(QWheelEvent *event) {
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

/*******SystemBlock********/

// @brief: constructor of the SystemBlock class
// This constructor sets the value of and places the m_lable for each
// instantiated rectangle.
//
// @param: BlockType type with the type of the block,
// @param: std::string name with the name of the block,
// @param: qreal x with the x coordinate of the block,
// @param: qreal y with the y coordinate of the block,
// @param: qreal width with the width of the block,
// @param: qreal height with the height of the block,
// @param: QGraphicsItem *parent
SystemBlock::SystemBlock(BlockType type, std::string name, qreal x, qreal y,
                         qreal width, qreal height, QGraphicsItem *parent)
    : QGraphicsRectItem(x, y, width, height, parent), m_type(type),
      m_name(name) {

  m_label.setPlainText(QString::fromStdString(name));
  labelPosition =
      rect().center() - QPointF(m_label.boundingRect().width() / 2,
                                m_label.boundingRect().height() / 2);
  m_label.setPos(labelPosition);
}

//@brief: updateLabel
// This function updates the label of the block.
//
// @param: const QString &newName with the new name of the block
// @return: void
void SystemBlock::updateLabel(const QString &newName) {
  m_label.setPlainText(newName);
  labelPosition =
      rect().center() - QPointF(m_label.boundingRect().width() / 2,
                                m_label.boundingRect().height() / 2);
  m_label.setPos(labelPosition);
}

} // namespace Ripes
