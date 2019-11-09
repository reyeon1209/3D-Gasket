
/*
 * 반드시 vertexbuffer와 arraybuffer를 사용
 * 2D Gasket 구현하기
 */

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "InitShader.h"
#include "vec.h"
#include <iostream>

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))
#define Subdivision 5
#define NumTriangle 729		// 3^6
#define NumVertices 2187	// NumTriangle*3 = 3^7

typedef vec2 point2;

point2 points[NumVertices];
int index = 0;

void triangle (point2 points[], point2 a, point2 b, point2 c);
void divide_triangle (point2 points[], point2 a, point2 b, point2 c, int count);
void display();

int main()
{
	GLFWwindow* window;
	if (!glfwInit()) // glfw 라이브러리 초기화
		return -1;

	window = glfwCreateWindow(512, 512, "Hw1_2D", NULL, NULL);	// window 생성
	if (!window)	// window 생성 실패 시 프로그램 종료
	{
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window); // window context 생성
	
	if (glewInit() != GLEW_OK) std::cout << "Error\n";	// glew 라이브러리 초기화 및 초기화 실패 시 에러 메세지
														// glew 라이브러리는 반드시 window context 생성 후 초기화 아니면 에러

	////////////////////////////////////////////////////////////////////////////////////////////////////

	point2 v[3] = {
       point2(-1.0, -1.0),
       point2( 0.0,  1.0),
       point2( 1.0, -1.0)
    };

	divide_triangle(points, v[0], v[1], v[2], Subdivision);
	
	GLuint va;	// vertex array 선언, 메모리 할당, 바인드
	glGenVertexArrays(1, &va);
	glBindVertexArray(va);
	
	GLuint vb;	// vertex buffer 선언, 메모리 할당, 바인드, data가 구성된 형식에 맞게 buffer data 준비
	glGenBuffers(1, &vb);
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

	GLuint program = InitShader("vshader.glsl", "fshader.glsl");	// shader program 가져오기
	glUseProgram(program);	// 어떤 shader program을 사용할 것인지

	GLuint locPosition = glGetAttribLocation(program, "vPosition");	// position vertex에 대한 정보를 shader program 안의 어떤 변수와 연결시킬 것인가
	glEnableVertexAttribArray(locPosition);
	glVertexAttribPointer(locPosition, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); // vb의 0부터 vertex의 position에 대한 정보를 가져옴

	////////////////////////////////////////////////////////////////////////////////////////////////////

	// window 창이 종료될 때까지 무한루프 
	while (!glfwWindowShouldClose(window))
	{
		display();
		
		glfwSwapBuffers(window);	// front buffer와 back buffer 교체
		glfwPollEvents();
	}

	glfwTerminate();


	return 0;
}

void triangle (point2 points[], point2 a, point2 b, point2 c)
{
	points[index] = a;
	points[index+1] = b;
	points[index+2] = c;
	
	index += 3;
}

void divide_triangle (point2 points[], point2 a, point2 b, point2 c, int m)
{
	if (m > 0)
	{
		point2 ab = (a + b) / 2.0;
        point2 ac = (a + c) / 2.0;
        point2 bc = (b + c) / 2.0;

		divide_triangle(points, a, ab, ac, m-1);
        divide_triangle(points, c, ac, bc, m-1);
        divide_triangle(points, b, bc, ab, m-1);
	}

	else 
		triangle(points, a, b, c);
}

void display()
{
	glClearColor(1.0, 1.0, 1.0, 1.0); // 흰 배경으로 설정
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	// 삼각형 정보가 들어있는 indices 배열을 차례로 읽어 3개씩 하나의 삼각형을 위한 index으로 묶고,
	// index에 해당하는 vertex 정보를 positions 배열에서 찾아오는 방식.
	glFlush();
}
