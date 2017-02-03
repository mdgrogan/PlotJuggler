#include "../plotwidget.h"
#include "../plotlegend.h"
#include "../plotgrid.h"
#include "../plotmagnifier.h"
#include "../customtracker.h"
#include "../plotzoomer.h"

#include <QtCharts/QChartView>
#include <QtWidgets/QRubberBand>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>

PlotWidget::PlotWidget(PlotDataMap *datamap, QWidget *parent):
	QChartView(parent),
	_magnifier(0 ),
	_tracker ( 0 ),
	_legend( 0 ),
	_grid( 0 ),
	_mapped_data( datamap ),
	_line_style( PlotCurve::LINES),
	_current_transform( PlotCurve::noTransform )
{
	using namespace QtCharts;

	this->setChart( new QChart() );
	this->chart()->setAcceptDrops(true);

	this->setMinimumWidth( 100 );
	this->setMinimumHeight( 100 );

	this->sizePolicy().setHorizontalPolicy( QSizePolicy::Expanding);
	this->sizePolicy().setVerticalPolicy( QSizePolicy::Expanding);

	this->setRenderHint(QPainter::Antialiasing);
	this->chart()->installEventFilter( this );
	this->chart()->legend()->hide();

	QValueAxis* axis_x = new QValueAxis(this);
	axis_x->setRange(0, 100);
	QValueAxis* axis_y = new QValueAxis(this);
	axis_y->setRange(0, 100);

	chart()->setAxisX( axis_x );
	chart()->setAxisY( axis_y );

	chart()->setVisible( true );
	chart()->createDefaultAxes();
	//--------------------------

	_replot_timer.setInterval(1);
	_replot_timer.setSingleShot(true);
	QObject::connect(&_replot_timer, &QTimer::timeout, this, &PlotWidget::replotHandle);

	this->setContextMenuPolicy( Qt::ContextMenuPolicy::CustomContextMenu );
	connect( this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(canvasContextMenuTriggered(QPoint)) );

	//-------------------------

	buildActions();

	connect( chart()->scene(), &QGraphicsScene::changed, this, &PlotWidget::replot  );

}

QRectF PlotWidget::boundingRect() const
{
	return this->chart()->boundingRect();
}

QPointF PlotWidget::canvasToPlot(QPoint point)
{
	return chart()->mapToValue( point);
}

QPoint PlotWidget::plotToCanvas(QPointF point)
{
	QPointF p = chart()->mapToPosition(point);
	return QPoint(p.x(), p.y());
}

void PlotWidget::replot()
{
	static int count = 0;
	qDebug() << "replot " << count++;
	_replot_timer.start();
}

void PlotWidget::replotHandle()
{
	for(auto it = _curve_list.begin(); it != _curve_list.end(); ++it)
	{
		PlotCurve* curve = static_cast<PlotCurve*>( it->second.get() );
		curve->updateData(false);
	}
}


void PlotWidget::setTitle(QString text)
{

}

void PlotWidget::setAxisScale(Axis axisId, double min, double max, double step)
{
	if (axisId == AXIS_X)
	{
		for(auto it = _curve_list.begin(); it != _curve_list.end(); ++it)
		{
			PlotCurve* curve = static_cast<PlotCurve*>( it->second.get() );
			curve->setSubsampleFactor( );
		}
		this->chart()->axisX()->setRange( min, max );
	}
	else{
		this->chart()->axisY()->setRange( min, max );
	}
}







