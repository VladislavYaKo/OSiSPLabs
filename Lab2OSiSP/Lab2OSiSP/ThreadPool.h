#pragma once
#include <Windows.h>
#include <queue>
#include <string>

typedef void* (*LP_USER_FUNC)(LPVOID);

DWORD WINAPI RoutineWrapper(LPVOID lpParam);

class Logger;

class ThreadPool
{	
	typedef enum _state
	{
		NOT_EXIST,
		READY,
		IN_QUEUE,
		EXECUTING,
		HAS_RESULT,
		EXCEPTION
	} State;	

	typedef struct _ThreadElem
	{
		HANDLE thread;
		DWORD threadId;
		DWORD threadInd;
		LP_USER_FUNC threadFunc;
		LPVOID funcParams;
		LPVOID funcResult;
		ThreadPool::State state;
	}ThreadElem;

public:
	ThreadPool(int threadsNum = 20);

	int ExecuteRoutineInThread(LP_USER_FUNC reqRoutine, LPVOID args);
	void* StartThreadIndRoutine(int threadInd);
	void WriteRoutineExecutionResult(int threadInd, void* result);
	void* GetThreadIndResult(int threadInd);
	DWORD WINAPI ClassRoutineWrapper(LPVOID lpParam);
	int RefreshPoolThread();
private:
	CRITICAL_SECTION _CS;
	CRITICAL_SECTION _fileCS;

	Logger* _logger;

	ThreadElem* _threads;
	int _threadsNum;
	std::queue<ThreadElem> *_tasksQueue;

	int FindReadyThread();
	
};

class Logger
{
public:
	Logger();
	Logger(std::string filePath);

	void CreateLogFile(std::string filePath);
	void OpenLogFile(std::string filePath);
	DWORD WriteToLogFile(std::string writeStr);
	void CloseLogFile();

private:
	HANDLE _logFile;
};

