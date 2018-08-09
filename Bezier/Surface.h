#ifndef SURFACE_H
#define SURFACE_H

#include<GLEW\glew.h>

//点结构体
struct point3 {
	double x;
	double y;
	double z;
	point3() :x(0), y(0), z(0) {}
	point3(double _x, double _y, double _z) :x(_x), y(_y), z(_z) {}
};

//4*4Bezier曲面
class Surface {
public:
	Surface(int _row, int _column, int mrow, int mcolumn);
	~Surface();

	//绘制曲面
	void ctrlMesh();
	void BezierMesh();
	void ComputeBezier();
	void DrawCtrlMesh();
	void DrawCtrlPoints();
	void DrawBezierMesh();

private:
	//记录当前曲面大小
	int row, column;

	//横向与纵向插值点
	int m_row, m_column;

	//横向与纵向的间隔
	float space_row, space_column;

	//openGL
	unsigned int cvao;
	unsigned int cvbo;
	unsigned int cebo;

	unsigned int bvao;
	unsigned int bvbo;
	unsigned int bebo;

	//Bezier曲面
	float* Bezier = NULL;
	//对于每个u、v，在计算过程中有一部分都是相同的，不需要每次都进行重复计算
	point3** common = NULL;

	//三角面片索引
	unsigned int* b_index = NULL;
	unsigned int* c_index = NULL;

	//使用一维数组来进行存储
	float ctrlPoint[4 * 4 * 3] = {
		-0.75f, -0.50f, -0.75f,
		-0.25f,  0.00f, -0.75f,
		 0.25f,  0.00f, -0.75f,
		 0.75f, -0.50f, -0.75f,

		-0.75f, -0.50f, -0.25f,
		-0.25f,  0.00f, -0.25f,
		 0.25f,  0.00f, -0.25f,
		 0.75f, -0.50f, -0.25f,

		-0.75f, -0.50f,  0.25f,
		-0.25f,  0.00f,  0.25f,
		 0.25f,  0.00f,  0.25f,
		 0.75f, -0.50f,  0.25f,

		-0.75f, -0.50f,  0.75f,
		-0.25f,  0.00f,  0.75f,
		 0.25f,  0.00f,  0.75f,
		 0.75f, -0.50f,  0.75f
	};

	//openGL初始化
	void GLInit();
	//组合数计算
	double C(int n, int m);
	//根据u, v计算插值点
	point3 P(double u, double v);
	//计算网格
	void ComputeMesh(int row, int column, unsigned int* index, unsigned int ebo);
};

#endif // !SURFACE_H
