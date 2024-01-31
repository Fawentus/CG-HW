#pragma once

#include <Base/GLWidget.hpp>

#include <QElapsedTimer>
#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QSlider>

#include <functional>
#include <memory>
#include <tinygltf/tiny_gltf.h>

struct ParamGlDraw {
	int mode;
	size_t count;
	int componentType;
    size_t byteOffset;
	bool is_ibo = false;
};

class Window final : public fgl::GLWidget
{
	Q_OBJECT
public:
	Window() noexcept;
	~Window() override;

public: // fgl::GLWidget
	void onInit() override;
	void onRender() override;
	void onResize(size_t width, size_t height) override;
	void keyReleaseEvent(QKeyEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;

private:
	class PerfomanceMetricsGuard final
	{
	public:
		explicit PerfomanceMetricsGuard(std::function<void()> callback);
		~PerfomanceMetricsGuard();

		PerfomanceMetricsGuard(const PerfomanceMetricsGuard &) = delete;
		PerfomanceMetricsGuard(PerfomanceMetricsGuard &&) = delete;

		PerfomanceMetricsGuard & operator=(const PerfomanceMetricsGuard &) = delete;
		PerfomanceMetricsGuard & operator=(PerfomanceMetricsGuard &&) = delete;

	private:
		std::function<void()> callback_;
	};

private:
	[[nodiscard]] PerfomanceMetricsGuard captureMetrics();

signals:
	void updateUI();

private:
	double speed = 5;
   	QVector3D currPosition{2., 0., 0.};
	
	GLint modelUniform_ = -1;
	GLint viewUniform_ = -1;
	GLint projectionUniform_ = -1;
	GLint lightPosUniform_ = -1;
	GLint viewPosUniform_ = -1;
	GLint lightColorUniform_ = -1;
	GLint morphingUniform_ = -1;
	GLint ambientStrengthUniform_ = -1;
	GLint specularStrengthUniform_ = -1;
	
	QVector3D morphing_{1., 0., 0.}; 
	QSlider morphingEdit = QSlider(Qt::Horizontal);
	float ambientStrength_ = 0.5;
	QSlider ambientStrengthEdit = QSlider(Qt::Horizontal);
	float specularStrength_ = 0.5;
	QSlider specularStrengthEdit = QSlider(Qt::Horizontal);
	QVector3D lightColor_{1., 0., 0.};
	QSlider lightColorEdit = QSlider(Qt::Horizontal);

	QMatrix4x4 model_;
	QMatrix4x4 view_;
	QMatrix4x4 projection_;

	std::unique_ptr<QOpenGLShaderProgram> program_;

	QElapsedTimer timer_;
	size_t frameCount_ = 0;

	struct {
		size_t fps = 0;
	} ui_;

	bool animated_ = true;

	tinygltf::Model model;

	std::vector<std::unique_ptr<QOpenGLVertexArrayObject>> vaos;
	std::vector<std::unique_ptr<QOpenGLTexture>> texture;
	std::vector<ParamGlDraw> paramsGlDraw;

	std::vector<QOpenGLBuffer> vbos;
    std::vector<QOpenGLBuffer> ibos;

	std::map<int, GLuint> id_vbo;
	std::map<int, GLuint> id_ibo;

	// https://github.com/syoyo/tinygltf/blob/release/examples/basic/main.cpp
    bool loadModel(const char *filename);
	void bindMesh(tinygltf::Mesh &mesh);
	void bindModelNodes(tinygltf::Node &node);
};
