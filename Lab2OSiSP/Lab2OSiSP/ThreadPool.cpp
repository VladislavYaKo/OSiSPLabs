#include "ThreadPool.h"

//DWORD WINAPI RoutineWrapper(LPVOID lpParam);

ThreadPool::ThreadPool(int threadsNum)
{
	InitializeCriticalSectionAndSpinCount(&_CS, 4000);
	InitializeCriticalSectionAndSpinCount(&_fileCS, 4000);

	this->_logger = new Logger("Log.txt");
	this->_logger->WriteToLogFile("Created " + std::to_string(threadsNum) + " threads.\r\n");

	this->_tasksQueue = new std::queue<ThreadElem>();

	this->_threadsNum = threadsNum;
	this->_threads = new ThreadElem[this->_threadsNum];
	for (int i = 0; i < this->_threadsNum; i++)
	{
		this->_threads[i].thread = CreateThread(NULL, 0, RoutineWrapper,/*ThreadPool::ExecuteRoutineInThread, */this, 
			CREATE_SUSPENDED, &this->_threads[i].threadId);
		this->_threads[i].threadInd = i;
		this->_threads[i].funcParams = NULL;
		this->_threads[i].threadFunc = NULL;
		this->_threads[i].funcResult = NULL;
		this->_threads[i].state = ThreadPool::State::READY;
	}
}

int ThreadPool::ExecuteRoutineInThread(LP_USER_FUNC reqRoutine, LPVOID args)
{
	int readyThreadInd = FindReadyThread();
	if (readyThreadInd < 0)
	{
		this->_logger->WriteToLogFile("No ready threads.\r\n");		
		return -1;
	}

	this->_threads[readyThreadInd].threadFunc = reqRoutine;	
	this->_threads[readyThreadInd].funcParams = args;
	this->_threads[readyThreadInd].state = ThreadPool::State::IN_QUEUE;
	this->_logger->WriteToLogFile("Task executing in thread " +std::to_string(readyThreadInd) + "\r\n");
	this->_tasksQueue->push(this->_threads[readyThreadInd]);
	ResumeThread(this->_threads[readyThreadInd].thread);
	return (DWORD)readyThreadInd;
}

void* ThreadPool::StartThreadIndRoutine(int threadInd)
{
	_threads[threadInd].state = ThreadPool::State::EXECUTING;
	void* res;
	try
	{
		res = _threads[threadInd].threadFunc(_threads[threadInd].funcParams);
	}
	catch (const std::exception& ex)
	{
		res = NULL;
		EnterCriticalSection(&this->_fileCS);
		std::string bufStr = ex.what();
		this->_logger->WriteToLogFile("Exeption: " + bufStr + "\r\n");
		LeaveCriticalSection(&this->_fileCS);
		_threads[threadInd].state = ThreadPool::State::EXCEPTION;
	}
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
	this->_threads[threadInd].funcResult = (LPVOID)result;
	this->_threads[threadInd].state = ThreadPool::State::HAS_RESULT;
}

void* ThreadPool::GetThreadIndResult(int threadInd)
{
	void* result;
	
	if (this->_threads[threadInd].state == ThreadPool::_state::HAS_RESULT)
		result = this->_threads[threadInd].funcResult;
	else if (this->_threads[threadInd].state != ThreadPool::_state::NOT_EXIST &&
		this->_threads[threadInd].state != ThreadPool::_state::READY &&
		this->_threads[threadInd].state != ThreadPool::_state::EXCEPTION)  //Только для экономии времени ожидания
	{
		DWORD waitRes = WaitForSingleObject(_threads[threadInd].thread, 5000);
		if (waitRes == WAIT_TIMEOUT)
		{
			result = NULL;
		}
		else
			result = this->_threads[threadInd].funcResult;
	}
	else
		result = NULL;

	CloseHandle(_threads[threadInd].thread);
	_threads[threadInd].thread = NULL;
	_threads[threadInd].state = ThreadPool::State::NOT_EXIST;
	return result;
}

DWORD ThreadPool::ClassRoutineWrapper(LPVOID lpParam)
{
	EnterCriticalSection(&this->_CS);
	ThreadElem curExecThread = this->_tasksQueue->front();
	this->_tasksQueue->pop();
	LeaveCriticalSection(&this->_CS);

	void* result = this->StartThreadIndRoutine(curExecThread.threadInd);
	this->WriteRoutineExecutionResult(curExecThread.threadInd, result);  
	
	return 0;
}

int ThreadPool::RefreshPoolThread()
{	
	for (int i = 0; i < this->_threadsNum; i++)
		if (this->_threads[i].state == ThreadPool::State::NOT_EXIST)
		{
			this->_threads[i].thread = CreateThread(NULL, 0, RoutineWrapper,/*ThreadPool::ExecuteRoutineInThread, */this,
				CREATE_SUSPENDED, &this->_threads[i].threadId);
			this->_threads[i].funcParams = NULL;
			this->_threads[i].threadFunc = NULL;
			this->_threads[i].funcResult = NULL;
			this->_threads[i].state = ThreadPool::State::READY;

			this->_logger->WriteToLogFile("Thread " + std::to_string(i) + " refreshed.\r\n");
			return 0;
		}

	this->_logger->WriteToLogFile("All threads are ready.\r\n");
	return 1;
}

//Возвращает индекс в массиве потоков, того потока, 
//который содержит результат выполнения функции
DWORD WINAPI RoutineWrapper(LPVOID lpParam)
{
	ThreadPool curTP = *(ThreadPool*)lpParam;
	DWORD res = curTP.ClassRoutineWrapper(NULL);
	
	return res;
}
//LPTHREAD_START_ROUTINE

//Logger class's methods
Logger::Logger()
{
	this->_logFile = NULL;
}

Logger::Logger(std::string filePath)
{
	this->_logFile = CreateFileA((LPCSTR)filePath.c_str(), GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

void Logger::CreateLogFile(std::string filePath)
{
	this->_logFile = CreateFileA((LPCSTR)filePath.c_str(), GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

void Logger::OpenLogFile(std::string filePath)
{
	this->_logFile = CreateFileA((LPCSTR)filePath.c_str(), GENERIC_ALL, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
}

DWORD Logger::WriteToLogFile(std::string writeStr)
{
	DWORD res;
	WriteFile(this->_logFile, (LPCVOID)writeStr.c_str(), writeStr.length(), &res, NULL);
	return res;
}

void Logger::CloseLogFile()
{
	CloseHandle(this->_logFile);
}