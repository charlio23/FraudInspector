#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
using namespace std;

//Define your structs here

struct transactionData {
	string source;
	string target;
	string date;
	string time;
	string amount;
	string currency;
};

struct clientData {
	string first_name;
	string last_name;
	string age;
	string email;
	string ocupation;
	string political_views;
	string nationality;
	string university;
	string academic_degree;
	string address;
	string postal_code;
	string country;
	string city;
	vector<string> transactions;
};

struct atmData {
	string latitude;
	string longitude;
	vector<string> transactions;
};

struct companyData {
	string type;
	string name;
	string country;
	vector<string> transactions;
};

//Define yoyr global data here

int MAX_COLUMNS_CLIENT = 14;
int MAX_COLUMNS_TRANSACTION = 7;

map<string, clientData> Clients;
map<string, transactionData> Transactions;
map<string, companyData> Companies;
map<string, atmData> Atms;

void parseClientData(string csv) {
	ifstream clientFile(csv);
	bool first = true;
	while (clientFile.good()) {
		clientData tuple;
		string idClient;
		getline(clientFile,idClient,',');
		getline (clientFile, tuple.first_name, ',' );
		getline(clientFile, tuple.last_name, ',');
		getline(clientFile, tuple.age, ',');
		getline(clientFile, tuple.email, ',');
		getline(clientFile, tuple.ocupation, ',');
		getline(clientFile, tuple.political_views, ',');
		getline(clientFile, tuple.nationality, ',');
		getline(clientFile, tuple.university, ',');
		getline(clientFile, tuple.academic_degree, ',');
		getline(clientFile, tuple.address, ',');
		getline(clientFile, tuple.country, ',');
		getline(clientFile, tuple.city, '\n');
	    if (first) first = false;
	    else if (idClient != "") Clients.insert(pair<string,clientData>(idClient,tuple));
	}
}

void parseCompanyData(string csv) {
	ifstream companyFile(csv);
	bool first = true;
	while (companyFile.good()) {
		companyData tuple;
		string idCompany;
		getline(companyFile,idCompany,',');
		getline (companyFile, tuple.type, ',' );
		getline(companyFile, tuple.name, ',');
		getline(companyFile, tuple.country, '\n');
	    if (first) first = false;
	    else if (idCompany != "") Companies.insert(pair<string,companyData>(idCompany,tuple));
	}
}

void parseAtmData(string csv) {
	ifstream atmFile(csv);
	bool first = true;
	while (atmFile.good()) {
		atmData tuple;
		string idAtm;
		getline(atmFile, idAtm, ',');
		getline(atmFile, tuple.latitude, ',');
		getline(atmFile, tuple.longitude, '\n');
	    if (first) first = false;
	    else if (idAtm != "") Atms.insert(pair<string,atmData>(idAtm,tuple));
	}
}

void parseTransactionData(string csv) {
	ifstream transactionsFile(csv);
	bool first = true;
	while(transactionsFile.good()) {
		transactionData tuple;
		string idTransaction;
		getline(transactionsFile,idTransaction, ',');
		getline(transactionsFile, tuple.source, ',');
		getline(transactionsFile, tuple.target, ',');	
		getline(transactionsFile, tuple.date, ',');	
		getline(transactionsFile, tuple.time, ',');
		getline(transactionsFile, tuple.amount, ',');
	    getline(transactionsFile, tuple.currency, '\n');
	    if (first) first = false;
	    else if (idTransaction != "") Transactions.insert(pair<string,transactionData>(idTransaction,tuple));
	}
}

int main() {
	parseClientData("clients.small.csv");
	parseAtmData("atms.small.csv");
	parseCompanyData("companies.small.csv");
	parseTransactionData("transactions.small.csv");
	for (map<string,transactionData>::iterator it=Transactions.begin(); it!=Transactions.end(); ++it){
		string transactionID = it->first;
		transactionData transaction = it->second;
		if (Atms.find(transaction.source) != Atms.end()) {
			Atms[transaction.source].transactions.push_back(transactionID);
		} else if (Companies.find(transaction.source) != Companies.end()) {
			Companies[transaction.source].transactions.push_back(transactionID);
		} else {
			Clients[transaction.source].transactions.push_back(transactionID);
		}
	}
	for (map<string,clientData>::iterator it=Clients.begin(); it!=Clients.end(); ++it) {
		clientData client = it->second;
		//cout << client.first_name << ' ' << client.last_name << endl;
		int sizeT = client.transactions.size();
		for (int i = 0; i < sizeT; ++i) {
			transactionData transaction = Transactions[client.transactions[i]];
			if (Clients.find(transaction.target) != Clients.end()){
				cout << '\t' << Clients[transaction.target].first_name << ' ' << Clients[transaction.target].last_name << ' ' << transaction.amount << endl;
			} else if (Companies.find(transaction.target) != Companies.end()) {
				cout << '\t' << Companies[transaction.target].name << ' ' << transaction.amount << endl;
			} else {
				cout << '\t' << Atms[transaction.target].latitude << ' ' << transaction.amount << endl;
			}
		}
	}
	for (map<string,atmData>::iterator it=Atms.begin(); it!=Atms.end(); ++it) {
		atmData atm = it->second;
		cout << atm.latitude << ' ' << endl;
		int sizeT = atm.transactions.size();
		for (int i = 0; i < sizeT; ++i) {
			transactionData transaction = Transactions[atm.transactions[i]];
			if (Clients.find(transaction.target) != Clients.end()){
				cout << '\t' << Clients[transaction.target].first_name << ' ' << Clients[transaction.target].last_name << ' ' << transaction.amount << endl;
			} else if (Companies.find(transaction.target) != Companies.end()) {
				cout << '\t' << Companies[transaction.target].name << ' ' << transaction.amount << endl;
			} else {
				cout << '\t' << Atms[transaction.target].latitude << ' ' << transaction.amount << endl;
			}
		}
	}

	for (map<string,companyData>::iterator it=Companies.begin(); it!=Companies.end(); ++it) {
		companyData company = it->second;
		cout << company.name << ' ' << endl;
		int sizeT = company.transactions.size();
		for (int i = 0; i < sizeT; ++i) {
			transactionData transaction = Transactions[company.transactions[i]];
			if (Clients.find(transaction.target) != Clients.end()){
				cout << '\t' << Clients[transaction.target].first_name << ' ' << Clients[transaction.target].last_name << ' ' << transaction.amount << endl;
			} else if (Companies.find(transaction.target) != Companies.end()) {
				cout << '\t' << Companies[transaction.target].name << ' ' << transaction.amount << endl;
			} else {
				cout << '\t' << Atms[transaction.target].latitude << ' ' << transaction.amount << endl;
			}
		}
	}
}