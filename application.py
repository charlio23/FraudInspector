import os
from flask import Flask, url_for, render_template, request
app = Flask(__name__)

from pymongo import MongoClient
uri = "mongodb://user:lauzhack2017@165.227.134.175/lauzhack"
client = MongoClient(uri)
db = client.lauzhack

@app.route('/fraudinspector/')
def fraudinspector(name=None):
    url_for('static', filename='bootstrap.css')
    url_for('static', filename='narrow-jumbotron.css')
    url_for('static', filename='starter-template.css')
    url_for('static', filename='jquery-3.js')
    url_for('static', filename='popper.js')
    return render_template('index.html', name=name)

@app.route('/fraudinspector/seetransactions')
def seetransactions():
    orderby = request.args.get('sort')
    direction = request.args.get('dir')

    if orderby == None:
        orderby = 'id'
    if direction == None:
        direction = '1'

    collection = db.Transactions
    cursor = collection.find({'amount': {"$gt": 50000}}).limit(500)
    transactions_dod = []
    for document in cursor:
        mydict = dict(document)
        transactions_dod.append(mydict)

    return render_template('transactions.html', limited=1, transactions=transactions_dod)

@app.route('/fraudinspector/seetransactions/about')
def seetransactionsab():
    url_for('static', filename='bootstrap.css')
    url_for('static', filename='narrow-jumbotron.css')
    url_for('static', filename='starter-template.css')
    url_for('static', filename='jquery-3.js')
    url_for('static', filename='popper.js')

    aboutid = request.args.get('id')

    collection = db.Transactions
    if not (aboutid == None):
       # cursor = collection.find({'amount': {"$gt": 50000}}).limit(500)
        cursor = collection.find({'source': str(aboutid)})
        lim = 0
    else:
        cursor = collection.find({'amount': {"$gt": 50000}}).limit(500)
        lim = 1

    transactions_dod = []
    for document in cursor:
        mydict = dict(document)
        transactions_dod.append(mydict)

    return render_template('transactions.html', limited=lim, transactions=transactions_dod)

@app.route('/fraudinspector/seetransactions/unlimited')
def seetransactionsul():
    url_for('static', filename='bootstrap.css')
    url_for('static', filename='narrow-jumbotron.css')
    url_for('static', filename='starter-template.css')
    url_for('static', filename='jquery-3.js')
    url_for('static', filename='popper.js')

    orderby = request.args.get('sort')
    direction = request.args.get('dir')

    if orderby == None:
        orderby = 'id'
    if direction == None:
        direction = '1'

    collection = db.Transactions
    cursor = collection.find({}).sort([(orderby, int(direction))])
    transactions_dod = []
    for document in cursor:
        mydict = dict(document)
        transactions_dod.append(mydict)

    return render_template('transactions.html', limited=0, transactions=transactions_dod)

@app.route('/fraudinspector/seeclients')
def seeclients():
    orderby = request.args.get('sort')
    direction = request.args.get('dir')

    if orderby == None:
        orderby = 'id'
    if direction == None:
        direction = '1'

    collection = db.Clients
    cursor = collection.find({}).limit(1000).sort([(orderby, int(direction))])
    clients_dod = []
    for document in cursor:
        mydict = dict(document)
        clients_dod.append(mydict)

    return render_template('clients.html', limited=1, clients=clients_dod)

@app.route('/fraudinspector/seeclients/unlimited')
def seeclientsul():
    url_for('static', filename='bootstrap.css')
    url_for('static', filename='narrow-jumbotron.css')
    url_for('static', filename='starter-template.css')
    url_for('static', filename='jquery-3.js')
    url_for('static', filename='popper.js')

    orderby = request.args.get('sort')
    direction = request.args.get('dir')

    if orderby == None:
        orderby = 'id'
    if direction == None:
        direction = '1'

    collection = db.Clients
    cursor = collection.find({}).sort([(orderby, int(direction))])
    clients_dod = []
    for document in cursor:
        mydict = dict(document)
        clients_dod.append(mydict)

    return render_template('clients.html', limited=0, clients=clients_dod)

@app.route('/fraudinspector/seecompanies')
def seecompanies():
    orderby = request.args.get('sort')
    direction = request.args.get('dir')
    datafrom = request.args.get('about')

    if orderby == None:
        orderby = 'id'
    if direction == None:
        direction = '1'

    collection = db.Companies
    cursor = collection.find({}).sort([(orderby, int(direction))])
    companies_dod = []
    for document in cursor:
        mydict = dict(document)
        companies_dod.append(mydict)

    if not (datafrom == None):
        ide = 0
        for elem in companies_dod:
            print elem, datafrom
            if (elem["id"] == datafrom):
                theid = ide
                break
            ide += 1

        print theid

        dname = companies_dod[ide]["name"]
        dcountry = companies_dod[ide]["country"]
        dtype = companies_dod[ide]["type"]
        cname = 0
        ccountry = 0
        ctype = 0

	i = 0
        while (i < len(companies_dod)):
            if (companies_dod[i]["name"] == dname):
                cname = 1

	    if (companies_dod[i]["country"] == dcountry):
                ccountry += 1

            if (companies_dod[i]["type"] == dtype):
                ctype += 1

            i += 1
	
	datavec = [(float(cname)/float(i))*100, (float(cname)/float(ccountry))*100, (float(ccountry)/float(i))*100, (float(ctype)/float(i))*100, dname, dcountry, dtype]

    else:
        datavec = None

    return render_template('companies.html', companies=companies_dod, data=datavec)

@app.route('/fraudinspector/inspection', methods=['GET', 'POST'])
def inspection():
    ammony = request.args.get('amountmoney')
    amtrans = request.args.get('amounttrans')
    country = request.args.get('countries')

    file = open("transactions.csv", "w+")
    file.write("id,source,target,date,time,amount,currency\n")

    collection = db.Transactions
    cursor = collection.find({'amount': {"$gt": int(ammony)}}).limit(int(amtrans))

    for document in cursor:
        mydict = dict(document)
        file.write(str(mydict["id"]).strip()+","+str(mydict["source"]).strip()+","+str(mydict["target"]).strip()+","+str(mydict["date"])+","+str(mydict["time"])+","+str(mydict["amount"])+","+str(mydict["currency"])+"\n")

    file.close()

    os.system("./a.out")

    fraudtopfile = open("fraudulent_top.txt", "r")
    fraudtop = fraudtopfile.readlines()
    graphlogfile = open("graph_logs.txt", "r")
    graphlog = graphlogfile.readlines()

    return render_template('inspection.html', send=[fraudtop, graphlog])
