
#include "NetworkCore.h"

class NetworkServer : public NetworkCore
{
public:
	NetworkServer();
	~NetworkServer();

	void Start(int listenPort);

protected:
	//void ProcessPacket(SOCKET socket, const NetworkPacket& packet);
};
