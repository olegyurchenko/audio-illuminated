/*----------------------------------------------------------------------------*/
/**
* @pkg demo_panel
*/
/**
* Demonstration panel for led controller.
*
* Copyright (C) 2010 Oleg Yurchenko, Kiev, Ukraine.<br>
* started 13.12.2010 16:39:40<br>
* @pkgdoc demo_panel
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "demo_panel.h"
#include <QRect>
#include <QPainter>
#include <QtDebug>
/*----------------------------------------------------------------------------*/
DemoPanel :: DemoPanel(QWidget *parent)
  : EffectDemoPanel(parent)
{
  m_value = 0;
  setFixedHeight(20);
  m_bgColor = palette().color(QPalette::Window);
  m_fgColor = palette().color(QPalette::Foreground);
  m_ledColor = Qt::darkBlue;
}
/*----------------------------------------------------------------------------*/
DemoPanel :: ~DemoPanel()
{
}
/*----------------------------------------------------------------------------*/
void DemoPanel :: effectStart(const EffectProperties &prop)
{
  if(prop.property("led").toInt())
  {
    qint32 value = prop.property("value").toInt();
    if(m_value != value)
    {
      m_value = value;
      update();
    }
  }
}
/*----------------------------------------------------------------------------*/
void DemoPanel :: paintEvent(QPaintEvent *event)
{
  QPainter painter(this);

  painter.fillRect(rect(), m_bgColor);

  QRect r = rect();
  r.setWidth(32 * ((r.width())/32));
  r.moveLeft((rect().width() - r.width())/2);

  //qDebug() << rect() << r;

  int dx = r.width() / 32;

  r.setHeight(dx);
  r.moveTop((rect().height() - r.height())/2);

  QPen pen(m_fgColor);
  painter.setPen(pen);

  painter.fillRect(r, Qt::white);
  painter.drawRect(r);

  for(int i = 0; i < 32; i++)
  {
    if(m_value & (1 << i))
      painter.fillRect(r.left() + dx * i, r.top(), dx, r.height(), m_ledColor);
    painter.drawLine(r.left() + dx + dx * i, r.top(), r.left() + dx + dx * i, r.bottom());
  }
}
/*----------------------------------------------------------------------------*/

