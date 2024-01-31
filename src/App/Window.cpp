#include "Window.h"

#include <QMouseEvent>
#include <QLabel>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QVBoxLayout>
#include <QScreen>

#include <array>
#include <iostream>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

const auto ARRAY_BUFFER = 34962;
const auto ELEMENT_ARRAY_BUFFER = 34963;

Window::Window() noexcept {
  morphingEdit.setRange(0, 100);
  ambientStrengthEdit.setRange(0, 100);
  specularStrengthEdit.setRange(0, 100);
  lightColorEdit.setRange(0, 100);

	const auto formatFPS = [](const auto value) {
		return QString("FPS: %1").arg(QString::number(value));
	};
  const auto formatMorphing = [](const auto morphing) {
		return QString("Morphing: %1").arg(QString::number(morphing));
	};
  const auto formatAmbientStrength = [](const auto ambientStrength) {
		return QString("Ambient strength: %1").arg(QString::number(ambientStrength));
	};
  const auto formatSpecularStrength = [](const auto specularStrength) {
		return QString("Specular strength: %1").arg(QString::number(specularStrength));
	};
  const auto formatLightColor = [](const auto lightColor) {
		return QString("Light color: %1").arg(QString::number(lightColor));
	};

	auto fps = new QLabel(formatFPS(0), this);
	fps->setStyleSheet("QLabel { color : white; }");
  auto morphing = new QLabel(formatMorphing(0), this);
	morphing->setStyleSheet("QLabel { color : white; }");
  auto ambientStrength = new QLabel(formatAmbientStrength(0), this);
	ambientStrength->setStyleSheet("QLabel { color : white; }");
  auto specularStrength = new QLabel(formatSpecularStrength(0), this);
	specularStrength->setStyleSheet("QLabel { color : white; }");
  auto lightColor = new QLabel(formatLightColor(0), this);
	lightColor->setStyleSheet("QLabel { color : white; }");

	auto layout1 = new QVBoxLayout();
  auto layout2 = new QHBoxLayout();
  auto layout3 = new QHBoxLayout();
  auto layout4 = new QHBoxLayout();
  auto layout5 = new QHBoxLayout();
	layout1->addWidget(fps);
  layout2->addWidget(morphing);
  layout2->addWidget(&morphingEdit);
  layout3->addWidget(ambientStrength);
  layout3->addWidget(&ambientStrengthEdit);
  layout4->addWidget(specularStrength);
  layout4->addWidget(&specularStrengthEdit);
  layout5->addWidget(lightColor);
  layout5->addWidget(&lightColorEdit);
  layout1->addLayout(layout2);
  layout1->addLayout(layout3);
  layout1->addLayout(layout4);
  layout1->addLayout(layout5);

	setLayout(layout1);

	timer_.start();

	connect(this, &Window::updateUI, [=] {
		fps->setText(formatFPS(ui_.fps));
    morphing->setText(formatMorphing(morphingEdit.value()));
    ambientStrength->setText(formatAmbientStrength(ambientStrengthEdit.value()));
    specularStrength->setText(formatSpecularStrength(specularStrengthEdit.value()));
    lightColor->setText(formatLightColor(lightColorEdit.value()));
	});
  connect(&morphingEdit, SIGNAL(valueChanged(int)), &morphingEdit, SLOT(setValue(int)));
  connect(&ambientStrengthEdit, SIGNAL(valueChanged(int)), &ambientStrengthEdit, SLOT(setValue(int)));
  connect(&specularStrengthEdit, SIGNAL(valueChanged(int)), &specularStrengthEdit, SLOT(setValue(int)));
  connect(&lightColorEdit, SIGNAL(valueChanged(int)), &lightColorEdit, SLOT(setValue(int)));
}

Window::~Window() {
	{
		// Free resources with context bounded.
		const auto guard = bindContext();
		program_.reset();
	}
}

