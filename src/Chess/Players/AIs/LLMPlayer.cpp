#include "LLMPlayer.h"

#include "Core/Logger.h"

#include <nlohmann/json.hpp>

#include "Chess/Exceptions/LLMExceptions.h"

static std::string initialPrompt = "you are playing chess with me. You need to give me your moves with a start cell and end cell. return your moves in a json format like so: { \"start\": \"startCell\", \"end\": \"endCell\" }. ";

static constexpr std::string_view TeamToString(TEAM team)
{
	switch (team)
	{
		case TEAM::NONE: return "None";
		case TEAM::ONE: return "One";
		case TEAM::TWO: return "Two";
	}

	return "unknown team";
}

static inline bool isValidCoordinate(std::string_view coord)
{
	return coord.length() == 2
		&& ('h' - coord[0]) >= 0
		&& ('8' - coord[1]) >= 0;
}

static inline bool isValidCoordinate(const pt2di& pt, int width = 8, int height = 8)
{
	return pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height;
}

static inline bool isValidCoordinate(const pt2di& pt, Board* pBoard)
{
	return isValidCoordinate(pt, pBoard->GetWidth(), pBoard->GetHeight());
}

LLMPlayer::LLMPlayer(Board* pBoard, TEAM team, const std::string& apiKey)
	: AIPlayer(pBoard, team, 10000), m_agent(apiKey)
{
	LOG_INFO("LLMPlayer initialized!");
}

void LLMPlayer::PlayThread()
{
	Move move;
	const size_t maxAttempts = 5;
	size_t currentAttempt;
	bool played = false;
	std::string prompt;
	for (currentAttempt = 0; currentAttempt < maxAttempts && m_isPlaying; currentAttempt++)
	{
		try
		{
			if (!m_isAgentInitialized)
			{
				if (m_pBoard->IsFirstMove())
				{
					prompt = GetInitialYouGoFirstPrompt();
				}
				else
				{
					Move lastMove = m_pBoard->GetLastMove();
					prompt = GetInitialYouGoSecondPrompt(lastMove);
				}
				m_isAgentInitialized = true;
			}
			else
			{
				Move lastMove = m_pBoard->GetLastMove();
				prompt = GetMovePrompt(lastMove);
			}


			// Send the prompt to the LLM
			// Receive the message back from the LLM
			std::string llmResponse = m_agent.Send(prompt);

			// validate the message is correctly formated (eg a json)
			// validate the data that's received
			nlohmann::json llmJsonResponse = ParseResponse(llmResponse);
			
			std::string origin = llmJsonResponse["start"].get<std::string>();
			std::string target = llmJsonResponse["end"].get<std::string>();

			// validate data
			if (!isValidCoordinate(origin))
			{
				LOG_ERROR("wrong origin coordinate");
				throw LLMInvalidDataChessException("origin", origin);
			}
			if (!isValidCoordinate(target))
			{
				LOG_ERROR("wrong target coordinate");
				throw LLMInvalidDataChessException("target", target);
			}

			// convert to coordinates
			Move move {
				.origin = m_pBoard->ToBoardCoordinates(origin),
				.target = m_pBoard->ToBoardCoordinates(target)
			};

			// validate the coordinates are valid (ie on the board)
			if (!isValidCoordinate(move.origin, m_pBoard))
			{
				throw LLMInvalidCellChessException("origin", move.origin);
			}

			if (!isValidCoordinate(move.target, m_pBoard))
			{
				throw LLMInvalidCellChessException("target", move.target);
			}

			// verify there's a piece at the origin location
			Piece* piece = m_pBoard->GetPieceAtCell(move.origin);
			if (piece == nullptr)
			{
				throw LLMNoPieceChessException(move.origin);
			}
			// verify the piece belongs to the same team
			if (piece->Team() != GetTeam())
			{
				throw LLMInvalidPieceChessException(move.origin);
			}

			// verify the move is valid
			// check for invalid moves
			piece->CalculateAvailableMoves();
			const auto& availableMoves = piece->GetAvailableMoves();
			auto ite = std::find_if(availableMoves.begin(), availableMoves.end(), [move](const pt2di& m)
			{
				return m.x == move.target.x && m.y == move.target.y;
			});

			if (ite == availableMoves.end())
			{
				throw LLMInvalidMoveChessException(piece->GetTypeString(), move.origin, move.target);
			}
			
			// play the move
			piece->Move(move.target);

			m_lastMoveStart = move.origin;
			m_lastMoveEnd = move.target;

			LOG_INFO("LLM Player finished playing");
			EndTurn();
			m_isPlaying = false;
		}
		catch (const LLMDeserializationChessException& e)
		{
			// TODO:
		}
		catch (const LLMInvalidDataChessException& e)
		{
			OnInvalidDataReceived();
		}
		catch (const LLMInvalidCellChessException& e)
		{
			// TODO:
		}
		catch (const LLMNoPieceChessException& e)
		{
			OnNoPieceFound();
		}
		catch (const LLMInvalidPieceChessException& e)
		{
			OnWrongPieceChosen();
		}
		catch (const LLMInvalidMoveChessException& e)
		{
			OnInvalidMove();
		}
		catch (const std::exception& e)
		{
			LOG_ERROR("non LLM error has occurred: ", e.what());
		}
		catch (...)
		{
			LOG_ERROR("an unknown error has occurred!");
		}
	}

	if (currentAttempt == maxAttempts)
	{
		LOG_FATAL("unable to get a move from the LLM!");
		throw std::runtime_error("unable to get a move from the LLM!");
	}
}

