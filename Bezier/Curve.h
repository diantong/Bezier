#ifndef CURVE_H
#define CURVE_H

/*
	需要明确的Curve目前仅为了操作封装考虑，仍然是一个不安全的类
	因为暂时未对拷贝构造函数与赋值操作符进行定义，而其包含了指针与索引，直接操作会导致浅拷贝
	而浅拷贝的直接后果则是同一块内存或索引会被多次释放或删除
*/

#include<iostream>

//点结构体
struct point2 {
	double x;
	double y;
	point2() :x(0), y(0) {}
	point2(double _x, double _y) :x(_x), y(_y) {}
};

class Curve {
public:
	//最多输入控制点
	int maxSize;
	//插值点数
	int m;

	//构造函数
	Curve(int _maxSize, int _m);
	//析构函数
	~Curve();

	//opengl初始化
	void GLInit();
	
	//计算Bezier
	void ComputeBezier();
	
	//新增加控制点
	bool push(float x, float y);
	//删除控制点
	bool pop();

	//绘制曲线
	void DrawBezier();
	void DrawCtrlPoints();
	void DrawDynamic();

	//更新操作
	void Update();
	//动画开关
	void AniSwitch();

private:
	//当前输入控制点个数
	int currentSize;

	//输入控制点
	float* ctrlPoint = NULL;
	unsigned int cvao, cvbo;
	//Bezier曲线
	float* Bezier = NULL;
	unsigned int vao, vbo;
	//Bezier动态生成点
	float* Anibezier = NULL;
	unsigned int avao, avbo;

	//更新
	bool isUpdate;
	//是否开启动画
	bool isAnimation;
	//动画间隔
	float aniTime;

	//bezier间隔
	double space;
	//预计算部分
	point2* common = NULL;

	//组合数计算
	double C(int n, int m);
	//根据t计算插值点
	point2 B(double t, int n, point2* common);
};

#endif // !CURVE_H
