#include<iostream>
#include<sstream>
#include<fstream>
#include<map>
#include<set>
#include<list>
#include<cctype>

using namespace std;

class FnF
{
public:
	set<string> first;
	set<string> follow;
	FnF(){}
	void print()
	{
		cout<<"First : ";
		for(set<string>::iterator iter=first.begin(); iter!=first.end();iter++)
			cout<<(*iter)<<", ";
		cout<<endl;
		cout<<"Follow : ";
		for(set<string>::iterator iter=follow.begin(); iter!=follow.end();iter++)
			cout<<(*iter)<<", ";
		cout<<endl;
	}
	
};

map<string,FnF> elements;
map<string,list<list<string> > > productions;

bool isTerminal(string s){
	return !isupper(s[0]);
}

string getElement(string line){
	stringstream s(line);
	string el;
	getline(s,el,' ');
	return el;
}

list<list<string> > getProductions(string line){
	list<list<string> > prods;
	int len = line.find("-->")+3;
	stringstream s(line);
	s.seekg(len,s.cur);
	string pString;
	while(getline(s,pString,'|'))
	{
		list<string> prod;
		string el;
		stringstream t(pString);
		while(getline(t,el,' '))
		{
			if(el!="")
				prod.push_back(el);
		}
		prods.push_back(prod);
	}
	return prods;
}


void addFirstSet(string var){
	list<list<string> > prods = productions[var];
	for(list<list<string> >::iterator j=prods.begin();j!=prods.end();j++){
		for(list<string>::iterator k=j->begin();k!=j->end();k++){
			if(isTerminal(*k)){
				elements[var].first.insert(*k);
				break;
			}else{
				addFirstSet(*k);
				bool repeat = false;
				for(set<string>::iterator i=elements[*k].first.begin();i!=elements[*k].first.end();i++){
					if(*k=="e")
						repeat = true;
					else
						elements[var].first.insert(*i);
				}
				if(!repeat)
					break;
			}
		}
	}
}

void createFirstSet(){
	for(map<string,list<list<string > > >::iterator i=productions.begin();i!=productions.end();i++){
		addFirstSet(i->first);
	}
}

void createFollowSet(){
	bool changed = false;
	for(map<string,list<list<string > > >::iterator i=productions.begin();i!=productions.end();i++){
		string var = i->first;
		for(list<list<string > >::iterator j=i->second.begin();j!=i->second.end();j++){
			for(list<string>::iterator k=j->begin();k!=j->end();k++){
				if(isTerminal(*k)) continue;
				list<string>::iterator next = k;
				next++;
				bool repeat = true;
				while(next!=j->end()&&repeat){
					repeat = false;
					if(isTerminal(*next)){
						repeat = false;
						if(elements[*k].follow.insert(*next).second)
							changed = true;
					}
					else{
						for(set<string>::iterator l=elements[*next].first.begin();l!=elements[*next].first.end();l++){
							if(*l=="e")
								repeat = true;
							else{
								if(elements[*k].follow.insert(*l).second)
									changed = true;
							}
						}
					}
					next++;
				}
				if(repeat){
					// cout<<"Repeat : "<<var<<" "<<*k<<endl;
					for(set<string>::iterator l=elements[var].follow.begin();l!=elements[var].follow.end();l++){
						if(*l!="e"){
							if(elements[*k].follow.insert(*l).second)
								changed = true;
						}
					}
				}
			}
		}
	}
	if(changed){
		createFollowSet();
	}
}


int main(int argc, char* argv[]){

	if(argc<2){
		cout<<"invalid input"<<endl;
		return 0;
	}
	ifstream f(argv[1]);
	string line;
	getline(f,line);
	productions[getElement(line)]=getProductions(line);
	elements[getElement(line)]=FnF();
	elements[getElement(line)].follow.insert("$");

	while(getline(f,line)){
		productions[getElement(line)] = getProductions(line);		
	}

	createFirstSet();
	createFollowSet();
	for(map<string,FnF>::iterator i=elements.begin();i!=elements.end();i++){
		cout<<i->first<<endl;
		i->second.print();
		cout<<endl;
	}

	return 0;
}