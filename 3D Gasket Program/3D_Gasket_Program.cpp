/*
   PLUS(+) 대신 EQUAL(=) 버튼을 누르면 사면체 크기가 증가하게 구현
*/

#include <GL/glew.h>
#include <GLFW\glfw3.h>
#include "InitShader.h"
#include "vec.h"
#include "mat.h"
#include <iostream>
#include <Windows.h>

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

unsigned int width = 512;
unsigned int height = 512;

GLuint vao;
GLuint buffer;
GLuint simpleShader;

int NumSubdivides;
int NumTriangles;
int NumVertices;

vec3* points;
vec3* colors;
int index = 0;
int dNum = 2;
int rNum = 0;
int col[4] = { 0, 1, 2, 3 };
float theta = 0.0f;
float scale = 0.5f;
float scaleUnit = 0.05f;

vec3 baseVertices[] =
{
   vec3(0.0f, 0.0f, 1.0f),
   vec3(0.0f, 0.942809f, -0.333333f),
   vec3(-0.816497f, -0.471405f, -0.333333f),
   vec3(0.816497f, -0.471405f, -0.333333f)
};

vec3 baseColors[] =
{
   vec3(0.0f, 0.0f, 0.0f),
   vec3(1.0f, 0.0f, 0.0f),
   vec3(0.0f, 1.0f, 0.0f),
   vec3(0.0f, 0.0f, 1.0f),

   vec3(0.745098f, 0.745098f, 0.745098f)   // 회색
};

void init();
void display();
void idle();
void gasketKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void gasketMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void divide_triangle(vec3 a, vec3 b, vec3 c, unsigned int cnt, unsigned int coli);
void triangle(vec3 a, vec3 b, vec3 c, unsigned int coli);
void setGasketVertices(unsigned int col1, unsigned int col2, unsigned int col3, unsigned int col4);
void changeColor(int num);
void changeScale(bool isPLUS);

int main()
{
   GLFWwindow* window;
   if (!glfwInit())
      return -1;

   window = glfwCreateWindow(width, height, "HW2", NULL, NULL);
   if (!window)
   {
      glfwTerminate();
      return -1;
   }
   glfwMakeContextCurrent(window);

   if (glewInit() != GLEW_OK) std::cout << "Error\n";

   ///////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////

   init();


   glfwSetKeyCallback(window, gasketKeyCallback);
   glfwSetMouseButtonCallback(window, gasketMouseButtonCallback);

   ///////////////////////////////////////////////////////////////////////////
   // Loop until the user closes the window 
   while (!glfwWindowShouldClose(window))
   {
      display();
      idle();
      Sleep(5);

      glfwSwapBuffers(window);
      glfwPollEvents();
   }
   glfwTerminate();
   free(points);
   free(colors);
   return 0;
}

void init()
{
   NumSubdivides = dNum;
   NumTriangles = 4 * pow(4.0f, NumSubdivides);
   NumVertices = 3 * NumTriangles;

   points = (vec3*)malloc(sizeof(vec3)*NumVertices);
   colors = (vec3*)malloc(sizeof(vec3)*NumVertices);

   setGasketVertices(col[0], col[1], col[2], col[3]);

   // set gasket vertices

   glGenVertexArrays(1, &vao);
   glBindVertexArray(vao);

   glGenBuffers(1, &buffer);
   glBindBuffer(GL_ARRAY_BUFFER, buffer);
   glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(vec3)*NumVertices, NULL, GL_STATIC_DRAW);
   glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3)*NumVertices, points);
   glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec3)*NumVertices, sizeof(vec3)*NumVertices, colors);

   simpleShader = InitShader("vshader.glsl", "fshader.glsl");
   glUseProgram(simpleShader);

   GLuint vPosition = glGetAttribLocation(simpleShader, "vPosition");
   glEnableVertexAttribArray(vPosition);
   glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

   GLuint vColor = glGetAttribLocation(simpleShader, "vColor");
   glEnableVertexAttribArray(vColor);
   glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vec3)*NumVertices));

   GLuint uTmp = glGetUniformLocation(simpleShader, "uTmp");
   glUniform1i(uTmp, 1);

   glEnable(GL_DEPTH_TEST);
   glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void display()
{

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

   //glViewport(0, 0, width/2.0f, height/2.0f);

   glBindVertexArray(vao);
   glBindBuffer(GL_ARRAY_BUFFER, buffer);
   glUseProgram(simpleShader);

   mat4 rotation = RotateX(0);

   switch (rNum)
   {
      theta = 0;

   case 0:
      rotation = RotateX(0.0f);
      break;
   case 1:
      rotation = RotateZ(theta);
      break;
   case 2:
      rotation = RotateX(theta);
      break;
   case 3:
      rotation = RotateY(theta); 
      break;
   }

   mat4 scaling = Scale(scale);
   GLuint uMat = glGetUniformLocation(simpleShader, "uMat");
   glUniformMatrix4fv(uMat, 1, GL_FALSE, rotation*scaling);

   glDrawArrays(GL_TRIANGLES, 0, NumVertices);

}

