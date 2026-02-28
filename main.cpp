#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <cmath>

GLuint Program;
GLuint ProgramStripes;

GLuint VAO_test, VBO_test, EBO_test;
GLuint VAO_square, VBO_square, EBO_square;
GLuint VAO_triangle, VBO_triangle, EBO_triangle;
GLuint VAO_pentagon, VBO_pentagon, EBO_pentagon;
GLuint VAO_cube, VBO_cube, EBO_cube;

GLfloat transformMatrix[16];
GLfloat pivotRot1[16];
GLfloat pivotRot2[16];
GLfloat tmp[16];
GLuint modelLoc;

float pedestalAngleY = 0.0f;
float pedestalAngleCubeY = 0.0f;
float pedestalAngleCenterY = 0.0f;

int task;

std::vector<GLfloat> vertices_test = {
    // x,     y,    z      r,   g,   b
    -0.95f,  0.9f,  0.0f,  1.0f, 0.0f, 0.0f,
    -0.9f,  -0.5f,  0.0f,  0.0f, 1.0f, 0.0f,
    -0.2f,  -0.2f,  0.0f,  0.5f, 0.0f, 1.0f,
    -0.3f,   0.5f,  0.0f,  0.2f, 0.7f, 1.0f,
	 0.1f,   0.95f, 0.0f,  1.0f, 1.0f, 0.0f,
	 0.2f,  -0.85f, 0.0f,  1.0f, 0.0f, 1.0f,
	 0.6f,  -0.3f,  0.0f,  0.0f, 1.0f, 1.0f,
	 0.7f,   0.4f,  0.0f,  0.5f, 0.5f, 0.5f,
	 0.9f,   0.1f,  0.0f,  0.3f, 0.3f, 0.3f
};

std::vector<GLuint> indices_test = {
    0, 1, 2, 3, 4, 5, 6, 7, 8
};

// Вершины: coord (x,y,z) + color (r,g,b)
std::vector<GLfloat> vertices_square = {
    // x,     y,    z      r,   g,   b
    -0.5f, -0.5f, 0.0f,  0.2f, 0.7f, 1.0f, // нижняя левая
     0.5f, -0.5f, 0.0f,  0.2f, 0.7f, 1.0f, // нижняя правая
     0.5f,  0.5f, 0.0f,  0.2f, 0.7f, 1.0f, // верхняя правая
    -0.5f,  0.5f, 0.0f,  0.2f, 0.7f, 1.0f  // верхняя левая
};

std::vector<GLuint> indices_square = {
    0, 1, 2, 3
};

// Вершины: coord (x,y,z) + color (r,g,b)
std::vector<GLfloat> vertices_triangle = {
    // x,     y,    z      r,   g,   b
    -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // нижняя левая
     0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f, // нижняя правая
     0.0f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f, // верхняя
};

std::vector<GLuint> indices_triangle = {
    0, 1, 2
};

std::vector<GLfloat> vertices_pentagon = {
    // x,     y,    z      r,   g,   b
	 0.0f,  -0.5f,   0.0f, 1.0f, 0.0f, 0.0f,
     0.475f,-0.155f, 0.0f, 1.0f, 0.0f, 0.0f,
     0.293f, 0.405f, 0.0f, 1.0f, 0.0f, 0.0f,
    -0.293f, 0.405f, 0.0f, 1.0f, 0.0f, 0.0f,
    -0.475f,-0.155f, 0.0f, 1.0f, 0.0f, 0.0f,
};

std::vector<GLuint> indices_pentagon = {
    0, 1, 2, 3, 4
};

