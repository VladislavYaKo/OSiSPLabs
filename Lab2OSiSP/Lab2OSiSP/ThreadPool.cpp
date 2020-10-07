#include "ThreadPool.h"

//DWORD WINAPI RoutineWrapper(LPVOID lpParam);

ThreadPool::ThreadPool(int threadsNum)
{
	this->_tasksQueue = new std::queue<ThreadElem>();

	this->_threadsNum = threadsNum;
	this->_threads = new ThreadElem[this->_threadsNum];
	for (int i = 0; i < this->_threadsNum; i++)
	{
		this->_threads[i].thread = CreateThread(NULL, 0, RoutineWrapper,/*ThreadPool::ExecuteRoutineInThread, */this, 
			CREATE_SUSPENDED, &this->_threads[i].threadId);
		this->_threads[i].threadId = i;
		this->_threads[i].state = ThreadPool::State::READY;
	}
	//TODO, management
}

DWORD ThreadPool::ExecuteRoutineInThread(LP_USER_FUNC reqRoutine, LPVOID args)
{
	int readyThreadInd = FindReadyThread();
	if (readyThreadInd < 0)
	{
		//TODO
		return -1;
	}

	this->_threads[readyThreadInd].threadFunc = reqRoutine;
	this->_threads[readyThreadInd].funcParams = args;
	this->_threads[readyThreadInd].state = ThreadPool::State::IN_QUEUE;
	this->_tasksQueue->push(this->_threads[readyThreadInd]);
	ResumeThread(this->_threads[readyThreadInd].thread);
	return (DWORD)readyThreadInd;
}

void* ThreadPool::StartThreadIndRoutine(int threadInd)
{
	void* res = _threads[threadInd].threadFunc(_threads[threadInd].funcParams);
	return res;
}

int ThreadPool::FindReadyThread()
{
	for (int i = 0; i < this->_threadsNum; i++)
		if (this->_threads[i].state == ThreadPool::State::READY)
			return i;

	return -1;
}

void ThreadPool::WriteRoutineExecutionResult(int threadInd, void* result)
{	
	this->_threads[threadInd].funcResult = result;
	this->_threads[threadInd].state = ThreadPool::State::HAS_RESULT;
}

void* ThreadPool::GetThreadIndResult(int threadInd)
{
	void* result;
	if (this->_threads[threadInd].state == ThreadPool::_state::HAS_RESULT)
		result = this->_threads[threadInd].funcResult;
	else
	{
		DWORD waitRes = WaitForSingleObject(_threads[threadInd].thread, 5000);
		if (waitRes == WAIT_TIMEOUT)
		{
			result = new DWORD;
			*(DWORD*)result = WAIT_TIMEOUT;
		}
		else
			result = this->_threads[threadInd].funcResult;
	}

	return result;
	//return this->_threads[threadInd].funcResult;
}

DWORD ThreadPool::ClassRoutineWrapper(LPVOID lpPAram)
{
	ThreadElem curExecThread = this->_tasksQueue->front();
	void* result = this->StartThreadIndRoutine(curExecThread.threadId);
	this->WriteRoutineExecutionResult(curExecThread.threadId, result);
	this->_tasksQueue->pop();
	return 0;
}
//Возвращает индекс потока в массиве, 
//который содержит результат выполнения функции
DWORD WINAPI RoutineWrapper(LPVOID lpParam)
{
	ThreadPool curTP = *(ThreadPool*)lpParam;
	DWORD res = curTP.ClassRoutineWrapper(NULL);
	
	return res;
}
//LPTHREAD_START_ROUTINE