#include"Snake.h"

Snake::Snake(GameTexture* snakeTexture, int x, int y){
	mSpeed = 2;
	mSnakeLenght = 1;
	mSpriteStepPx = 50;
	mChainRadius = mSpriteStepPx;
	mSnakeChain.reserve(5);
	mSnakeTexture = snakeTexture;
	initSpriteClips();
	Chain headChain;
	headChain.bodyRect = &mSpriteClips[0];
	headChain.x = x;
	headChain.y = y;
	headChain.dir = RIGHT;
	mSnakeChain.push_back(headChain); 
}

void Snake::render() {
	for (int i = mSnakeLenght - 1; i > 0; --i) {
		mSnakeTexture->render(mSnakeChain[i].x, mSnakeChain[i].y, mSnakeChain[i].bodyRect);
	}
	switch (mSnakeChain[0].dir) {
	case LEFT:
		mSnakeTexture->render(mSnakeChain[0].x, mSnakeChain[0].y, mSnakeChain[0].bodyRect, 0.0, SDL_FLIP_HORIZONTAL);
		break;
	case RIGHT:
		mSnakeTexture->render(mSnakeChain[0].x, mSnakeChain[0].y, mSnakeChain[0].bodyRect);
		break;
	case UP:
		mSnakeTexture->render(mSnakeChain[0].x, mSnakeChain[0].y, mSnakeChain[0].bodyRect, -90.0);
		break;
	case DOWN:
		mSnakeTexture->render(mSnakeChain[0].x, mSnakeChain[0].y, mSnakeChain[0].bodyRect, 90.0);
		break;
	}
}

void Snake::renderFood(int x, int y, int clipNumber){
	mSnakeTexture->render(x, y, &mSpriteClips[clipNumber]);
}

Snake::~Snake() {
	for (int i = 0; i < mSnakeLenght; i++){
		std::queue<TurnEvent*>* currentPathHistory = &mSnakeChain[i].pathHistory;
		while (!currentPathHistory->empty()) {
			delete currentPathHistory->front();
			currentPathHistory->pop();
		}
	}
}

bool Snake::isCollide()
{
	return false;
}

int Snake::addBodyChain()
{
	Chain bodyChain;
	bodyChain.bodyRect = &mSpriteClips[3];
	Chain lastChain = mSnakeChain[mSnakeLenght - 1];
	switch (lastChain.dir) {
	case LEFT:
		bodyChain.x = lastChain.x + mChainRadius;
		bodyChain.y = lastChain.y;
		break;
	case RIGHT:
		bodyChain.x = lastChain.x - mChainRadius;
		bodyChain.y = lastChain.y;
		break;
	case UP:
		bodyChain.x = lastChain.x;
		bodyChain.y = lastChain.y + mChainRadius;
		break;
	case DOWN:
		bodyChain.x = lastChain.x;
		bodyChain.y = lastChain.y - mChainRadius;
		break;
	}
	bodyChain.dir = lastChain.dir;
	mSnakeChain.push_back(bodyChain);
	return ++mSnakeLenght;
}

void Snake::move()
{
	if (!isCollide()) {
		if (mSnakeLenght > 1) {
			for (int i = mSnakeLenght - 1; i > 0; --i) {
				bool needToChangeDir = true;
				Chain& currentChainItem = mSnakeChain[i];
				TurnEvent* nextTurnState = readNextTurnState(mSnakeChain[i - 1]);
				if (nextTurnState != NULL && currentChainItem.dir != nextTurnState->dir)  {
					switch (currentChainItem.dir) {
					case LEFT:
						if (currentChainItem.x != nextTurnState->x) {
							currentChainItem.x -= mSpeed;
							needToChangeDir = false;
						}
						break;
					case RIGHT:
						if (currentChainItem.x != nextTurnState->x) {
							currentChainItem.x += mSpeed;
							needToChangeDir = false;
						}
						break;
					case UP:
						if (currentChainItem.y != nextTurnState->y) {
							currentChainItem.y -= mSpeed;
							needToChangeDir = false;
						}
						break;
					case DOWN:
						if (currentChainItem.y != nextTurnState->y) {
							currentChainItem.y += mSpeed;
							needToChangeDir = false;
						}
						break;
					}
					if (needToChangeDir) {
						currentChainItem.dir = nextTurnState->dir;
						if (i < mSnakeLenght - 1) {
							currentChainItem.pathHistory.push(new TurnEvent{ currentChainItem.x, currentChainItem.y, currentChainItem.dir });
						}
						moveDirection(currentChainItem);
						delete nextTurnState;
						mSnakeChain[i - 1].pathHistory.pop();
					}
				}
				else {
					moveDirection(currentChainItem);
				}
			}
		}
		//move Head
		moveDirection(mSnakeChain[0]);
	}
	return;
}

