
#include "Game.h"
#include <algorithm>
#include <deque>
#include <string>
#include <iostream>
#include <vector>




int thickness = 15;
float paddleH = 15.00f;

Game::Game()
{
	mWindow=nullptr;
	mRenderer=nullptr;
	mTicksCount=0;
	mIsRunning=true;
	snakeDir.x = 0;
	snakeDir.y = 0;

	mCurrentScore = 0;
	mHiScore = 0;

}



bool Game::Initialize()
{


	// Initialize SDL
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
	
	// Create an SDL Window
	mWindow = SDL_CreateWindow(
		"SurviveGame", // Window title
		100,	// Top left x-coordinate of window
		100,	// Top left y-coordinate of window
		1024,	// Width of window
		768,	// Height of window
		0		// Flags (0 for no flags set)
	);

	 

	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}
	
	//// Create SDL renderer
	mRenderer = SDL_CreateRenderer(
		mWindow, // Window to create renderer for
		-1,		 // Usually -1
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	mBounce = Mix_LoadWAV("bounce.wav");
	mMusic = Mix_LoadMUS("bgm.wav");


	TTF_Init();
	mFont = TTF_OpenFont("OpenSans-Regular.ttf", 28);
	if (mFont == nullptr) {
		SDL_Log("failed to create font: %s", SDL_GetError());
		return false;
	}

	std::ifstream inputFile("scores.txt");
	if (inputFile.is_open()) {
		std::string temp;
		inputFile >> temp;
		mHiScore = atoi(temp.c_str());
	}

	SDL_Surface* tempSurface = IMG_Load("spikeballreal.png");
	mBallTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
	SDL_FreeSurface(tempSurface);

	tempSurface = IMG_Load("smiley.png");
	mSnakeTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
	SDL_FreeSurface(tempSurface);

	tempSurface = TTF_RenderText_Solid(mFont, "score: 0", { 255,255,255,255 });
	mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
	mCurrentScoreSize[0] = tempSurface->w;
	mCurrentScoreSize[1] = tempSurface->h;
	SDL_FreeSurface(tempSurface);


	tempSurface = TTF_RenderText_Solid(mFont, ("HiScore: " + std::to_string(mHiScore)).c_str(), { 255, 255, 255, 255 });
	mHiScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
	mHiScoreSize[0] = tempSurface->w;
	mHiScoreSize[1] = tempSurface->h;
	SDL_FreeSurface(tempSurface);

	tempSurface = tempSurface = IMG_Load("backgroundimg.png");
	background = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
	SDL_FreeSurface(tempSurface);




	//
	snake.x = 1024.0f/2.0f;
	snake.y = 768.0f/2.0f;
	mBallPos.x = 600.0f;
	mBallPos.y = 400.0f;
	mBallVel.x = 450.0f;
	mBallVel.y = 600.0f;

	mBall2Pos.x = 320.0f;
	mBall2Pos.y = 400.0f;
	mBall2Vel.x = -700.0f;
	mBall2Vel.y = 600.0f;

	mBall3Pos.x = 450.0f;
	mBall3Pos.y = 700.0f;
	mBall3Vel.x = -700.0f;
	mBall3Vel.y = -400.0f;

	mBall4Pos.x = 400.0f;
	mBall4Pos.y = 500.0f;
	mBall4Vel.x = -450.0f;
	mBall4Vel.y = -700.0f;
	

	Mix_PlayMusic(mMusic, -1);



	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			// If we get an SDL_QUIT event, end loop
			case SDL_QUIT:
				mIsRunning = false;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_m:
					if (Mix_PausedMusic() == 1) {
						Mix_ResumeMusic();
					}
					else {
						Mix_PauseMusic();
					}
				}
		}
	}
	
	// Get state of keyboard
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// If escape is pressed, also end loop
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}
	
	// Update paddle direction based on W/S keys
	snakeDir.y = 0;
	snakeDir.x = 0;
	if (state[SDL_SCANCODE_W])
	{
		snakeDir.y -= 1;
	}
	if (state[SDL_SCANCODE_S])
	{
		snakeDir.y += 1;
	}
	if (state[SDL_SCANCODE_A]) {
		snakeDir.x -= 1;
	}
	if (state[SDL_SCANCODE_D]) {
		snakeDir.x += 1;
	}

	if (state[SDL_SCANCODE_LEFT]) {

	}
}

