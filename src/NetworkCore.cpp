
#include "Fizz3d.h"
#include "NetworkCore.h"

using namespace std;

ProcessPacketFunction packetProcessor;
ErrorHandlerFunction errorHandler;

NetworkCore::NetworkCore()
{
	InitializeNetwork();
}

NetworkCore::~NetworkCore()
{
	ShutdownNetwork();
}

int GetPacketSize(const NetworkPacket& packet)
{
	return 3 + strlen(packet.MessageData) + 1;
}



void DumpPacket(const NetworkPacket& packet)
{
	printf("Size: %d\n", packet.MessageSize);
	printf("Message: %d\n", packet.MessageId);
	printf("Data: %s\n", packet.MessageData);
}


bool NetworkCore::InitializeNetwork()
{
	WSADATA wsaData;
	WORD version = MAKEWORD( 2, 0 );
	int error = WSAStartup( version, &wsaData );
	return error == ERROR_SUCCESS;
}


void NetworkCore::ShutdownNetwork()
{
	WSACleanup();
}


void ReadLoop(SOCKET socket)
{	
	Assert(packetProcessor != NULL);
	Assert(errorHandler != NULL);
	bool running = true;

	while(running)
	{
		char tempPacketBufSize = 0;
		char tempPacketBuf[512];
		char buf[64];
		int pointer = 0;

		Sleep(0);
		
		int result = recv(socket, (char*)buf, sizeof(buf), 0);
		if(result == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			cout << "Recv failed with error code " + errorCode << endl;
			errorHandler(socket, errorCode);

			return;
		}

		while(result > 0)
		{
			int length = buf[pointer];
			cout << "Length: " << length;
			NetworkPacket packet;
			if(length <= result)
			{
				memcpy(&packet, buf + pointer, length);
				DumpPacket(packet);
				packetProcessor(socket, packet);
				//ProcessPacket(socket, packet);
				result -= length;
				pointer += length;
			}
			else
			{
				// Copy the portion of the packet into the buffer
				memset(tempPacketBuf, 0, sizeof(tempPacketBuf));
				tempPacketBufSize = result;
				memcpy(tempPacketBuf, buf + pointer, result);

				// Then read the remaining portion
				int size = buf[pointer];
				result = recv(socket, tempPacketBuf + tempPacketBufSize, size - tempPacketBufSize + 1, 0);
				
				memcpy(&packet, tempPacketBuf, size);
				result -= size - tempPacketBufSize;
				DumpPacket(packet);
				packetProcessor(socket, packet);
			}
		}
	}
}


int SendPacket(SOCKET socket, NetworkPacket& packet)
{
	packet.MessageSize = GetPacketSize(packet);
	return send(socket, (char*)&packet, packet.MessageSize, 0);
}