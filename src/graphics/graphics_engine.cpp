#include "graphics/graphics_engine.hpp"

namespace GE {

void GraphicsEngine::mainLoop() {
  float positions[] = {
      +0,   +0,   0.0f, 0.0f,  // i=0 vec2 of pos, vec2 of tex bounds
      +200, +0,   1.0f, 0.0f,  // i=1 vec2 of pos, vec2 of tex bounds
      +200, +200, 1.0f, 1.0f,  // i=2 vec2 of pos, vec2 of tex bounds
      +0,   +200, 0.0f, 1.0f   // i=3 vec2 of pos, vec2 of tex bounds
  };
  unsigned int indices[] = {
      0, 1, 2,  // 1st triangle indices of positions array
      2, 3, 0,  // 2nd triangle indices of positions array
  };

  VertexArray va;
  //                     4 vertex with 4 floats each
  VertexBuffer vb{positions, 4 * 4 * sizeof(float)};

  VertexBufferLayout layout;
  layout.push<float>(2);  // vec2 of float
  layout.push<float>(2);  // vec2 of float
  va.addBuffer(vb, layout);

  IndexBuffer ib{indices, 6};  // 6 indices will be drawn

  glm::mat4 proj_matrix =
      glm::ortho(0.0f, (float)m_width, 0.0f, (float)m_height, -1.0f, 1.0f);

  glm::mat4 view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

  Shader shader{"res/shaders/triangle_vf.shader"};
  shader.bind();

  Texture tex{"res/textures/grass.png"};
  tex.bind();
  shader.setUniform1i("u_Texture", 0);

  va.unbind();
  vb.unbind();
  ib.unbind();
  shader.unbind();

  Renderer re;
  glm::vec3 translation{-100 + m_width / 2, -100 + m_height / 2, 0};

  Gui gui{this->m_window, false};

  while(!glfwWindowShouldClose(this->m_window)) {
    // re.clear({0.1f, 0.9f, 0.2f, 1.0f});
    re.clear();

    gui.newFrame();

    shader.bind();

    glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), translation);

    /* P * V * M because opengl uses coloum-major matrices */
    glm::mat4 mvp = proj_matrix * view_matrix * model_matrix;

    shader.setUniformMat4f("u_MVP", mvp);

    re.draw(va, ib, shader);

    gui.drawSliders(translation, {m_width - 200, m_height - 200});
    gui.draw();

    glfwSwapBuffers(this->m_window);
    glfwPollEvents();
  }
}

GLFWwindow* GraphicsEngine::getWindow() const {
  return m_window;
}

void GraphicsEngine::init() {
  LOG(INFO) << "Initializing the GraphicsEngine instance";
  /* Initialize GLFW library */
  if(!glfwInit()) {
    LOG(ERROR) << "GLFW COULD NOT INITIALIZE";
    return;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);  // not working
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);  // not working
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  /* Create a windowed mode window and its OpenGL context */
  std::string window_name{std::to_string(m_width) + "x" +
                          std::to_string(m_height)};
  m_window =
      glfwCreateWindow(m_width, m_height, window_name.c_str(), NULL, NULL);
  if(!m_window) {
    LOG(ERROR) << "GLFW COULD NOT CREATE THE WINDOW";
    glfwTerminate();
    return;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(m_window);

  /* Sincronizes the framerate with the monitors refresh rate */
  glfwSwapInterval(1);

  /* Initialize GLEW library */
  if(glewInit() != GLEW_OK) {
    LOG(ERROR) << "GLEW COULD NOT INITIALIZE";
    return;
  }

  /* Enable blending in order to transparency to work */
  GLCALL(glEnable(GL_BLEND));
  GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

  /* Print OpenGL Version */
  LOG(INFO) << "OpenGL Version: " << glGetString(GL_VERSION);

  /* Window initiated successfully */
  LOG(INFO) << "Window initiated successfully";

  /* Callbacks setting */
  glfwSetKeyCallback(this->m_window, inputs::handleKeyboard);
}

GraphicsEngine::GraphicsEngine(const unsigned short& width,
                               const unsigned short& height)
    : m_width{width},
      m_height{height} {
  this->init();
}

GraphicsEngine::~GraphicsEngine() {
  glfwTerminate();
  LOG(INFO) << "Window terminated successfully";
}

}  // namespace GE