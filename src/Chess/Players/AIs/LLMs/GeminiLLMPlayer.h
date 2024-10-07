#pragma once

#include "../LLMPlayer.h"

class GeminiLLMPlayer : public LLMPlayer
{
public:
	GeminiLLMPlayer(Board* pBoard, TEAM team, const std::string& apiKey);
	virtual ~GeminiLLMPlayer();

	// prompt based
	virtual std::string GetInitialYouGoFirstPrompt() const override;
	virtual std::string GetInitialYouGoSecondPrompt(const Move& lastMove) const override;
	virtual std::string GetMovePrompt(const Move& lastMove) const override;
	virtual nlohmann::json ParseResponse(const std::string& llmResponse) const override;
	virtual std::string OnDeserializationErrorPrompt(const std::string& data) const override;
	virtual std::string OnInvalidDataReceivedPrompt(const std::string& type, const std::string& data) const override;
	virtual std::string OnInvalidChessCellPrompt(const std::string& type, const pt2di& coordinates) const override;
	virtual std::string OnNoPieceFoundPrompt(const pt2di& location) const override;
	virtual std::string OnWrongPieceChosenPrompt(const pt2di& location) const override;
	virtual std::string OnInvalidMovePrompt(const std::string& pieceType, const pt2di& start, const pt2di& end) const override;
	virtual std::string OnUnknownErrorPrompt() const override;

	// behavior based
	virtual Move AskForMoveYouGoFirst() override;
	virtual Move AskForMoveYouGoSecond(const Move& lastMove) override;
	virtual Move AskForMove(const Move& lastMove) override;
	virtual void OnDeserializationError() override;
	virtual void OnInvalidDataReceived() override;
	virtual void OnInvalidChessCell() override;
	virtual void OnNoPieceFound() override;
	virtual void OnWrongPieceChosen() override;
	virtual void OnInvalidMove() override;

private:
	GeminiAgent m_agent;
	bool m_isAgentInitialized{ false };
};