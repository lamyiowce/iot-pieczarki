import datetime
import logging
import asyncio
import aiocoap.resource as resource
import aiocoap
import json
import sqlite3 as db

CULTIVATION = 1

class DataResource(resource.Resource):
    async def render_put(self, request):
        print('PUT request: %s' % request.payload)
        try:
            req = json.loads(str(request.payload))
            conn = db.connect('database')
            c = conn.cursor()
            c.execute("INSERT INTO stats (value, date, type, cultivation) VALUES (" + req['v'] + ", datetime('now'), " + req['t'] + ", " + str(CULTIVATION) + ")")
            return aiocoap.Message(code = aiocoap.CREATED)
        except Exception as e:
            print(e)
            return aiocoap.Message(code = aiocoap.INTERNAL_SERVER_ERROR)

#I dont know why it aint working, these are testing resources, first doesn't work, second does

class CoapTest(resource.Resource):
    async def render_get(self, request):
        print('GET request: %s' % request.payload)
        return aiocoap.Message(payload = "JOŁ")


class SeparateLargeResource(resource.Resource):
    """Example resource which supports the GET method. It uses asyncio.sleep to
    simulate a long-running operation, and thus forces the protocol to send
    empty ACK first. """

    def get_link_description(self):
        # Publish additional data in .well-known/core
        return dict(**super().get_link_description(), title="A large resource")

    async def render_get(self, request):
        await asyncio.sleep(3)

        payload = "Three rings for the elven kings under the sky, seven rings "\
                "for dwarven lords in their halls of stone, nine rings for "\
                "mortal men doomed to die, one ring for the dark lord on his "\
                "dark throne.".encode('ascii')
        return aiocoap.Message(payload=payload)

# logging setup

logging.basicConfig(level=logging.INFO)
logging.getLogger("coap-server").setLevel(logging.DEBUG)

def main():
    root = resource.Site()
    root.add_resource('data', DataResource())
    root.add_resource('test', CoapTest())
    root.add_resource(('other', 'separate'), SeparateLargeResource())
    asyncio.Task(aiocoap.Context.create_server_context(root))
    asyncio.get_event_loop().run_forever()

if __name__ == "__main__":
    main()
