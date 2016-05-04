#include<iostream>
#include<sstream>
#include<fstream>
#include<vector>
#include<algorithm>

using namespace std;

vector<string> delims = { "{", "}", "[", "]", ",", ";", "." , "(", ")" };
vector<string> operators = { "+", "-", "*", "/","<", ">", "!", ">=", "<=", "!=", "&", "|", "||","<<",">>","=","+=","-=","*=","/=","++","--"};
vector<string> keywords = {
	"int", "float", "double", "char", "void",
	"while", "for", "do", "if", "return",
	"using", "namespace",
	"cout", "cin"
};

bool isIn(vector<string>& vec,string token){
	return find(vec.begin(),vec.end(),token)!=vec.end();
}

bool isPreproc(string token){
	return token[0]=='#'&&token.substr(1,8)=="include<"&&token.back()=='>';
}

bool isDelim(string token){
	return isIn(delims,token);
}

bool isKeyword(string token){
	return isIn(keywords,token);
}

bool isOperator(string token){
	return isIn(operators,token);
}

bool isIdentifier(string token){
	if(!isalpha(token[0]))
		return false;
	for(int i=1;i<token.length()-1;i++)
		if(!isalnum(token[i])&&token[0]!='_')
			return false;
	return true;
}

bool isNum(string token){
	for(auto i=token.begin();i!=token.end();i++)
		if(!isdigit(*i))
			return false;
	return true;
}

string check(string token,string tail){
	if(isPreproc(token)){
		return "Preprocessor";
	}else if(isDelim(token)){
		return "Delimiter";
	}else if(isKeyword(token)){
		return "Keyword";
	}else if(tail!=""&&isOperator(token)&&isOperator(token+tail[0])){
		return "";
	}else if(isOperator(token)){
		return "Operator";
	}
	return "";
}

string checkValid(string token){
	if(token=="")
		return "";
	if(isIdentifier(token))
		return "Identifier";
	else if(isNum(token))
		return "Number";
	return "";
}

void analyze(string s){
	string temp="";
	for(auto i = s.begin();i!=s.end();i++){
		temp+=*i;
		auto j=i;
		j++;
		string ch = check(temp,string(j,s.end()));
		if(ch!=""){
			cout<<ch<<" :\t"<<temp<<endl;
			temp="";
		}else{
			if(!isalnum(*i)){
				string t = temp.substr(0,temp.length()-1);
				ch = checkValid(t);
				if(ch!=""){
					cout<<ch<<" :\t"<<t<<endl;
					temp = "";
					i--;
				}
			}
		}
	}
	if(temp!="")
		cout<<"Invalid Token"<<endl;
}

bool isWhiteSpace(string s){
	for(auto i=s.begin();i!=s.end();i++)
		if(*i!=' '&&*i!='\t')
			return false;
	return true;
}

int main(int argc, char* argv[]){
	if(argc<2){
		cout<<"invalid arguments"<<endl;
		return 0;
	}
	ifstream fin(argv[1]);
	string line;
	int lineNo = 1;
	while(getline(fin,line)){
		if(isWhiteSpace(line)){
			lineNo++;
			continue;
		}
		cout<<endl<<"Line "<<lineNo++<<"\t : "<<line<<endl<<"-----------------"<<endl;
		stringstream ss(line);
		string el;
		while(ss>>el){
			analyze(el);
		}


		cout<<endl;
	}





	return 0;
}