void LLMPlayer::PlayThread_alt()
{
	Move move;
	const size_t maxAttempts = 5;
	size_t currentAttempts = 0;
	while (currentAttempts < maxAttempts)
	{
		try
		{
			Move move;
			if (!m_isAgentInitialized)
			{
				if (m_pBoard->IsFirstMove())
				{
					move = AskForMoveYouGoFirst();
				}
				else
				{
					Move lastMove = m_pBoard->GetLastMove();
					move = AskForMoveYouGoSecond(lastMove);
				}
				m_isAgentInitialized = true;
			}
			else
			{
				Move lastMove = m_pBoard->GetLastMove();
				move = AskForMove(lastMove);
			}

			if (!isValidCoordinate(move.origin, m_pBoard))
			{
				throw LLMInvalidCellChessException("origin", move.origin);
			}

			if (!isValidCoordinate(move.target, m_pBoard))
			{
				throw LLMInvalidCellChessException("target", move.target);
			}

			Piece* piece = m_pBoard->GetPieceAtCell(move.origin);
			if (piece == nullptr)
			{
				throw LLMNoPieceChessException(move.origin);
			}
			if (piece->Team() != GetTeam())
			{
				throw LLMInvalidPieceChessException(move.origin);
			}

			// check for invalid moves
			piece->CalculateAvailableMoves();
			const auto& availableMoves = piece->GetAvailableMoves();
			auto ite = std::find_if(availableMoves.begin(), availableMoves.end(), [move](const pt2di& m)
			{
				return m.x == move.target.x && m.y == move.target.y;
			});

			if (ite == availableMoves.end())
			{
				throw LLMInvalidMoveChessException(piece->GetTypeString(), move.origin, move.target);
			}
			
			piece->Move(move.target);

			m_lastMoveStart = move.origin;
			m_lastMoveEnd = move.target;

			LOG_INFO("LLM Player finished playing");
			EndTurn();
			m_isPlaying = false;

		}
		catch (const LLMDeserializationChessException& e)
		{
			// TODO:
		}
		catch (const LLMInvalidDataChessException& e)
		{
			OnInvalidDataReceived();
		}
		catch (const LLMInvalidCellChessException& e)
		{
			// TODO:
		}
		catch (const LLMNoPieceChessException& e)
		{
			OnNoPieceFound();
		}
		catch (const LLMInvalidPieceChessException& e)
		{
			OnWrongPieceChosen();
		}
		catch (const LLMInvalidMoveChessException& e)
		{
			OnInvalidMove();
		}
		catch (const std::exception& e)
		{
			LOG_ERROR("non LLM error has occurred: ", e.what());
		}
		catch (...)
		{
			LOG_ERROR("an unknown error has occurred!");
		}
	}

	if (currentAttempts == maxAttempts)
	{
		LOG_FATAL("unable to get a move from the LLM!");
		throw std::runtime_error("unable to get a move from the LLM!");
	}
}

void LLMPlayer::PlayThread_old()
{
	std::string prompt;
	if (!m_isAgentInitialized)
	{
		if (m_pBoard->IsFirstMove())
		{
			prompt = initialPrompt + "You go first.";
		}
		else
		{
			Move lastMove = m_pBoard->GetLastMove();
			prompt = initialPrompt + "I'll go first: {\"start\": \"" + m_pBoard->GetBoardCoordinates(lastMove.origin) + "\", \"end\": \"" + m_pBoard->GetBoardCoordinates(lastMove.target) + "\"}";
		}
		m_isAgentInitialized = true;
	}
	else
	{
		Move lastMove = m_pBoard->GetLastMove();
		prompt = "{\"start\": \"" + m_pBoard->GetBoardCoordinates(lastMove.origin) + "\" \"end\": \"" + m_pBoard->GetBoardCoordinates(lastMove.target) + "\"}";
	}

	LOG_DEBUG("sending: " + prompt);
	std::string llmResponse = m_agent.Send(prompt);
	LOG_DEBUG("received: " + llmResponse);
	
	nlohmann::json res = nlohmann::json::parse(llmResponse);
	std::string origin = res["start"].get<std::string>();
	std::string target = res["end"].get<std::string>();

	// validate coordinates
	if (!isValidCoordinate(origin))
	{
		LOG_ERROR("wrong origin coordinate");
	}
	if (!isValidCoordinate(target))
	{
		LOG_ERROR("wrong target coordinate");
	}

	// convert to coordinates
	Move move {
		.origin = m_pBoard->ToBoardCoordinates(origin),
		.target = m_pBoard->ToBoardCoordinates(target)
	};

	if (!isValidCoordinate(move.origin, m_pBoard))
	{
		LOG_ERROR("wrong origin coordinate");
	}

	if (!isValidCoordinate(move.target, m_pBoard))
	{
		LOG_ERROR("wrong target coordinate");
	}

	Piece* piece = m_pBoard->GetPieceAtCell(move.origin);
	if (piece == nullptr)
	{
		LOG_ERROR("no piece found at: " + origin);
	}
	if (piece->Team() != GetTeam())
	{
		LOG_ERROR("piece not from same team! (selected piece from team ", TeamToString(piece->Team()), " when player is part of team ", TeamToString(GetTeam()), ")");
	}

	// check for invalid moves
	piece->CalculateAvailableMoves();
	const auto& availableMoves = piece->GetAvailableMoves();
	auto ite = std::find_if(availableMoves.begin(), availableMoves.end(), [move](const pt2di& m)
	{
		return m.x == move.target.x && m.y == move.target.y;
	});

	if (ite == availableMoves.end())
	{
		LOG_ERROR("not a legal move for ", piece->Type(), "! (", move.origin.x, ", ", move.origin.y, ") to (", move.target.y, ", ", move.target.y, ")");
	}
	
	piece->Move(move.target);

	m_lastMoveStart = move.origin;
	m_lastMoveEnd = move.target;

	LOG_INFO("LLM Player finished playing");
	EndTurn();
	m_isPlaying = false;
}