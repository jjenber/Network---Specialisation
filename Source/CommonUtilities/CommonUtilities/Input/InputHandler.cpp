#include "InputHandler.h"
#include <windowsx.h>
#include <iostream>
#include <string>


namespace CommonUtilities
{
	InputHandler::InputHandler() : myMouseWheelDelta(0)
	{
		for (int i = 0; i < myGamepads.size(); i++)
		{
			myGamepads[i].Init(i);
		}
	}

	bool InputHandler::UpdateEvents(UINT aMessage, WPARAM aWParam, LPARAM aLParam)
	{
		MSG msg{ 0 };
		switch (aMessage)
		{
		case WM_KILLFOCUS:
		{
			// Reset everything if window loses focus.
			myMouseState.reset();
			myKeyboardState.reset();
			ClearGamepadStates();
			ClearFrame();
			return false;
		}
		// Mouse
		case WM_MOUSEMOVE:
		{
			myMousePosition.x = GET_X_LPARAM(aLParam);
			myMousePosition.y = GET_Y_LPARAM(aLParam);
			myMousePositionDelta = myMousePosition - myMousePositionLast;
			break;
		}
		case WM_LBUTTONDOWN:
		{
			myMouseState.set(static_cast<size_t>(MouseButton::Left));
			break;
		}
		case WM_NCLBUTTONUP:
		case WM_LBUTTONUP:
		{
			myMouseState.set(static_cast<size_t>(MouseButton::Left), false);
			break;
		}
		case WM_RBUTTONDOWN:
		{
			myMouseState.set(static_cast<size_t>(MouseButton::Right));
			break;
		}
		case WM_NCRBUTTONUP:
		case WM_RBUTTONUP:
		{
			myMouseState.set(static_cast<size_t>(MouseButton::Right), false);
			break;
		}
		case WM_MOUSEWHEEL:
		{
			myMouseWheelDelta += GET_WHEEL_DELTA_WPARAM(aWParam);
			break;
		}
		// Keyboard
		case WM_UNICHAR:
		case WM_CHAR:
		{
			wchar_t key = static_cast<wchar_t>(aWParam);
			myKeyboardState.set(static_cast<size_t>(VkToKeyCode(key)));
			break;
		}
		case WM_KEYDOWN:
		{
			const size_t index = static_cast<size_t>(VkToKeyCode(aWParam));
  			myKeyboardState.set(index);
			if ((aLParam & (30 << 1)) > 0)
			{
				myKeyboardStateLast.set(index);
			}
			break;
		}
		break;
		case WM_KEYUP:
		{
			myKeyboardState.set(static_cast<size_t>(VkToKeyCode(aWParam)), false);
			break;
		}
		default:
			return false;
		}
		return true;
	}

	void InputHandler::ClearFrame()
	{
		myKeyboardStateLast = myKeyboardState;
		myMouseStateLast    = myMouseState;
		myMousePositionLast = myMousePosition;
		myMouseWheelDelta   = 0;

		UpdateGamepadStates();
	}

	bool InputHandler::GetKey(KeyCode aKeyCode) const
	{
		return myKeyboardState[static_cast<size_t>(aKeyCode)];
	}

	bool InputHandler::GetKeyDown(KeyCode aKeyCode) const
	{
		
		const size_t index = static_cast<size_t>(aKeyCode);
		return myKeyboardState[index] && (myKeyboardStateLast[index] == false);
	}

	bool InputHandler::GetKeyUp(KeyCode aKeyCode) const
	{
		const size_t index = static_cast<size_t>(aKeyCode);
		return (myKeyboardState[index] == false) && myKeyboardStateLast[index];
	}

	bool InputHandler::GetMouseButton(MouseButton aButton) const
	{
		return myMouseState[static_cast<size_t>(aButton)];
	}

	bool InputHandler::GetMouseButtonDown(MouseButton aButton) const
	{
		const size_t index = static_cast<size_t>(aButton);
		return myMouseState[index] && (myMouseStateLast[index] == false);
	}

	bool InputHandler::GetMouseButtonUp(MouseButton aButton) const
	{
		const size_t index = static_cast<size_t>(aButton);
		return (myMouseState[index] == false) && myMouseStateLast[index];
	}

	const InputHandler::Vector2i& InputHandler::GetMousePosition() const
	{
		return myMousePosition;
	}

	const InputHandler::Vector2i& InputHandler::GetMouseDelta() const
	{
		return myMousePositionDelta;
	}

	int InputHandler::GetMouseWheelDelta() const
	{
		return myMouseWheelDelta;
	}

	void InputHandler::SetGamepadConnectionChangedCallback(GamepadConnectionChangedFunc aCallback)
	{
		myGamepadConnectionChangedCallback = aCallback;
	}

	int InputHandler::GetConnectedGamepadID() const
	{
		for (int i = 0; i < myGamepads.size(); i++)
		{
			if (myGamepads[i].myIsConnected) return i;
		}
		return -1;
	}

	Gamepad& InputHandler::GetGamepad(const int aID)
	{
		return myGamepads[aID];
	}

	std::array<Gamepad, MAX_NUM_GAMEPADS>& InputHandler::GetGamepads()
	{
		return myGamepads;
	}

	void InputHandler::SetCursorPosition(HWND aWindow, const int aX, const int aY)
	{
		RECT wndRect{ 0 };
		GetWindowRect(aWindow, &wndRect);

		const int newX = wndRect.left + aX;
		const int newY = wndRect.top + aY;

		SetCursorPos(newX, newY);
		myMousePosition.x = aX;
		myMousePosition.y = aY;
	}

