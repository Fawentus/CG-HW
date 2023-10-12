#include "FractalWidget.h"
#include <QHBoxLayout>
#include <QSlider>

FractalWidget::FractalWidget(QWidget * parent) : QWidget(parent)
{
	QGridLayout * grid = new QGridLayout();


	iterLabel = new QLabel("Number of iterations: ", this);
	iterEdit = new QSlider(this);
	
	iterEdit->setOrientation(Qt::Horizontal);
	iterEdit->setMaximum(500);
	iterEdit->setMinimum(0);
	iterEdit->setValue(200);

	grid->addWidget(iterLabel, 0, 0);
	grid->addWidget(iterEdit, 0, 1);


	bailOutLabel = new QLabel("Bail-out: ", this);
	bailOutEdit = new QSlider(this);

	bailOutEdit->setOrientation(Qt::Horizontal);
	bailOutEdit->setMaximum(10);
	bailOutEdit->setMinimum(0);
	bailOutEdit->setValue(2);

	grid->addWidget(bailOutLabel, 1, 0);
	grid->addWidget(bailOutEdit, 1, 1);


    colorLabel = new QLabel("Color: ", this);
	colorEdit = new QSlider(this);

	colorEdit->setOrientation(Qt::Horizontal);
	colorEdit->setMaximum(255);
	colorEdit->setMinimum(0);
	colorEdit->setValue(0);

	grid->addWidget(colorLabel, 2, 0);
	grid->addWidget(colorEdit, 2, 1);
	

	FPSLabel = new QLabel("FPS: ", this);
	FPSvalLabel =  new QLabel(QString::number(0), this);

	grid->addWidget(FPSLabel, 3, 0);
	grid->addWidget(FPSvalLabel, 3, 1);


	setLayout(grid);
}