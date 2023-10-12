#include <QApplication>
#include <QVBoxLayout>
#include <QSurfaceFormat>

#include "FractalWindow.h"
#include "FractalWidget.h"

namespace
{
constexpr auto g_sampels = 16;
constexpr auto g_gl_major_version = 3;
constexpr auto g_gl_minor_version = 3;
}// namespace

int main(int argc, char ** argv)
{
	QApplication app(argc, argv);

	QSurfaceFormat format;
	format.setSamples(g_sampels);
	format.setVersion(g_gl_major_version, g_gl_minor_version);
	format.setProfile(QSurfaceFormat::CoreProfile);
	
	FractalWidget* fractalWidget = new FractalWidget(nullptr);

	FractalWindow* fractalWindow = new FractalWindow(fractalWidget->FPSvalLabel);
	fractalWindow->setFormat(format);
	fractalWindow->setAnimated(true);

	QVBoxLayout * layout = new QVBoxLayout(nullptr);

	QWidget * container = QWidget::createWindowContainer(fractalWindow);
	container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	layout->addWidget(container);
	layout->addWidget(fractalWidget, 0, Qt::Alignment(Qt::AlignBottom));
	QObject::connect(fractalWidget->iterEdit, &QSlider::valueChanged, fractalWindow, &FractalWindow::setIter);
	QObject::connect(fractalWidget->bailOutEdit, &QSlider::valueChanged, fractalWindow, &FractalWindow::setBailOut);
	QObject::connect(fractalWidget->colorEdit, &QSlider::valueChanged, fractalWindow, &FractalWindow::setColor);

	auto window = new QWidget();
	window->resize(1000, 800);
	window->setLayout(layout);
	window->show();

	return app.exec();
}

