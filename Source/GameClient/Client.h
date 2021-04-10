#include "Common.h"
#include "Connection/UnaryConnection.h"
#include "../Game/Entt/entt.hpp"

#include <atomic>

namespace Network
{
	class Client
	{
	public:
		Client();
		void Init();
		void Disconnect();
		void Update(float aDeltatime);

		ClientSlot_t GetWorldServerClientSlot() const { return myWorldServerConnection.GetConnectionSlot();   }
		eConnectionStatus GetConnectionStatus() const { return myWorldServerConnection.GetConnectionStatus(); }

	private:
		void HandleWorldServerMessages();
		void HandleAreaServerMessages();


		UDPSocket myWorldServerSocket;
		Network::UnaryConnection myWorldServerConnection;
		Network::Address myWorldServerAddress;

		entt::id_type myUniqueID;
		entt::id_type myLocalID;

		UDPSocket myAreaServerSocket;
		Network::Address myAreaServerAddress;
		Network::UnaryConnection myAreaServerConnection;

		CommonUtilities::Vector3f myPosition;
		CommonUtilities::Vector3f myVelocity;

		float mySendMoveTimer = 0;
		int myClientSlot = INT_MAX;
	};
}