	void InputHandler::SetCursorPositionRelativeToMonitor(HWND aWindow, const int aX, const int aY)
	{
		RECT wndRect{ 0 };
		GetWindowRect(aWindow, &wndRect);

		const int newX = wndRect.left + aX;
		const int newY = wndRect.top + aY;

		SetCursorPos(aX, aY);
		myMousePosition.x = newX;
		myMousePosition.y = newY;
	}

	void InputHandler::UpdateGamepadStates()
	{
		for (int i = 0; i < myGamepads.size(); i++)
		{
			bool wasConnected = myGamepads[i].myIsConnected;
			bool connectionChanged = wasConnected != myGamepads[i].UpdateState();
			
			if (myGamepadConnectionChangedCallback != nullptr && connectionChanged)
			{
				myGamepadConnectionChangedCallback(i, !wasConnected);
			}
		}
	}

	void InputHandler::ClearGamepadStates()
	{
		for (Gamepad& gamepad : myGamepads)
		{
			gamepad.ClearState();
		}
	}

	KeyCode InputHandler::VkToKeyCode(WPARAM aWParam) const
	{
		switch (aWParam)
		{
		case VK_SPACE:      return KeyCode::Space;
		case VK_ESCAPE:     return KeyCode::Escape;
		case VK_RETURN:     return KeyCode::Enter;
		case VK_BACK:       return KeyCode::Backspace;
		case VK_F1:         return KeyCode::F1;
		case VK_F2:         return KeyCode::F2;
		case VK_F3:         return KeyCode::F3;
		case VK_F4:         return KeyCode::F4;
		case VK_F5:         return KeyCode::F5;
		case VK_F6:         return KeyCode::F6;
		case VK_F7:         return KeyCode::F7;
		case VK_F8:         return KeyCode::F8;
		case VK_F9:         return KeyCode::F9;
		case VK_F10:        return KeyCode::F10;
		case VK_F11:        return KeyCode::F11;
		case VK_F12:        return KeyCode::F12;
		case VK_INSERT:     return KeyCode::Insert;
		case VK_HOME:       return KeyCode::Home;
		case VK_PRIOR:      return KeyCode::PageUp;
		case VK_DELETE:     return KeyCode::Delete;
		case VK_NEXT:       return KeyCode::PageDown;
		case VK_UP:         return KeyCode::Up;
		case VK_LEFT:       return KeyCode::Left;
		case VK_DOWN:       return KeyCode::Down;
		case VK_RIGHT:      return KeyCode::Right;
		case VK_TAB:        return KeyCode::Tab;
		case VK_CAPITAL:    return KeyCode::CapsLock;
		case VK_SHIFT:      return KeyCode::Shift;
		case VK_CONTROL:    return KeyCode::Control;
		case VK_MENU:       return KeyCode::Alt;
		case VK_NUMLOCK:    return KeyCode::NumPadNumLock;
		case VK_DIVIDE:     return KeyCode::Divide;
		case VK_MULTIPLY:   return KeyCode::Multiply;
		case VK_SUBTRACT:   return KeyCode::Minus;
		case VK_ADD:        return KeyCode::Plus;
		case VK_NUMPAD0:    return KeyCode::NumPad0;
		case VK_NUMPAD1:    return KeyCode::NumPad1;
		case VK_NUMPAD2:    return KeyCode::NumPad2;
		case VK_NUMPAD3:    return KeyCode::NumPad3;
		case VK_NUMPAD4:    return KeyCode::NumPad4;
		case VK_NUMPAD5:    return KeyCode::NumPad5;
		case VK_NUMPAD6:    return KeyCode::NumPad6;
		case VK_NUMPAD7:    return KeyCode::NumPad7;
		case VK_NUMPAD8:    return KeyCode::NumPad8;
		case VK_NUMPAD9:    return KeyCode::NumPad9;
		case VK_DECIMAL:    return KeyCode::NumPadComma;
		case VK_END:	    return KeyCode::Comma;
		case VK_OEM_PERIOD: return KeyCode::Period;
		case VK_OEM_MINUS:  return KeyCode::UnderScore;
		case '0': return KeyCode::Nr_0;
		case '1': return KeyCode::Nr_1;
		case '2': return KeyCode::Nr_2;
		case '3': return KeyCode::Nr_3;
		case '4': return KeyCode::Nr_4;
		case '5': return KeyCode::Nr_5;
		case '6': return KeyCode::Nr_6;
		case '7': return KeyCode::Nr_7;
		case '8': return KeyCode::Nr_8;
		case '9': return KeyCode::Nr_9;
		case 'A': return KeyCode::A;
		case 'B': return KeyCode::B;
		case 'C': return KeyCode::C;
		case 'D': return KeyCode::D;
		case 'E': return KeyCode::E;
		case 'F': return KeyCode::F;
		case 'G': return KeyCode::G;
		case 'H': return KeyCode::H;
		case 'I': return KeyCode::I;
		case 'J': return KeyCode::J;
		case 'K': return KeyCode::K;
		case 'L': return KeyCode::L;
		case 'M': return KeyCode::M;
		case 'N': return KeyCode::N;
		case 'O': return KeyCode::O;
		case 'P': return KeyCode::P;
		case 'Q': return KeyCode::Q;
		case 'R': return KeyCode::R;
		case 'S': return KeyCode::S;
		case 'T': return KeyCode::T;
		case 'U': return KeyCode::U;
		case 'V': return KeyCode::V;
		case 'W': return KeyCode::W;
		case 'X': return KeyCode::X;
		case 'Y': return KeyCode::Y;
		case 'Z': return KeyCode::Z;
		}
		return KeyCode::NotBoundKey;
	}
	
}
