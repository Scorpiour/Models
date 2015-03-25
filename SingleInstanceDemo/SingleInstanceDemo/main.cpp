#include "Singleton.h"

int main(int argc, char** argv){

	SingletonHandle* sg = nullptr;

	if(argc > 1){
		sg = SingletonHandle::getInstance(argv[1]);
	}else{
		sg = SingletonHandle::getInstance();
	}

	if(sg!=nullptr){
		//This is running process, do other stuffs here
	}

	SingletonHandle::releaseInstance();

	system("pause");
	return 0;
}