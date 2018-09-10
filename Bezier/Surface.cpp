#include "stdafx.h"
#include "Surface.h"

//���캯��
Surface::Surface(int _row, int _column, int mrow, int mcolumn): row(_row), column(_column), m_row(mrow), m_column(mcolumn) {
	//������
	space_row = 1.0f / (float)m_row;
	space_column = 1.0f / (float)m_column;

	//�Բ�ͬ��(u, v)�õ�����Ӧ���������д洢
	Bezier = new float[(m_row + 1)*(m_column + 1) * 3];

	//�Թ��в��ֽ��д洢
	common = new point3*[row];
	for (int i = 0; i < row; i++)
		common[i] = new point3[column];

	//һ���ı�����6����������
	b_index = new unsigned int[m_row * m_column * 6];
	c_index = new unsigned int[(row - 1)*(column - 1) * 6];

	//openGL
	GLInit();
}

void Surface::GLInit() {
	//openGL
	//���Ƶ�
	glGenVertexArrays(1, &cvao);
	glBindVertexArray(cvao);
	glGenBuffers(1, &cvbo);
	glBindBuffer(GL_ARRAY_BUFFER, cvbo);
	glBufferData(GL_ARRAY_BUFFER, row * column * 3 * sizeof(float), ctrlPoint, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//ʹ����������
	glGenBuffers(1, &cebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (row - 1) * (column - 1) * 6 * sizeof(unsigned int), NULL, GL_STATIC_DRAW);
	glBindVertexArray(0);

	//�����
	glGenVertexArrays(1, &bvao);
	glBindVertexArray(bvao);
	glGenBuffers(1, &bvbo);
	glBindBuffer(GL_ARRAY_BUFFER, bvbo);
	glBufferData(GL_ARRAY_BUFFER, (m_row + 1)*(m_column + 1) * 3 * sizeof(float), NULL, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//����
	glGenBuffers(1, &bebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_row * m_column * 6 * sizeof(unsigned int), NULL, GL_STATIC_DRAW);
	//�Զ���Patch�������
	glPatchParameteri(GL_PATCH_VERTICES, 3);
	glBindVertexArray(0);
}

//��������
Surface::~Surface() {
	//�ͷ�����
	delete[] Bezier;
	delete[] b_index;
	delete[] c_index;
	for (int i = 0; i < row; i++)
		delete[] common[i];
	delete[] common;

	//ɾ��
	glDeleteVertexArrays(1, &cvao);
	glDeleteBuffers(1, &cvbo);
	glDeleteBuffers(1, &cebo);

	glDeleteVertexArrays(1, &bvao);
	glDeleteBuffers(1, &bvbo);
	glDeleteBuffers(1, &bebo);
}

//���������
double Surface::C(int n, int m) {
	//����Ϸ����ж�
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

//���ݸ�����u��vֵ���������
point3 Surface::P(double u, double v) {
	point3 result;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < column; j++) {
			//����(i, j)��ȡ��Ӧ�Ŀ��Ƶ�
			int pos = (i * row + j) * 3;
			double c = pow(u, i) * pow(1 - u, row - 1 - i) * pow(v, j) * pow(1 - v, column - 1 - j);
			result.x += common[i][j].x * c;
			result.y += common[i][j].y * c;
			result.z += common[i][j].z * c;
		}
	}
	return result;
}

//����Bezier����
void Surface::ComputeBezier() {
	//���㹫������
	for(int i = 0; i < row; i++)
		for (int j = 0; j < column; j++) {
			int pos = (i * row + j) * 3;
			double c = C(row - 1, i) * C(column - 1, j);
			common[i][j].x = c * ctrlPoint[pos + 0];
			common[i][j].y = c * ctrlPoint[pos + 1];
			common[i][j].z = c * ctrlPoint[pos + 2];
		}

	//���������
	double u, v = 0;
	for (int i = 0; i <= m_row; i++) {
		u = i * space_row;
		for (int j = 0; j <= m_column; j++) {
			v = j * space_column;
			point3 point = P(u, v);
			int pos = (i * (m_row + 1) + j) * 3;
			Bezier[pos + 0] = point.x;
			Bezier[pos + 1] = point.y;
			Bezier[pos + 2] = point.z;
		}
	}

	//���»���
	glBindBuffer(GL_ARRAY_BUFFER, bvbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (m_row + 1) * (m_column + 1) * 3 * sizeof(float), Bezier);
}

//������ɢ�Ŀ��Ƶ������������� Ԥ�������е�����
void Surface::ComputeMesh(int row, int column, unsigned int* index, unsigned int ebo) {
	for (int i = 0; i < row - 1; i++) {
		for (int j = 0; j < column - 1; j++) {
			int v1 = i * row + j;
			int v2 = i * row + j + 1;
			int v3 = v1 + row;
			int v4 = v2 + row;
			//��ǰ������ǵ�pos���ı���
			int pos = (i * (row - 1) + j) * 6;
			index[pos + 0] = v1;
			index[pos + 1] = v2;
			index[pos + 2] = v3;
			index[pos + 3] = v2;
			index[pos + 4] = v3;
			index[pos + 5] = v4;
		}
	}
	//��������
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, (row - 1) * (column - 1) * 6 * sizeof(unsigned int), index); 
}

//������Ƶ�����
void Surface::ctrlMesh() {
	ComputeMesh(row, column, c_index, cebo);
}

//����Bezier����
void Surface::BezierMesh() {
	ComputeMesh(m_row + 1, m_column + 1, b_index, bebo);
}

//����Bezier����
void Surface::DrawBezierMesh() {
	glBindVertexArray(bvao);
	glDrawElements(GL_PATCHES, m_row * m_column * 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

//���ƿ��Ƶ�����
void Surface::DrawCtrlMesh() {
	glBindVertexArray(cvao);
	glDrawElements(GL_TRIANGLES, (row - 1) * (column - 1) * 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

//���ƿ��Ƶ�
void Surface::DrawCtrlPoints() {
	glBindVertexArray(cvao);
	glPointSize(5.0f);
	glDrawArrays(GL_POINTS, 0, row * column);
	glBindVertexArray(0);
}