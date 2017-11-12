#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
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
	vector<string> receptions;
};

struct atmData {
	string latitude;
	string longitude;
	vector<string> transactions;
	vector<string> receptions;
};

struct companyData {
	string type;
	string name;
	string country;
	vector<string> transactions;
	vector<string> receptions;
};

//Define yoyr global data here

int MAX_COLUMNS_CLIENT = 14;
int MAX_COLUMNS_TRANSACTION = 7;
int MAX_SUM_CLIENT_CUTOFF = 2e3;
int MAX_ENTRY_DEGREE = 3;
int MAX_CUTOFF_INDIVIDUAL = 1e2;
double STD_MAX_CUTTOFF = 20.0;

map<string, clientData> Clients;
map<string, transactionData> Transactions;
map<string, companyData> Companies;
map<string, atmData> Atms;
map<string,string> visited;

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

bool checkDecrease(string bigDate, string smallDate) {
	if (bigDate.substr(6,4) != smallDate.substr(6,4)) return false;
	else if (bigDate.substr(0,2) > smallDate.substr(0,2)) return true;
	else if (bigDate.substr(0,2) < smallDate.substr(0,2)) return false;
	return bigDate.substr(3,2) >= smallDate.substr(3,2);
}

bool checkIncrease(string smallDate, string bigDate) {
	if (bigDate.substr(6,4) != smallDate.substr(6,4)) return false;
	else if (bigDate.substr(0,2) > smallDate.substr(0,2)) return true;
	else if (bigDate.substr(0,2) < smallDate.substr(0,2)) return false;
	return bigDate.substr(3,2) >= smallDate.substr(3,2);
}

map<string,string> visit;

void printDAG(const string &idGoal,const string &idTransaction, const int &num) {
	transactionData transaction = Transactions[idTransaction];
	cout << "A circular pattern of length " << num+1 << " has been found! (Average amount: " << transaction.amount <<")" << endl;
	string idActual = transaction.target;
	string idTrans;
	cout << "From " << transaction.date << ", ";
	if (Companies.find(idGoal) != Companies.end()) {
			cout << Companies[idGoal].name;
	} else {
		cout << Clients[idGoal].first_name << ' ' << Clients[idActual].last_name;
	}
	while(idGoal != idActual) {
		cout << " -> ";
		if (Companies.find(idActual) != Companies.end()) {
			cout << Companies[idActual].name;
		} else {
			cout << Clients[idActual].first_name << ' ' << Clients[idActual].last_name;
		}
		idTrans = visit[idActual];
		idActual = Transactions[idTrans].target;
	}
	cout <<". Until " << Transactions[idTrans].date << endl;
}

int checkCicle(string idStart, string idGoal, string idTransaction) {
	int num = 0;
	double initAmount = stoi(Transactions[idTransaction].amount);
	queue<pair<string,string>> q;
 	visit.clear();
	q.push(pair<string,string>(idStart,idTransaction));
	visit.insert(pair<string,string>(idStart,idTransaction));
	while (not q.empty()) {
		++num;
		string id = q.front().first;
		string idTrans = q.front().second; q.pop();
		vector<string> receptions;
		if (Companies.find(id) != Companies.end()) {
			receptions = Companies[id].receptions;
		} else {
			receptions = Clients[id].receptions;
		}
		string bigDate = Transactions[idTransaction].date;
		for (int i = 0; i < receptions.size(); ++i) {
			transactionData reception = Transactions[receptions[i]];
			double actualAmount = stod(reception.amount);
			double err = abs(actualAmount-initAmount) - 0.05*initAmount;
			if (err < 0 and checkDecrease(bigDate,reception.date) and actualAmount > MAX_CUTOFF_INDIVIDUAL) {
				if (idGoal == reception.source) {
					printDAG(idGoal,receptions[i], num);
					return num;
				} else if (visit.find(reception.source) == visit.end()){
					q.push(pair<string,string>(reception.source,receptions[i]));
					visit.insert(pair<string,string>(reception.source,receptions[i]));
				}
			}
		}
	}
	return 0;
}

double sumAmounts(const vector<string> &transactions) {
	double sum = 0;
	for (int i = 0; i < transactions.size(); ++i) {
		sum += stod(Transactions[transactions[i]].amount);
	}
	return sum;
}

double meanAmounts(const vector<string> &transactions) {
	return sumAmounts(transactions)/float(transactions.size());
}

double stdeviationAmounts(const vector<string> &transactions) {
	double mean = meanAmounts(transactions);
	double sum = 0;
	for (int i = 0; i < transactions.size(); ++i) {
		double x = stod(Transactions[transactions[i]].amount);;
		sum += (x - mean)*(x - mean);
	}
	return sqrt(sum/double(transactions.size()));
}

