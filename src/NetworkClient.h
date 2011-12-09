
#include "NetworkCore.h"
#include "Fizz3d.h"

class NetworkClient : public NetworkCore
{
public:
	NetworkClient();
	~NetworkClient();

	void Start(std::string sever, int connectPort, std::string name);

protected:
//	void ProcessPacket(SOCKET socket, const NetworkPacket& packet);
};
