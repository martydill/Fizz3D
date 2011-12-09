
#include "Fizz3d.h"
#include "NetworkClient.h"

using namespace std;

void ProcessPacket(SOCKET socket, const NetworkPacket& packet);
extern ProcessPacketFunction packetProcessor;

NetworkClient::NetworkClient()
{
	packetProcessor = ProcessPacket;
}

NetworkClient::~NetworkClient()
{
}

int clientId;

void ProcessPacket(SOCKET socket, const NetworkPacket& packet)
{
	cout << "Processing message" << packet.MessageId << endl;

	switch(packet.MessageId)
	{
		case NetworkMessage_Join:
			memcpy(&clientId, &packet.MessageData, sizeof(BYTE));
			cout << "Connected as client " << (int)	clientId << endl;
			break;

		case NetworkMessage_Message:
			cout << packet.MessageData << endl;
			break;

		default:
			break;
	}
}


void NetworkClient::Start(string server, int connectPort, string name)
{
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);

	cout << "Connecting to " << server << endl;

	struct hostent* host = gethostbyname(server.c_str());

	struct sockaddr_in sin;

	memset( &sin, 0, sizeof sin );

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = ((struct in_addr*)(host->h_addr))->s_addr;
	sin.sin_port = htons(connectPort);
	
	int s =  connect(clientSocket, (const sockaddr*)&sin, sizeof(sin));
	if(s == SOCKET_ERROR)
		return;
	cout << "Connected" << endl;

	NetworkPacket packet;
	memset(&packet, 0, sizeof(NetworkPacket));
	packet.MessageId  = NetworkMessage_Join;
	strncpy(packet.MessageData, name.c_str(), name.size());
	int result = SendPacket(clientSocket, packet);
	cout << "Sent packet " << result << endl;

	/*memset(&packet, 0, sizeof(NetworkPacket));
	packet.MessageId  = NetworkMessage_Talk;
	strncpy(packet.MessageData, "This is a test", strlen("This is a test"));
	result = SendPacket(clientSocket, packet);
	cout << "Sent packet " << result << endl;*/

	ReadLoop(clientSocket);
}