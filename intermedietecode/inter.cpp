#include<iostream>
#include<fstream>
#include<sstream>
#include<stack>

using namespace std;
stringstream postFix;
stack<char> exp;
stack<string> postExp;

bool isOperator(char a) {
    switch (a) {
        case '+': case '-':case '*': case '/':
        case '(': case ')': case '=': return true;
        default: return false;
    }
}

int isp(char e){
    switch(e){
        case '+': case '-': return 2;
        case '*': case '/': return 3;
        case '(': case '=': return 0;
    }
}

int icp(char e){
    switch(e){
        case '+': case '-': return 2;
        case '*': case '/': return 3;
        case '(': return 7;
        case ')': return 1;
        case '=': return 0;
    }
}

int main(int argc, char* argv[]){
    if(argc<2){
        cout<<"invalid input"<<endl;
        return 0;
    }
    ifstream f(argv[1]);
    string line, el, result;
    int i = 0;
    while(getline(f,line)){
        postFix.str("");
        // cout<<line<<endl;
        stringstream s(line);
        while(getline(s,el,' ')){
            char e = el[0];
            if(!isOperator(e)){
                postFix<<el<<" ";
            }
            else if(exp.empty()||icp(e)>isp(exp.top())){
                exp.push(e);
            }
            else{
                while(!exp.empty()&& isp(exp.top())>=icp(e)){
                    postFix<<exp.top()<<" ";
                    exp.pop();
                }
                if(e==')'){
                    exp.pop();
                }else{
                    exp.push(e);
                }
            }
        }
        while(!exp.empty()){
            postFix<<exp.top()<<" ";
            exp.pop();
        }
        stringstream ss(postFix.str());
        while(getline(ss,el,' ')){
            if(!isOperator(el[0])){
                postExp.push(el);
            }else{
                string first = postExp.top(); postExp.pop();
                string second = postExp.top(); postExp.pop();
                if(el=="="){
                    result = second+" "+el+" "+first;
                }else{
                    result="t"+to_string(i++); //only in c++11
                    cout<<result+" = "+second+" "+el+" "+first<<endl;
                }
                postExp.push(result);
            }
        }
        cout<<result<<endl;
    }
    return 0;
}