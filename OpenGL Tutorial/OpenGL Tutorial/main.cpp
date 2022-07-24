//OpenGL Tutorial for personal study(Code gonna be messy)
#include <GL/glew.h>
#include <GL/GL.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
x;\
ASSERT(GLLogCall(#x,__FILE__, __LINE__))

static void GLClearError() {
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line) {
	while (GLenum error = glGetError()) {
		std::cout << "[OpenGL Error] (" << error << "):" << function << " " << file << " : " << line << std::endl;
		return false;
	}
}

struct ShaderSource {
	std::string Vertex;
	std::string Fragment;
};

enum TYPE {
	NONE = -1, VERTEX = 0, FRAGMENT = 1
};

static ShaderSource ParseShaderSource(const std::string& filePath) {
	std::fstream fs(filePath);
	std::stringstream ss[2];
	std::string line;
	
	TYPE type = TYPE::NONE;

	if (!fs.is_open()) {
		std::cout << "There is no file" << std::endl;
		return {"", ""};
	}
	else {
		while (std::getline(fs, line)) {
			if (line.find("#shader") != std::string::npos) {
				if (line.find("vertex") != std::string::npos) {
					type = TYPE::VERTEX;
				}
				else if (line.find("fragment") != std::string::npos) {
					type = TYPE::FRAGMENT;
				}
			}
			else {
				ss[(int)type] << line << '\n';
			}
		}
	}

	return { ss[0].str(), ss[1].str() };
}



static unsigned int CompileShader(unsigned int type, const std::string& source) {
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = new char[length];
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") 
			<< " shader!" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

static unsigned int CreateShader(const std::string& vertex, const std::string& fragment) {
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertex);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragment);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

int main() {
	GLFWwindow* window;

	if (!glfwInit()) {
		std::cout << "GLFW intitiation error" << std::endl;
		return -1;
	}

	window = glfwCreateWindow(640, 540, "Hello OpenGL", NULL, NULL);

	if (!window) {
		std::cout << "GLFW window creation error" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW initiation error" << std::endl;
		return -1;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;

	float positions[] = {
		-0.5f, -0.5f,
		0.5f, -0.5f,
		0.5f,  0.5f,
		-0.5f, 0.5f
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	unsigned int vao;
	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//vertex buffer
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions) * sizeof(float), positions, GL_STATIC_DRAW);

	//vertex layout
	glEnableVertexArrayAttrib(vao ,0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0);

	//index buffer
	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	//shader
	ShaderSource source = ParseShaderSource("someShader.shader");
	unsigned int shader = CreateShader(source.Vertex, source.Fragment);
	glUseProgram(shader);

	int location = glGetUniformLocation(shader, "u_Color");
	float r = 0.0f;
	float increment = 0.05f;

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);

		GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));
		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr););

		if (r > 1)
			increment = -0.05f;
		else if (r < 0.0f)
			increment = 0.05f;

		r += increment;

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glDeleteProgram(shader);

	glfwTerminate();
	
	return 0;
}

//memo : what i studied

/*
* 1.Setting : You need two libraries called glfw(it is about making window) and glew(for using opengl function and variables)
* 2.How to use glfw : it is almost same as mfc programming : create window -> add it to scheduler -> pop up the window(poll events and draw things) 
* just opengl course(not making game engine)
* vertex buffer and shader needed for drawing triangles
* vertex buffer : just buffer(at this point), for gpu(bunch of data)
how to read the data, and draw it(something to tell the gpu : program for gpu)
which buffer and which shader you gonna chose 
we need to know layout of data(what the data means)
vertex = more than position,but point itself
shader :  program on gpu : vertex vs fragment
element = draw something(ptr for vertex buffer(?))
error code : black screen = panic
glGetError(), glDebugMessageCallBack
uniform : way c++ -> shader program(cpu -> gpu) vs vertex buffer AttribPointer
*/