#ifndef SURFACE_H
#define SURFACE_H

#include<GLEW\glew.h>

//��ṹ��
struct point3 {
	double x;
	double y;
	double z;
	point3() :x(0), y(0), z(0) {}
	point3(double _x, double _y, double _z) :x(_x), y(_y), z(_z) {}
};

//4*4Bezier����
class Surface {
public:
	Surface(int _row, int _column, int mrow, int mcolumn);
	~Surface();

	//��������
	void ComputeMesh(int row, int column, unsigned int* index, unsigned int ebo);
	void ctrlMesh();
	void BezierMesh();
	void ComputeBezier();
	void DrawCtrlMesh();
	void DrawCtrlPoints();
	void DrawBezierMesh();

private:
	//��¼��ǰ�����С
	int row, column;

	//�����������ֵ��
	int m_row, m_column;

	//����������ļ��
	float space_row, space_column;

	//openGL
	unsigned int cvao;
	unsigned int cvbo;
	unsigned int cebo;

	unsigned int bvao;
	unsigned int bvbo;
	unsigned int bebo;

	//Bezier����
	float* Bezier = NULL;

	//������Ƭ����
	unsigned int* b_index = NULL;
	unsigned int* c_index = NULL;

	//ʹ��һά���������д洢
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

	//���������
	double C(int n, int m);
	//Bernstein������
	double B(int i, int n, double t);
	//����u, v�����ֵ��
	point3 P(double u, double v);
};

#endif // !SURFACE_H