void Window::onInit() {
  const std::string name = "gun";
  // const std::string name = "dinosaur_skeleton";

  if (!loadModel(std::string("../src/App/Models/"+name+".glb").c_str())) {
    return;
  }
   
	// Configure shaders
	program_ = std::make_unique<QOpenGLShaderProgram>(this);
	program_->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/diffuse.vs");
	program_->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/diffuse.fs");
	program_->link();

  // Create
  vbos.reserve(model.bufferViews.size());
  ibos.reserve(model.bufferViews.size());
  for (size_t i = 0; i < model.bufferViews.size(); ++i) {
    const tinygltf::BufferView &bufferView = model.bufferViews[i];
    if (bufferView.target == 0 || bufferView.target == ARRAY_BUFFER) {
      id_vbo[i] = vbos.size();

      QOpenGLBuffer glBuffer_{QOpenGLBuffer::Type::VertexBuffer};
      glBuffer_.create();
      glBuffer_.bind();
      glBuffer_.setUsagePattern(QOpenGLBuffer::StaticDraw);
      glBuffer_.allocate(model.buffers[bufferView.buffer].data.data() + bufferView.byteOffset, bufferView.byteLength);
      vbos.push_back(glBuffer_);
    } else if (bufferView.target == ELEMENT_ARRAY_BUFFER) {
      id_ibo[i] = ibos.size();

      QOpenGLBuffer glBuffer_{QOpenGLBuffer::Type::IndexBuffer};
      glBuffer_.create();
      glBuffer_.bind();
      glBuffer_.setUsagePattern(QOpenGLBuffer::StaticDraw);
      glBuffer_.allocate(model.buffers[bufferView.buffer].data.data() + bufferView.byteOffset, bufferView.byteLength);
      ibos.push_back(glBuffer_);
    }
  }

  // Bind attributes
  program_->bind();
  const tinygltf::Scene &scene = model.scenes[model.defaultScene];
  for (size_t i = 0; i < scene.nodes.size(); ++i) {
    assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
    bindModelNodes(model.nodes[scene.nodes[i]]); 
  }

	modelUniform_ = program_->uniformLocation("model");
  viewUniform_ = program_->uniformLocation("view");
  projectionUniform_ = program_->uniformLocation("projection");
  morphingUniform_ = program_->uniformLocation("morphing");
  ambientStrengthUniform_ = program_->uniformLocation("ambientStrength");
  specularStrengthUniform_ = program_->uniformLocation("specularStrength");
  lightColorUniform_ = program_->uniformLocation("lightColor");
  lightPosUniform_ = program_->uniformLocation("lightPos");
  viewPosUniform_ = program_->uniformLocation("viewPos");

	// Release all
	program_->release();

	// Еnable depth test and face culling
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Clear all FBO buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::onRender() {
	const auto guard = captureMetrics();

	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Calculate MVP matrix
	model_.setToIdentity();
  view_.setToIdentity();
  view_.translate(0, -2, -7);

	// Bind VAO and shader program
	program_->bind();

	// Update uniform value
	program_->setUniformValue(modelUniform_, model_);
  
  QMatrix4x4 view;
  view.lookAt(currPosition, QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));
  program_->setUniformValue(viewUniform_, view);
  
  program_->setUniformValue(projectionUniform_, projection_);
  if (lightColorEdit.value() == 0)
    lightColor_ = QVector3D(.0, 1, .0);
  else
    lightColor_ = QVector3D(static_cast<float>(lightColorEdit.value() % 10) / 10, static_cast<float>(lightColorEdit.value() / 10) / 10, static_cast<float>(lightColorEdit.value() % 10 + lightColorEdit.value() / 10) / 20);
  program_->setUniformValue(lightColorUniform_, lightColor_);
  program_->setUniformValue(lightPosUniform_, QVector3D(10., 10., 10.));
  program_->setUniformValue(viewPosUniform_, currPosition);
  morphing_.setX(static_cast<float>(morphingEdit.value()) / 100.0f);
	program_->setUniformValue(morphingUniform_, morphing_);
  ambientStrength_ = static_cast<float>(ambientStrengthEdit.value()) / 100.0f;
  program_->setUniformValue(ambientStrengthUniform_, ambientStrength_);
  specularStrength_ = static_cast<float>(specularStrengthEdit.value()) / 100.0f;
  program_->setUniformValue(specularStrengthUniform_, specularStrength_);

	// Draw
  for (size_t i = 0; i < paramsGlDraw.size(); i++) {
    static QOpenGLFunctions funcs;
    funcs.initializeOpenGLFunctions();
    funcs.glActiveTexture(GL_TEXTURE0);

    vaos[i]->bind();
    texture[i]->bind();

    if (paramsGlDraw[i].is_ibo) {
      funcs.glDrawElements(paramsGlDraw[i].mode, paramsGlDraw[i].count, paramsGlDraw[i].componentType, BUFFER_OFFSET(paramsGlDraw[i].byteOffset));
    } else {
      funcs.glDrawArrays(paramsGlDraw[i].mode, 0, paramsGlDraw[i].count);
    }

    texture[i]->release();
    vaos[i]->release();
  }

	// Release VAO and shader program
	program_->release();

	++frameCount_;

	// Request redraw if animated
	if (animated_) {
		update();
	}
}

void Window::onResize(const size_t width, const size_t height) {
	// Configure viewport
	glViewport(0, 0, static_cast<GLint>(width), static_cast<GLint>(height));

	// Configure matrix
	const auto aspect = static_cast<float>(width) / static_cast<float>(height);
	const auto zNear = 0.1f;
	const auto zFar = 100.0f;
	const auto fov = 60.0f;
	projection_.setToIdentity();
	projection_.perspective(fov, aspect, zNear, zFar);
}

Window::PerfomanceMetricsGuard::PerfomanceMetricsGuard(std::function<void()> callback)
: callback_{ std::move(callback) } {}