void printReceptions(const vector<string> &receptions) {
	clientData client = Clients[Transactions[receptions[0]].target];
	cout << '\t' << client.first_name << ' ' << client.last_name << " received " << sumAmounts(receptions)<< " from:" << endl;
	for (int i = 0; i < receptions.size(); ++i) {
		if (Companies.find(Transactions[receptions[i]].source) != Companies.end()) {
			companyData emissor = Companies[Transactions[receptions[i]].source];
			cout << "\t\t-> On " << Transactions[receptions[i]].date << ", " << emissor.name << " transferred " <<  Transactions[receptions[i]].amount << endl;
		} else { 
			clientData emissor = Clients[Transactions[receptions[i]].source];
			cout << "\t\t-> On " << Transactions[receptions[i]].date << ", " << emissor.first_name << ' ' << emissor.last_name << " transferred " <<  Transactions[receptions[i]].amount << endl;
		}
	}
}

void printTransmisions(const vector<string> &transmisions) {
	clientData client = Clients[Transactions[transmisions[0]].source];
	cout << '\t' << client.first_name << ' ' << client.last_name << " transmitted " << sumAmounts(transmisions)<< " from:" << endl;
	for (int i = 0; i < transmisions.size(); ++i) {
		if (Companies.find(Transactions[transmisions[i]].target) != Companies.end()) {
			companyData emissor = Companies[Transactions[transmisions[i]].target];
			cout << "\t\t-> On " << Transactions[transmisions[i]].date << ", " << emissor.name << ' ' << " received " <<  Transactions[transmisions[i]].amount << endl;
		} else {
			clientData emissor = Clients[Transactions[transmisions[i]].target];
			cout << "\t\t-> On " << Transactions[transmisions[i]].date << ", " << emissor.first_name << ' ' << emissor.last_name << " received " <<  Transactions[transmisions[i]].amount << endl;
		}
	}
}

