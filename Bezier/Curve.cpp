#include "stdafx.h"
#include "Curve.h"
#include <GLEW\glew.h>

//构造函数
Curve::Curve(int _maxSize, int _m):maxSize(_maxSize), m(_m), currentSize(0), isUpdate(false), isAnimation(false), aniTime(0) {
	space = 1.0 / (double)(m + 1);
	ctrlPoint = new float[maxSize * 3];
	Bezier = new float[m * 3];
	Anibezier = new float[maxSize * 3];
	common = new point2[maxSize];
	//openGL初始化
	GLInit();
}

//初始化
void Curve::GLInit() {
	//openGL
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, m * 3 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	
	glGenVertexArrays(1, &cvao);
	glBindVertexArray(cvao);
	glGenBuffers(1, &cvbo);
	glBindBuffer(GL_ARRAY_BUFFER, cvbo);
	glBufferData(GL_ARRAY_BUFFER, maxSize * 3 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	glGenVertexArrays(1, &avao);
	glBindVertexArray(avao);
	glGenBuffers(1, &avbo);
	glBindBuffer(GL_ARRAY_BUFFER, avbo);
	glBufferData(GL_ARRAY_BUFFER, maxSize * 3 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

//析构函数
Curve::~Curve() {
	//对数组进行释放
	delete[] Bezier;
	delete[] common;
	delete[] ctrlPoint;
	delete[] Anibezier;

	//删除
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &cvao);
	glDeleteBuffers(1, &cvbo);
	glDeleteVertexArrays(1, &avao);
	glDeleteBuffers(1, &avbo);
}

//组合数计算
double Curve::C(int n, int m) {
	//输入合法性判断
	if (m > n)
		return 0;

	//C(n, m) = C(n, n-m)
	if (m < n / 2.0)
		m = n - m;

	double first = 0;
	for (int i = m + 1; i <= n; i++)
		first += log((double)i);
	double second = 0;
	int upper = n - m;
	for (int i = 2; i <= upper; i++)
		second += log((double)i);

	return exp(first - second);
}

//根据给定的t计算点
point2 Curve::B(double t, int n, point2* common) {
	double x = 0, y = 0;
	for (int i = 0; i <= n; i++) {
		double c = pow(1 - t, n - i) * pow(t, i);
		x += c * common[i].x;
		y += c * common[i].y;
	}
	return point2(x, y);
}

//根据m值生成整条Bezier曲线
void Curve::ComputeBezier() {
	//对公共部分进行计算
	for (int i = 0; i < currentSize; i++) {
		double c = C(currentSize - 1, i);
		common[i].x = c * ctrlPoint[i * 3 + 0];
		common[i].y = c * ctrlPoint[i * 3 + 1];
	}

	//根据t依次计算中间的m个点
	for (int i = 1; i <= m; i++) {
		double t = i * space;
		point2 point = B(t, currentSize - 1, common);
		Bezier[(i - 1) * 3 + 0] = point.x;
		Bezier[(i - 1) * 3 + 1] = point.y;
		Bezier[(i - 1) * 3 + 2] = 0;
	}
}

//添加控制点
bool Curve::push(float x, float y) {
	if (currentSize < maxSize) {
		ctrlPoint[currentSize * 3 + 0] = x;
		ctrlPoint[currentSize * 3 + 1] = y;
		ctrlPoint[currentSize * 3 + 2] = 0;
		currentSize++;
		isUpdate = true;
		aniTime = 0;
		return true;
	}
	return false;
}

//删除控制点
bool Curve::pop() {
	if (currentSize > 0) {
		currentSize--;
		isUpdate = true;
		aniTime = 0;
		return true;
	}
	return false;
}

//绘制Bezier曲线
void Curve::DrawBezier() {
	if (currentSize > 1) {
		glBindVertexArray(vao);
		glDrawArrays(GL_LINE_STRIP, 0, m);
		glBindVertexArray(0);
	}
}

//绘制输入的控制点
void Curve::DrawCtrlPoints() {
	glBindVertexArray(cvao);
	glPointSize(15.0f);
	glDrawArrays(GL_POINTS, 0, currentSize);
	glDrawArrays(GL_LINE_STRIP, 0, currentSize);
	glBindVertexArray(0);
}

//绘制生成动画
void Curve::DrawDynamic() {
	if (currentSize > 1 && isAnimation) {
		glBindVertexArray(avao);
		glBindBuffer(GL_ARRAY_BUFFER, avbo);
		int round = currentSize - 1;
		//初始化
		//由动态点由相邻点决定的特性，可以反复在该数组的基础上生成下一轮动态点
		for (int i = 0; i < currentSize * 3; i++)
			Anibezier[i] = ctrlPoint[i];
		while (round > 0) {
			for (int i = 0; i < round; i++) {
				Anibezier[i * 3 + 0] = Anibezier[i * 3 + 0] + (Anibezier[(i + 1) * 3 + 0] - Anibezier[i * 3 + 0])*aniTime;
				Anibezier[i * 3 + 1] = Anibezier[i * 3 + 1] + (Anibezier[(i + 1) * 3 + 1] - Anibezier[i * 3 + 1])*aniTime;
			}
			//将数据输入缓冲
			glBufferSubData(GL_ARRAY_BUFFER, 0, round * 3 * sizeof(float), Anibezier);
			//绘制点
			glPointSize(5.0f);
			glDrawArrays(GL_POINTS, 0, round);
			//绘制连线
			glDrawArrays(GL_LINE_STRIP, 0, round);
			//下一轮动态生成点的绘制
			round--;
		}
		//绘制完毕
		glBindVertexArray(0);
		aniTime += 0.0001;
		if (aniTime > 1)
			aniTime = 0;
	}
}

//动画开关
void Curve::AniSwitch() {
	isAnimation = !isAnimation;
	if (isAnimation)
		std::cout << "Bezier animation open!" << std::endl;
	else {
		std::cout << "Bezier animation close!" << std::endl;
		aniTime = 0;
	}
}

//更新操作
void Curve::Update() {
	if (isUpdate == true) {
		//控制点发生变化，但是至少有两个控制点时重新计算插值点
		if (currentSize > 1)
			ComputeBezier();

		//更新vbo缓冲
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, m * 3 * sizeof(float), Bezier);
		//更新cvbo缓冲
		glBindBuffer(GL_ARRAY_BUFFER, cvbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, currentSize * 3 * sizeof(float), ctrlPoint);
		isUpdate = false;
	}
}