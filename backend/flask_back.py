from flask import Flask
import sqlite3 as db
import json

app = Flask(__name__)

@app.route('/')
def index():
    return 'The best automatic mushroom farm manager ever!\n'

@app.route('/cultivations')
def cultivations():
    conn = db.connect('database')
    c = conn.cursor()
    c.execute('SELECT id, name FROM cultivations ORDER BY id')
    res = []
    for row in c:
        res.append({"id": row[0], "name": row[1]})
    return json.dumps(res)

@app.route('/stats/<cultivation>')
def stats(cultivation):
    conn = db.connect('database')
    types = conn.cursor()
    types.execute('SELECT name,id FROM types')
    res = {}
    for typ in types:
        t = typ[0]
        tid = typ[1]
        c = conn.cursor()
        sql = 'SELECT date, value FROM stats WHERE type = ' + str(tid) + ' AND cultivation = ' + str(cultivation) + ' ORDER BY datetime(date)'
        c.execute(sql)
        res[t] = []
        for row in c:
            res[t].append({"date": row[0], "value": row[1]})
            print(res)
    return json.dumps(res)


@app.route('/freq/<string:type>/<int:value>')
def freq(type, value):
    import time, random
    print(type, value)
    time.sleep(2)
    r = random.random()
    print(r)
    if r > 0.5:
        return json.dumps({'success': True}), 200, {'ContentType': 'application/json'}
    else:
        return json.dumps({'success': False}), 500, {'ContentType': 'application/json'}


@app.after_request
def after_request(response):
  response.headers.add('Access-Control-Allow-Origin', '*')
  response.headers.add('Access-Control-Allow-Headers', 'Content-Type,Authorization')
  response.headers.add('Access-Control-Allow-Methods', 'GET,PUT,POST,DELETE,OPTIONS')
  return response
