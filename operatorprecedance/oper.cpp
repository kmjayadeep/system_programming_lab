// Complile this program with the command g++ inter.cpp --std=c++11 
#include<iostream>
#include<fstream>
#include<sstream>
#include<map>
#include<list>
#include<set>

using namespace std;

bool isTerminal(string s){
	return !isupper(s[0]);
}

class Op{
public:
	set<string> firstop;
	set<string> lastop;
	list<list<string> > productions;
	void print(){
		cout<<"firstop\t:\t";
		for(auto i=firstop.begin();i!=firstop.end();i++)
			cout<<*i<<" ";
		cout<<endl<<"Lastop\t:\t";
		for(auto i=lastop.begin();i!=lastop.end();i++)
			cout<<*i<<" ";
		cout<<endl;
	}
	void buildFirstAndLast(){
		for(auto i=productions.begin();i!=productions.end();i++){
			for(auto j=i->begin();j!=i->end();j++){
				firstop.insert(*j);
				if(isTerminal(*j))
					break;
			}
			for(auto j=i->rbegin();j!=i->rend();j++){
				lastop.insert(*j);
				if(isTerminal(*j))
					break;
			}
		}
	}
};

map<string,Op> nonTerminals;
set<string> terminals;
map<string,map<string,string> > table;

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
			if(el!=""){
				prod.push_back(el);
				if(isTerminal(el))
					terminals.insert(el);
			}
		}
		prods.push_back(prod);
	}
	return prods;
}

void applyClosure(){
	for(auto i=nonTerminals.begin();i!=nonTerminals.end();i++){
		string element = i->first;
		auto firstop = i->second.firstop;
		auto lastop = i->second.lastop;
		for(auto j=nonTerminals.begin();j!=nonTerminals.end();j++){
			if(j->second.firstop.find(element)!=j->second.firstop.end()){
				j->second.firstop.insert(firstop.begin(),firstop.end());
			}
			if(j->second.lastop.find(element)!=j->second.lastop.end()){
				j->second.lastop.insert(lastop.begin(),lastop.end());
			}

		}
	}
}

void removeNonTerminals(){
	for(auto i=nonTerminals.begin();i!=nonTerminals.end();i++){
		auto j=i->second.firstop.begin();
		while(j!=i->second.firstop.end()){
			if(!isTerminal(*j))
				j = i->second.firstop.erase(j);
			else
				j++;
		}
		j=i->second.lastop.begin();
		while(j!=i->second.lastop.end()){
			if(!isTerminal(*j))
				j = i->second.lastop.erase(j);
			else
				j++;
		}
	}
}

void fillStart(string s){
	auto start = nonTerminals[s];
	for(auto j=start.firstop.begin();j!=start.firstop.end();j++){
		table["$"][*j] = "<";
	}
	for(auto j=start.lastop.begin();j!=start.lastop.end();j++){
		table[*j]["$"] = ">";
	}
}

void fillTable(){
	for(auto i=nonTerminals.begin();i!=nonTerminals.end();i++){
		for(auto j=i->second.productions.begin();j!=i->second.productions.end();j++){
			for(auto k=j->begin();k!=j->end();k++){
				auto next = k;
				auto after = k;
				if(next!=j->end()){
					next++;
					after++;
					if(next==j->end()){
						after--;
						next--;
					}
				}
				if(after!=j->end()){
					after++;
					if(after==j->end())
						after--;
				}
				if(isTerminal(*k)&&!isTerminal(*next)){
					Op op = nonTerminals[*next];
					for(auto l=op.firstop.begin();l!=op.firstop.end();l++)
						table[*k][*l] = "<";
					if(isTerminal(*after))
						table[*k][*after]="=";
				}
				if(!isTerminal(*k)&&isTerminal(*next)){
					Op op = nonTerminals[*k];
					for(auto l=op.lastop.begin();l!=op.lastop.end();l++)
						table[*l][*next] = ">";
				}
				if(k!=next&&isTerminal(*k)&&isTerminal(*next)){
					table[*k][*next]="=";
				}

			}
		}
	}
}

int main(int argc, char* argv[]){
    if(argc<2){
        cout<<"invalid input"<<endl;
        return 0;
    }
    ifstream fin(argv[1]);
    string line, el, start;
    getline(fin,line);
    el = getElement(line);
	nonTerminals[el].productions = getProductions(line);
	nonTerminals[el].buildFirstAndLast();
	start = el;
    while(getline(fin,line)){
    	el = getElement(line);
		nonTerminals[el].productions = getProductions(line);
		nonTerminals[el].buildFirstAndLast();
	}
	terminals.insert("$");
	applyClosure();
	removeNonTerminals();
	fillStart(start);
	fillTable();

	for(auto i = nonTerminals.begin();i!=nonTerminals.end();i++){
		cout<<endl<<i->first<<endl<<"-----"<<endl;
		i->second.print();
	}

	cout<<"\n\n\t";
	for(auto i = terminals.begin();i!=terminals.end();i++){
		cout<<*i<<'\t';
	}
	cout<<endl<<endl;
	for(auto i = terminals.begin();i!=terminals.end();i++){
		cout<<*i<<"\t";
		for(auto j = terminals.begin();j!=terminals.end();j++){
			cout<<table[*i][*j]<<'\t';
		}
		cout<<endl<<endl;
	}
	cout<<endl;

    return 0;
}