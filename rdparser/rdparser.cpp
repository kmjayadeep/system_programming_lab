#include<iostream>
#include<cstdlib>

using namespace std;

string exp;
int pos=0;

void match(char c){
	if(exp[pos++]==c)
		return;
	cout<<"Syntax Error";
	exit(0);
}

char getChar(){
	return exp[pos];
}

void E();
void E_PRIME();
void T();
void T_PRIME();
void F();

void E(){
	T();
	E_PRIME();
}

void E_PRIME(){
	if(getChar()=='+'){
		match('+');
		T();
		E_PRIME();
	}
}

void T(){
	F();
	T_PRIME();
}

void T_PRIME(){
	if(getChar()=='*'){
		match('*');
		F();
		T_PRIME();
	}
}

void F(){
	if(getChar()=='('){
		match('(');
		E();
		match(')');
	}else{
		match('x'); //id
	}
}



int main(){
	cout<<"Enter expression"<<endl;
	cin>>exp;
	E();
	if(pos==exp.length()){
		cout<<"Parsed Successfully"<<endl;
	}else{
		cout<<"Invalid Expression"<<endl;
	}
	return 0;
}