#include<iostream>
#include<sstream>
#include<fstream>
#include<map>
#include<iomanip>
using namespace std;

map<int,string> memory;

int main(int argc, char* argv[]){
	if(argc<3){
		cout<<"Execute as ./executable inputFile location"<<endl;
		return 0;
	}
	ifstream fin(argv[1]);
	stringstream s;
	string line,str;
	s<<hex<<argv[2];
	int startAddress,addr,size,location;
	s>>location;
	s.str(""); s.clear();
	getline(fin,str,'^');
	getline(fin,str,'^');
	getline(fin,str,'^');
	s<<hex<<str;
	s>>startAddress;
	startAddress+=location;
	getline(fin,str);
	s.str(""); s.clear();
	s<<hex<<str;
	s>>size;

	char byte[3];
	byte[2] = 0;
	while(getline(fin,line)&&line[0]!='E'){
		stringstream ss(line);
		getline(ss,str,'^');
		getline(ss,str,'^');
		s.str(""); s.clear();
		s<<hex<<str;
		s>>addr;
		addr+=location;
		getline(ss,str,'^');
		int bitMask,iMask=2048,temp;
		ss>>hex>>bitMask;
		getline(ss,str,'^');
		while(getline(ss,line,'^')){
			s.str(line); s.clear();
			if((bitMask&iMask)==iMask){
				s.read(byte,2);
				memory[addr++] = string(byte);
				s>>hex>>temp;
				temp+=location;
				s.str(""); s.clear();
				s<<setw(4)<<setfill('0')<<temp;
			}
			while(s.read(byte,2)){
				memory[addr++] = string(byte);
			}
			iMask/=2;
		}
	}

	for(int i=startAddress;i<startAddress+size;i++)
		cout<<hex<<i<<"\t"<<memory[i]<<endl;

	return 0;
}