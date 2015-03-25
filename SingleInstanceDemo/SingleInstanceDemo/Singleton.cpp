#include "Singleton.h"

using namespace std;

SingletonHandle* SingletonHandle::instance = nullptr;
HANDLE SingletonHandle::sema = nullptr;


SingletonHandle* SingletonHandle::getInstance(const char* message){
	if(SingletonHandle::instance == nullptr){

		do{
			SingletonHandle::sema = ::CreateSemaphore(NULL,1,1,"SingletonDemoSema");

			if(sema == nullptr){
				break;
			}

			//An instance in another process is running
			if(WAIT_OBJECT_0 != WaitForSingleObject(SingletonHandle::sema,0)){
				SingletonHandle::sendInnerMessage(message);
				::CloseHandle(SingletonHandle::sema);
				break;
			}

			SingletonHandle::instance = new SingletonHandle(message);
		}while(false);
	}
	return SingletonHandle::instance;
}

void SingletonHandle::releaseInstance(void){
	if(SingletonHandle::instance != nullptr){
		delete SingletonHandle::instance;
		SingletonHandle::instance = nullptr;
		::CloseHandle(SingletonHandle::sema);
	}
}

void SingletonHandle::sendInnerMessage(const char* message){
	if(nullptr == message){
		return;
	}

	WSADATA wsadata;
	SOCKET s;
	sockaddr_in localaddr,remoteaddr;

	if(0!=::WSAStartup(MAKEWORD(2,0),&wsadata)){
		int err = ::WSAGetLastError();
		::printf("WSAStartup Failed : %d",err);
		::WSACleanup();
		return;
	}

	::memset(&localaddr,0,sizeof(localaddr));

	localaddr.sin_family = AF_INET;
	localaddr.sin_addr.s_addr = INADDR_ANY;
	localaddr.sin_port = ::htons(20001);

	::memset(&remoteaddr,0,sizeof(remoteaddr));
	
	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_addr.s_addr = ::inet_addr("127.0.0.1");
	remoteaddr.sin_port = ::htons(20000);

	s = ::socket(AF_INET,SOCK_STREAM,0);
	if(SOCKET_ERROR == s){
		int err = ::WSAGetLastError();
		::printf("Create Socket Failed : %d",err);
		::WSACleanup();
		return;
	}

	if(0!=::connect(s,(sockaddr*)(&remoteaddr),sizeof(remoteaddr))){
		int err = ::WSAGetLastError();
		::printf("Connect to Malody Failed : %d",err);
		::WSACleanup();
		return;
	}

	int err = ::send(s,message,strlen(message)+1,0);
	if(err<0){
		err = ::WSAGetLastError();
		::printf("Sent Message [ %s ] Failed : %d",message,err);
		::WSACleanup();
		return;
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	::closesocket(s);
}

void* SingletonHandle::retrieveInnerMessage(void* ptr){

	sockaddr_in localaddr,remoteaddr;
	SOCKET s;
	SOCKET ns;
	WSADATA wsaData;

	char recv_buffer[1024];	//1kb buffer;

	int32_t n;
	int32_t bytes;
	int32_t addrlen;

	//Handle Exceptions
	do
	{
		if(::WSAStartup(MAKEWORD(2,0),&wsaData)!=0){
			::printf("WSA init fail. Error Code : %d",WSAGetLastError());
			::WSACleanup();
			break;
		}

		s = ::socket(PF_INET,SOCK_STREAM,0);
		if(SOCKET_ERROR == s){
			::printf("Socket init fail. Error Code : %d",WSAGetLastError());
			::WSACleanup();
			break;
		}

		addrlen = sizeof(remoteaddr);
		::memset(&localaddr,0,sizeof(localaddr));
		::memset(&remoteaddr,0,sizeof(remoteaddr));

		localaddr.sin_family		= AF_INET;
		localaddr.sin_port			= htons(28571);
		localaddr.sin_addr.s_addr	= INADDR_ANY;

		if(SOCKET_ERROR==::bind(s,(sockaddr*)(&localaddr),sizeof(sockaddr_in))){
			::printf("Socket binding fail. Error Code : %d",WSAGetLastError());
			::WSACleanup();
			break;
		}

		if(SOCKET_ERROR==::listen(s,5)){
			::printf("Socket listening fail. Error Code : %d",WSAGetLastError());
			::WSACleanup();
			break;
		}

		while(true)
		{

			addrlen = sizeof(remoteaddr);
			ns = accept(s,(sockaddr*)(&remoteaddr),&addrlen);	
			memset(recv_buffer,0,1024);

			bytes = recv(ns,&recv_buffer[0],1024,0);
			if(bytes <= 0){
				continue;
			}
			
			//force write '\0' to ensure the correctness of string format
			if(recv_buffer[1023] != '\0'){
				recv_buffer[1023] = '\0';
			}

			//cope with the messages
			SingletonHandle::getInstance()->dispatchMessage(recv_buffer);

			closesocket(ns);
		}

		closesocket(s);
		return nullptr;
		
	}while(0);

	return nullptr;
}

void SingletonHandle::dispatchMessage(const char* message){
	//do something here
}

SingletonHandle::SingletonHandle(const char* message){
	this->dispatchMessage(message);

	subThd = ::CreateThread(NULL,(DWORD)NULL,(LPTHREAD_START_ROUTINE)(SingletonHandle::retrieveInnerMessage),nullptr,(DWORD)NULL,NULL);
}

SingletonHandle::~SingletonHandle(void){
	::TerminateThread(subThd,0);
}