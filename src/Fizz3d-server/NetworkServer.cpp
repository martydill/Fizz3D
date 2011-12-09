
#include "stdafx.h"
#include "NetworkServer.h"

using namespace std;

struct ClientConnectionInfo
{
	SOCKET socket;
	string name;
	BYTE id;
};

void ProcessPacket(SOCKET socket, const NetworkPacket& packet);
void ErrorHandler(SOCKET socket, int errorCode);

extern ProcessPacketFunction packetProcessor;
extern ErrorHandlerFunction errorHandler;

CRITICAL_SECTION _dataStructureLock;

vector<ClientConnectionInfo*> _clients;

map<uintptr_t, ClientConnectionInfo*> _clientMap;

NetworkServer::NetworkServer()
{
	InitializeCriticalSection(&_dataStructureLock);
	packetProcessor = ProcessPacket;
	errorHandler = ErrorHandler;
}

NetworkServer::~NetworkServer()
{
}

void Broadcast(NetworkPacket& packet)
{

	EnterCriticalSection(&_dataStructureLock);

	std::vector<ClientConnectionInfo*>::iterator i;
	
	for(i = _clients.begin(); i != _clients.end(); ++i)
	{
		ClientConnectionInfo* conn = *i;
		SendPacket(conn->socket, packet);
	}

	LeaveCriticalSection(&_dataStructureLock);
}


void ErrorHandler(SOCKET socket, int errorCode)
{
	if(errorCode == WSAECONNRESET)
	{
		
	}
}


void ProcessPacket(SOCKET socket, const NetworkPacket& packet)
{
	int id;
	ClientConnectionInfo* client;
	NetworkPacket resultPacket;
	string msg;

	switch(packet.MessageId)
	{
		// Send back a join message with the data set to the client id
		// Then broadcast a message to everyone
		case NetworkMessage_Join:
			client = new ClientConnectionInfo();
			client->id = _clients.size();
			client->name = packet.MessageData;
			client->socket = socket;

			EnterCriticalSection(&_dataStructureLock);
			_clients.push_back(client);
			LeaveCriticalSection(&_dataStructureLock);

			msg = client->name + " has joined.";

			cout << msg << endl;

			// Send join confirmation
			memset(&resultPacket, 0, sizeof(NetworkPacket));
			resultPacket.MessageId  = NetworkMessage_Join;
			memcpy(resultPacket.MessageData, &client->id, sizeof(BYTE));
			resultPacket.MessageId = NetworkMessage_Join;
			SendPacket(socket, resultPacket);

			// Send join message
			memset(&resultPacket, 0, sizeof(NetworkPacket));
			resultPacket.MessageId  = NetworkMessage_Message;
			memcpy(resultPacket.MessageData, msg.c_str(), msg.size());
			resultPacket.MessageId = NetworkMessage_Message;
			Broadcast(resultPacket);
			break;

		// Broadcast a MSG message to all clients
		case NetworkMessage_Talk:
			client = _clients[packet.SenderId]; //fixme - do proper find
			msg = client->name + " says \"" + packet.MessageData + "\"";

			cout << client->name << " says \"" << packet.MessageData << "\"" << endl;

			memset(&resultPacket, 0, sizeof(NetworkPacket));
			resultPacket.MessageId = NetworkMessage_Message;
			memcpy(&resultPacket.MessageData, msg.c_str(), msg.size());

			Broadcast(resultPacket);
			//SendPacket(socket, resultPacket);
			break;

		default:
			break;
	}
}


vector<uintptr_t> _threadHandles;


unsigned int __stdcall ThreadStart(void* argumentList)
{
	SOCKET clientSocket = (SOCKET)argumentList;
	ReadLoop(clientSocket);

	return 0;
}


void NetworkServer::Start(int listenPort)
{
	SOCKET serverSocket = socket( AF_INET, SOCK_STREAM, 0 );

	struct sockaddr_in sin;

	memset(&sin, 0, sizeof(sin));

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons( listenPort );

	cout << "Listening on port " << listenPort << endl;

	if ( bind( serverSocket, (const sockaddr*)&sin, sizeof sin ) == SOCKET_ERROR )
	{
		cout << "Couldn't bind to port " << listenPort << endl;
		return;
	}

	while(true)
	{
		// Wait for clients to connect...
		while ( listen( serverSocket, SOMAXCONN ) == SOCKET_ERROR ) { }

		SOCKET client;
		int length;

		length = sizeof(sin);
		client = accept(serverSocket, (sockaddr*)&sin, &length );
		
		cout << "Client connected from " << inet_ntoa(sin.sin_addr) << ": " << client << endl;

		EnterCriticalSection(&_dataStructureLock);
		uintptr_t handle = _beginthreadex(NULL, 0, ThreadStart, (void*)client, 0, NULL);
		LeaveCriticalSection(&_dataStructureLock);
	}
}
