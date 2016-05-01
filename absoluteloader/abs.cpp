#include<iostream>
#include<sstream>
#include<fstream>
#include<map>
using namespace std;

map<int,string> memory;

int main(int argc, char* argv[]){
	if(argc<2){
		cout<<"No input"<<endl;
		return 0;
	}
	ifstream fin(argv[1]);
	stringstream s;
	string line,str;
	int startAddress,addr,size;
	getline(fin,str,'^');
	getline(fin,str,'^');
	getline(fin,str,'^');
	s<<hex<<str;
	s>>startAddress;
	getline(fin,str);
	s.str("");
	s.clear();
	s<<hex<<str;
	s>>size;

	char byte[3];
	byte[2] = 0;
	while(getline(fin,line)&&line[0]!='E'){
		stringstream ss(line);
		getline(ss,str,'^');
		getline(ss,str,'^');
		s.str("");
		s.clear();
		s<<hex<<str;
		s>>addr;
		getline(ss,line,'^');
		while(getline(ss,line,'^')){
			s.str(line);
			s.clear();
			while(s.read(byte,2)){
				memory[addr++] = string(byte);
			}
		}
	}

	for(int i=startAddress;i<startAddress+size;i++)
		cout<<hex<<i<<"\t"<<memory[i]<<endl;

	return 0;
}