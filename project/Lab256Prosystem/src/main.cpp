#include "MainWindow.h"

#include <QApplication>
#include <QTextCodec>

#include <Log.h>

#ifdef Q_OS_WIN32

#include "ConfigServer.h"

#include <DbgHelp.h>
#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib,"OpenGL32.lib")


LONG __stdcall SingleMoleculeUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
	SYSTEMTIME tm;
	GetLocalTime(&tm);
	char ardmpname[128] = { 0 };
	sprintf_s(ardmpname, "SMD%d%02d%02d%02d%02d%02d.dmp", tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wSecond);

	std::string strDumpPath = ConfigServer::GetDumpPath();
	strDumpPath += ardmpname;
	//strDumpPath += ".dmp";

	HANDLE hFile = CreateFile(strDumpPath.c_str(), GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if ((hFile != NULL) && (hFile != INVALID_HANDLE_VALUE))
	{
		MINIDUMP_EXCEPTION_INFORMATION mdei;
		mdei.ThreadId = GetCurrentThreadId();
		mdei.ExceptionPointers = pExceptionInfo;
		mdei.ClientPointers = TRUE;

		//MINIDUMP_CALLBACK_INFORMATION mci;
		//mci.CallbackRoutine = (MINIDUMP_CALLBACK_ROUTINE)NULL;
		//mci.CallbackParam = 0;

		::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, MiniDumpNormal, (pExceptionInfo != 0) ? &mdei : 0, NULL, NULL);

		CloseHandle(hFile);
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

#else
#endif


int main(int argc, char *argv[])
{
	//try
	//{
		QApplication a(argc, argv);

#ifdef Q_OS_WIN32
		SetUnhandledExceptionFilter(SingleMoleculeUnhandledExceptionFilter);
#else
		//QTextCodec* codec = QTextCodec::codecForName("gbk");
		//QTextCodec::setCodecForLocale(codec);
#endif

		MainWindow w;
		w.show();
		return a.exec();
	//}
	//catch (const std::exception& exp)
	//{
	//	LOGE("main error!={}", exp.what());
	//}
}
