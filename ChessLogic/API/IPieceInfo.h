#pragma once

#include "EColor.h"
#include "EPieceType.h"

#include <memory>

// Informatii read-only despre o piesa, pentru UI.
class IPieceInfo
{
public:
	virtual ~IPieceInfo() = default;

	virtual EPieceType GetType() const = 0;
	virtual EColor GetColor() const = 0;
};

using IPieceInfoPtr = std::shared_ptr<IPieceInfo>;
