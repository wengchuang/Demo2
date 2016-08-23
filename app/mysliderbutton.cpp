#include "mysliderbutton.h"

mySliderButton::mySliderButton(QWidget *parent) :
    QWidget(parent)
{
    myBGColor1=QColor::fromRgb(46,132,243);
    myBGColor2=QColor::fromRgb(39,110,203);

    int step = 255/8;
    for(int i=0;i<8;i++){
        m_BGColorVec.push_back(qMakePair(QColor::fromRgb(255,step*(i+1),step*(i+1)),QColor::fromRgb(255,step*(8-i),step*(8-i))));
    }

    m_bForward = true;
    m_bShrink = false;
    m_bVirgin = true;


    setFixedHeight(PYSB_HEIGHT);
    setAttribute(Qt::WA_TranslucentBackground);
}

mySliderButton::~mySliderButton()
{

}


void mySliderButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing|QPainter::HighQualityAntialiasing);
    drawBg(&painter);
    drawBar(&painter);
    drawText(&painter);
}

void mySliderButton::SetmyBGColor(QColor c1, QColor c2)
{
    myBGColor1=c1;
    myBGColor2=c2;
}

void mySliderButton::drawBar(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::NoPen);
#if 0
    QLinearGradient BarGradient(m_BarRect.topLeft(),m_BarRect.bottomLeft());
//    BarGradient.setColorAt(0.0,PYSB_BAR_START_COLOR);
//    BarGradient.setColorAt(1.0,PYSB_BAR_END_COLOR);
    BarGradient.setColorAt(0.0,myBGColor1);
    BarGradient.setColorAt(1.0,myBGColor2);
    painter->setBrush(BarGradient);

    painter->drawRoundedRect(m_BarRect,PYSB_RECT_RADIUS,PYSB_RECT_RADIUS);

 #endif
    int ItemCnt = m_StrRectPairVec.count();
    for(int i = 0 ;i<ItemCnt;i++){
        QLinearGradient BarGradient(m_StrRectPairVec.at(i).second.topLeft(),m_StrRectPairVec.at(i).second.bottomLeft());
        BarGradient.setColorAt(0.0,m_BGColorVec.at(i).first);
        BarGradient.setColorAt(1.0,m_BGColorVec.at(i).second);
        painter->setBrush(BarGradient);

        painter->drawRoundedRect(m_StrRectPairVec.at(i).second.toRect(),PYSB_RECT_RADIUS,PYSB_RECT_RADIUS);
    }


    painter->restore();
}

void mySliderButton::drawBg(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::NoPen);

    QLinearGradient BgGradient(QPoint(0,0),QPoint(0,height()));
    BgGradient.setColorAt(0.0,PYSB_BG_START_COLOR);
    BgGradient.setColorAt(1.0,PYSB_BG_END_COLOR);
    painter->setBrush(BgGradient);
    painter->drawRoundedRect(rect(),PYSB_RECT_RADIUS,PYSB_RECT_RADIUS);


    painter->restore();
}

void mySliderButton::drawText(QPainter *painter)
{
    painter->save();

    painter->setPen(PYSB_TEXT_COLOR);
    QFont textFont;
    textFont.setBold(true);
    painter->setFont(textFont);

    int ItemCnt = m_StrRectPairVec.count();
    qreal initX = 0;
    qreal textWidth = 0;
    QString strText;
    for(int ItemIndex = 0;ItemIndex < ItemCnt ;ItemIndex++)
    {
        strText = m_StrRectPairVec.at(ItemIndex).first;
        textWidth  = fontMetrics().width(strText);
        QPointF topLeft(initX,0);
        QPointF bottomRight(initX + textWidth + 2 * PYSB_EXTRA_SPACE,height());
        QRectF textRect(topLeft,bottomRight);

        m_StrRectPairVec[ItemIndex].second = textRect;
        if(m_bVirgin)
        {
            m_BarRect = textRect;
            m_bVirgin = false;
        }
        painter->drawText(textRect,strText,Qt::AlignVCenter|Qt::AlignHCenter);

        /// increment initX
        initX +=textWidth + 2 * PYSB_EXTRA_SPACE;
    }

    setFixedWidth(initX);
    painter->restore();
}


void mySliderButton::removeAll()
{
    while(m_StrRectPairVec.count()){
        m_StrRectPairVec.pop_back();
    }
    update();
}


void mySliderButton::removeItem()
{
    m_StrRectPairVec.pop_back();
    update();
}

void mySliderButton::addItem(const QString &str)
{
    int ItemCnt = m_StrRectPairVec.count();
    for(int ItemIndex = 0;ItemIndex < ItemCnt ; ItemIndex++)
    {
        if(m_StrRectPairVec.at(ItemIndex).first.toLower() == str.toLower())
        {
            return ;
        }
    }

    m_StrRectPairVec.push_back(qMakePair(str,QRectF()));
    update();

    if(m_StrRectPairVec.count() > 0)
    {
        emit sig_itemClicked(m_StrRectPairVec.at(0).first);
    }

}
