#include <Windows.h>
#include <iostream>
#include "ThreadPool.h"

using namespace std;

void* TestFunc(LPVOID args);
void* TestFunc2(LPVOID args);
void* TestFunc3(LPVOID args);

int main()
{
	ThreadPool* tp = new ThreadPool(3);

	//TestFunc
	int* funcArgs = new int[2];
	funcArgs[0] = 128;
	funcArgs[1] = 6;
	int resultId = tp->ExecuteRoutineInThread(TestFunc, (void*)funcArgs);	
	void* funcResult = NULL;;
	if (resultId >= 0 )
		funcResult = tp->GetThreadIndResult(resultId);
	int res = 0;
	if (funcResult != NULL)
		res = *(int*)funcResult;

	//TestFunc2
	int* funcArgs2 = new int[2];
	funcArgs2[0] = 10;
	funcArgs2[1] = 2;
	int resultId2 = tp->ExecuteRoutineInThread(TestFunc2, (void*)funcArgs2);
	void* funcResult2 = NULL;;
	if (resultId2 >= 0)
		funcResult2 = tp->GetThreadIndResult(resultId2);
	int res2 = 0;
	if (funcResult2 != NULL)
		res2 = *(int*)funcResult2;

	//TestFunc3
	int* funcArgs3 = new int[2];
	funcArgs3[0] = 88;
	funcArgs3[1] = 99;
	int resultId3 = tp->ExecuteRoutineInThread(TestFunc3, (void*)funcArgs3);
	void* funcResult3 = NULL;;
	if (resultId3 >= 0)
		funcResult3 = tp->GetThreadIndResult(resultId3);
	int res3 = 0;
	if (funcResult3 != NULL)
		res3 = *(int*)funcResult3;

	cout << res << " " << res2;
	
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
	try
	{
		throw new std::exception("TestFunc 3 suck!");
	}
	catch (std::exception& err)
	{
		throw;
	}  //Разобраться, как вернуть исключение из функции

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