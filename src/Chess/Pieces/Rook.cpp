#include "Rook.h"

void Rook::Castle(const vec2di& direction)
{
	pt2di target = m_boardPosition + direction;
	m_pBoard->Castle(this, target);
	m_boardPosition = target;
	m_targetScreenPosition = m_pBoard->BoardToWindowCoordinates(m_boardPosition);
	m_isFirstMove = false;
}