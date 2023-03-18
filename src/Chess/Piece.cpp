#include "Piece.h"

#include <string>

#define USE_AVAILABLE_MOVES_SPRITE

int GenerateUniquePieceId()
{
	static int id = 0;
	return id++;
}

char TeamToChar(TEAM team)
{
	switch (team)
	{
	case TEAM::ONE: return '1';
	case TEAM::TWO: return '2';
	case TEAM::NONE: break;
	}

	return 'U'; // U for unknown
}

Piece::Piece(Board* pBoard, char type, TEAM team, const pt2di& initialBoardPosition, const std::vector<vec2di>& moves)
{
	m_isFirstMove = true;
	m_isCaptured = false;
	m_isMovesCalculated = false;
	m_pBoard = pBoard; 
	m_team = team;
	m_boardPosition = initialBoardPosition;
	m_pieceType = type;
	m_moves = moves;
	m_id = GenerateUniquePieceId();

	m_screenPosition = m_pBoard->BoardToWindowCoordinates(m_boardPosition);
	m_targetScreenPosition = m_screenPosition;

	switch (m_team)
	{
	case TEAM::ONE: m_teamColor = DARK_GREY; break;
	case TEAM::TWO: m_teamColor = DARK_YELLOW; break;
	default: m_teamColor = GREEN; break;
	}
}

void Piece::UpdateMovement(float dt)
{
	vec2df moveVector = m_targetScreenPosition - m_screenPosition;
	m_screenPosition = m_screenPosition + moveVector * m_speed * dt;
}

void Piece::DrawYourself(const Renderer2D* renderer) const
{
	vec2di cell = renderer->GetCellDim();
	char team = TeamToChar(m_team);
	std::string tag = "";
	tag += m_pieceType;
	tag += team;
	if (!renderer->DrawSprite(m_screenPosition, cell, tag))
	{
		pt2di center = m_screenPosition + vec2df(cell) / 2;
		renderer->FillCircle(center, cell.w / 3, m_teamColor);
		renderer->DrawText(m_screenPosition, { m_pieceType, '\0' }, WHITE);
	}
}

void Piece::DrawMoves(const Renderer2D* renderer) const
{
	pt2di padding = { 2, 2 };
	vec2di cell = renderer->GetCellDim();
	vec2di margin = m_pBoard->GetMargin();

	for (const auto& m : m_availableMoves)
	{
#ifdef USE_AVAILABLE_MOVES_SPRITE
		renderer->DrawSprite({ m.x * cell.w + padding.x + margin.w, m.y * cell.h + padding.y + margin.h },
						{ cell.w - 2 * padding.x, cell.h - 2 * padding.y }, "availableCell");
#else
		m_pBoard->HighlightCell(renderer, m, padding, DARK_GREEN);
#endif
	}
}

void Piece::Move(const pt2di& target)
{
	bool didCapture = m_pBoard->CaptureLocation(target);
	m_pBoard->MovePiece(this, target, didCapture);
	m_boardPosition = target;
	m_targetScreenPosition = m_pBoard->BoardToWindowCoordinates(m_boardPosition);
	// std::cout << "Moved " << m_pieceType << " from " << m_pBoard->GetBoardCoordinates(oldcell->GetCoordinates()) << " to " << m_pBoard->GetBoardCoordinates(newcell->GetCoordinates()) << std::endl;
	m_isFirstMove = false;
}

void Piece::UndoMove(const pt2di& origin, bool wasFirstMove)
{
	m_boardPosition = origin;
	m_targetScreenPosition = m_pBoard->BoardToWindowCoordinates(m_boardPosition);
	m_isFirstMove = wasFirstMove;
	m_isCaptured = false;
}

void Piece::GetCaptured(const pt2di& benchPosition)
{
	m_isCaptured = true;
	m_targetScreenPosition = benchPosition;
}

void Piece::ResetCaptured()
{
	m_isCaptured = false;
}

const std::vector<pt2di>& Piece::GetAvailableMoves()
{
	// if (!m_isMovesCalculated)
	CalculateAvailableMoves();
	return m_availableMoves;
}

void Piece::ResetAvailableMoves()
{
	m_availableMoves.clear();
	m_isMovesCalculated = false;
}

void Piece::AddAvailableMove(const pt2di& target)
{
	m_availableMoves.push_back(target);
	m_isMovesCalculated = true;
}