void idle()
{
   theta += 0.5f;
}

void gasketKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   if (action == GLFW_PRESS)
   {
      switch (key)
      {
      case GLFW_KEY_1:
      {
         std::cout << "Key 1 is pressed\n";
         changeColor(1);
         break;
      }

      case GLFW_KEY_2:
      {
         std::cout << "Key 2 is pressed\n";
         changeColor(2);
         break;
      }

      case GLFW_KEY_3:
      {
         std::cout << "Key 3 is pressed\n";
         changeColor(3);
         break;
      }

      case GLFW_KEY_4:
      {
         std::cout << "Key 4 is pressed\n";
         changeColor(4);
         break;
      }

      case GLFW_KEY_EQUAL:
      {
         std::cout << "Key EQUAL is pressed\n";
         changeScale(true);
         break;
      }

      case GLFW_KEY_MINUS:
      {
         std::cout << "Key MINUS is pressed\n";
         changeScale(false);
         break;
      }

      case GLFW_KEY_Q:
      {
         std::cout << "Key Q is pressed\n";
         std::cout << "Exit the program\n";
         exit(0);
      }

      case GLFW_KEY_U:
      {
         std::cout << "Key U is pressed\n";
         dNum++;
         init();
         break;
      }

      case GLFW_KEY_D:
      {
         std::cout << "Key D is pressed\n";
         if (dNum != 0)
         {
            dNum--;
            init();
         }
         break;
      }
	  }
      setGasketVertices(col[0], col[1], col[2], col[3]);
   }
}

void gasketMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
   if (action == GLFW_PRESS)
   {
      switch (button)
      {
      case GLFW_MOUSE_BUTTON_LEFT:
      {
         std::cout << "Mouse Left Button is pressed\n";

         for (int i = 0; i < 4; i++)
            col[i] = (col[i] + 1) % 4;

         break;
      }

      case GLFW_MOUSE_BUTTON_RIGHT:
      {
         std::cout << "Mouse Right Button is pressed\n";
         rNum = (rNum + 1) % 4;
         break;
      }
      }

      setGasketVertices(col[0], col[1], col[2], col[3]);
   }
}

void divide_triangle(vec3 a, vec3 b, vec3 c, unsigned int cnt, unsigned coli)
{
   if (cnt > 0)
   {
      vec3 ab = (a + b) / 2.0f;
      vec3 ac = (a + c) / 2.0f;
      vec3 bc = (c + b) / 2.0f;

      triangle(ab, ac, bc, 4);   // 회색

      divide_triangle(a, ab, ac, cnt - 1, coli);
      divide_triangle(c, ac, bc, cnt - 1, coli);
      divide_triangle(b, bc, ab, cnt - 1, coli);
   }

   else
   {
      triangle(a, b, c, coli);
      return;
   }
}

void triangle(vec3 a, vec3 b, vec3 c, unsigned int coli)
{
   points[index] = a;    colors[index++] = baseColors[coli];
   points[index] = b;    colors[index++] = baseColors[coli];
   points[index] = c;    colors[index++] = baseColors[coli];
}

void setGasketVertices(unsigned int col1, unsigned int col2, unsigned int col3, unsigned int col4)
{
   index = 0;
   divide_triangle(baseVertices[0], baseVertices[1], baseVertices[2], NumSubdivides, col1);
   divide_triangle(baseVertices[3], baseVertices[2], baseVertices[1], NumSubdivides, col2);
   divide_triangle(baseVertices[0], baseVertices[3], baseVertices[1], NumSubdivides, col3);
   divide_triangle(baseVertices[3], baseVertices[2], baseVertices[0], NumSubdivides, col4);

   glBindVertexArray(vao);

   glBindBuffer(GL_ARRAY_BUFFER, buffer);

   glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3)*NumVertices, points);
   glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec3)*NumVertices, sizeof(vec3)*NumVertices, colors);
}


void changeColor(int num)
{
   col[0] = num - 1;

   for (int i = 1; i < 4; i++)
      col[i] = (col[i - 1] + 1) % 4;
}

void changeScale(bool isPLUS)
{
   if (isPLUS)
      scale += scaleUnit;
   else
      scale -= scaleUnit;

   if (scale <= 0.1f)   scaleUnit *= -1.0;
   else if (scale >= 0.9f)   scaleUnit *= -1.0;
}
