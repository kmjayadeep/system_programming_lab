#include<iostream>
#include<fstream>
#include<sstream>
#include<map>
#include<list>
#include<algorithm>
#include<iomanip>

using namespace std;

// A line of program code : contans label. opcode and operand
class Line{
public:
	Line(string l,string oc,string op){
		label = l;
		opCode = oc;
		operand = op;
	}
	string label,opCode,operand;
};

// Reads a line from input file and returns a Line object which contains label, opcode and operand
Line readLine(ifstream &f){
	string label,opcode,operand;
	char c;
	f.get(c);
	if(c=='\n')
		f.get(c);
	if(c=='\t')
		label=""; // No label present, so set it to null string
	else{
		// We already read the first char of the label string into c
		// So seek a single character back
		f.seekg(-1,f.cur); 
		f>>label;
	}
	f>>opcode;
	f>>operand;
	return Line(label,opcode,operand);
}

// A class to hold each entry of symtab corresponding to each label
// Contains value and a linked list for forward reference
class Op{ public:
	int value;
	list<int> addresses;
	Op(){
		// initialize value to -1
		value = -1;
	}
};

// To hold each text record
class TextRecord{ public:
	int length,start;
	string record;

	// The starting address is passed and length is set to 0
	TextRecord(int s){
		start = s;
		length = 0;
	}

	// Upon adding a new entry to text record, A '^' character is added and then string is appended
	// length of the text record is incremented by length of appended string
	void push(string t){
		record+="^"+t;
		length+=t.length();
	}

	// Method to write a text record to disk
	void write (ofstream &s){
		if(length==0) return;
		// length is converted to bytes; and converted to hex
		// Set width as 6 and filled the remaining spaces with 0s
		s<<"T^"<<setw(6)<<setfill('0')<<hex<<start<<"^"<<setw(2)<<length/2;
		s<<record<<endl;
	}
};

// Structures for storing optab nad symtab
map<string,string> opTab;
map<string,Op> symTab;

int main(int argc,char *argv[]){

	// input file is passed as command line argument. Terminate if no input file specified
	if(argc<2){
		cout<<"Invalid input"<<endl;
		return 0;
	}
	ifstream opFile("optab.tab"),inFile(argv[1]);
	ofstream outFile((string(argv[1])+".out").c_str());
	string opCode,objCode;
	
	// Read code and objcode from file and add it to map
	while(opFile){
		opFile>>opCode>>objCode;
		opTab[opCode] = objCode;
	}

	// Read first line
	Line line = readLine(inFile);
	int startAddress = 0;
	if(line.opCode=="START")
		// convert string object to char array and use atoi function to convert to integer
		startAddress = atoi(line.operand.c_str());


	stringstream s;
	// Convert address to 6 column width hex string padded with zeros
	s<<setw(6)<<setfill('0')<<hex<<startAddress;

	// Write head record, write program size as 0 to change later
	outFile<<"H^"<<line.label;
	outFile<<"^"<<s.str()<<"^000000"<<endl;

	// Save the position where the program size is to be save
	int addressPos = line.label.length()+10;

	// Initialize location counter to starting address
	int locCtr = startAddress;

	//start new text record and read next line
	TextRecord *tRecord = new TextRecord(startAddress);
	line = readLine(inFile);
	while(line.opCode!="END"){
		cout<<line.label<<" "<<line.opCode<<" "<<line.operand<<endl;
		objCode = "";

		// If label is present in the line
		if(line.label!=""){
			// Position of the location counter is saved in symtab as the value of the label
			symTab[line.label].value = locCtr;
			list<int> addresses = symTab[line.label].addresses;

			// If the linked list of corresponding label is not empty
			// initialize new text record for each of the addresses and write it to file
			if(addresses.size()!=0){
				// Write all values in existing text record
				tRecord->write(outFile);

				// Looped through list using List iterator
				for(list<int>::iterator i = addresses.begin();i!=addresses.end();i++){
					stringstream s;
					// For each instructon, first 2 columns are objcode and remaining 4 are for address
					// So, we need to skip first byte, which is 2 columns and fill the remaining with the newly obtained value
					// For that, write new text record
					s<<setw(4)<<setfill('0')<<hex<<locCtr;
					tRecord = new TextRecord(*i+1);
					tRecord->push(s.str());
					tRecord->write(outFile);
				}
				tRecord = new TextRecord(locCtr);
			}
		}

		// Search optab for opcode
		if(opTab.find(line.opCode)!=opTab.end()){

			// retrieve corresponding objcode and Op object

			objCode = opTab[line.opCode];
			Op op = symTab[line.operand];

			// if value is not set, it will be -1, invalid address
			if(op.value==-1){
				// Value is not defined yet, so insert to symtab list and set 0000 in objcode
				symTab[line.operand].addresses.push_back(locCtr);
				objCode+="0000";
			}else{
				// Value is taken frm symtab and added to objcode
				stringstream s;
				s<<setw(4)<<setfill('0')<<hex<<op.value;
				objCode+=s.str();
			}
			locCtr+=3;
		}else if(line.opCode=="WORD"){
			stringstream s;
			// 6 column width word, in hex
			s<<setw(6)<<setfill('0')<<hex<<atoi(line.operand.c_str());
			objCode = s.str();
			locCtr+=3;
		}else if(line.opCode=="RESW"){
			// Add 3* no of words to location counter and leave the space
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
			// if opcode is byte, operand is converted to hex and added to text record
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

		// Add the obtained object code into text record and read next line
		tRecord->push(objCode);
		line = readLine(inFile);
	}
	// write last text record
	tRecord->write(outFile);

	// write end record
	outFile<<"E^"<<setw(6)<<setfill('0')<<hex<<startAddress;
	
	// seek to the saved address to change the program size
	outFile.seekp(addressPos,outFile.beg);

	// replace the program size
	outFile<<setw(6)<<setfill('0')<<hex<<locCtr-startAddress;
	cout<<"Assembling completed and written to file : "<<argv[1]<<".out"<<endl;
};