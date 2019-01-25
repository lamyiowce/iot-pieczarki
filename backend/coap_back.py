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
        logger.info('PUT request: %s' % request.payload)
        #req = json.loads(request.payload.decode('utf-8'))
        buf = request.payload
        sensor_id = buf[0]
        data = 0
        for i in range(1,5):
            data = data * 256 + buf[i]
        conn = db.connect('database')
        c = conn.cursor()
        c.execute("INSERT INTO stats (value, date, type, cultivation) VALUES (" + str(data) + ", datetime('now'), " + str(sensor_id) + ", " + str(CULTIVATION) + ")")
        conn.commit()
        return aiocoap.Message(code = aiocoap.CREATED)

class ConfigResource(resource.Resource):
    async def render_post(self, request):
        num = int.from_bytes(request.payload, byteorder='big')
        logger = logging.getLogger("coap-server")
        logger.info('GET config request: %s' % request.payload)
        logger.info('num = %d', num)
        conn = db.connect('database')
        c = conn.cursor()
        c.execute("SELECT interval FROM types WHERE id = %d" % num)
        t = c.fetchone()[0]
        if t:
            ret = t.to_bytes(4, byteorder='big')
            logger.info('Returned: t = %d, ret = %d' % (t, int.from_bytes(ret, byteorder='big')))
            return aiocoap.Message(code = aiocoap.CONTENT, payload = ret)
        else:
            logger.info('Bad Request')
            return aiocoap.Message(code = aiocoap.BAD_REQUEST)

class CoapTest(resource.Resource):
    async def render_get(self, request):
        logger = logging.getLogger("coap-server")
        logger.info('GET request: %s' % request.payload)
        return aiocoap.Message(payload = "JO".encode('ascii'))

# logging setup

logging.basicConfig(level=logging.INFO)
logging.getLogger("coap-server").setLevel(logging.DEBUG)

def main():
    root = resource.Site()
    root.add_resource(('data',), DataResource())
    root.add_resource(('test',), CoapTest())
    root.add_resource(('conf',), ConfigResource())
    asyncio.Task(aiocoap.Context.create_server_context(root, bind=('192.168.43.140', 8080)))
    #asyncio.Task(aiocoap.Context.create_server_context(root, bind=('localhost', 20002)))
    asyncio.get_event_loop().run_forever()

if __name__ == "__main__":
    main()
