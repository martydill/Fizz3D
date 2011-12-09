#include "Fizz3d.h"

const int NetworkPort = 31337;


enum NetworkMessage
{
	NetworkMessage_Join		= 0,
	NetworkMessage_Map		= 1,
	NetworkMessage_Exit		= 2,
	NetworkMessage_Talk		= 3,
	NetworkMessage_Action	= 4,
	NetworkMessage_Message	= 5
};


typedef struct
{
	BYTE MessageSize;
	BYTE SenderId;
	BYTE MessageId;
	char MessageData[256];
}NetworkPacket;


typedef void(*ProcessPacketFunction)(SOCKET socket, const NetworkPacket& packet);
typedef void(*ErrorHandlerFunction)(SOCKET socket, int errorCode);

class NetworkCore
{
public:
	NetworkCore();
	~NetworkCore();

protected:
	//virtual void ProcessPacket(SOCKET socket, const NetworkPacket& packet) = 0;

	bool InitializeNetwork();
	void ShutdownNetwork();

	//ProcessPacketFunction packetProcessor;
};


void ReadLoop(SOCKET socket);
int SendPacket(SOCKET socket, NetworkPacket& packet);
int GetPacketSize(const NetworkPacket& packet);
void DumpPacket(const NetworkPacket& packet);