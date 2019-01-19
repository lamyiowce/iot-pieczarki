import datetime
import logging
import asyncio
import aiocoap.resource as resource
import aiocoap
import json
import sqlite3 as db
import os

CULTIVATION = 1

class DataResource(resource.Resource):

    async def render_put(self, request):
        logger = logging.getLogger("coap-server")
        logger.info('PUT request: %s' % request.payload.decode('utf-8'))
        req = json.loads(request.payload.decode('utf-8'))
        conn = db.connect('database')
        c = conn.cursor()
        c.execute("INSERT INTO stats (value, date, type, cultivation) VALUES (" + str(req['v']) + ", datetime('now'), " + str(req['t']) + ", " + str(CULTIVATION) + ")")
        conn.commit()
        return aiocoap.Message(code = aiocoap.CREATED)

class CoapTest(resource.Resource):
    async def render_get(self, request):
        logger = logging.getLogger("coap-server")
        logger.info('GET request: %s' % request.payload)
        return aiocoap.Message(payload = "JO".encode('ascii'))

# logging setup

logging.basicConfig(level=logging.INFO)
logging.getLogger("coap-server").setLevel(logging.DEBUG)

def main():
    print("Transport engine:")
    print(os.environ['AIOCOAP_SERVER_TRANSPORT'])
    print(aiocoap.defaults.get_default_servertransports())
    root = resource.Site()
    root.add_resource(('data',), DataResource())
    root.add_resource(('test',), CoapTest())
    asyncio.Task(aiocoap.Context.create_server_context(root, bind=('192.168.0.129', 8080)))
    asyncio.get_event_loop().run_forever()

if __name__ == "__main__":
    main()
