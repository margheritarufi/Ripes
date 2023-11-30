#pragma once

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QListWidgetItem>


#include "ripestab.h"
#include "iomanager.h"

namespace Ripes {

namespace Ui {
class SystemTab;
}

enum BlockType {
  CPU,
  MEMORY,
  BUS,
  PERIPHERAL
};

class SystemBlock : public QGraphicsRectItem {
  //Q_OBJECT

public:
  SystemBlock(BlockType type, std::string name, qreal x, qreal y, qreal width, qreal height,
              QGraphicsItem *parent = nullptr);
  QString getQName(){return QString::fromStdString(m_name);}
  BlockType getBlockType(){return m_type;}
  void setName(std::string name){m_name=name;}
  void updateLabel(const QString& newName);

protected:


private:
  BlockType m_type;
  std::string m_name;
  QGraphicsTextItem* m_label;
  QPointF cpuLabelPosition;

};

class SystemTabView : public QGraphicsView {
  Q_OBJECT

public:
  SystemTabView(QWidget *parent);

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
  int startSceneX = 0;
  int startSceneY = 0;

  QGraphicsScene *scene;
  SystemTabView *view;

  SystemBlock *cpuRect;
  SystemBlock *memoryRect;
  SystemBlock *busRect;

  QVector<SystemBlock*> blocksVector;

  int i=0;
  int j=0;

  void setItemStyles(SystemBlock *item, const QColor &color);
  void updateSystemTab();
  void deletePeripheralBlocks(int nbActivePeripherals);
  void addNewPeripherals(std::set<IOBase*>& activePeripherals);
  //void connectItemsWithArrow(QGraphicsRectItem *startItem, QGraphicsRectItem *endItem, const QColor &color);


private slots:
  void onListItemClicked(QListWidgetItem *item);
  //void doSomething();
  //void doSomething2();

  };





} // namespace Ripes