std::vector<GLfloat> vertices_cube = {
    -0.5f, -0.5f,  0.5f,  0.8f, 0.5f, 0.1f,
    -0.5f,  0.5f,  0.5f,  0.8f, 0.5f, 0.1f,
     0.5f, -0.5f,  0.5f,  0.8f, 0.5f, 0.1f,
     0.5f,  0.5f,  0.5f,  0.8f, 0.5f, 0.1f,

    -0.5f, -0.5f, -0.5f,  0.8f, 0.5f, 0.1f,
    -0.5f,  0.5f, -0.5f,  0.8f, 0.5f, 0.1f,
     0.5f, -0.5f, -0.5f,  0.8f, 0.5f, 0.1f,
     0.5f,  0.5f, -0.5f,  0.8f, 0.5f, 0.1f,
};

std::vector<GLint> indices_cube = {
	0, 1, 3, 2, // передняя
	4, 5, 7, 6, // задняя
	0, 4, 5, 1, // левая
	2, 6, 7, 3, // правая
	0, 4, 6, 2, // нижняя
	1, 5, 7, 3  // верхняя
};

GLfloat finalMat[16];
auto Mul = [](GLfloat* a, GLfloat* b, GLfloat* out)
    {
        for (int col = 0; col < 4; col++)
            for (int row = 0; row < 4; row++)
            {
                out[col * 4 + row] = 0.0f;
                for (int k = 0; k < 4; k++)
                    out[col * 4 + row] +=
                    a[k * 4 + row] * b[col * 4 + k];
            }
    };

void CreateTransformMatrix(float angleX, float angleY, float angleZ,
    float scale = 1.0f,
    float tx = 0.0f, float ty = 0.0f, float tz = 0.0f)
{
    GLfloat rx[16] = {
        1,0,0,0,
        0,cos(angleX),-sin(angleX),0,
        0,sin(angleX), cos(angleX),0,
        0,0,0,1
    };

    GLfloat ry[16] = {
        cos(angleY),0,sin(angleY),0,
        0,1,0,0,
        -sin(angleY),0,cos(angleY),0,
        0,0,0,1
    };

    GLfloat rz[16] = {
        cos(angleZ),-sin(angleZ),0,0,
        sin(angleZ), cos(angleZ),0,0,
        0,0,1,0,
        0,0,0,1
    };

    GLfloat s[16] = {
        scale,0,0,0,
        0,scale,0,0,
        0,0,scale,0,
        0,0,0,1
    };

    auto Multiply4x4 = [](GLfloat* a, GLfloat* b, GLfloat* out)
        {
            for (int i = 0; i < 4; i++)
                for (int j = 0; j < 4; j++)
                {
                    out[i * 4 + j] = 0;
                    for (int k = 0; k < 4; k++)
                        out[i * 4 + j] += a[i * 4 + k] * b[k * 4 + j];
                }
        };

    GLfloat rxy[16], rxyz[16], rs[16];
    Multiply4x4(ry, rx, rxy);     // ry * rx
    Multiply4x4(rz, rxy, rxyz);   // rz * (ry*rx)
    Multiply4x4(rxyz, s, rs);     // (R*S)

    // Копируем R*S
    for (int i = 0; i < 16; i++)
        transformMatrix[i] = rs[i];

    // Прямое добавление сдвига в последний столбец
    transformMatrix[12] = tx;
    transformMatrix[13] = ty;
    transformMatrix[14] = tz;
    transformMatrix[15] = 1.0f;
}

void CreatePivotRotationY(float angle, float cx, float cy, float cz, GLfloat* out)
{
    GLfloat Tneg[16] = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        -cx,-cy,-cz,1
    };

    GLfloat RY[16] = {
        cos(angle),0,sin(angle),0,
        0,1,0,0,
        -sin(angle),0,cos(angle),0,
        0,0,0,1
    };

    GLfloat Tpos[16] = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        cx,cy,cz,1
    };

    auto Mul = [](GLfloat* a, GLfloat* b, GLfloat* out)
        {
            for (int i = 0; i < 4; i++)
                for (int j = 0; j < 4; j++)
                {
                    out[i * 4 + j] = 0;
                    for (int k = 0; k < 4; k++)
                        out[i * 4 + j] += a[i * 4 + k] * b[k * 4 + j];
                }
        };

    GLfloat tmp[16];
    Mul(RY, Tneg, tmp);     // R * T(-C)
    Mul(Tpos, tmp, out);    // T(C) * R * T(-C)
}

