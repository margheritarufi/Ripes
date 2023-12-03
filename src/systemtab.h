#pragma once

#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QListWidgetItem>
#include <QWidget>

#include "iomanager.h"
#include "ripestab.h"

namespace Ripes {

namespace Ui {
class SystemTab;
}

class SystemBlock;

class Arrow {

public:
  // Arrow(SystemBlock *startItem, SystemBlock *endItem, const QColor &color,
  // int distanceFromBus); void updateArrowLine(QGraphicsItem* startItem,
  // QGraphicsItem* endItem, qreal baseDistanceFromBus);
  QGraphicsLineItem &getArrowLine() { return m_arrowLine; }
  QPen &getPen() { return m_pen; }
  // QPainter& getPainter() {return m_painter;}
  QGraphicsPolygonItem &getArrowHeadItemUp() { return m_arrowHeadItemUp; }
  QGraphicsPolygonItem &getArrowHeadItemDown() { return m_arrowHeadItemDown; }

protected:
private:
  QGraphicsLineItem m_arrowLine; // = nullptr; //it was *arrowLine
  // QPainter m_painter;
  QPen m_pen;

  /*QPointF arrowP1Up;
  QPointF arrowP2Up;
  QPointF arrowP1Down;
  QPointF arrowP2Down;

  QPolygonF arrowHeadUp, arrowHeadDown;*/

  QGraphicsPolygonItem m_arrowHeadItemUp;
  QGraphicsPolygonItem m_arrowHeadItemDown;
};

enum BlockType { CPU, MEMORY, BUS, PERIPHERAL };

class SystemBlock : public QGraphicsRectItem {
  // Q_OBJECT

public:
  SystemBlock(BlockType type, std::string name, qreal x, qreal y, qreal width,
              qreal height, QGraphicsItem *parent = nullptr);
  QString getQName() { return QString::fromStdString(m_name); }
  BlockType getBlockType() { return m_type; }
  void setName(std::string name) { m_name = name; }
  void updateLabel(const QString &newName);
  Arrow &getArrow() { return m_arrow; }

  // deconstructor?

protected:
private:
  BlockType m_type;
  std::string m_name;
  QGraphicsTextItem *m_label; // perché pointer?
  QPointF cpuLabelPosition;   //????
  Arrow m_arrow; // = nullptr; //pointer to avoid initializing it when creating
                 // a block (don't know endItem yet)
  QGraphicsPolygonItem m_arrowHeadItemUp;
};

class SystemTabView : public QGraphicsView {
  Q_OBJECT

public:
  SystemTabView(QWidget *parent);
  // deconstructor?

protected:
  void wheelEvent(QWheelEvent *event) override;

private:
  void zoomIn();
  void zoomOut();
};

class SystemTab : public RipesTab {
  Q_OBJECT

public:
  SystemTab(QToolBar *toolbar, QWidget *parent = nullptr);
  ~SystemTab() override;

private:
  Ui::SystemTab *m_ui = nullptr;

  int baseBlockDimension = 100;
  int baseBusHeight = 45;
  int baseSpaceBetweenBlocks = 20;
  int baseDistanceFromBus = 50;
  int startSceneX = 0;
  int startSceneY = 0;

  QGraphicsScene *scene;
  SystemTabView *view;

  SystemBlock *cpuRect;
  SystemBlock *memoryRect;
  SystemBlock *busRect;

  QVector<SystemBlock *> blocksVector;

  int i = 0;
  int j = 0;

  void setItemStyles(SystemBlock *item, const QColor &color);
  void updateSystemTab();
  void deletePeripheralBlocks(int nbActivePeripherals);
  void addNewPeripherals(std::set<IOBase *> &activePeripherals);
  void connectItemsWithArrow(SystemBlock *startItem, SystemBlock *endItem,
                             const QColor &color);

private slots:
  void onListItemClicked(QListWidgetItem *item);
  // void doSomething();
  // void doSomething2();
};

} // namespace Ripes
