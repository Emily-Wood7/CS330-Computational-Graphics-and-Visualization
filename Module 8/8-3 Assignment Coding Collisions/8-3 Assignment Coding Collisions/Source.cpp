/*
* Emily Wood
* CS 330
* 8-3 Assignment
* Coding Collisions
* 6/24/2023
*/

#include <GLFW\glfw3.h>
#include "linmath.h"
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <vector>
#include <windows.h>
#include <time.h>

using namespace std;

const float DEG2RAD = 3.14159 / 180;

void processInput(GLFWwindow* window);

enum BRICKTYPE { REFLECTIVE, DESTRUCTABLE };
enum ONOFF { ON, OFF };
float brickHealth = 2;

class Brick
{
public:
	float red, green, blue;
	float x, y, width;
	BRICKTYPE brick_type;
	ONOFF onoff;


	Brick(BRICKTYPE bt, float xx, float yy, float ww, float rr, float gg, float bb)
	{
		brick_type = bt; x = xx; y = yy, width = ww; red = rr, green = gg, blue = bb;
		onoff = ON;
	};

	void drawBrick()
	{
		if (onoff == ON)
		{
			double halfside = width / 2;

			glColor3d(red, green, blue);
			glBegin(GL_POLYGON);

			glVertex2d(x + halfside, y + halfside);
			glVertex2d(x + halfside, y - halfside);
			glVertex2d(x - halfside, y - halfside);
			glVertex2d(x - halfside, y + halfside);

			glEnd();
		}
	}
};


class Circle
{
public:
	float red, green, blue;
	float radius;
	float x;
	float y;
	float speed = 0.03;
	int direction; // 1=up 2=right 3=down 4=left 5 = up right   6 = up left  7 = down right  8= down left

	Circle(double xx, double yy, double rr, int dir, float rad, float r, float g, float b)
	{
		x = xx;
		y = yy;
		radius = rr;
		red = r;
		green = g;
		blue = b;
		radius = rad;
		direction = dir;
	}

	void CheckCollision(Brick* brk)
	{
		// check for collisions with bricks
		if (brk->brick_type == REFLECTIVE)
		{
			if ((x > brk->x - brk->width && x <= brk->x + brk->width) && (y > brk->y - brk->width && y <= brk->y + brk->width))
			{
				direction = ChangeDirection();
				x = x + 0.03;
				y = y + 0.04;

				// brick will change color each hit indicating the health of the brick
				// green is full health, yellow is half, red is zero and will be destroyed with next hit
				if (brickHealth == 2)
				{
					brk->red = 1.0;
					brk->green = 1.0;
					brk->blue = 0.0;
					brickHealth =- 1;
				}
				else if (brickHealth == 1)
				{
					brk->red = 1.0;
					brk->green = 0.0;
					brk->blue = 0.0;
					brickHealth =- 1;
				}
				else if (brickHealth <= 0)
				{
					brk->brick_type = DESTRUCTABLE;
				}

			}
		
		}
		else if (brk->brick_type == DESTRUCTABLE)
		{
			if ((x > brk->x - brk->width && x <= brk->x + brk->width) && (y > brk->y - brk->width && y <= brk->y + brk->width))
			{
				brk->onoff = OFF;
			}
		}
	}

	// check for collisions with other circles
	// change color upon collision
	void CircleCollision(Circle* cir)
	{
		// check for collision with circles
		if ((x > cir->x - cir->radius && x <= cir->x + cir->radius) && (y > cir->y - cir->radius && y <= cir->y + cir->radius))
		{
			cir->red = rand() / 10000;
			cir->green = rand() / 10000;
			cir->blue = rand() / 10000;
		}
	}

	int GetRandomDirection()
	{
		return (rand() % 8) + 1;
	}