const char* VS = R"(
#version 330 core
layout(location=0) in vec3 coord;
layout(location=1) in vec3 color;

uniform mat4 model;
out vec3 vColor;

void main() {
    gl_Position = model * vec4(coord,1.0);
    vColor = color;
}
)";

const char* FS = R"(
#version 330 core
in vec3 vColor;
out vec4 color;

uniform bool useUniformColor = false;
uniform vec3 uColor;

void main() {
    if (useUniformColor)
        color = vec4(uColor, 1.0);
    else
        color = vec4(vColor, 1.0);
}
)";

const char* VS_stripes = R"(
#version 330 core
layout(location=0) in vec3 coord;

out vec3 vPosition;

void main() {
    gl_Position = vec4(coord, 1.0); // без матрицы
    vPosition = coord;              // передаём исходную позицию
}
)";

const char* FS_stripes = R"(
#version 330 core
in vec3 vPosition;
out vec4 color;

void main() {     
    float k = 20.0;
    int sum = int((vPosition.x + 0.5) * k);

    if ( (sum - (sum / 2 * 2)) == 0 ) {  
        color = vec4(0.6, 0.8, 1.0, 1.0);    // голубой
    } 
    else {  
        color = vec4(1.0, 1.0, 1.0, 1.0);    // белый
    }
}
)";

void InitShaderColor()
{
    GLuint v = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v, 1, &VS, nullptr);
    glCompileShader(v);

    GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f, 1, &FS, nullptr);
    glCompileShader(f);

    Program = glCreateProgram();
    glAttachShader(Program, v);
    glAttachShader(Program, f);
    glLinkProgram(Program);

    glDeleteShader(v);
    glDeleteShader(f);
}

void InitShaderStripes()
{
    GLuint v = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v, 1, &VS_stripes, nullptr);
    glCompileShader(v);

    GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f, 1, &FS_stripes, nullptr);
    glCompileShader(f);

    ProgramStripes = glCreateProgram();
    glAttachShader(ProgramStripes, v);
    glAttachShader(ProgramStripes, f);
    glLinkProgram(ProgramStripes);

    glDeleteShader(v);
    glDeleteShader(f);
}

void InitShaders()
{
    InitShaderColor();
	InitShaderStripes();
}

void InitTest()
{
    glGenVertexArrays(1, &VAO_test);
    glBindVertexArray(VAO_test);

    glGenBuffers(1, &VBO_test);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_test);
    glBufferData(GL_ARRAY_BUFFER,
        vertices_test.size() * sizeof(GLfloat),
        vertices_test.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO_test);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_test);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices_test.size() * sizeof(GLuint),
        indices_test.data(), GL_STATIC_DRAW);

    // coord
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void InitSquare()
{
    glGenVertexArrays(1, &VAO_square);
    glBindVertexArray(VAO_square);

    glGenBuffers(1, &VBO_square);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_square);
    glBufferData(GL_ARRAY_BUFFER,
        vertices_square.size() * sizeof(GLfloat),
        vertices_square.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO_square);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_square);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices_square.size() * sizeof(GLuint),
        indices_square.data(), GL_STATIC_DRAW);

    // coord
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void InitTriangle()
{
	glGenVertexArrays(1, &VAO_triangle);
	glBindVertexArray(VAO_triangle);

	glGenBuffers(1, &VBO_triangle);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_triangle);
    glBufferData(GL_ARRAY_BUFFER,
        vertices_triangle.size() * sizeof(GLfloat),
		vertices_triangle.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &EBO_triangle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_triangle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		indices_triangle.size() * sizeof(GLuint),
		indices_triangle.data(), GL_STATIC_DRAW);

    // coord
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void InitPentagon()
{
    glGenVertexArrays(1, &VAO_pentagon);
    glBindVertexArray(VAO_pentagon);

    glGenBuffers(1, &VBO_pentagon);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_pentagon);
    glBufferData(GL_ARRAY_BUFFER,
        vertices_pentagon.size() * sizeof(GLfloat),
        vertices_pentagon.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO_pentagon);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_pentagon);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices_pentagon.size() * sizeof(GLuint),
        indices_pentagon.data(), GL_STATIC_DRAW);

    // coord
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void InitCube() {
    glGenVertexArrays(1, &VAO_cube);
    glBindVertexArray(VAO_cube);

    glGenBuffers(1, &VBO_cube);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_cube);
    glBufferData(GL_ARRAY_BUFFER,
        vertices_cube.size() * sizeof(GLfloat),
        vertices_cube.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO_cube);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_cube);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices_cube.size() * sizeof(GLint),
        indices_cube.data(), GL_STATIC_DRAW);

    // coord
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void InitBuffers()
{
    InitTest();
	InitSquare();
	InitTriangle();
	InitPentagon();
    InitCube();
}

