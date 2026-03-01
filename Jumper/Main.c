#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <raymath.h>

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#define BOXES_COUNT 3

//const
const int GRAVITY = 3; //aka Jump Speed
const int SLIDE_SPEED = 200; // speed of blocks
const int MIN_PLAYER_POS_Y = 300;
const int MAX_PLAYER_POS_Y = 200;
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

//functions
static void Initialize();
static void Start();
static void Update();
static void Draw();
static double GetSmoothWave(double _time);
static char* ConvertToString(double v);
static void RespawnBox(int idx);

//variables
bool isJumping = false;
double jumpingTime = 0;
int ticksSinceTriggerTouched = 0;
bool isGameOver = false;
int score = 0;
int highScore = 0;

//objetcs
Rectangle groundRect;
Rectangle playerRect;
Rectangle triggerRect; // trigger needs to score accumulation
Rectangle boxRect[BOXES_COUNT];

//Initialize window
static void Initialize() {
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Jumper Game");
	SetTargetFPS(60);
}

int main() {
	Initialize();
	Start();
	
	while (!WindowShouldClose()) {
		Update();
		Draw();
	}
	CloseWindow();


}
//Setup variables and other stuff for game
static void Start() {
	//setup player rectangle
	playerRect.x = 200;
	playerRect.y = MIN_PLAYER_POS_Y;
	playerRect.width = 30;
	playerRect.height = 70;

	//setup ground rectangle
	groundRect.x = 0;
	groundRect.y = MIN_PLAYER_POS_Y + playerRect.height - 5;
	groundRect.width = WINDOW_WIDTH;
	groundRect.height = 400;

	//setup boxes(obstacles) rentangle
	for (int i = 0; i < BOXES_COUNT; i++) {

		RespawnBox(i);
		boxRect[i].y = MIN_PLAYER_POS_Y+40;
		boxRect[i].width = 40;
		boxRect[i].height = 40;
	}
	

	//setup trigger(for score) rectangle
	triggerRect.x = playerRect.x - 5;
	triggerRect.y = playerRect.y;
	triggerRect.width = 2;
	triggerRect.height = playerRect.height;
}
//Calculations, input handling and other stuff
static void Update() {
	float deltaTime = GetFrameTime();

	if (isGameOver) {
		if (IsKeyPressed(KEY_SPACE)) {
			for (int i = 0; i < BOXES_COUNT; i++) {
				RespawnBox(i);
			}
			isGameOver = false;
			score = 0;
			playerRect.y = MIN_PLAYER_POS_Y;
			isJumping = false;
		}
		return;
	}

	//INPUT
	if (IsKeyPressed(KEY_SPACE) && !isJumping) {
		isJumping = true;
		jumpingTime = 0;
	}

	//OBSTACLES
	for (int i = 0; i < BOXES_COUNT; i++) {
		boxRect[i].x -= deltaTime * SLIDE_SPEED;
		//if box is outside of screen, respawn it!
		if (boxRect[i].x <= -50) {
			RespawnBox(i);
		}
		//check colliding with player
		if (CheckCollisionRecs(playerRect, boxRect[i])) {
			isGameOver = true;
		}
		//check colliding with trigger to increment scores
		if (CheckCollisionRecs(triggerRect, boxRect[i])) {

			if (ticksSinceTriggerTouched >= 5) {
				score++;
			}
			ticksSinceTriggerTouched = 0;
		}
	}
	
	
	//PLAYER
	if (isJumping) {
		jumpingTime += deltaTime;
		playerRect.y = (int)Lerp(MIN_PLAYER_POS_Y, MAX_PLAYER_POS_Y, GetSmoothWave(jumpingTime *GRAVITY));
		if (CheckCollisionRecs(playerRect, groundRect) && jumpingTime > 0.5) {
			isJumping = false;
		}
	}

	//SCORE
	if (highScore < score) {
		highScore = score;
	}
	ticksSinceTriggerTouched++;

}

//Rendering only
static void Draw() {
	BeginDrawing();
	ClearBackground(YELLOW);

	//Player
	DrawRectangle(playerRect.x, playerRect.y, playerRect.width, playerRect.height, BLACK);

	//Ground
	DrawRectangle(groundRect.x, groundRect.y, groundRect.width, groundRect.height, DARKGREEN);

	//Boxes(obstacles)
	for (int i = 0; i < BOXES_COUNT; i++) {
		DrawRectangle(boxRect[i].x, boxRect[i].y, boxRect[i].width, boxRect[i].height, DARKBLUE);
	}

	//Trigger
	DrawRectangle(triggerRect.x, triggerRect.y, triggerRect.width, triggerRect.height, Fade(BLACK, 0));
	
	//Draw Score and High Score
	char scoreStr[200];
	snprintf(scoreStr, sizeof(scoreStr),"Score: %d\nHigh Score: %d\n",score, highScore);
	DrawText(scoreStr, 0, 50, 20, RED);
	
	//Draw GameOver UI
	if (isGameOver) {
		//Center GameOver panel
		int H = WINDOW_HEIGHT * 0.40;
		int W = WINDOW_WIDTH * 0.60;
		int X = (WINDOW_WIDTH / 2) - (W / 2);
		int Y = (WINDOW_HEIGHT / 2) - (H / 2);
		DrawRectangle(X, Y, W, H, Fade(GRAY,0.7f));
		DrawText("GAME OVER", X, Y, 20, RED);
		DrawText("press 'SPACE' to play again", X, Y+40, 20, RED);
	}
	EndDrawing();
}

//Some Utils
static double GetSmoothWave(double _time) {
	return fabs(sin(_time));
}

static void RespawnBox(int idx) {
	int max = 2500;
	int min = 850;
	int newPosX = rand() % (max - min + 1) + min;
	boxRect[idx].x = newPosX;
}
