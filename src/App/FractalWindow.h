#pragma once

#include <Base/GLWindow.hpp>

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QQuaternion>
#include <QVector2D>
#include <QVector3D>

#include <memory>

class FractalWindow final : public fgl::GLWindow
{

public:
	FractalWindow();
	void init() override;
	void render() override;
	void setIter(int iter_);
	void setBailOut(float bailOut_);
	void setColor(int color_);

protected:
	void mousePressEvent(QMouseEvent * e) override;
	void mouseReleaseEvent(QMouseEvent * e) override;
	void mouseMoveEvent(QMouseEvent * e) override;
	void wheelEvent(QWheelEvent * e) override;

private:
	GLint iterUniform_ = -1;
	GLint bailOutUniform_ = -1;
	GLint shiftUniform_ = -1;
	GLint colorUniform_ = -1;
	GLint scaleUniform_ = -1;

	int iter = 200;
	float bailOut = 2;
	int color = 0;
	QVector2D shift{0., 0.};
	QVector2D shiftGlobal{0., 0.};
	float scale = 1;

	QOpenGLBuffer vbo_{QOpenGLBuffer::Type::VertexBuffer};
	QOpenGLBuffer ibo_{QOpenGLBuffer::Type::IndexBuffer};
	QOpenGLVertexArrayObject vao_;

	std::unique_ptr<QOpenGLShaderProgram> program_ = nullptr;

	size_t frame_ = 0;

	QVector2D mousePressPosition{0., 0.};
	bool isMousePress = false;
};
