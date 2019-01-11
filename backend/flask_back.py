from flask import Flask
import sqlite3 as db

app = Flask(__name__)

@app.route('/')
def index():
    return 'The best automatic mushroom farm manager ever!\n'

@app.route('/cultivations')
def cultivations():
    conn = db.connect('database')
    c = conn.cursor()
    c.execute('SELECT * FROM cultivations ORDER BY id')
    res = '['
    for row in c:
        res += '{ "id": ' + str(row['id']) + ', "name": "' + row['name'] + '"},'
    res += ']'
    return res

@app.route('/stats/<cultivation>')
def stats(cultivation):
    conn = db.connect('database')
    types = conn.cursor()
    types.execute('SELECT name,id FROM types')
    res = '{'
    for typ in types:
        t = typ[0]
        tid = typ[1]
        c = conn.cursor()
        sql = 'SELECT date, value FROM stats WHERE type = ' + str(tid) + ' AND cultivation = ' + str(cultivation) + ' ORDER BY datetime(date)'
        c.execute(sql)
        if(c.rowcount > 0):
            res += '"' + t + '": [';
            for row in c:
                res += '{ "date": "' + row[0] + '", "value": ' + str(row[1]) + '},'
            res += ']'
    res += '}'
    return res
