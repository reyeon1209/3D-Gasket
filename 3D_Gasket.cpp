
/*
 * 반드시 vertexbuffer와 arraybuffer를 사용
 * 3D Gasket 구현하기
 */

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "InitShader.h"
#include "vec.h"
#include <iostream>

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))
#define Subdivision 5
#define NumTetrahedrons 1024	// 4^5
#define NumTriangles 4096		// NumTetrahedrons*4 = 4^6
#define NumVertices 12288		// NumTriangles*3 = 3*(4^6)

typedef vec3 point3;
typedef vec3 color3;

point3 points[NumVertices];
color3 colors[NumVertices];
int index = 0;

void triangle (point3 &a, point3 &b, point3 &c, int colorindex);
void tetra (point3 &a, point3 &b, point3 &c, point3 &d);
void divide_tetra (point3 &a, point3 &b, point3 &c, point3 &d, int m);
void display();

int main()
{
	GLFWwindow* window;
	if (!glfwInit()) // glfw 라이브러리 초기화
		return -1;

	window = glfwCreateWindow(512, 512, "Hw1_3D", NULL, NULL);	// window 생성
	if (!window)	// window 생성 실패 시 프로그램 종료
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window); // window context 생성
	
	if (glewInit() != GLEW_OK) std::cout << "Error\n";	// glew 라이브러리 초기화 및 초기화 실패 시 에러 메세지
														// glew 라이브러리는 반드시 window context 생성 후 초기화 아니면 에러

	////////////////////////////////////////////////////////////////////////////////////////////////////

	point3 v[4] = {
		point3(0.0, 0.0, -1.0),
		point3(0.0, 1, 0.3),
		point3(-1.0, -0.5, 0.3),
		point3(1.0, -0.5, 0.3)
	};

	divide_tetra(v[0], v[1], v[2], v[3], Subdivision);

	GLuint va;	// vertex array 선언, 메모리 할당, 바인드
	glGenVertexArrays(1, &va);
	glBindVertexArray(va);

	GLuint vb;	// vertex buffer 선언, 메모리 할당, 바인드, data가 구성된 형식에 맞게 buffer data 준비
	glGenBuffers(1, &vb);
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW);	// vertex의 position에 대한 정보와 color에 대한 정보를 저장할 수 있는 크기의 공간 준비
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);	// 공간의 처음(0)부터 sizeof(positions) 크기까지는 positions 배열의 정보를 저장
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);	// 공간의 sizeof(positions)부터 sizeof(colors) 크기까지는 colors 배열의 정보를 저장

	GLuint program = InitShader("vshader.glsl", "fshader.glsl");	// shader program 가져오기
	glUseProgram(program);	// 어떤 shader program을 사용할 것인지

	GLuint locPosition = glGetAttribLocation(program, "vPosition");	// position vertex에 대한 정보를 shader program 안의 어떤 변수와 연결시킬 것인가
	glEnableVertexAttribArray(locPosition);
	glVertexAttribPointer(locPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); // vb의 0부터 vertex의 position에 대한 정보를 가져옴

	////////////////////////////////////////////////////////////////////////////////////////////////////

	GLuint locColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(locColor);
	glVertexAttribPointer(locColor, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));	// vb의 sizeof(positions) 위치에서부터 color에 대한 정보를 가져옴

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

void triangle (point3 &a, point3 &b, point3 &c, int colorindex)
{
	color3 base_colors[] = {
		color3(1.0, 0.0, 0.0),
		color3(0.0, 1.0, 0.0),
		color3(0.0, 0.0, 1.0),
		color3(0.0, 0.0, 0.0)
	};

	points[index]	= a;	colors[index]	= base_colors[colorindex];
	points[index+1] = b;	colors[index+1] = base_colors[colorindex];
	points[index+2] = c;	colors[index+2] = base_colors[colorindex];
	
	index += 3;
}

void tetra (point3 &a, point3 &b, point3 &c, point3 &d)
{
	int colorindex = 0;
	triangle(a, b, c, colorindex);
	triangle(a, c, d, colorindex+1);
	triangle(a, d, b, colorindex+2);
	triangle(b, d, c, colorindex+3);
}


void divide_tetra (point3 &a, point3 &b, point3 &c, point3 &d, int m)
{
	if (m > 0)
	{
		point3 mid[6];

		mid[0] = (a + b) / 2.0;
		mid[1] = (a + c) / 2.0;
		mid[2] = (a + d) / 2.0;
		mid[3] = (b + c) / 2.0;
		mid[4] = (c + d) / 2.0;
		mid[5] = (b + d) / 2.0;

		divide_tetra(a, mid[0], mid[1], mid[2], m-1);
		divide_tetra(mid[0], b, mid[3], mid[5], m-1);
		divide_tetra(mid[1], mid[3], c, mid[4], m-1);
		divide_tetra(mid[2], mid[4], mid[5], d, m-1);
	}

	else
		tetra(a, b, c, d);
}

void display()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0); // 흰 배경으로 설정
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	// 삼각형 정보가 들어있는 indices 배열을 차례로 읽어 3개씩 하나의 삼각형을 위한 index으로 묶고,
	// index에 해당하는 vertex 정보를 positions 배열에서 찾아오는 방식.
	glFlush();
}
