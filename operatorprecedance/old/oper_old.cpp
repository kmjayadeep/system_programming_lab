// Complile this program with the command g++ inter.cpp --std=c++11 
#include<iostream>
#include<fstream>
#include<sstream>
#include<stack>

using namespace std;

stack<string> opr;
stack<string> val;

int priority(string input){
    switch(input[0]){
        case '$': return 0;
        case 'x': return 5;
        case '+': case '-': return 1;
        case '*': case '/': return 2;
        default: return -1;
    }
}

bool isOperator(string s) {
    switch (s[0]) {
        case '+': case '-':case '*': case '/': return true;
        default: return false;
    }
}

int main(int argc, char* argv[]){
    if(argc<2){
        cout<<"invalid input"<<endl;
        return 0;
    }
    ifstream fin(argv[1]);
    string el,s,f,op,res;
    opr.push("$");
    val.push("$");

    while(getline(fin,el,' ')){
        if(isOperator(el)){
            while(priority(el)<=priority(opr.top())&&opr.top()!="$"){
                s = val.top(); val.pop();
                f = val.top(); val.pop();
                op = opr.top(); opr.pop();
                res = string("(")+f+op+s+")";
                val.push(string("(")+f+op+s+")");
            }
            opr.push(el);
        }else{
            val.push(el);
        }
    }

    while(!opr.empty()&&opr.top()!="$"){
        string s = val.top(); val.pop();
        string f = val.top(); val.pop();
        string op = opr.top(); opr.pop();
        val.push(string("(")+f+op+s+")");
    }
    cout<<val.top()<<endl;

    return 0;
}