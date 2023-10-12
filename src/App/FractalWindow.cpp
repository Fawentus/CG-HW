#include "FractalWindow.h"

#include <QMouseEvent>
#include <QWheelEvent>
#include <QOpenGLFunctions>
#include <QScreen>

#include <array>
#include <iostream>

namespace
{

constexpr std::array<GLfloat, 8u> vertices = {
	-1.f, -1.f,
	-1.f, 1.f,
	1.f, 1.f,
	1.f, -1.f,
};
constexpr std::array<GLuint, 6u> indices = {0, 1, 3, 1, 2, 3};

}// namespace

void FractalWindow::init()
{
	// Configure shaders
	program_ = std::make_unique<QOpenGLShaderProgram>(this);
	program_->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/diffuse.vs"); // это вершинный шейдер -- обрабатывает каждую вершину
	program_->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/diffuse.fs"); // можно думать, что обрабатывает каждый пиксель
	program_->link();

	// Create VAO object -- это объекты в видеопамяти
	vao_.create();
	vao_.bind();

	// Create VBO
	vbo_.create();
	vbo_.bind();
	vbo_.setUsagePattern(QOpenGLBuffer::StaticDraw); // StaticDraw -- особо менять не собираемся, есть DYNAMIC/STREAM
	vbo_.allocate(vertices.data(), static_cast<int>(vertices.size() * sizeof(GLfloat))); // помещаем туда наши объекты

	// Create IBO
	ibo_.create();
	ibo_.bind();
	ibo_.setUsagePattern(QOpenGLBuffer::StaticDraw);
	ibo_.allocate(indices.data(), static_cast<int>(indices.size() * sizeof(GLuint)));

	// Bind attributes
	program_->bind();

	program_->enableAttributeArray(0);
	program_->setAttributeBuffer(0, GL_FLOAT, 0, 2, static_cast<int>(2 * sizeof(GLfloat)));

	iterUniform_ = program_->uniformLocation("iter");
	bailOutUniform_ = program_->uniformLocation("bailOut");
	shiftUniform_ = program_->uniformLocation("shift");
	colorUniform_ = program_->uniformLocation("color");
	scaleUniform_ = program_->uniformLocation("scale");

	// Release all
	program_->release();

	vao_.release();

	ibo_.release();
	vbo_.release();

	// Uncomment to enable depth test and face culling
	// glEnable(GL_DEPTH_TEST);
	// glEnable(GL_CULL_FACE);

	// Clear all FBO buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	timer.start();
}

void FractalWindow::render()
{
	// Configure viewport
	const auto retinaScale = devicePixelRatio();
	glViewport(0, 0, static_cast<GLint>(width() * retinaScale),
			   static_cast<GLint>(height() * retinaScale));

	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind VAO and shader program
	program_->bind();
	vao_.bind();

	// Update uniform value
	program_->setUniformValue(iterUniform_, iter);
	program_->setUniformValue(bailOutUniform_, bailOut);
	program_->setUniformValue(shiftUniform_, shift + shiftGlobal);
	program_->setUniformValue(colorUniform_, color);
	program_->setUniformValue(scaleUniform_, scale);

	// Draw
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	// Release VAO and shader program
	vao_.release();
	program_->release();

	// Increment frame counter
    ++frame_;
	++currNumFrames;
	float currDTime = timer.elapsed() / 1000;
    if (currDTime > 1)
    {
		FPSvalLabel->setText(QString::number(static_cast<int>(currNumFrames / currDTime)));
        currNumFrames = 0;
		timer.start();
    }
}

void FractalWindow::mousePressEvent(QMouseEvent * e)
{
	isMousePress = true;
	mousePressPosition = QVector2D(e->localPos());
}

void FractalWindow::mouseReleaseEvent(QMouseEvent * e)
{
	isMousePress = false;
	const auto diff = QVector2D(e->localPos()) - mousePressPosition;
	shift = QVector2D(-2 * diff.x() / width(), 2 * diff.y() / height()) * scale;
	shiftGlobal += shift;
	shift = QVector2D{0., 0.};
}

void FractalWindow::mouseMoveEvent(QMouseEvent * e)
{
	if (isMousePress) {
		const auto diff = QVector2D(e->localPos()) - mousePressPosition;
		shift = QVector2D(-2 * diff.x() / width(), 2 * diff.y() / height()) * scale;
	}
}

void FractalWindow::wheelEvent(QWheelEvent * e)
{
	float oldScale = scale;
	if (e->angleDelta().y() < 0) {
		scale *= 1.06;
	} else {
		scale /= static_cast<float>(1.06);
	}
	float x = 2. * e->position().x() / width() - 1;
	float y = -(2. * e->position().y() / height() - 1);
	shiftGlobal.setX(x - (x - shiftGlobal.x()) * scale / oldScale);
	shiftGlobal.setY(y - (y - shiftGlobal.y()) * scale / oldScale);

	// float x = 2. * e->position().x() / width() - 1;
	// float y = -(2. * e->position().y() / height() - 1);
	// shiftGlobal.setX((x - (x - shiftGlobal.x()) * scale / oldScale));
	// shiftGlobal.setY((y - (y - shiftGlobal.y()) * scale / oldScale));

	// float x = 2. * e->position().x() / width() - 1;
	// float y = -(2. * e->position().y() / height() - 1);
	// shiftGlobal += QVector2D(x, y) * (scale - oldScale);

	// float x = e->position().x();
	// float y = -e->position().y();
	// shiftGlobal.setX(-2*((x + shiftGlobal.x()) * scale / oldScale - x)/width());
	// shiftGlobal.setY(2*((y + shiftGlobal.y()) * scale / oldScale - y)/height());

	// float x = 2. * e->position().x() / width() - 1;
	// float y = -(2. * e->position().y() / height() - 1);
	// shiftGlobal.setX((1-scale/oldScale)*scale*(x+shiftGlobal.x()));
	// shiftGlobal.setY((1-scale/oldScale)*scale*(y+shiftGlobal.y()));
	// TODO update shiftGlobal
}

void FractalWindow::setIter(int iter_) {
	iter = iter_;
}

void FractalWindow::setBailOut(float bailOut_) {
	bailOut = bailOut_;
}

void FractalWindow::setColor(int color_) {
	color = color_;
}
