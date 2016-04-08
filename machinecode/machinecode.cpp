#include<iostream>
#include<fstream>
#include<sstream>
#include<list>

using namespace std;

stringstream postFix;
string elements[3];
int main(int argc, char* argv[]){
    if(argc<2){
        cout<<"invalid input"<<endl;
        return 0;
    }
    ifstream f(argv[1]);
    string line, el;
    while(getline(f,line)){
        stringstream s(line);
        int i=0, n;
        while(getline(s,el,' ')){
            elements[i++] = el;
        }
        cout<<"MOV "<<elements[0]<<", "<<elements[2]<<endl;
        if(i==5){ // 3 address code : eg a = b + c => a,b,=,+ and c ; total 5 elements
            switch(elements[3][0]){
                case '+': cout<<"ADD "; break;
                case '-': cout<<"SUB "; break;
                case '*': cout<<"MUL "; break;
                case '/': cout<<"DIV "; break;
            }
            cout<<elements[0]<<", "<<elements[4]<<endl;
        }
    }
    return 0;
}