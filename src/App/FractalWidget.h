#pragma once

#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QWidget>

class FractalWidget : public QWidget
{
public:
	FractalWidget(QWidget *parent = nullptr);
    
    QLabel * FPSLabel;
    QLabel * FPSvalLabel;

    QLabel * iterLabel;
	QSlider * iterEdit;

	QLabel * bailOutLabel;
    QSlider * bailOutEdit;

    QLabel * colorLabel;
    QSlider * colorEdit;
};