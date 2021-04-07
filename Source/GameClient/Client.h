#include "Common.h"
#include "Connection\UnaryConnection.h"

namespace Network
{
	class Client
	{
	public:
		Client();
		void Init();
		void Disconnect();
		void Update(float aDeltatime);

		int GetClientSlot() const { return myClientSlot; }
		eConnectionStatus GetConnectionStatus() const { return myWorldServerConnection.GetConnectionStatus(); }

	private:
		void HandleWorldServerMessages();

		UDPSocket myUDPSocket;
		Network::UnaryConnection myWorldServerConnection;
		Network::Address myWorldServerAddress;

		int myClientSlot = INT_MAX;
	};
}