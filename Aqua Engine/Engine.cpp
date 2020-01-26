#include "Engine.h"
Engine::Engine(std::string title) {
	_console = new Console(0, "logs/engine.html");


	_console->log("Creating window");
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	if (!glfwInit()) {
		_console->error("GLFW NOT INITIATED!");
	}

	_window = new Window(800, 600, title, true);
	glfwMakeContextCurrent(_window->getWindow());
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		_console->error("GLEW NOT INITIATED!");
	}

	_console->log("Window created, creating local scene");
	_scene = new Scene();

	if (_console != nullptr && _window != nullptr) {
		stopLoop = false;
		_console->log("Started engine succesfully!");
	}

	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	_VAID = new GLuint;
	glGenVertexArrays(1, _VAID);
	glBindVertexArray(*_VAID);

	glClearColor(0.6392156862f, 0.7372549019607f, 0.90196078431372549f, 1.0f);

	_programID = glCreateProgram();
	_matrixID = glGetUniformLocation(_programID, "MVP");

	_projection = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);
	_view = glm::lookAt(
		glm::vec3(4, 3, 3),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
	);
	_model = glm::mat4(1.0f);
	_MVP = _projection * _view * _model;

}
Engine::~Engine() {
	glDeleteVertexArrays(1, _VAID);
	glDeleteProgram(_programID);
}

Window* Engine::getWindow()
{
	return _window;
}

Console* Engine::getConsole()
{
	return _console;
}

Scene* Engine::getScene()
{
	return _scene;
}

GLuint* Engine::getProgramID()
{
	return &_programID;
}

void Engine::loadShaders(std::string pathV, std::string pathF){
	_console->log("Loading shader files...");
	GLuint _vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint _fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	std::string VShaderCode;
	std::ifstream VertexShaderStream(pathV, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream VFInput;
		VFInput << VertexShaderStream.rdbuf();
		VShaderCode = VFInput.str();
		VertexShaderStream.close();
	}

	std::string FShaderCode;
	std::ifstream FragmentShaderStream(pathF, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream FFInput;
		FFInput << FragmentShaderStream.rdbuf();
		FShaderCode = FFInput.str();
		FragmentShaderStream.close();
	}

	GLint result = GL_FALSE;
	int infoLogSize;

	_console->log("Compiling shaders...");
	// Vertex shader
	char const* VSP = VShaderCode.c_str();
	glShaderSource(_vertexShaderID, 1, &VSP, NULL);
	glCompileShader(_vertexShaderID);

	glGetShaderiv(_vertexShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(_vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogSize);
	if (infoLogSize > 0) {
		std::vector<char> vertexShaderErrMsg((unsigned __int64)infoLogSize + 1);
		glGetShaderInfoLog(_vertexShaderID, infoLogSize, NULL, &vertexShaderErrMsg[0]);
		_console->error(&vertexShaderErrMsg[0]);
	}

	// Fragment shader
	char const* FSP = FShaderCode.c_str();
	glShaderSource(_fragmentShaderID, 1, &FSP, NULL);
	glCompileShader(_fragmentShaderID);

	glGetShaderiv(_fragmentShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(_fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogSize);
	if (infoLogSize > 0) {
		std::vector<char> fragmentShaderErrMsg((unsigned __int64)infoLogSize + 1);
		glGetShaderInfoLog(_fragmentShaderID, infoLogSize, NULL, &fragmentShaderErrMsg[0]);
		_console->error(&fragmentShaderErrMsg[0]);
	}

	_console->log("Creating program...");
	glAttachShader(_programID, _vertexShaderID);
	glAttachShader(_programID, _fragmentShaderID);
	glLinkProgram(_programID);

	glGetProgramiv(_programID, GL_LINK_STATUS, &result);
	glGetProgramiv(_programID, GL_INFO_LOG_LENGTH, &infoLogSize);
	if (infoLogSize > 0) {
		std::vector<char> progErrMsg((unsigned __int64)infoLogSize + 1);
		glGetProgramInfoLog(_programID, infoLogSize, NULL, &progErrMsg[0]);
		_console->error(&progErrMsg[0]);
	}

	glDetachShader(_programID, _vertexShaderID);
	glDetachShader(_programID, _fragmentShaderID);

	glDeleteShader(_vertexShaderID);
	glDeleteShader(_fragmentShaderID);
	_console->log("Done creating shaders!");

	_matrixID = glGetUniformLocation(_programID, "MVP");
	_MVP = _projection * _view * _model;
}

void Engine::update() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(_programID);
	glUniformMatrix4fv(_matrixID, 1, GL_FALSE, &_MVP[0][0]);
	// Draw here
	_scene->updateScene();

	glfwSwapBuffers(_window->getWindow());
	glfwPollEvents();
}