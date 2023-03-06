#pragma once

#include <cstdint>

enum class MouseButton : uint8_t
{
	Left, Right, Middle, X1, X2,

	N_MOUSE_BUTTONS
};

enum class Key : uint8_t
{
	Unknown,
	A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	Space, Enter, Backspace, Tab, Escape,
	Up, Down, Left, Right,
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18,
	K0, K1, K2, K3, K4, K5, K6, K7, K8, K9, Tilda, Minus, Equals,

	N_KEYS
};

struct WindowCloseEvent
{

};

struct MouseMoveEvent
{
	int x, y;
};

struct MouseButtonEvent
{
	MouseButton button;
	bool isPressed;
};

struct MouseButtonDownEvent : public MouseButtonEvent
{
	MouseButtonDownEvent(MouseButton button) : MouseButtonEvent{ button, true } {}
};

struct MouseButtonUpEvent : public MouseButtonEvent
{
	MouseButtonUpEvent(MouseButton button) : MouseButtonEvent{ button, false } {}
};

struct MouseWheelEvent
{
	int horizontal;
	int vertical;
};

struct KeyboardEvent
{
	Key key;
	bool isPressed;
};

struct KeyboardDownEvent : public KeyboardEvent
{
	KeyboardDownEvent(Key key) : KeyboardEvent{ key, true } {}
};

struct KeyboardUpEvent : public KeyboardEvent
{
	KeyboardUpEvent(Key key) : KeyboardEvent{ key, false } {}
};
