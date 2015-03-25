/*
	Created by Scorpiour, March 2015
	All Right Reserved
*/

/*
	This demo is about to show how to make only one program process instance runs on Win32, 
	and how to pass custom protocol messages to a running program via socket

	It assumes that the program has been registered the URL Protocol in HKEY_CLASSES_ROOT

	VS2012 & C++11 requires
*/

#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <cstdint>
#include <mutex>
#include <functional>
#include <algorithm>
#include <thread>
#include <chrono>

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"netapi32.lib")

class SingletonHandle{
protected:
	static SingletonHandle* instance;
	static HANDLE sema;

	HANDLE subThd;
protected:
	explicit SingletonHandle(const char* message = nullptr);
	virtual ~SingletonHandle(void);

	static void sendInnerMessage(const char* message);
	static void* retrieveInnerMessage(void* ptr);

	void dispatchMessage(const char* message);

public:


	static SingletonHandle* getInstance(const char* message = nullptr);
	static void releaseInstance(void);	
};