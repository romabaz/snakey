#include"GameTexture.h"
#include<vector>
#include<queue>

typedef enum Directions{
	LEFT,
	RIGHT,
	UP,
	DOWN
} Directions;

typedef struct TurnEvent{
	int x;
	int y;
	Directions dir;
} TurnEvent;

typedef struct Chain{
	GameTexture* bodyTexture;
	int x;
	int y;
	Directions dir;
	std::queue<TurnEvent*> pathHistory;
} Chain;


class Snake
{
public:
	Snake(GameTexture* headTexture, int x, int y);
	~Snake();

	//Returns total snake length including head
	int addBodyChain(GameTexture* bodyTexture);
	void move();
	SDL_Point moveLeft();
	SDL_Point moveRight();
	SDL_Point moveUp();
	SDL_Point moveDown();

	bool setDirection(Directions newDirection);
	void setSpeed(short newSpeed);
	short getSpeed();

	//Renders full snake
	void render();

private:
	int mSnakeLenght;
	short mSpeed;
	short mChainRadius;
	//array of headTextures
	std::vector<Chain> mSnakeChain;
	

	//Check collision for the next frame
	bool isCollide();

	void moveDirection(Chain& itemChain);
	void changeChainDirection(Chain& bodyChain, Directions newDirection, int chainNumber, Chain& nextChain);

	TurnEvent* readNextTurnState(Chain& bodyItem);
};