void Snake::moveDirection(Chain& itemChain) {
	switch (itemChain.dir) {
	case LEFT:
		itemChain.x -= mSpeed;
		break;
	case RIGHT:
		itemChain.x += mSpeed;
		break;
	case UP:
		itemChain.y -= mSpeed;
		break;
	case DOWN:
		itemChain.y += mSpeed;
		break;
	}
}

bool Snake::setDirection(Directions newDirection) 
{
	if (newDirection == mSnakeChain[0].dir) {
		return true;
	}
	if (!mSnakeChain[0].pathHistory.empty() && mSnakeChain[0].x == mSnakeChain[0].pathHistory.back()->x && mSnakeChain[0].y == mSnakeChain[0].pathHistory.back()->y) {
		return false;
	}
	if (!isCollide()) {
		if (mSnakeLenght > 1) {
			mSnakeChain[0].pathHistory.push(new TurnEvent{ mSnakeChain[0].x, mSnakeChain[0].y, newDirection });
		}
		mSnakeChain[0].dir = newDirection;
		return true;
	}
	else {
		return false;
	}
}

void Snake::setSpeed(short newSpeed) 
{
	mSpeed = newSpeed;
}

short Snake::getSpeed() {
	return mSpeed;
}


TurnEvent* Snake::readNextTurnState(Chain& bodyItem){
	if (bodyItem.pathHistory.size() > 0) {
		return bodyItem.pathHistory.front();
	}
	else {
		return NULL;
	}
}

void Snake::initSpriteClips(){
	//Snake's head
	mSpriteClips[0].x = 0;
	mSpriteClips[0].y = 0;
	mSpriteClips[0].w = mSpriteStepPx;
	mSpriteClips[0].h = mSpriteStepPx;

	//Snake's body chain outer
	mSpriteClips[1].x = 50;
	mSpriteClips[1].y = 0;
	mSpriteClips[1].w = mSpriteStepPx;
	mSpriteClips[1].h = mSpriteStepPx;

	//Snake's body chain outer
	mSpriteClips[2].x = 100;
	mSpriteClips[2].y = 0;
	mSpriteClips[2].w = mSpriteStepPx;
	mSpriteClips[2].h = mSpriteStepPx;

	//Snake's body chain outer
	mSpriteClips[3].x = 150;
	mSpriteClips[3].y = 0;
	mSpriteClips[3].w = mSpriteStepPx;
	mSpriteClips[3].h = mSpriteStepPx;

	//Snake's body chain outer
	mSpriteClips[4].x = 200;
	mSpriteClips[4].y = 0;
	mSpriteClips[4].w = mSpriteStepPx;
	mSpriteClips[4].h = mSpriteStepPx;

	//Food: leave
	mSpriteClips[5].x = 250;
	mSpriteClips[5].y = 0;
	mSpriteClips[5].w = mSpriteStepPx;
	mSpriteClips[5].h = mSpriteStepPx;

	//Food: apple
	mSpriteClips[6].x = 300;
	mSpriteClips[6].y = 0;
	mSpriteClips[6].w = mSpriteStepPx;
	mSpriteClips[6].h = mSpriteStepPx;

	//Food: potato
	mSpriteClips[7].x = 350;
	mSpriteClips[7].y = 0;
	mSpriteClips[7].w = mSpriteStepPx;
	mSpriteClips[7].h = mSpriteStepPx;

	//Food: carrot
	mSpriteClips[8].x = 400;
	mSpriteClips[8].y = 0;
	mSpriteClips[8].w = mSpriteStepPx;
	mSpriteClips[8].h = mSpriteStepPx;

}

