#include "Piece.h"

#include <iostream>
#include <string>

int GenerateUniquePieceId()
{
	static int id = 0;
	return id++;
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
	if (!m_isCaptured)
	{
		vec2di cell = renderer->GetCellDim();
		pt2di center = m_screenPosition + vec2df(cell) / 2;
		renderer->FillCircle(center, cell.w / 3, m_teamColor);
		renderer->DrawText(m_screenPosition, {m_pieceType, '\0'}, WHITE);
	}
}

void Piece::DrawMoves(const Renderer2D* renderer) const
{
	int padx = 2;
	int pady = 2;

	for (const auto& m : m_availableMoves)
	{
		m_pBoard->HighlightCell(renderer, m, { padx, pady }, DARK_GREEN);
	}
}

void Piece::Move(const pt2di& target)
{
	Cell* oldcell = m_pBoard->GetCell(m_boardPosition);
	Cell* newcell = m_pBoard->GetCell(target);
	if (oldcell == nullptr || newcell == nullptr)
		return;

	m_boardPosition = target;
	oldcell->RemovePiece();
	newcell->CaptureCell();
	newcell->PlacePiece(this);
	m_targetScreenPosition = m_pBoard->BoardToWindowCoordinates(target);
	// std::cout << "Moved " << m_pieceType << " from " << m_pBoard->GetBoardCoordinates(oldcell->GetCoordinates()) << " to " << m_pBoard->GetBoardCoordinates(newcell->GetCoordinates()) << std::endl;
	m_isFirstMove = false;
}

void Piece::Capture()
{
	m_isCaptured = true;
}

void Piece::ResetCaptured()
{
	m_isCaptured = false;
}

const std::vector<pt2di>& Piece::GetAvailableMoves()
{
	if (!m_isMovesCalculated)
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