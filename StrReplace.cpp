#include <string>
#include <iostream>

using namespace std;

void rep(std::string& src,const std::string& oldv,const std::string& newv){
	std::string n;
	size_t i = 0;
	for(;i<src.size()-oldv.size();i++){
		if(strncmp(&(src.c_str()[i]),oldv.c_str(),oldv.size())==0){
			n+=newv;
			i+=oldv.size()-1;
		}else{
			n.push_back(src.c_str()[i]);
		}
	}
	n+=&(src.c_str()[i]);
	src = n;
	
}

int main(){
	
	string raw = "this is a test test string.";
	string r = "replaced";
	
	cout<<raw<<endl;
	cout<<r<<endl;
	
	rep(raw,"test",r);
	
	cout<<raw<<endl;
	
	raw = "this is another string.";
	rep(raw,"test",r);
	
	cout<<raw<<endl;
	
	
	
	
	system("pause");
	return 0;
}