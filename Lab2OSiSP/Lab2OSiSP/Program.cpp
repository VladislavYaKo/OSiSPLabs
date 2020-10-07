#include <iostream>;
#include <windows.h>;
#include <thread>;
#include "ThreadPool.h"

using namespace std;

void* TestFunc(LPVOID args);

int main()
{
	ThreadPool* tp = new ThreadPool(5);
	int* funcArgs = new int[2];
	funcArgs[0] = 128;
	funcArgs[1] = 6;
	DWORD resultId = tp->ExecuteRoutineInThread(TestFunc, (void*)funcArgs);  //ThreadPool работает с указателем на аргументы. Можно ли их копировать?
	void* funcResult = tp->GetThreadIndResult(resultId);
	int res = *(int*)funcResult;
	cout << res;
	
	return 0;
}

void* TestFunc(LPVOID args)
{
	int *argsInt = (int*)args;
	int x = argsInt[0];
	int y = argsInt[1];
	int* res = new int;
	*res = x + y;
	return (void*)res;
}

void* TestFunc2(LPVOID args)
{
	int* argsInt = (int*)args;
	int x = argsInt[0];
	int y = argsInt[1];
	int* res = new int;
	*res = x + y;
	return (void*)res;
}

void* TestFunc3(LPVOID args)
{
	int* argsInt = (int*)args;
	int x = argsInt[0];
	int y = argsInt[1];
	int* res = new int;
	*res = x + y;
	return (void*)res;
}

void* TestFunc4(LPVOID args)
{
	int* argsInt = (int*)args;
	int x = argsInt[0];
	int y = argsInt[1];
	int* res = new int;
	*res = x + y;
	return (void*)res;
}