	// circle physics
	int ChangeDirection()
	{
		// if up, go down
		if (direction == 1)
		{
			return 3;
		}

		// if right, go left
		if (direction == 2)
		{
			return 4;
		}

		// if down, go up
		if (direction == 3)
		{
			return 1;
		}

		// if left, go right
		if (direction == 4);
		{
			return 2;
		}

		// if up right, go down left
		if (direction == 5)
		{
			return 8;
		}

		// if up left, go down right
		if (direction == 6)
		{
			return 7;
		}

		// if down right, go up left
		if (direction == 7)
		{
			return 6;
		}

		// if down left, go up right
		if (direction == 8)
		{
			return 5;
		}
	}

	void MoveOneStep()
	{
		if (direction == 1 || direction == 5 || direction == 6)  // up
		{
			if (y > -1 + radius)
			{
				y -= speed;
			}
			else
			{
				direction = GetRandomDirection();
			}
		}

		if (direction == 2 || direction == 5 || direction == 7)  // right
		{
			if (x < 1 - radius)
			{
				x += speed;
			}
			else
			{
				direction = GetRandomDirection();
			}
		}

		if (direction == 3 || direction == 7 || direction == 8)  // down
		{
			if (y < 1 - radius) {
				y += speed;
			}
			else
			{
				direction = GetRandomDirection();
			}
		}

		if (direction == 4 || direction == 6 || direction == 8)  // left
		{
			if (x > -1 + radius) {
				x -= speed;
			}
			else
			{
				direction = GetRandomDirection();
			}
		}
	}

	void DrawCircle()
	{
		glColor3f(red, green, blue);
		glBegin(GL_POLYGON);
		for (int i = 0; i < 360; i++) {
			float degInRad = i * DEG2RAD;
			glVertex2f((cos(degInRad) * radius) + x, (sin(degInRad) * radius) + y);
		}
		glEnd();
	}
};


vector<Circle> world;


int main(void) {
	srand(time(NULL));

	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	GLFWwindow* window = glfwCreateWindow(600, 600, "Emily Wood 8-1 Assignment: Coding Collisions", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	Brick brick1(REFLECTIVE, -0.75, 0.75, 0.2, 0, 1, 0);
	Brick brick2(REFLECTIVE, -0.25, 0.75, 0.2, 0, 1, 0);
	Brick brick3(REFLECTIVE, 0.25, 0.75, 0.2, 0, 1, 0);
	Brick brick4(REFLECTIVE, 0.75, 0.75, 0.2, 0, 1, 0);
	Brick brick5(REFLECTIVE, -0.75, 0.0, 0.2, 0, 1, 0);
	Brick brick6(REFLECTIVE, -0.25, 0.0, 0.2, 0, 1, 0);
	Brick brick7(REFLECTIVE, 0.25, 0.0, 0.2, 0, 1, 0);
	Brick brick8(REFLECTIVE, 0.75, 0.0, 0.2, 0, 1, 0);

	while (!glfwWindowShouldClose(window)) {
		//Setup View
		float ratio;
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);

		processInput(window);

		//Movement
		for (int i = 0; i < world.size(); i++)
		{
			world[i].CheckCollision(&brick1);
			world[i].CheckCollision(&brick2);
			world[i].CheckCollision(&brick3);
			world[i].CheckCollision(&brick4);
			world[i].CheckCollision(&brick5);
			world[i].CheckCollision(&brick6);
			world[i].CheckCollision(&brick7);
			world[i].CheckCollision(&brick8);
			world[i].CircleCollision(&world[i]);
			world[i].MoveOneStep();
			world[i].DrawCircle();

		}

		brick1.drawBrick();
		brick2.drawBrick();
		brick3.drawBrick();
		brick4.drawBrick();
		brick5.drawBrick();
		brick6.drawBrick();
		brick7.drawBrick();
		brick8.drawBrick();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate;
	exit(EXIT_SUCCESS);
}


void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		double r, g, b;
		r = rand() / 10000;
		g = rand() / 10000;
		b = rand() / 10000;
		Circle B(0, -0.90, 02, 2, 0.05, r, g, b);
		world.push_back(B);
	}
}