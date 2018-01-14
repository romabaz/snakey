#include "Snakey.h"

Snakey::Snakey(): mSnakeyLength(1), mSpeed(1)
{
	//set initial capacity
	mSnakeyBody.reserve(8);
	//create snakey's head
	mSnakeyBody.push_back(new SnakeyQuantum(0, 0, LEFT, SNAKEY_HEAD));
}


Snakey::~Snakey()
{
	//1. Traverse mSnakeyBody and delete all SnakeyQuantum objects
	for (SnakeyQuantum* sq : mSnakeyBody) {
		if (sq) delete sq;
	}
	//2. Traverse mSnakeyEvents and delete all SnakeyEvent objects
	for (SnakeyEvent* seve : mSnakeyEvents) {
		if (seve) delete seve;
	}
}

void Snakey::init(int x, int y)
{
	int currX = x;
	int currY = y;
	if (mSnakeyLength == 0) {
		throw std::runtime_error("Snakey has zero length");
	}
	mSnakeyBody[0]->x = currX;
	mSnakeyBody[0]->y = currY;
	for (std::size_t i = 1; i < mSnakeyLength; ++i) {
		switch (mSnakeyBody[i]->direction) {
		case LEFT:
			currX += radius;
			break;
		case RIGHT:
			currX -= radius;
			break;
		case UP:
			currY += radius;
			break;
		case DOWN:
			currY -= radius;
			break;
		}
		mSnakeyBody[i]->x = currX;
		mSnakeyBody[i]->y = currY;
	}
}

void Snakey::applyGameEvent(const GameEvent gEvent)
{
	if (mSnakeyLength < 1) {
		return;
	}

	SnakeyQuantum* snakeHead = mSnakeyBody[0];
	int eventX = snakeHead->x;
	int eventY = snakeHead->y;

	//Apply to head first
	applyGameEvent(snakeHead, gEvent);
	if (mSnakeyLength > 1) {
		//Add SnakeyEvent to the vector if the event has happenned
		if (gEvent != GE_NONE && gEvent != GE_GROW) {
			mSnakeyEvents.push_back(new SnakeyEvent(eventX, eventY, gEvent));
			//Inform every waiting quantum to get ready for the new event
			for (std::size_t i = 1; i < mSnakeyLength; i++) {
				SnakeyQuantum* sq = mSnakeyBody[i];
				if (sq->nextSnakeyEventId == UINT_MAX) {
					sq->nextSnakeyEventId = 0;
				}
			}
		}
	}
}

void Snakey::checkQueuedEventToHappen(SnakeyQuantum* sq, std::size_t quantumId) {
	if (sq->nextSnakeyEventId == UINT_MAX || sq->nextSnakeyEventId > mSnakeyEvents.size() - 1) {
		return;
	}
	if (mSnakeyEvents.size() > 0) {
		//1. Check each quantum for its event to happen
		SnakeyEvent* nextSEvent = mSnakeyEvents[sq->nextSnakeyEventId];
		if (sq->x == nextSEvent->x &&
			sq->y == nextSEvent->y) {

			//2. Apply event
			applyGameEvent(sq, nextSEvent->event);

			//3. Reset or move forward the nextEventId
			if (nextSEvent == mSnakeyEvents.back()) {
				//No events left for this quantum
				sq->nextSnakeyEventId = UINT_MAX;
			}
			else {
				//set the next event for this quantum
				sq->nextSnakeyEventId++;
			}

			//4. Delete SnakeyEvent if this is the last quantum
			if (quantumId == mSnakeyLength - 1) {
				//mSnakeyEvents.pop_front();
				delete nextSEvent;
			}
		}
	}
}

std::vector<DrawConstruct> Snakey::getDrawConstruct() const
{
	std::vector<DrawConstruct> dc;
	for (SnakeyQuantum* sq : mSnakeyBody) {
		dc.push_back({ sq->iam, sq->x, sq->y, sq->direction });
	}
	return dc;
}

void Snakey::grow()
{
	SnakeyQuantum* lastQuantum = mSnakeyBody.back();
	int x = 0, y = 0;
	switch (lastQuantum->direction) {
	case LEFT:
		x = lastQuantum->x + radius;
		y = lastQuantum->y;
		break;
	case RIGHT:
		x = lastQuantum->x - radius;
		y = lastQuantum->y;
		break;
	case UP:
		x = lastQuantum->x;
		y = lastQuantum->y + radius;
		break;
	case DOWN:
	default:
		x = lastQuantum->x;
		y = lastQuantum->y - radius;
		break;
	}
	mSnakeyBody.push_back(new SnakeyQuantum(x, y, lastQuantum->direction, SNAKEY_QUANTUM1));
	++mSnakeyLength;
}

void Snakey::applyGameEvent(SnakeyQuantum* sq, GameEvent gameEvent)
{
	switch (gameEvent) {
	case GE_UP:
		sq->direction = UP;
		break;
	case GE_DOWN:
		sq->direction = DOWN;
		break;
	case GE_LEFT:
		sq->direction = LEFT;
		break;
	case GE_RIGHT:
		sq->direction = RIGHT;
		break;
	case GE_GROW:
		grow();
		break;
	case GE_NONE:
	default:
		break;
	}
}

void Snakey::move()
{
	for (std::size_t i = 0; i < mSnakeyLength; i++) {
		SnakeyQuantum* sq = mSnakeyBody[i];
		if (i > 0) {
			checkQueuedEventToHappen(sq, i);
		}
		move(sq);
	}
}

void Snakey::move(SnakeyQuantum* sq)
{
	switch (sq->direction)
	{
	case UP:
		sq->y -= mSpeed;
		break;
	case DOWN:
		sq->y += mSpeed;
		break;
	case LEFT:
		sq->x -= mSpeed;
		break;
	case RIGHT:
		sq->x += mSpeed;
		break;
	default:
		break;
	}
}