void Draw()
{
    if (task != 3)
    {
        GLint useColorLoc = glGetUniformLocation(Program, "useUniformColor");
        glUniform1i(useColorLoc, 0);
    }
    if (task == 0)
    {
        CreateTransformMatrix(0, 0, 0);
        modelLoc = glGetUniformLocation(Program, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, transformMatrix);

        glViewport(0, 600, 200, 200);
        glUseProgram(Program);
        glBindVertexArray(VAO_test);
        glDrawElements(GL_POINTS, indices_test.size(), GL_UNSIGNED_INT, 0);

        glViewport(200, 600, 200, 200);
        glUseProgram(Program);
        glBindVertexArray(VAO_test);
        glDrawElements(GL_LINES, indices_test.size(), GL_UNSIGNED_INT, 0);

        glViewport(400, 600, 200, 200);
        glUseProgram(Program);
        glBindVertexArray(VAO_test);
        glDrawElements(GL_LINE_STRIP, indices_test.size(), GL_UNSIGNED_INT, 0);

        glViewport(600, 600, 200, 200);
        glUseProgram(Program);
        glBindVertexArray(VAO_test);
        glDrawElements(GL_LINE_LOOP, indices_test.size(), GL_UNSIGNED_INT, 0);

        glViewport(800, 600, 200, 200);
        glUseProgram(Program);
        glBindVertexArray(VAO_test);
        glDrawElements(GL_POLYGON, indices_test.size(), GL_UNSIGNED_INT, 0);

        glViewport(1000, 600, 200, 200);
        glUseProgram(Program);
        glBindVertexArray(VAO_test);
        glDrawElements(GL_QUADS, indices_test.size(), GL_UNSIGNED_INT, 0);

        glViewport(1200, 600, 200, 200);
        glUseProgram(Program);
        glBindVertexArray(VAO_test);
        glDrawElements(GL_QUAD_STRIP, indices_test.size(), GL_UNSIGNED_INT, 0);

        glViewport(1400, 600, 200, 200);
        glUseProgram(Program);
        glBindVertexArray(VAO_test);
        glDrawElements(GL_TRIANGLES, indices_test.size(), GL_UNSIGNED_INT, 0);

        glViewport(0, 400, 200, 200);
        glUseProgram(Program);
        glBindVertexArray(VAO_test);
        glDrawElements(GL_TRIANGLE_STRIP, indices_test.size(), GL_UNSIGNED_INT, 0);

        glViewport(200, 400, 200, 200);
        glUseProgram(Program);
        glBindVertexArray(VAO_test);
        glDrawElements(GL_TRIANGLE_FAN, indices_test.size(), GL_UNSIGNED_INT, 0);
    }
    else if (task == 1)
    {
        CreateTransformMatrix(0, 0, 0);

        modelLoc = glGetUniformLocation(Program, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, transformMatrix);

        // Левая половина экрана (квадрат)
        glViewport(0, 0, 800, 800);
        glUseProgram(Program);
        glBindVertexArray(VAO_square);
        glDrawElements(GL_QUADS, indices_square.size(), GL_UNSIGNED_INT, 0);

        // Правая половина экрана (треугольник)
        glViewport(800, 0, 800, 800);
        glUseProgram(Program);
        glBindVertexArray(VAO_triangle);
        glDrawElements(GL_TRIANGLES, indices_triangle.size(), GL_UNSIGNED_INT, 0);
    }
    else if (task == 2)
    {
        CreateTransformMatrix(0, 0, 0);

        modelLoc = glGetUniformLocation(Program, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, transformMatrix);

        glViewport(400, 0, 800, 800);
        glUseProgram(Program);
        glBindVertexArray(VAO_pentagon);
        glDrawElements(GL_TRIANGLE_FAN, indices_pentagon.size(), GL_UNSIGNED_INT, 0);
    }
    else if (task == 2)
    {
        CreateTransformMatrix(0, 0, 0);

        modelLoc = glGetUniformLocation(Program, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, transformMatrix);

        glViewport(400, 0, 800, 800);
        glUseProgram(Program);
        glBindVertexArray(VAO_pentagon);
        glDrawElements(GL_TRIANGLE_FAN, indices_pentagon.size(), GL_UNSIGNED_INT, 0);
    }
    else if (task == 21)
    {
        CreateTransformMatrix(0.5f, 0.5f, 0);

        modelLoc = glGetUniformLocation(Program, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, transformMatrix);

        glViewport(400, 0, 800, 800);
        glUseProgram(Program);
        glBindVertexArray(VAO_cube);
        glDrawElements(GL_QUADS, indices_cube.size(), GL_UNSIGNED_INT, 0);
    }
    else if (task == 22)
    {
        glViewport(400, 0, 800, 800);
        glUseProgram(ProgramStripes);
        glBindVertexArray(VAO_square);
        glDrawElements(GL_QUADS, indices_square.size(), GL_UNSIGNED_INT, 0);
    }
    else if (task == 3)
    {
        glViewport(0, 0, 800, 800);
        glUseProgram(Program);
        glBindVertexArray(VAO_cube);

        GLint useColorLoc = glGetUniformLocation(Program, "useUniformColor");
        GLint colorLoc = glGetUniformLocation(Program, "uColor");
        GLint modelLoc = glGetUniformLocation(Program, "model");

        glUniform1i(useColorLoc, 1);

        // Нижний золотой кубик (чуть темнее)
        CreateTransformMatrix(0.0f, pedestalAngleCubeY, 0.0f, 0.3f, -0.45f, -0.1f);
        CreatePivotRotationY(pedestalAngleCenterY, 0.45f, 0.0f, 0.0f, pivotRot1);
        CreatePivotRotationY(pedestalAngleY, 0.0f, 0.0f, 0.0f, pivotRot2);
        Mul(pivotRot1, transformMatrix, tmp);
        Mul(pivotRot2, tmp, finalMat);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, finalMat);
        glUniform3f(colorLoc, 0.7f, 0.6f, 0.1f);
        glDrawElements(GL_QUADS, indices_cube.size(), GL_UNSIGNED_INT, 0);

        // Верхний золотой кубик
        CreateTransformMatrix(0.0f, pedestalAngleCubeY, 0.0f, 0.3f, -0.45f, 0.2f);
        CreatePivotRotationY(pedestalAngleCenterY, 0.45f, 0.0f, 0.0f, pivotRot1);
        CreatePivotRotationY(pedestalAngleY, 0.0f, 0.0f, 0.0f, pivotRot2);
        Mul(pivotRot1, transformMatrix, tmp);
        Mul(pivotRot2, tmp, finalMat);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, finalMat);
        glUniform3f(colorLoc, 1.0f, 0.84f, 0.0f);
        glDrawElements(GL_QUADS, indices_cube.size(), GL_UNSIGNED_INT, 0);

        // Серебряный кубик слева
        CreateTransformMatrix(0.0f, pedestalAngleCubeY, 0.0f, 0.3f, -0.75f, -0.1f);
        CreatePivotRotationY(pedestalAngleCenterY, 0.45f, 0.0f, 0.0f, pivotRot1);
        CreatePivotRotationY(pedestalAngleY, 0.0f, 0.0f, 0.0f, pivotRot2);
        Mul(pivotRot1, transformMatrix, tmp);
        Mul(pivotRot2, tmp, finalMat);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, finalMat);
        glUniform3f(colorLoc, 0.75f, 0.75f, 0.75f);
        glDrawElements(GL_QUADS, indices_cube.size(), GL_UNSIGNED_INT, 0);

        // Бронзовый кубик справа (меньше)
        CreateTransformMatrix(0.0f, pedestalAngleCubeY, 0.0f, 0.27f, -0.165f, -0.115f);
        CreatePivotRotationY(pedestalAngleCenterY, 0.45f, 0.0f, 0.0f, pivotRot1);
        CreatePivotRotationY(pedestalAngleY, 0.0f, 0.0f, 0.0f, pivotRot2);
        Mul(pivotRot1, transformMatrix, tmp);
        Mul(pivotRot2, tmp, finalMat);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, finalMat);
        glUniform3f(colorLoc, 0.8f, 0.5f, 0.2f);
        glDrawElements(GL_QUADS, indices_cube.size(), GL_UNSIGNED_INT, 0);
    }
}

