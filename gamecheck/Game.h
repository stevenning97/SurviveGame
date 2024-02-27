#pragma once
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_image.h"
#include "fstream"
#include <string>

struct Vector2
{
	float x;
	float y;
};

class Game
{
public:
	Game();
	bool Initialize();
	void RunLoop();
	void Shutdown();
private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;
	Uint32 mTicksCount;
	
	bool mIsRunning;
	Vector2 snakeDir;
	Vector2 snake;
	Vector2 mBallPos;
	Vector2 mBall2Pos;
	Vector2 mBall3Pos;
	Vector2 mBall4Pos;
	Vector2 mBallVel;
	Vector2 mBall2Vel;
	Vector2 mBall3Vel;
	Vector2 mBall4Vel;

	SDL_Texture* background;
	SDL_Texture* mBallTexture;
	SDL_Texture* mSnakeTexture;
	TTF_Font* mFont;
	Mix_Chunk* mBounce;
	Mix_Music* mMusic;

	int mCurrentScore;
	int mHiScore;

	SDL_Texture* mCurrentScoreTexture;
	int mCurrentScoreSize[2];

	SDL_Texture* mHiScoreTexture;
	int mHiScoreSize[2];


	

};
