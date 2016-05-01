#include<iostream>
#include<fstream>
#include<sstream>
#include<map>
#include<list>
#include<algorithm>
#include<iomanip>

using namespace std;

class Line{
public:
	Line(string l,string oc,string op){
		label = l;
		opCode = oc;
		operand = op;
	}
	string label,opCode,operand;
};

class LineInt : public Line{
public:
	LineInt(int lo,int le,string l,string oc,string op):Line(l,oc,op){
		location = lo;
		length = le;
	}
	int location, length;
};

Line readLine(ifstream &f){
	string label,opcode,operand;
	getline(f,label,'\t');
	getline(f,opcode,'\t');
	getline(f,operand);
	return Line(label,opcode,operand);
}

LineInt readLineInt(ifstream &f){
	string label,opCode,operand;
	int location; int len;
	f>>hex>>location;
	f>>len;
	getline(f,label,'\t');
	getline(f,label,'\t');
	getline(f,opCode,'\t');
	getline(f,operand);
	return LineInt(location,len,label,opCode,operand);
}

class TextRecord{ public:
	int length,start;
	string record;
	TextRecord(int s){
		start = s;
		length = 0;
	}
	void push(string t){
		if(t=="")
			return;
		record+="^"+t;
		length+=t.length();
	}
	void write (ofstream &s){
		if(length==0) return;
		s<<"T^"<<setw(6)<<setfill('0')<<hex<<start<<"^"<<setw(2)<<length/2;
		s<<record<<endl;
	}
};

map<string,string> opTab;
map<string,int> symTab;

int main(int argc,char *argv[]){
	if(argc<2){
		cout<<"Invalid input"<<endl;
		return 0;
	}
	ifstream opFile("optab.tab"),inFile(argv[1]);
	ofstream outFile((string(argv[1])+".out").c_str());
	ofstream intFile((string(argv[1])+".intr").c_str());
	ifstream intFileIn((string(argv[1])+".intr").c_str());
	string opCode,objCode,str;
	while(opFile){
		opFile>>opCode>>objCode;
		opTab[opCode] = objCode;
	}
	Line line = readLine(inFile);
	int startAddress = 0;
	stringstream s;
	s<<line.operand;
	s>>hex>>startAddress;
	s.clear();
	s<<setw(6)<<setfill('0')<<startAddress;
	s>>str;
	intFile<<setw(4)<<setfill('0')<<hex<<startAddress;
	intFile<<"\t0\t"<<line.label<<"\t"<<line.opCode<<"\t"<<line.operand<<endl;
	outFile<<"H^"<<line.label;
	outFile<<"^"<<str;
	s.clear();
	s<<startAddress;
	int locCtr;
	s>>hex>>locCtr;
	TextRecord *tRecord = new TextRecord(startAddress);
	line = readLine(inFile);
	while(line.opCode!="END"){
		objCode = "";
		stringstream ss;
		ss<<hex<<locCtr;
		intFile<<ss.str()<<'\t';
		if(line.label!=""){
			symTab[line.label] = locCtr;
		}
		if(opTab.find(line.opCode)!=opTab.end()){
			locCtr+=3;
			intFile<<3<<"\t";
		}else if(line.opCode=="WORD"){
			locCtr+=3;
			intFile<<3<<"\t";
		}else if(line.opCode=="RESW"){
			int len = 3*atoi(line.operand.c_str());
			locCtr+=len;
			intFile<<len<<"\t";
		}else if(line.opCode=="RESB"){
			int len = atoi(line.operand.c_str());
			locCtr+=len;
			intFile<<len<<"\t";
		}else if(line.opCode=="BYTE"){
			stringstream s;
			int len = line.operand.length() - 3;
			if(line.operand[0]=='X'){
				locCtr+=len/2;
				intFile<<len/2<<"\t";
			}else if(line.operand[0] = 'C'){
				locCtr+=len;
				intFile<<len<<"\t";
			}
		}
		intFile<<line.label<<'\t'<<line.opCode<<'\t'<<line.operand<<endl;
		line = readLine(inFile);
	}

	stringstream ss;
	ss<<hex<<locCtr;
	intFile<<ss.str()<<"\t0\t"<<"END";

	outFile<<"^"<<setw(6)<<setfill('0')<<hex<<locCtr-startAddress<<endl;
	inFile.open((string(argv[1])+".intr").c_str());
	LineInt lineInt = readLineInt(intFileIn);
	while(lineInt.opCode!="END"&&lineInt.opCode!=""){
		line = lineInt;
		objCode = "";
		if(opTab.find(line.opCode)!=opTab.end()){
			objCode = opTab[line.opCode];
			stringstream s;
			s<<setw(4)<<setfill('0')<<hex<<symTab[line.operand];
			objCode+=s.str();
		}else if(line.opCode=="WORD"){
			stringstream s;
			s<<setw(6)<<setfill('0')<<hex<<atoi(line.operand.c_str());
			objCode = s.str();
		}else if(line.opCode=="RESW"){
			int len = 3*atoi(line.operand.c_str());
			tRecord->write(outFile);
			tRecord = new TextRecord(lineInt.location+lineInt.length);
		}else if(line.opCode=="RESB"){
			int len = atoi(line.operand.c_str());
			locCtr+=len;
			tRecord->write(outFile);
			tRecord = new TextRecord(lineInt.location+lineInt.length);
		}else if(line.opCode=="BYTE"){
			stringstream s;
			int len = line.operand.length() - 3;
			if(line.operand[0]=='X'){
				s<<line.operand.substr(2,len);
			}else if(line.operand[0] = 'C'){
				string op = line.operand.substr(2,len);
				for(int i=0;i<op.length();i++)
					s<<hex<<(int) op[i];
			}else{
				s<<hex<<line.operand;
			}
			objCode = s.str();
		}
		tRecord->push(objCode);
		lineInt = readLineInt(intFileIn);
	}

	tRecord->write(outFile);
	outFile<<"E^"<<setw(6)<<setfill('0')<<hex<<startAddress;
	cout<<"Assembling completed and written to file : "<<argv[1]<<".out"<<endl;
};