int main() {
	cout << "Parsing data..." << endl;
	parseClientData("clients.small.csv");
	parseAtmData("atms.small.csv");
	parseCompanyData("companies.small.csv");
	parseTransactionData("transactions.small.csv");
	cout << "Generating graph..." << endl;
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
		if (Atms.find(transaction.target) != Atms.end()) {
			Atms[transaction.target].receptions.push_back(transactionID);
		} else if (Companies.find(transaction.target) != Companies.end()) {
			Companies[transaction.target].receptions.push_back(transactionID);
		} else {
			Clients[transaction.target].receptions.push_back(transactionID);
		}
	}
	//Client map clean
	cout << "Cleaning graph form client current transactions..." << endl;
	for (map<string,clientData>::iterator it=Clients.begin(); it!=Clients.end();){
		string clientID = it->first;
		if (sumAmounts(it->second.transactions) < MAX_SUM_CLIENT_CUTOFF and 
			sumAmounts(it->second.receptions) < MAX_SUM_CLIENT_CUTOFF) {
			++it;
			Clients.erase(clientID);
		} else ++it;
	}

	cout << "Searching for suspicious behaviour related to reveiver's degree..." << endl;
	cout << "------------------------------------------------------------------" << endl;
	for (map<string,clientData>::iterator it = Clients.begin(); it!=Clients.end(); ++it) {
		clientData client = it->second;
		map<string, vector<string> > dateMap;
		for (int i = 0; i < client.receptions.size(); ++i) {
			transactionData transaction = Transactions[client.receptions[i]];
			if (dateMap.find(transaction.date) != dateMap.end()) {
				dateMap[transaction.date].push_back(client.receptions[i]);
			} else {
				vector<string> aux(1);
				aux[0] = client.receptions[i];
				dateMap.insert(pair<string, vector<string> >(transaction.date,aux));
			}
		}
		for (map<string,vector<string> >::iterator it = dateMap.begin(); it != dateMap.end(); ++it) {
			if (it->second.size() >= MAX_ENTRY_DEGREE) {
				if (stdeviationAmounts(it->second) < STD_MAX_CUTTOFF and
					sumAmounts(it->second) > MAX_SUM_CLIENT_CUTOFF) printReceptions(it->second);
			}
		}
		dateMap.clear();
	}
	cout << "------------------------------------------------------------------" << endl;
	cout << "Searching for suspicious behaviour related to transmitter's degree..." << endl;
	cout << "------------------------------------------------------------------" << endl;
	for (map<string,clientData>::iterator it = Clients.begin(); it!=Clients.end(); ++it) {
		clientData client = it->second;
		map<string, vector<string> > dateMap;
		for (int i = 0; i < client.transactions.size(); ++i) {
			transactionData transaction = Transactions[client.transactions[i]];
			if (dateMap.find(transaction.date) != dateMap.end()) {
				dateMap[transaction.date].push_back(client.transactions[i]);
			} else {
				vector<string> aux(1);
				aux[0] = client.transactions[i];
				dateMap.insert(pair<string, vector<string> >(transaction.date,aux));
			}
		}
		for (map<string,vector<string> >::iterator it = dateMap.begin(); it != dateMap.end(); ++it) {
			if (it->second.size() >= MAX_ENTRY_DEGREE) {
				if (stdeviationAmounts(it->second) < STD_MAX_CUTTOFF and
					sumAmounts(it->second) > MAX_SUM_CLIENT_CUTOFF) printTransmisions(it->second);
			}
		}
		dateMap.clear();
	}

	int countDag = 0;
 	//Search for DAG between clients and enterprises
 	cout << "------------------------------------------------------------------" << endl;
	cout << "Searching for suspicious DAGs relationships... (This may take a while)"<< endl;
	cout << "------------------------------------------------------------------" << endl;
	queue<pair<string,string> > q;
	double currentAmount;
	for (map<string,clientData>::iterator it = Clients.begin(); it!=Clients.end(); ++it) {
		if (visited.find(it->first) == visited.end()) {
			visited.insert(pair<string,string>(it->first,""));
			q.push(pair<string,string> (it->first,""));
			while (not q.empty()) {
				string id = q.front().first; 
				string idTransaction = q.front().second; q.pop();
				vector<string> transactions;
				if (Companies.find(id) != Companies.end()) {
					transactions = Companies[id].transactions;
				} else transactions = Clients[id].transactions;
				for (int i = 0; i < transactions.size(); ++i) {
					transactionData transaction = Transactions[transactions[i]];
					if (idTransaction == "" or checkIncrease(Transactions[idTransaction].date, transaction.date)) {
						if (visited.find(transaction.target) == visited.end() and
							 stod(transaction.amount) > MAX_CUTOFF_INDIVIDUAL){
							q.push(pair<string,string> (transaction.target,transactions[i]));
							visited.insert(pair<string,string>(transaction.target,id));
						} else if (checkCicle(id,transaction.target,transactions[i])> 0) ++countDag;
					}
				}
			}
		}
	}
	/*
	for (map<string,companyData>::iterator it = Companies.begin(); it!=Companies.end(); ++it) {
		if (visited.find(it->first) == visited.end()) {
			q.push(it->first);
			visited.insert(pair<string,string>(it->first,""));
			while (not q.empty()) {
				string id = q.front(); q.pop();
				vector<string> transactions;
				if (Companies.find(id) != Companies.end()) transactions = Companies[id].transactions;
				else transactions = Clients[id].transactions;
				for (int i = 0; i < transactions.size(); ++i) {
					transactionData transaction = Transactions[transactions[i]];
					int num;
					if (visited.find(transaction.target) == visited.end()) {
						q.push(transaction.target);
						visited.insert(pair<string,string>(transaction.target,id));
					} else if (checkCicle(id,transaction.target)) {
					}
				}
			}
		}
	}*/
	
	/* LOGS
	for (map<string,clientData>::iterator it=Clients.begin(); it!=Clients.end(); ++it) {
		clientData client = it->second;
		cout << client.first_name << ' ' << client.last_name << endl;
		int sizeT = client.transactions.size();
		for (int i = 0; i < sizeT; ++i) {
			transactionData transaction = Transactions[client.transactions[i]];
			if (stoi(transaction.amount) > 100) {
				if (Companies.find(transaction.target) != Companies.end()) {
					cout << '\t' << Companies[transaction.target].name << ' ' << transaction.amount << endl;
				} else if (Atms.find(transaction.target) != Atms.end()) {
					cout << '\t' << Atms[transaction.target].latitude << ' ' << transaction.amount << endl;
				} else {
					cout << '\t' << Clients[transaction.target].first_name << ' ' << Clients[transaction.target].last_name << ' ' << transaction.amount << endl;
				}
			}
		}
	}/*
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
	//*/

}

/*
int checkCicle2(string idStart,string idGoal) {
	int num = 0;
	map<string,bool> visit;
	queue<string> q;
	q.push(idStart);
	visit.insert(pair<string,bool>(idStart,true));
	while (not q.empty()) {
		string id = q.front(); q.pop();
		vector<string> receptions;
		if (Companies.find(id) != Companies.end()) {
				receptions = Companies[id].receptions;
		} else {
				receptions = Clients[id].receptions;
		}
		for (int i = 0; i < receptions.size(); ++i) {
			transactionData reception = Transactions[receptions[i]];
			if (visit.find(reception.source) == visit.end()) {
				q.push(reception.source);
				visit.insert(pair<string,bool>(reception.source,true));
			} else if (reception.source == idGoal) return num;
		}
		++num;
	}
	return 0;
}
*/