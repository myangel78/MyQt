// MyPythonInvoke.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
#include <iostream>
#include <Python.h>
#include<string>
using namespace std;
int main()
{
	Py_Initialize();//使用python之前，要调用Py_Initialize();这个函数进行初始化

	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('E:/project/tmp/MyPythonInvoke/MyPythonInvoke')");
	PyRun_SimpleString("sys.path.append('E:/project/tmp/MyPythonInvoke/MyPythonInvoke/DLLs')");
	PyRun_SimpleString("sys.path.append('E:/project/tmp/MyPythonInvoke/MyPythonInvoke/Lib')");

	//PyRun_SimpleString("import matplotlib.pyplot as plt"); /*调用python文件*/
	//PyRun_SimpleString("plt.plot([1,2,3,4], [12,3,23,231])"); /*调用python文件*/
	//PyRun_SimpleString("plt.show()"); /*调用python文件*/

	if (!Py_IsInitialized())
	{
		printf("初始化失败！");
		return 0;
	}

	PyObject* pModule = NULL;//声明变量
	PyObject* pFunc = NULL;// 声明变量
	pModule = PyImport_ImportModule("hkk");//这里是要调用的文件名
	if (pModule == NULL)
	{
		PyErr_Print();
		cout << "PyImport_ImportModule Fail!" << endl;
		return -1;
	}

#if 0
	pFunc = PyObject_GetAttrString(pModule, "func1");//这里是要调用的函数名
#else
	pFunc = PyObject_GetAttrString(pModule, "plot_histogram_");//这里是要调用的函数名
#endif
	PyObject* args2 = Py_BuildValue("i", 25);//给python函数参数赋值

	PyObject* pRet = PyObject_CallObject(pFunc, nullptr);//调用函数
	if(pRet == nullptr)
	{
		PyErr_Print();
		cout << "PyObject_CallObject Fail!" << endl;
	}
	int res = 0;
	PyArg_Parse(pRet, "i", &res);//转换返回类型
	cout << "res:" << res << endl;//输出结果

	Py_Finalize(); // 与初始化对应
	system("pause");
	return 0;

}