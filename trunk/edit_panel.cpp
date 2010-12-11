/*----------------------------------------------------------------------------*/
/**
* @pkg edit_panel
*/
/**
* Edit panel to host waveform + channels edit + scroll.
*
* Long description of edit_panel.<br>
* Copyright (C) 2010 Oleg Yurchenko, Kiev, Ukraine.<br>
* started 10.12.2010 10:23:56<br>
* @pkgdoc edit_panel
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "edit_panel.h"
/*----------------------------------------------------------------------------*/
EditPanel :: EditPanel(QWidget *parent)
  :QWidget(parent)
{
  vbLayout = new QVBoxLayout(this);
  vbLayout->setMargin(0);

  for(int i = 0; i < 1; i++)
  {
    ChannelEdit *edit = new ChannelEdit(this);
    vbLayout->addWidget(edit);
    edits.append(edit);
  }

  waveForm = new WaveFormWidget(this);
  vbLayout->addWidget(waveForm);

  hScroll = new QScrollBar(Qt::Horizontal);
  vbLayout->addWidget(hScroll);

  setLayout(vbLayout);
}
/*----------------------------------------------------------------------------*/
EditPanel :: ~EditPanel()
{
}
/*----------------------------------------------------------------------------*/

