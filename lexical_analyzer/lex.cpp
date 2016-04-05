#include<iostream>
#include<fstream>
#include<regex>

using namespace std;

fstream fin;

#define NEWLINE "[NEWLINE]"
#define NUMBER "[NUMBER]"
#define VARIABLE "[VARIABLE]"
#define KEYWORD "[KEYWORD]"
#define OPERATOR "[OPERATOR]"
#define EQUAL "[EQUAL]"

regex var("[a-zA-Z][a-zA-Z0-9]*");
regex num("[0-9]+");
regex key("print");
regex op("[\\+\\*\\-]");
regex eq("=");

bool analyze(string word){
	if(regex_match(word,key))
		cout<<KEYWORD;
	else if(regex_match(word,var))
		cout<<VARIABLE;
	else if(regex_match(word,num))
		cout<<NUMBER;
	else if(regex_match(word,op))
		cout<<OPERATOR;
	else if(regex_match(word,eq))
		cout<<EQUAL;
	else{
		cout<<endl<<"syntax error"<<endl;
		return false;
	}
	return true;

}

int main(int argc, char* argv[]){
	if(argc<2){
		cout<<"invalid input";
		return 0;
	}
	fin.open(argv[1]);
	string line;
	string word;
	int j,m;
	while(getline(fin,line)){
		cout<<line<<endl;
		j=0;
		while(j!=-1){
			m = line.find(' ',j);
			word = line.substr(j,m-j);
			if(!analyze(word))
				return 0;
			// cout<<'\t';
			if(m!=-1)
				j=m+1;
			else
				j=-1;
		}
		cout<<NEWLINE<<endl;
	}
	return 0;
}