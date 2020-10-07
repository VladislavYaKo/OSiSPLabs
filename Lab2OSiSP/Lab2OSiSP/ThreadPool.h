#pragma once
#include <Windows.h>
#include <processthreadsapi.h>;
#include <synchapi.h>
//#include <winerror.h>
//#include <WinBase.h>
#include <queue>;

typedef void* (*LP_USER_FUNC)(LPVOID);

DWORD WINAPI RoutineWrapper(LPVOID lpParam);

class ThreadPool
{	
	typedef enum _state
	{
		NOT_EXIST,
		READY,
		IN_QUEUE,
		EXECUTING,
		HAS_RESULT		
	} State;	

	typedef struct _ThreadElem
	{
		HANDLE thread;
		DWORD threadId;
		LP_USER_FUNC threadFunc;
		LPVOID funcParams;
		LPVOID funcResult;
		ThreadPool::State state;
	}ThreadElem;

public:
	ThreadPool(int threadsNum = 20);

	DWORD ExecuteRoutineInThread(LP_USER_FUNC reqRoutine, LPVOID args);
	void* StartThreadIndRoutine(int threadInd);
	//int FindInQueueThread();
	void WriteRoutineExecutionResult(int threadInd, void* result);
	void* GetThreadIndResult(int threadInd);
	DWORD WINAPI ClassRoutineWrapper(LPVOID lpParam);
private:
	ThreadElem* _threads;
	int _threadsNum;
	std::queue<ThreadElem> *_tasksQueue;

	int FindReadyThread();
	
};

