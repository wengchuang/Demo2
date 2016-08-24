#include "barchart.h"

#include <qwt_plot_renderer.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_barchart.h>
#include <qwt_column_symbol.h>
#include <qwt_plot_layout.h>
#include <qwt_legend.h>
#include <qwt_scale_draw.h>

class DistroScaleDraw:public QwtScaleDraw{
   public:
    DistroScaleDraw(Qt::Orientation ori,const QStringList& labels):labels(labels){
        setTickLength(QwtScaleDiv::MinorTick,0);
        setTickLength(QwtScaleDiv::MediumTick,0);
        setTickLength(QwtScaleDiv::MajorTick,5);
        enableComponent(QwtScaleDraw::Backbone,false);
        if(ori == Qt::Vertical){
            setLabelRotation(-60.0);
        }else{
            setLabelRotation(-20.0);
        }
        setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    }

    virtual QwtText label(double value)const{

        QwtText label;
        const int index = qRound(value);
        if(index >= 0 && index < labels.size()){
            label = labels[index];
        }

        return label;
    }
private:
    QStringList labels;
};

class ColumnSymbol: public QwtColumnSymbol
{
public:
    ColumnSymbol(int hit, QColor color,QwtColumnSymbol::Style  e = NoStyle ): QwtColumnSymbol(e)
    {
        d_hit = hit;
        d_color = color;
    }

    //绘制柱形图
    virtual void draw(QPainter *p, const QwtColumnRect &rect) const
    {
        QRectF rect2 = rect.toRect();


        p->setBrush(d_color);
        p->setPen(Qt::NoPen);
        p->drawRect(rect.toRect().x(),rect.toRect().y()+10,rect.toRect().width(),rect.toRect().height());
        p->setPen(Qt::black);
        int top = rect2.top();
        int height = rect2.height();
        Qt::Alignment alignment;
        if (d_hit >= 0){
            top -= 15;
            height += 15;
            alignment = Qt::AlignTop | Qt::AlignHCenter;
        }else{
            height += 15;
            alignment = Qt::AlignBottom | Qt::AlignHCenter;
        }
        p->setPen(Qt::white);
        p->drawText(QRect(rect2.left(), top+10, rect2.width(), height), alignment, QString::number(d_hit));
    }
private:
    int d_hit;
    QColor d_color;
};

class DistroChartItem:public QwtPlotBarChart{
  public:
    DistroChartItem():QwtPlotBarChart("Page Hits"){
        setLegendMode(QwtPlotBarChart::LegendBarTitles);
        setLegendIconSize(QSize(0,0));
    }
    void clearData(){
      dataList.clear();
    }
    void addData(double data){
        dataList += data;
    }
    void addDistro(const QString& distro,const QColor& color){
        distros += distro;
        colors  += color;
        itemChanged();
    }

    virtual QwtColumnSymbol* specialSymbol(int index,const QPointF&)const{
        QColor c(Qt::white);
        if(index >=0 && index < colors.size()){
            c = colors[index];
        }
        ColumnSymbol* symbol = new ColumnSymbol(dataList[index],c,QwtColumnSymbol::Box);
        symbol->setLineWidth(1);
        symbol->setFrameStyle(QwtColumnSymbol::Plain);
        return symbol;
    }

private:
    QList<QColor>       colors;
    QList<QString>      distros;
    QList<double>       dataList;
};

const struct{
    const char* distro;
    const int hits;
    QColor color;
}pageHits[]={
    {"1号通道",1114,QColor(0, 160, 230)},
    {"2号通道", 1373, QColor( 255, 130, 0 ) },
    {"3号通道", 1638, QColor( 0,0x5f,0 ) },
    {"4号通道", 1395, QColor( 0,0,0x5f ) },
    {"5号通道", 3874, QColor( 183, 255, 183 ) },
    {"6号通道", 1532, QColor( 115, 186, 37 ) },
    {"7号通道", 1059, QColor( 0x5f,0x5f,0 ) },
    {"8号通道", 2391, QColor( 0x5f,0,0x5f ) },
    {"坏砖统计",3000, QColor(0xff, 0, 0)}
};
Barchart::Barchart(QWidget *parent) :
    QwtPlot(parent)
{
    setAutoFillBackground(true);


    QwtPlotCanvas* canvas = new QwtPlotCanvas;
    canvas->setLineWidth(2);
    canvas->setFrameStyle(QFrame::Box);
    canvas->setBorderRadius(10);

    setTitle("生产报表");

    item = new DistroChartItem;

    QVector<double> samples;

    for(uint i = 0; i < sizeof(pageHits)/(sizeof(pageHits[0]));i++){
        mDistros += pageHits[i].distro;
        samples += pageHits[i].hits;
        item->addDistro(pageHits[i].distro,pageHits[i].color);
        item->addData(pageHits[i].hits);
    }

    item->setSamples(samples);

    item->attach(this);
    insertLegend(new QwtLegend());

    item->setOrientation(Qt::Vertical);
    item->setSpacing(20);
    item->setMargin(30);

    setAxisMaxMinor(QwtPlot::xBottom,3);
    //setAxisMaxMajor(QwtPlot::xBottom,10);
    setAxisScaleDraw(QwtPlot::xBottom,new DistroScaleDraw(Qt::Vertical,mDistros));

    setAxisTitle(QwtPlot::yLeft, QString::fromAscii("砖块数量") );
    setAxisMaxMinor( QwtPlot::yLeft, 3 );

    QwtScaleDraw *scaleDraw = new QwtScaleDraw();
    scaleDraw->setTickLength( QwtScaleDiv::MediumTick, 4 );
    setAxisScaleDraw(QwtPlot::yLeft,scaleDraw );

    plotLayout()->setCanvasMargin(0);

    QPalette canvasPalette(QColor(255,0,0));
    canvasPalette.setColor(QPalette::Foreground, QColor( 0,255,0));
    canvas->setPalette(canvasPalette);
    setCanvas(canvas);

    replot();

    setAutoReplot(false);



}
void Barchart::repaintChart(const QString& title,QVector<double>& samples)
{
    item->clearData();
    while(samples.count() < sizeof(pageHits)/(sizeof(pageHits[0]))){
        samples.append(0);
    }

    for(uint i = 0; i < sizeof(pageHits)/(sizeof(pageHits[0]));i++){
        item->addData(samples[i]);
    }

    item->setSamples(samples);
    setAxisTitle(QwtPlot::xBottom, title);



    replot();
}


void Barchart::exportChart()
{
    QwtPlotRenderer renderer;
    renderer.exportTo( this, "生产报表.pdf" );
}
