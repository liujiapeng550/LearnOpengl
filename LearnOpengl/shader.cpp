#include "shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath) {
  //1.检索顶点和片段着色器源代码路径
  std::string vertexCode;
  std::string fragmentCode;
  std::string geometryCode;
  std::ifstream  vShaderFile;
  std::ifstream  fShaderFile;
  std::ifstream  gShaderFile;
  //确保输入对象能够抛出异常
  vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    //open file
    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);
    std::stringstream vShaderStream, fShaderStream;
    //read
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    //close file handlers
    vShaderFile.close();
    fShaderFile.close();
    //convert stream into string
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
    if (geometryPath!=nullptr) {
      gShaderFile.open(geometryPath);
      std::stringstream gShaderStream;
      gShaderStream << gShaderFile.rdbuf();
      gShaderFile.close();
      geometryCode = gShaderStream.str();
    }
  } catch (std::ifstream::failure e) {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
  }

  const char* vShaderCode = vertexCode.c_str();
  const char* fshaderCode = fragmentCode.c_str();
  //2.compile shaders
  unsigned int vertex, fragment;
  //vertex shader
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex,1,&vShaderCode,NULL);
  glCompileShader(vertex);
  checkCompileErrors(vertex,"VERTEX");

  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment,1,&fshaderCode,NULL);
  glCompileShader(fragment);
  checkCompileErrors(fragment,"FRAGMENT");

  unsigned int geometry;
  if (geometryPath!=nullptr) {
    const char * gShaderCode = geometryCode.c_str();
    geometry = glCreateShader(GL_GEOMETRY_SHADER);
    glCompileShader(geometry);
    checkCompileErrors(geometry,"GEO");
  }
  //shader program
  ID = glCreateProgram();
  glAttachShader(ID,vertex);
  glAttachShader(ID,fragment);
  if (geometryPath != nullptr) {
    glAttachShader(ID,geometry);
  }
  glLinkProgram(ID);
  checkCompileErrors(ID,"PROGRAM");

  glDeleteShader(vertex);
  glDeleteShader(fragment);
  if (geometryPath != nullptr)
    glDeleteShader(geometry);
}

void Shader::checkCompileErrors(GLuint shader,std::string type) {
  GLint success;
  GLchar infoLog[1024];
  if (type != "PROGRAM") {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader,1024,NULL,infoLog);
      std::cout << "ERROR" << type << '\n' << infoLog << "\n----------------" << std::endl;
    }
  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shader, 1024, NULL, infoLog);
      std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
    }
  }
}