int main()
{
    sf::Window window(
        sf::VideoMode(1600, 800),
        "Square right side",
        sf::Style::Default,
        sf::ContextSettings(24)
    );

    window.setVerticalSyncEnabled(true);
    glewInit();
	glEnable(GL_DEPTH_TEST);

    InitShaders();
    InitBuffers();

    task = 1;

    while (window.isOpen())
    {
        sf::Event e;
        while (window.pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
                window.close();
            if (e.type == sf::Event::KeyPressed)
            {
                switch (e.key.code)
                {
                case sf::Keyboard::Num1:
                    task = 1;
                    break;
                case sf::Keyboard::Q:
                    task = 0;
                    break;
                case sf::Keyboard::Num2:
                    task = 2;
                    break;
                case sf::Keyboard::W:
                    task = 21;
                    break;
                case sf::Keyboard::E:
                    task = 22;
                    break;
				case sf::Keyboard::Num3:
                    task = 3;
                    break;
                case sf::Keyboard::Z:
                    pedestalAngleCenterY += 0.05f;
                    break; // влево
                case sf::Keyboard::X:
                    pedestalAngleCenterY -= 0.05f;
                    break; // вправо
                case sf::Keyboard::A:
                    pedestalAngleY += 0.05f;
                    break; // влево
                case sf::Keyboard::S:
                    pedestalAngleY -= 0.05f;
                    break; // вправо
                case sf::Keyboard::C:
                    pedestalAngleCubeY += 0.05f;
                    break; // влево
                case sf::Keyboard::V:
                    pedestalAngleCubeY -= 0.05f;
                    break; // вправо
                }

                sf::Vector2u size = window.getSize();
                if (task == 3)
                {
                    if (size.x != 800 || size.y != 800)
                    {
                        window.create(sf::VideoMode(800, 800), "Pedestal", sf::Style::Default, sf::ContextSettings(24));
                        glewInit();
                        glEnable(GL_DEPTH_TEST);
                        InitShaders();
                        InitBuffers();
                        pedestalAngleY = 0.0f;
                        pedestalAngleCubeY = 0.0f;
                        pedestalAngleCenterY = 0.0f;
                    }
                }
                else
                {
                    if (size.x != 1600 || size.y != 800)
                    {
                        window.create(sf::VideoMode(1600, 800), "Square right side", sf::Style::Default, sf::ContextSettings(24));
                        glewInit();
                        glEnable(GL_DEPTH_TEST);
                        InitShaders();
                        InitBuffers();
                    }
                }
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Draw();

        window.display();
    }
}