void Game::UpdateGame()
{
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	// Delta time is the difference in ticks from last frame
	// (converted to seconds)
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	
	// Clamp maximum delta time value
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	// Update tick counts (for next frame)
	mTicksCount = SDL_GetTicks();
	
	// Update paddle position based on direction
	if (snakeDir.y != 0)
	{
		snake.y += snakeDir.y * 300.0f * deltaTime;
		// Make sure paddle doesn't move off screen!
		if (snake.y < (paddleH/2.0f + thickness))
		{
			//SDL_Quit();
			mIsRunning = false;
		}
		else if (snake.y > (768.0f - paddleH/2.0f - thickness))
		{
			//SDL_Quit();
			mIsRunning = false;
		}
	}
	if (snakeDir.x != 0)
	{
		snake.x += snakeDir.x * 300.0f * deltaTime;
		// Make sure paddle doesn't move off screen!
		if (snake.x < 0)
		{
			//SDL_Quit();
			mIsRunning = false;
		}
		else if(snake.x>1009)
		{
			//SDL_Quit();
			mIsRunning = false;
		}
	}


	mBallPos.x += mBallVel.x * deltaTime;
	mBallPos.y += mBallVel.y * deltaTime;

	mBall2Pos.x += mBall2Vel.x * deltaTime;
	mBall2Pos.y += mBall2Vel.y * deltaTime;

	mBall3Pos.x += mBall3Vel.x * deltaTime;
	mBall3Pos.y += mBall3Vel.y * deltaTime;
	
	mBall4Pos.x += mBall4Vel.x * deltaTime;
	mBall4Pos.y += mBall4Vel.y * deltaTime;

	float diff = snake.y - mBallPos.y;
	float diffx = snake.x - mBallPos.x;
	diff = (diff > 0.0f) ? diff : -diff;
	diffx = (diffx > 0.0f) ? diffx : -diffx;

	float diff2 = snake.y - mBall2Pos.y;
	float diff2x = snake.x - mBall2Pos.x;
	diff2 = (diff2 > 0.0f) ? diff2 : -diff2;
	diff2x = (diff2x > 0.0f) ? diff2x : -diff2x;

	float diff3 = snake.y - mBall3Pos.y;
	float diff3x = snake.x - mBall3Pos.x;
	diff3 = (diff3 > 0.0f) ? diff3 : -diff3;
	diff3x = (diff3x > 0.0f) ? diff3x : -diff3x;


	float diff4 = snake.y - mBall4Pos.y;
	float diff4x = snake.x - mBall4Pos.x;
	diff4 = (diff4 > 0.0f) ? diff4 : -diff4;
	diff4x = (diff4x > 0.0f) ? diff4x : -diff4x;
	
	if (diffx<=30 && diff <=30  )
	{
		//SDL_Quit();
		mIsRunning = false;
	}
	// Did the ball collide with the bottom wall?
	//ball1
	else if (mBallPos.y >= (768 - 2*thickness) &&
		mBallVel.y > 0.0f)
	{	
		if (mBallVel.y > 700.0f) {
			mBallVel.y = 700.0f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}
		else {
			mBallVel.y *= -1.2f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}
		
	}
	else if (mBallPos.x >= (1024.0f - 2*thickness) && mBallVel.x > 0.0f)
	{
		if (mBallVel.x > 700.0f) {
			mBallVel.x = 700.0f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}
		else {
			mBallVel.x *= -1.2f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}
	}
	else if (mBallPos.x <= 15.0f) {
		if (mBallVel.x > 700.0f) {
			mBallVel.x = 700.0f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}
		else {
			mBallVel.x *= -1.2f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}
	}
	else if (mBallPos.y <= 2*thickness && mBallVel.y < 0.0f)
	{
		if (mBallVel.y > 700.0f) {
			mBallVel.y = 700.0f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}
		else {
			mBallVel.y *= -1.2f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}
	}


	//ball3


	if (diff3x <= 30 && diff3 <= 30)
	{
		//SDL_Quit();
		mIsRunning = false;
	}

	else if (mBall3Pos.y <= thickness && mBall3Vel.y < 0.0f)
	{
		if (mBall3Vel.y > 700.0f) {
			mBall3Vel.y = 700.0f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}
		else {
			mBall3Vel.y *= -1.2f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}
	}
	
	else if (mBall3Pos.y >= (768 - thickness) &&
		mBall3Vel.y > 0.0f)
	{
		if (mBall3Vel.y > 700.0f) {
			mBall3Vel.y = 700.0f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}
		else {
			mBall3Vel.y *= -1.2f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}
	}
	else if (mBall3Pos.x >= (1024.0f - thickness) && mBall3Vel.x > 0.0f)
	{
		if (mBall3Vel.x > 700.0f) {
			mBall3Vel.x = 700.0f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}
		else {
			mBall3Vel.x *= -1.2f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}
	}
	else if (mBall3Pos.x <= 15.0f) {
		if (mBall3Vel.x > 700.0f) {
			mBall3Vel.x = 700.0f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}
		else {
			mBall3Vel.x *= -1.2f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}
	}

	
	//ball2


	if (diff2x <= 30 && diff2 <= 30)
	{
		//SDL_Quit();
		mIsRunning = false;
	}
	else if (mBall2Pos.y <= thickness && mBall2Vel.y < 0.0f)
	{
		if (mBall2Vel.y > 700.0f) {
			mBall2Vel.y = 700.0f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}
		else {
			mBall2Vel.y *= -1.2f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}
	}

	else if (mBall2Pos.y >= (768 -thickness) &&
		mBall2Vel.y > 0.0f)
	{
		if (mBall2Vel.y > 700.0f) {
			mBall2Vel.y = 700.0f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}
		else {
			mBall2Vel.y *= -1.2f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}
	}
	else if (mBall2Pos.x >= (1024.0f -thickness) && mBall2Vel.x > 0.0f)
	{
		if (mBall2Vel.x > 700.0f) {
			mBall2Vel.x = 700.0f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}
		else {
			mBall2Vel.x *= -1.2f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}
	}
	else if (mBall2Pos.x <= 15.0f) {
		if (mBall2Vel.x > 700.0f) {
			mBall2Vel.x = 700.0f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}
		else {
			mBall2Vel.x *= -1.2f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}
	}
	//diffx <= 165.0f

	//ball 4

	if (diff4x <= 30 && diff4 <= 30)
	{
		//SDL_Quit();
		mIsRunning = false;
	}
	
	else if (mBall4Pos.y <= thickness && mBall4Vel.y < 0.0f)
	{
		if (mBall4Vel.y > 700.0f) {
			mBall4Vel.y = 700.0f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}
		else {
			mBall4Vel.y *= -1.2f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}
	}
	// Did the ball collide with the bottom wall?
	else if (mBall4Pos.y >= (768 - thickness) &&
		mBall4Vel.y > 0.0f)
	{
		if (mBall4Vel.y > 700.0f) {
			mBall4Vel.y = 700.0f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}
		else {
			mBall4Vel.y *= -1.1f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}

	}
	else if (mBall4Pos.x >= (1024.0f - thickness) && mBall4Vel.x > 0.0f)
	{
		if (mBall4Vel.x > 700.0f) {
			mBall4Vel.x = 700.0f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}
		else {
			mBall4Vel.x *= -1.3f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}
	}
	else if (mBall4Pos.x <= 15.0f) {
		if (mBall4Vel.x > 700.0f) {
			mBall4Vel.x = 700.0f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}
		else {
			mBall4Vel.x *= -1.56f;
			Mix_PlayChannel(-1, mBounce, 0);
			mCurrentScore++;
			SDL_DestroyTexture(mCurrentScoreTexture);
			SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("score: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
			mCurrentScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
			mCurrentScoreSize[0] = tempSurface->w;
			mCurrentScoreSize[1] = tempSurface->h;
			SDL_FreeSurface(tempSurface);
		}
	}
	
	if (mCurrentScore > mHiScore) {
		mHiScore = mCurrentScore;

		SDL_DestroyTexture(mHiScoreTexture);
		SDL_Surface* tempSurface = TTF_RenderText_Solid(mFont, ("HiScore: " + std::to_string(mCurrentScore)).c_str(), { 255, 255, 255, 255 });
		mHiScoreTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
		mHiScoreSize[0] = tempSurface->w;
		mHiScoreSize[1] = tempSurface->h;
		SDL_FreeSurface(tempSurface);
	}

}

void Game::GenerateOutput()
{
	
	
	// Set draw color to blue
	SDL_SetRenderDrawColor(
		mRenderer,
		0,		// R
		0,		// G 
		255,	// B
		255		// A
	);
	
	// Clear back buffer
	SDL_RenderClear(mRenderer);

	SDL_Rect BackgroundPos{
		0,000,1024,768 };
	SDL_RenderCopyEx(mRenderer, background, nullptr, &BackgroundPos, 0, NULL, SDL_FLIP_NONE);

	// Draw walls
	SDL_SetRenderDrawColor(mRenderer, 100, 100, 220, 85);
	
	// Draw top wall
	SDL_Rect wall{
		0,			// Top left x
		0,			// Top left y
		1024,		// Width
		thickness	// Height
	};
	SDL_RenderFillRect(mRenderer, &wall);
	
	// Draw bottom wall
	wall.y = 768 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);
	
	// Draw right wall
	wall.x = 1024 - thickness;
	wall.y = 0;
	wall.w = thickness;
	wall.h = 1024;
	SDL_RenderFillRect(mRenderer, &wall);

	wall.x = 0;
	wall.y = 0;
	wall.w = thickness;
	wall.h = 1024;
	SDL_RenderFillRect(mRenderer, &wall);
	
	// Draw paddle
	SDL_Rect paddle{
		static_cast<int>(snake.x),
		static_cast<int>(snake.y - paddleH/2),
		thickness,
		static_cast<int>(paddleH)
	};
	//SDL_RenderFillRect(mRenderer, &paddle);
	SDL_RenderCopyEx(mRenderer, mSnakeTexture, NULL, &paddle, 0, NULL, SDL_FLIP_NONE);
	
	// Draw ball
	SDL_Rect ball{	
		static_cast<int>(mBallPos.x - thickness/2),
		static_cast<int>(mBallPos.y - thickness/2),
		3*thickness,
		3*thickness
	};
	//SDL_RenderFillRect(mRenderer, &ball);
	SDL_RenderCopyEx(mRenderer, mBallTexture, NULL, &ball, 0, NULL, SDL_FLIP_NONE);

	//draw ball2
	SDL_Rect ball2{
		static_cast<int>(mBall2Pos.x - thickness / 2),
		static_cast<int>(mBall2Pos.y - thickness / 2),
		3*thickness,
		3*thickness
	};
	//SDL_RenderFillRect(mRenderer, &ball2);
	SDL_RenderCopyEx(mRenderer, mBallTexture, NULL, &ball2, 0, NULL, SDL_FLIP_NONE);

	//draw ball3
	SDL_Rect ball3{
		static_cast<int>(mBall3Pos.x - thickness / 2),
		static_cast<int>(mBall3Pos.y - thickness / 2),
		3*thickness,
		3*thickness
	};
	//SDL_RenderFillRect(mRenderer, &ball3);
	SDL_RenderCopyEx(mRenderer, mBallTexture, NULL, &ball3, 0, NULL, SDL_FLIP_NONE);
	
	//draw ball4
	SDL_Rect ball4{
		static_cast<int>(mBall4Pos.x - thickness / 2),
		static_cast<int>(mBall4Pos.y - thickness / 2),
		3*thickness,
		3*thickness
	};
	//SDL_RenderFillRect(mRenderer, &ball4);
	SDL_RenderCopyEx(mRenderer, mBallTexture, NULL, &ball4, 0, NULL, SDL_FLIP_NONE);
	
	// draw score

	SDL_Rect HiScoreDrawPos{
		1024 - 30 - mHiScoreSize[0],30,mHiScoreSize[0],mHiScoreSize[1]
	};
	SDL_RenderCopyEx(mRenderer, mHiScoreTexture, nullptr, &HiScoreDrawPos, 0, NULL, SDL_FLIP_NONE);

	SDL_Rect CurrentScoreDrawPos{
		1024 - 30 - mCurrentScoreSize[0],80,mCurrentScoreSize[0],mCurrentScoreSize[1]
	};
	SDL_RenderCopyEx(mRenderer, mCurrentScoreTexture, nullptr, &CurrentScoreDrawPos, 0, NULL, SDL_FLIP_NONE);

	

	
	

	SDL_SetRenderDrawColor(mRenderer, 255, 0, 0, 255);
	
	
	// Swap front buffer and back buffer
	SDL_RenderPresent(mRenderer);
}

void Game::Shutdown()
{

	std::ofstream outputFile("scores.txt");
	if (outputFile.is_open()) {
		outputFile << std::to_string(mHiScore);
	}
	outputFile.close();

	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}