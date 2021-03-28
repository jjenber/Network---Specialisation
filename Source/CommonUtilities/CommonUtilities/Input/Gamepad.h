#pragma once
#include <functional>
#include "../Timer/Timer.h"
#include "../Math/Vector2.h"
#include "../Math/Math.hpp"

namespace CommonUtilities
{
	constexpr int MAX_NUM_GAMEPADS = 4;
	constexpr float DEADZONE_STICK_L = 0.2f;
	constexpr float DEADZONE_STICK_R = 0.25f;

	enum class Button
	{
		DPad_Up       = 0x0001,
		DPad_Down     = 0x0002,
		DPad_Left     = 0x0004,
		DPad_Right    = 0x0008,
		Start         = 0x0010,
		Back          = 0x0020,
		LeftStick     = 0x0040,
		RightStick    = 0x0080,
		LeftShoulder  = 0x0100,
		RightShoulder = 0x0200,
		A             = 0x1000,
		B             = 0x2000,
		X             = 0x4000,
		Y             = 0x8000,
	};

	enum class Axis
	{
		LeftStick,
		RightStick,
		LeftTrigger,
		RightTrigger,
	};

	struct GamepadState
	{
		Vector2f myLeftStick;
		Vector2f myRightStick;
		float myLeftTrigger = 0.f;
		float myRightTrigger = 0.f;
		unsigned short myButtons = 0;
	};

	class Gamepad
	{
		friend class InputHandler;
	public:
		Gamepad();
		~Gamepad();

		Gamepad(const Gamepad& copy) = delete;
		Gamepad(Gamepad&& copy) = delete;
		Gamepad& operator=(const Gamepad& copy) = delete;
		Gamepad& operator=(Gamepad&& copy) = delete;

		bool IsConnected() const;
		int GetID() const;

		/// Returns true while the user holds down the button. 
		bool GetButton(Button aButton) const;
		/// Returns true during the frame the user starts pressing down the button.
		bool GetButtonDown(Button aButton) const;
		/// Returns true during the frame the user release the button.
		bool GetButtonUp(Button aButton) const;

		// Returns the left analog stick with a value between [-1, 1].
		const Vector2f& GetStickL() const { return myState.myLeftStick; };
		// Returns the right analog stick with a value between [-1, 1].
		const Vector2f& GetStickR() const { return myState.myRightStick; };
		// Returns the left trigger value between [0, 1].
		const float GetTriggerL() const { return myState.myLeftTrigger; };
		// Returns the right trigger value between [0, 1].
		const float GetTriggerR() const { return myState.myRightTrigger; };

		/// Returns the time in seconds since the controller sent or recieved input.
		float GetTimeSinceLastStateChange() const;
		
		/// Immediately stop vibration if it is currently on. Otherwise nothing happens. 
		void StopVibration();
		/// Vibrate both motors with an intensity range between [0 - 1]. Stops automatically after time is up. To stop immediately call StopVibration(). 
		void Vibrate(float aIntensity, float aTimeInSeconds);

	private:
		void Init(const int aUserID);
		void ClearState();

		/// Call once per update to set the current state as well as store the previous state.
		/// Returns true if connected.
		bool UpdateState();
		std::function<void()> myVibrationUpdate;

		GamepadState myState;
		GamepadState myPreviousState;

		Timer myLastStateChangeTimer;
		unsigned long myPacketNumber;
		int myID;
		bool myIsConnected;
	};
}

