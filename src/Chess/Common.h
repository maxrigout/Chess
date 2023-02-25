#pragma once

#include "Math/point2d.h"
#define COPY_EMPTY_CELL '.'

class Piece;

enum class TEAM
{
	ONE,
	TWO,
	NONE,
};

struct MoveInfo
{
	enum REASON
	{
		NONE             = 0,
		BLOCKED          = 1 << 0,
		SAME_TEAM        = 1 << 1,
		ALREADY_OCCUPIED = 1 << 2,
		KING_GUARDED     = 1 << 3,
		EMPTY_CELL       = 1 << 4,
		CELL_GUARDED     = 1 << 5,
		CAPTURABLE       = 1 << 6,
		UNCAPTURABLE     = 1 << 7,
		INVALID_CELL     = 1 << 8,
		INVALID_MOVE     = 1 << 9,
	};

	int reason;
};

struct Move
{
	pt2di target;
	pt2di origin;
};

enum class EventType
{
	Move,
	GetCaptured,
	Castle
};

struct MoveEvent
{
	Piece* piece;
	pt2di target;
	pt2di origin;
	EventType eventType;
	bool wasFirstMove = false;
	bool hasSideEffect = false;
};