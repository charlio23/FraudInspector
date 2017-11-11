#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
using namespace std;

int MAX_COLUMNS_CLIENT = 14;
int MAX_COLUMNS_TRANSACTION = 7;

struct transactionData {
	string source;
	string dest;
	string date;
	string time;
	string amount;
	string currency;
};

struct clientData {
	string firstName;
	vector< pair< pair<string,string>, string> > transactions;
};



int main() {
	map<string,clientData> Clients;
	map<string,transactionData> Transactions;
	ifstream clientFile("clients.small.csv");
	ifstream transactionsFile("transactions.small.csv");
	bool first = true;
	while (clientFile.good()) {
		clientData tuple;
		string idClient;
		getline(clientFile,idClient,',');
		getline ( clientFile, tuple.firstName, ',' );
		string uselessValue;
		for(int i = 0; i < MAX_COLUMNS_CLIENT-3; ++i) {
			getline(clientFile, uselessValue, ',');
		}
		getline(clientFile, uselessValue, '\n');
	    if (first) first = false;
	    else Clients.insert(pair<string,clientData>(idClient,tuple));
			
	} 
	first = true;
	while(transactionsFile.good()) {
		transactionData tuple;
		string idTransaction;
		getline(transactionsFile,idTransaction, ',');
		getline(transactionsFile, tuple.source, ',');
		getline(transactionsFile, tuple.dest, ',');	
		getline(transactionsFile, tuple.date, ',');	
		string uselessValue;
		for(int i = 0; i < MAX_COLUMNS_TRANSACTION-5; ++i) {
			getline(transactionsFile, tuple.amount, ',');
		}
		
	    getline(transactionsFile, uselessValue, '\n');
	    if (first) first = false;
	    else Transactions.insert(pair<string,transactionData>(idTransaction,tuple));
	}
	cout << Clients["856508a8-10ca-485c-b290-9cb79e0a6065"].firstName << endl;
	cout << Clients["762f3be5-1637-4bc4-8cd4-3ebe895236db"].firstName << endl;
	first = true;
	for (map<string,transactionData>::iterator it=Transactions.begin(); it!=Transactions.end(); ++it){
		string source = it->second.source;
		string dest = it->second.dest;
		string amount = it->second.amount;
		string date = it->second.date;
		if (first) cout << ' ' << source << ' ' << dest << endl;
		Clients[source].transactions.push_back(pair< pair<string,string>, string>(pair<string,string>(dest,amount),date));
		first = false;
	}
	for (map<string,clientData>::iterator it=Clients.begin(); it!=Clients.end(); ++it) {
		int sizeT = it->second.transactions.size();
		cout << it->second.firstName << endl;
		for (int i = 0; i < sizeT; ++i){
			string idClient = it->second.transactions[i].first.first;
			string amount = it->second.transactions[i].first.second;
			string date = it->second.transactions[i].second;
			if (amount != "" and stoi(amount) > 100) cout << '\t' << Clients[idClient].firstName << ' ' << amount << ' ' << date << endl;
		}
	}
}