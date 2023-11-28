#include "systemtab.h"
#include "ui_systemtab.h"
#include "hwdescription.h"
#include "iomanager.h"

#include <QGraphicsItem>
#include <QPushButton>
#include <QToolBar>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <QVBoxLayout>
#include <iostream>
#include <QListWidgetItem>




#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QPen>
#include <QBrush>
#include <QPropertyAnimation>
#include <QToolTip>
#include <QWheelEvent>
//


namespace Ripes {

SystemTabView::SystemTabView(QWidget *parent)
    : QGraphicsView(parent) {
  // Constructor needs to be declared in .cpp otherwise link error (external symbol)
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

SystemTab::SystemTab(QToolBar *toolbar, QWidget *parent)
    : RipesTab(toolbar, parent), m_ui(new Ui::SystemTab) {
  m_ui->setupUi(this);

  // Imposta le dimensioni iniziali dello splitter (percentuali rispetto alla finestra principale)
  int width = this->width();  // Larghezza della finestra principale
  m_ui->splitter->setSizes({static_cast<int>(width * 0.8), static_cast<int>(width * 0.2)});


  // Crea la scena
  QGraphicsScene *scene = new QGraphicsScene(this);

  // Crea un rettangolo
  cpuItem = new QGraphicsRectItem(0, 0, 50, 50); // Adjust the dimensions as needed
  memoryItem = new QGraphicsRectItem(100, 0, 50, 50); // Adjust the dimensions as needed
  busItem = new QGraphicsRectItem(-10, 75, 170, 25); // Adjust the dimensions as needed
  periphItem = new QGraphicsRectItem(0, 125, 50, 50);

         // Add items to the scene
  scene->addItem(cpuItem);
  scene->addItem(memoryItem);
  scene->addItem(busItem);
  scene->addItem(periphItem);

  cpuItem->setBrush(QBrush(Qt::lightGray));
  memoryItem->setBrush(QBrush(Qt::lightGray));
  periphItem->setBrush(QBrush(Qt::lightGray));
  busItem->setBrush(QBrush(Qt::lightGray));

  QGraphicsTextItem *cpuLabel = new QGraphicsTextItem(processorIDToQString(thisID));
  QGraphicsTextItem *memoryLabel = new QGraphicsTextItem("Memory");
  QGraphicsTextItem *periphLabel = new QGraphicsTextItem("Peripheral");
  QGraphicsTextItem *busLabel = new QGraphicsTextItem("Bus");
  /*QFont font = label->font();
  font.setPointSize(12);  // Imposta la grandezza del testo a 12 punti (puoi personalizzare questo valore)
  label->setFont(font);
  label->setPos(10, 10); // Posizione della label all'interno del rettangolo */

  QPointF cpuLabelPosition = cpuItem->rect().center() - QPointF(cpuLabel->boundingRect().width() / 2, cpuLabel->boundingRect().height() / 2);
  QPointF memoryLabelPosition = memoryItem->rect().center() - QPointF(memoryLabel->boundingRect().width() / 2, memoryLabel->boundingRect().height() / 2);
  QPointF busLabelPosition = busItem->rect().center() - QPointF(busLabel->boundingRect().width() / 2, busLabel->boundingRect().height() / 2);
  QPointF periphLabelPosition = periphItem->rect().center() - QPointF(periphLabel->boundingRect().width() / 2, periphLabel->boundingRect().height() / 2);

  cpuLabel->setPos(cpuLabelPosition);
  memoryLabel->setPos(memoryLabelPosition);
  periphLabel->setPos(periphLabelPosition);
  busLabel->setPos(busLabelPosition);

  scene->addItem(cpuLabel);
  scene->addItem(memoryLabel);
  scene->addItem(periphLabel);
  scene->addItem(busLabel);

  // Crea la view e imposta la scena
  SystemTabView *view = new SystemTabView(this); // Usa la tua classe derivata da QGraphicsView
  view->setScene(scene);
  //view->setSceneRect(scene->sceneRect());

  // Crea un QVBoxLayout e aggiungi la tua vista
  /*setLayout(new QVBoxLayout);
  layout()->addWidget(view);*/
  m_ui->myGridLayout->addWidget(view);
  //m_ui->myGridLayout->addWidget(m_ui->myButton2);

  view->setRenderHint(QPainter::Antialiasing, true);
  view->setRenderHint(QPainter::SmoothPixmapTransform, true);
  view->setDragMode(QGraphicsView::ScrollHandDrag);
  view->setInteractive(true);

  // Add items to the QListWidget
  m_ui->myListWidget->addItem(processorIDToQString(thisID));
  m_ui->myListWidget->addItem("Memory");

  // Get the list of all active peripherals
  auto activePeripherals = Ripes::IOManager::get().getPeripherals();

  // For each active peripheral, check its base name and increment the
  // corresponding counter
  for (const Ripes::IOBase *peripheral : activePeripherals) {
    m_ui->myListWidget->addItem(peripheral->baseName()); // + " " + QString::number(peripheral->getm_id()));
  }

  connect(m_ui->myListWidget, &QListWidget::itemClicked, this, &SystemTab::onListItemClicked);

}

void SystemTab::onListItemClicked(QListWidgetItem *item) {
  // Gestisci la selezione della QListWidget
  QString itemName = item->text();

  std::cout << "printo" << std::endl;

  // Evidenzia il rettangolo corrispondente nella QGraphicsView
  if (itemName == processorIDToQString(thisID)) {
    cpuItem->setBrush(QBrush(Qt::blue));
    memoryItem->setBrush(QBrush(Qt::lightGray));
    periphItem->setBrush(QBrush(Qt::lightGray));
  } else if (itemName == "Memory") {
    cpuItem->setBrush(QBrush(Qt::lightGray));
    memoryItem->setBrush(QBrush(Qt::blue));
    periphItem->setBrush(QBrush(Qt::lightGray));
  } else if (itemName == "LED Matrix") {
    cpuItem->setBrush(QBrush(Qt::lightGray));
    memoryItem->setBrush(QBrush(Qt::lightGray));
    periphItem->setBrush(QBrush(Qt::blue));
  }

  //view->update(); crusha!!! (update is automatic)
}

void SystemTabView::zoomIn() {
  scale(1.2, 1.2);
}

void SystemTabView::zoomOut() {
  scale(1.0 / 1.2, 1.0 / 1.2);
}


void SystemTab::setItemStyles(QGraphicsRectItem *item, const QColor &color) {
  // Set colors and styles for items
  item->setBrush(QBrush(color));
  item->setPen(QPen(Qt::black));
  item->setFlag(QGraphicsItem::ItemIsSelectable, true);
  item->setAcceptHoverEvents(true);
}

void SystemTab::connectItemsWithArrow(QGraphicsRectItem *startItem, QGraphicsRectItem *endItem, const QColor &color) {
  // Create arrow line
  QGraphicsLineItem *arrowLine = new QGraphicsLineItem(startItem->sceneBoundingRect().center().x(),
                                                       startItem->sceneBoundingRect().height() + startItem->sceneBoundingRect().y(),
                                                       startItem->sceneBoundingRect().center().x(),
                                                       endItem->sceneBoundingRect().center().y() - endItem->sceneBoundingRect().height()/2);
  arrowLine->setPen(QPen(color, 2));
  scene->addItem(arrowLine);

         // Tooltip for arrow line
  arrowLine->setToolTip("Data Flow");
}





SystemTab::~SystemTab() { delete m_ui; }



} // namespace Ripes
