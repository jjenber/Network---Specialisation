#include "Gamepad.h"

#include <Windows.h>
#include <Xinput.h>

#pragma comment (lib, "Xinput9_1_0.lib")

namespace CommonUtilities
{
	Gamepad::Gamepad() :
		myID(-1),
		myIsConnected(false),
		myState(GamepadState()),
		myPreviousState(GamepadState()),
		myPacketNumber(0)
	{
	}

	Gamepad::~Gamepad()
	{
		StopVibration();
	}

	float Gamepad::GetTimeSinceLastStateChange() const
	{
		return myLastStateChangeTimer.GetDeltaTime();
	}

	void Gamepad::StopVibration()
	{
		XINPUT_VIBRATION vibration{0, 0};
		XInputSetState(myID, &vibration);
		myVibrationUpdate = nullptr;
	}

	void Gamepad::Vibrate(float aIntensity, float aTime)
	{
		XINPUT_VIBRATION vibration;
		aIntensity = Clamp(aIntensity, 0.f, 1.f);
		vibration.wLeftMotorSpeed = static_cast<WORD>(aIntensity * USHRT_MAX);
		vibration.wRightMotorSpeed = static_cast<WORD>(aIntensity * USHRT_MAX);
		XInputSetState(myID, &vibration);
		
		using namespace std::chrono;
		auto timestart = std::chrono::system_clock::now();
		myVibrationUpdate = [&, timestart, aTime]() 
		{
			const float elapsed = duration<float, std::milli>(system_clock::now() - timestart).count() / 1000.f;
			if (elapsed >= aTime)
			{
				StopVibration();
			}
		};
	}

	void Gamepad::Init(const int aUserID)
	{
		myID = aUserID;
		UpdateState();
	}

	void Gamepad::ClearState()
	{
		ZeroMemory(&myState, sizeof(GamepadState));
		ZeroMemory(&myPreviousState, sizeof(GamepadState));
	}

	bool Gamepad::UpdateState()
	{
		myPreviousState = myState;
		XINPUT_STATE xstate;
		ZeroMemory(&xstate, sizeof(XINPUT_STATE));
		
		myIsConnected     = XInputGetState(myID, &xstate) == ERROR_SUCCESS;
		bool stateChanged = myPacketNumber != xstate.dwPacketNumber;
		myPacketNumber    = xstate.dwPacketNumber;

		if (stateChanged)
		{
			myLastStateChangeTimer.Update();
			myState.myButtons      = xstate.Gamepad.wButtons;

			// Left analog stick
			const float normLX = fmaxf(-1, (float)xstate.Gamepad.sThumbLX / 32767);
			const float normLY = fmaxf(-1, (float)xstate.Gamepad.sThumbLY / 32767);

			myState.myLeftStick.x = (fabs(normLX) < DEADZONE_STICK_L ? 0 : (fabs(normLX) - DEADZONE_STICK_L) * (normLX / fabs(normLX)));
			myState.myLeftStick.y = (fabs(normLY) < DEADZONE_STICK_L ? 0 : (fabs(normLY) - DEADZONE_STICK_L) * (normLY / fabs(normLY)));

			myState.myLeftStick.x *= 1 / (1 - DEADZONE_STICK_L);
			myState.myLeftStick.y *= 1 / (1 - DEADZONE_STICK_L);

			// Right analog stick
			const float normRX = fmaxf(-1, (float)xstate.Gamepad.sThumbRX / 32767);
			const float normRY = fmaxf(-1, (float)xstate.Gamepad.sThumbRY / 32767);

			myState.myRightStick.x = (fabs(normRX) < DEADZONE_STICK_R ? 0 : (fabs(normRX) - DEADZONE_STICK_R) * (normRX / fabs(normRX)));
			myState.myRightStick.y = (fabs(normRY) < DEADZONE_STICK_R ? 0 : (fabs(normRY) - DEADZONE_STICK_R) * (normRY / fabs(normRY)));

			myState.myRightStick.x *= 1 / (1 - DEADZONE_STICK_R);
			myState.myRightStick.y *= 1 / (1 - DEADZONE_STICK_R);

			// Triggers

			myState.myLeftTrigger  = static_cast<float>(xstate.Gamepad.bLeftTrigger) / UCHAR_MAX;
			myState.myRightTrigger = static_cast<float>(xstate.Gamepad.bRightTrigger) / UCHAR_MAX;
		}
		
		if (myVibrationUpdate != nullptr) myVibrationUpdate();

		return myIsConnected;
	}

	bool Gamepad::IsConnected() const
	{
		return myIsConnected;
	}

	int Gamepad::GetID() const
	{
		return myID;
	}

	bool Gamepad::GetButton(Button aButton) const
	{
		return (myState.myButtons & static_cast<int>(aButton)) != 0;
	}

	bool Gamepad::GetButtonDown(Button aButton) const
	{
		return 
			(myState.myButtons & static_cast<int>(aButton)) != 0 &&
			(myPreviousState.myButtons & static_cast<int>(aButton)) == 0;
	}

	bool Gamepad::GetButtonUp(Button aButton) const
	{
		return
			(myState.myButtons & static_cast<int>(aButton)) == 0 &&
			(myPreviousState.myButtons & static_cast<int>(aButton)) != 0;
	}
}