Window::PerfomanceMetricsGuard::~PerfomanceMetricsGuard()
{
	if (callback_) {
		callback_();
	}
}

auto Window::captureMetrics() -> PerfomanceMetricsGuard
{
	return PerfomanceMetricsGuard{
		[&] {
			if (timer_.elapsed() >= 1000)
			{
				const auto elapsedSeconds = static_cast<float>(timer_.restart()) / 1000.0f;
				ui_.fps = static_cast<size_t>(std::round(frameCount_ / elapsedSeconds));
				frameCount_ = 0;
				emit updateUI();
			}
		}
	};
}

bool Window::loadModel(const char *filename) {
  tinygltf::TinyGLTF loader;
  std::string err;
  std::string warn;

  bool res = loader.LoadBinaryFromFile(&model, &err, &warn, filename);
  
  if (!warn.empty()) {
    std::cout << "WARN: " << warn << std::endl;
  }
  if (!err.empty()) {
    std::cout << "ERR: " << err << std::endl;
  }

  if (!res)
    std::cout << "Failed to load glb: " << filename << std::endl;
  else
    std::cout << "Loaded glb: " << filename << std::endl;

  return res;
}

void Window::bindModelNodes(tinygltf::Node &node) {
  if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
    bindMesh(model.meshes[node.mesh]);
  }

  for (size_t i = 0; i < node.children.size(); i++) {
    assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
    bindModelNodes(model.nodes[node.children[i]]);
  }
}

void Window::bindMesh(tinygltf::Mesh &mesh) {
  for (size_t i = 0; i < mesh.primitives.size(); ++i) {
    ParamGlDraw param = ParamGlDraw();
    std::unique_ptr<QOpenGLVertexArrayObject> vao = std::make_unique<QOpenGLVertexArrayObject>();
    vao->create();
 		vao->bind();
    vaos.push_back(std::move(vao));

    tinygltf::Primitive primitive = mesh.primitives[i];
    tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

    if (indexAccessor.bufferView >= 0) {
        assert(id_ibo.find(indexAccessor.bufferView) != id_ibo.end()); 
        ibos[id_ibo[indexAccessor.bufferView]].bind();
        
        param.is_ibo = true;
        param.componentType = indexAccessor.componentType;
        param.byteOffset = indexAccessor.byteOffset;
    }

    for (auto &attrib : primitive.attributes) {
      tinygltf::Accessor accessor = model.accessors[attrib.second];
      int byteStride = accessor.ByteStride(model.bufferViews[accessor.bufferView]);

      int size = 1;
      if (accessor.type != TINYGLTF_TYPE_SCALAR) {
        size = accessor.type;
      }

      int vaa = -1;
      if (attrib.first.compare("POSITION") == 0) vaa = 0;
      if (attrib.first.compare("NORMAL") == 0) vaa = 1;
      if (attrib.first.compare("TEXCOORD_0") == 0) vaa = 2;
      if (vaa > -1) {
        assert(id_vbo.find(accessor.bufferView) != id_vbo.end()); 
        vbos[id_vbo[accessor.bufferView]].bind();
        program_->enableAttributeArray(vaa);
	      program_->setAttributeBuffer(vaa, accessor.componentType, accessor.byteOffset, size, byteStride);

      } else
        std::cout << "vaa missing: " << attrib.first << std::endl;
    }

    if (model.textures.size() > 0) {
      tinygltf::Texture &tex = model.textures[0];

      if (tex.source > -1) {
        tinygltf::Image &image = model.images[tex.source];

        texture.push_back(std::make_unique<QOpenGLTexture>(QImage{image.image.data(), image.width, image.height, QImage::Format::Format_RGB32}));
      }
    }

    param.mode = primitive.mode;
    param.count = indexAccessor.count;
    paramsGlDraw.push_back(param);
  }
}

// https://habr.com/ru/articles/319144/
void Window::keyReleaseEvent(QKeyEvent *event) {
  QMatrix4x4 m;
  QVector3D v{0., 1., 0.};
  
  switch (event->key()) {
  case Qt::Key_W:
    m.rotate(-speed, QVector3D::crossProduct(v, currPosition));
    break;
  case Qt::Key_D:
    m.rotate(speed, v);
    break;
  case Qt::Key_S:
    m.rotate(speed, QVector3D::crossProduct(v, currPosition));
    break;
  case Qt::Key_A:
    m.rotate(-speed, v);
    break;
  default:
    return;
  }
  currPosition = m * currPosition;

  // std::cout << currPosition.x() << " " << currPosition.y() << " " << currPosition.z() << "\n";
}

void Window::wheelEvent(QWheelEvent *event) {
  if (event->angleDelta().y() < 0) {
    currPosition *= (100 - speed)/100;
  } else {
    currPosition *= (100 + speed)/100;
  }
}
