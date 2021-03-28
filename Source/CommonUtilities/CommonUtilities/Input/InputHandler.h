#pragma once

#include <wtypes.h>
#include <bitset>
#include <functional>
#include <array>
#include <vector>

#include "Gamepad.h"

#include "../Math/Vector2.h" // Also change typedefs in class name scope.

enum class KeyCode
{
	NotBoundKey,
	Space,
	Escape,
	Enter,
	Backspace,
	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,
	Insert,
	Home,
	PageUp,
	Delete,
	End,
	PageDown,
	Up,
	Left,
	Down,
	Right,
	Tab,
	CapsLock,
	Shift,
	Control,
	Alt,
	NumPadNumLock,
	Divide,
	Multiply,
	Minus,
	Plus,
	NumPad0,
	NumPad1,
	NumPad2,
	NumPad3,
	NumPad4,
	NumPad5,
	NumPad6,
	NumPad7,
	NumPad8,
	NumPad9,
	NumPadComma,
	Comma,
	Period,
	UnderScore, // (_ -) underscore/minus.
	LessThan,
	Nr_0,
	Nr_1,
	Nr_2,
	Nr_3,
	Nr_4,
	Nr_5,
	Nr_6,
	Nr_7,
	Nr_8,
	Nr_9,
	A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,
	Count,
};

enum class MouseButton
{
	Left,
	Right,
	Count
};
namespace CommonUtilities
{
	class InputHandler
	{
		////////////////////////////////////////////////////
		// Change these if the Inputhandler should use another vector class.
		using Vector2f = CommonUtilities::Vector2<float>;
		using Vector2i = CommonUtilities::Vector2<int>;
		////////////////////////////////////////////////////
		
		typedef std::function<void(int aID, bool aIsConnected)> GamepadConnectionChangedFunc;

	public:
		InputHandler();
		InputHandler(const InputHandler&)			 = delete;
		InputHandler(InputHandler&&)				 = delete;
		InputHandler operator=(const InputHandler&)	 = delete;
		InputHandler operator=(InputHandler&&)		 = delete;

		bool UpdateEvents(UINT aMessage, WPARAM aWParam, LPARAM aLParam);
		void ClearFrame();
		
		bool GetKey(KeyCode aKeyCode) const;
		bool GetKeyDown(KeyCode aKeyCode) const;
		bool GetKeyUp(KeyCode aKeyCode) const;

		bool GetMouseButton(MouseButton aButton) const;
		bool GetMouseButtonDown(MouseButton aButton) const;
		bool GetMouseButtonUp(MouseButton aButton) const;

		const Vector2i& GetMousePosition() const;
		const Vector2i& GetMouseDelta() const;
		int GetMouseWheelDelta() const;

		/// Add a function that will be called each time a supported(!) gamepad is connected/disconnected. 
		/// The signature is void(int aID, bool aIsConnected).
		void SetGamepadConnectionChangedCallback(GamepadConnectionChangedFunc aCallback);

		// Returns the ID of the first connected Gamepad. Returns -1 if no one is connected.
		int GetConnectedGamepadID() const;

		/// Returns a single Gamepad with the provided ID.
		Gamepad& GetGamepad(const int aID);
		
		/// Returns the container with all gamepads.
		std::array<Gamepad, MAX_NUM_GAMEPADS>& GetGamepads();

		/// Sets the cursor position in pixel space relative to the Window.
		void SetCursorPosition(HWND aWindow, const int aX, const int aY);
		/// Sets the cursor position in pixel space relative to the monitor screen. GetMousePosition() will still return the position relative to the window.
		void SetCursorPositionRelativeToMonitor(HWND aWindow, const int aX, const int aY);
	private:
		void UpdateGamepadStates();
		void ClearGamepadStates();

		KeyCode VkToKeyCode(WPARAM aWParam) const;
		
		Vector2i myMousePosition;
		Vector2i myMousePositionLast;
		Vector2i myMousePositionDelta;

		int myMouseWheelDelta;
		
		std::bitset<static_cast<size_t>(KeyCode::Count)> myKeyboardStateLast;
		std::bitset<static_cast<size_t>(KeyCode::Count)> myKeyboardState;
		std::bitset<static_cast<size_t>(MouseButton::Count)> myMouseState;
		std::bitset<static_cast<size_t>(MouseButton::Count)> myMouseStateLast;
		
		std::array<Gamepad, MAX_NUM_GAMEPADS> myGamepads;

		GamepadConnectionChangedFunc myGamepadConnectionChangedCallback;
	};
}

