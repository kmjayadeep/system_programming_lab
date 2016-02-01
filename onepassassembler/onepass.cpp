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

Line readLine(ifstream &f){
	string label,opcode,operand;
	char c;
	f.get(c);
	if(c=='\n')
		f.get(c);
	if(c=='\t')
		label="";
	else{
		f.seekg(-1,f.cur);
		f>>label;
	}
	f>>opcode;
	f>>operand;
	return Line(label,opcode,operand);
}

class Op{ public:
	int value;
	list<int> addresses;
	Op(){
		value = -1;
	}
};

class TextRecord{ public:
	int length,start;
	string record;
	TextRecord(int s){
		start = s;
		length = 0;
	}
	void push(string t){
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
map<string,Op> symTab;

int main(int argc,char *argv[]){
	if(argc<2){
		cout<<"Invalid input"<<endl;
		return 0;
	}
	ifstream opFile("optab.tab"),inFile(argv[1]);
	ofstream outFile((string(argv[1])+".out").c_str());
	string opCode,objCode;
	while(opFile){
		opFile>>opCode>>objCode;
		opTab[opCode] = objCode;
	}
	Line line = readLine(inFile);
	int startAddress = 0;
	if(line.opCode=="START")
		startAddress = atoi(line.operand.c_str());
	stringstream s;
	s<<setw(6)<<setfill('0')<<hex<<startAddress;
	outFile<<"H^"<<line.label;
	outFile<<"^"<<s.str()<<"^000000"<<endl;
	int addressPos = line.label.length()+10, locCtr = startAddress;
	TextRecord *tRecord = new TextRecord(startAddress);
	line = readLine(inFile);
	while(line.opCode!="END"){
		objCode = "";
		if(line.label!=""){
			symTab[line.label].value = locCtr;
			list<int> addresses = symTab[line.label].addresses;
			if(addresses.size()!=0){
				tRecord->write(outFile);
				for(list<int>::iterator i = addresses.begin();i!=addresses.end();i++){
					stringstream s;
					s<<setw(4)<<setfill('0')<<hex<<locCtr;
					tRecord = new TextRecord(*i+1);
					tRecord->push(s.str());
					tRecord->write(outFile);
				}
				tRecord = new TextRecord(locCtr);
			}
		}
		if(opTab.find(line.opCode)!=opTab.end()){
			objCode = opTab[line.opCode];
			Op op = symTab[line.operand];
			if(op.value==-1){
				symTab[line.operand].addresses.push_back(locCtr);
				objCode+="0000";
			}else{
				stringstream s;
				s<<setw(4)<<setfill('0')<<hex<<op.value;
				objCode+=s.str();
			}
			locCtr+=3;
		}else if(line.opCode=="WORD"){
			stringstream s;
			s<<setw(6)<<setfill('0')<<hex<<atoi(line.operand.c_str());
			objCode = s.str();
			locCtr+=3;
		}else if(line.opCode=="RESW"){
			int len = 3*atoi(line.operand.c_str());
			locCtr+=len;
			tRecord->write(outFile);
			tRecord = new TextRecord(locCtr);
		}else if(line.opCode=="RESB"){
			int len = atoi(line.operand.c_str());
			locCtr+=len;
			tRecord->write(outFile);
			tRecord = new TextRecord(locCtr);
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
			locCtr+=s.str().length()/2;
		}
		tRecord->push(objCode);
		line = readLine(inFile);
	}
	tRecord->write(outFile);
	outFile<<"E^"<<setw(6)<<setfill('0')<<hex<<startAddress;
	outFile.seekp(addressPos,outFile.beg);
	outFile<<setw(6)<<setfill('0')<<hex<<locCtr-startAddress;
	cout<<"Assembling completed and written to file : "<<argv[1]<<".out"<<endl;
};