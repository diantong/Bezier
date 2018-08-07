#ifndef CURVE_H
#define CURVE_H

/*
	��Ҫ��ȷ��CurveĿǰ��Ϊ�˲�����װ���ǣ���Ȼ��һ������ȫ����
	��Ϊ��ʱδ�Կ������캯���븳ֵ���������ж��壬���������ָ����������ֱ�Ӳ����ᵼ��ǳ����
	��ǳ������ֱ�Ӻ������ͬһ���ڴ�������ᱻ����ͷŻ�ɾ��
*/

#include<iostream>

//��ṹ��
struct point2 {
	double x;
	double y;
	point2() :x(0), y(0) {}
	point2(double _x, double _y) :x(_x), y(_y) {}
};

class Curve {
public:
	//���������Ƶ�
	int maxSize;
	//��ֵ����
	int m;

	//���캯��
	Curve(int _maxSize, int _m);
	//��������
	~Curve();

	//opengl��ʼ��
	void GLInit();
	
	//����Bezier
	void ComputeBezier();
	
	//�����ӿ��Ƶ�
	bool push(float x, float y);
	//ɾ�����Ƶ�
	bool pop();

	//��������
	void DrawBezier();
	void DrawCtrlPoints();
	void DrawDynamic();

	//���²���
	void Update();
	//��������
	void AniSwitch();

private:
	//��ǰ������Ƶ����
	int currentSize;

	//������Ƶ�
	float* ctrlPoint = NULL;
	unsigned int cvao, cvbo;
	//Bezier����
	float* Bezier = NULL;
	unsigned int vao, vbo;
	//Bezier��̬���ɵ�
	float* Anibezier = NULL;
	unsigned int avao, avbo;

	//����
	bool isUpdate;
	//�Ƿ�������
	bool isAnimation;
	//�������
	float aniTime;

	//bezier���
	double space;
	//Ԥ���㲿��
	point2* common = NULL;

	//���������
	double C(int n, int m);
	//����t�����ֵ��
	point2 B(double t, int n, point2* common);
};

#endif // !CURVE_H
