#include <iostream>
#include <list>
#include <thread>

#include <Windows.h>

/*
 * I remember seeing this:
 * https://github.com/OneLoneCoder/videos/blob/master/OneLoneCoder_Snake.cpp
 * 
 * And I remember the result looked amazing but I wanted to see if I could make my own
 * version of it (eg: not looking at the code while I make it)
 */

// FORMULA FOR FINDING 2D POSITION IN 1D ARRAY:
// [Y * WIDTH + X] = 1D ARRAY POSITION

struct Vector2
{
	int X;
	int Y;
};

enum class SnakeDirection
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};

int main()
{
	int ScreenWidth = 120;
	int ScreenHeight = 40;

	wchar_t *screen = new wchar_t[ScreenWidth * ScreenHeight]; // <- Possible overflow (4 bytes being cast to 8 without protection)
	for(int ii = 0; ii < ScreenWidth * ScreenHeight; ii++)
		screen[ii] = L' ';
	DWORD bytesWritten = 0;
	HANDLE console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(console);

	while(true)
	{
		std::list<Vector2> snake = { {60, 20}, {61, 20}, {62, 20}, {63, 20}, {64, 20}, {65, 20}, {66, 20}, {67, 20}, {68, 20}, {69, 20} };

		int score = 0;
		SnakeDirection snakeDir = SnakeDirection::LEFT;

		Vector2 foodPos = { 30, 20 };

		bool dead = false;

		while(!dead)
		{
			//========[TIMING]=========//
			{
				using namespace std;

				// Compensate for Movement UP/DOWN being faster due to nature of text in console

				if(snakeDir == SnakeDirection::UP || snakeDir == SnakeDirection::DOWN)
					this_thread::sleep_for(200ms);
				else
					this_thread::sleep_for(100ms);
			}

			//========[INPUT]==========//
			if(GetAsyncKeyState(VK_UP))
			{
				snakeDir = SnakeDirection::UP;
			}
			else if(GetAsyncKeyState(VK_DOWN))
			{
				snakeDir = SnakeDirection::DOWN;
			}
			else if(GetAsyncKeyState(VK_LEFT))
			{
				snakeDir = SnakeDirection::LEFT;
			}
			else if(GetAsyncKeyState(VK_RIGHT))
			{
				snakeDir = SnakeDirection::RIGHT;
			}

			//========[LOGIC]==========//

			switch(snakeDir)
			{
				case SnakeDirection::UP:
					snake.push_front({ snake.front().X, snake.front().Y - 1 });
					break;
				case SnakeDirection::DOWN:
					snake.push_front({ snake.front().X, snake.front().Y + 1 });
					break;
				case SnakeDirection::LEFT:
					snake.push_front({ snake.front().X - 1, snake.front().Y });
					break;
				case SnakeDirection::RIGHT:
					snake.push_front({ snake.front().X + 1, snake.front().Y });
					break;
			}

			// Snake & Food
			if(snake.front().X == foodPos.X && snake.front().Y == foodPos.Y)
			{
				score += 5;

				foodPos.X = rand() % ScreenWidth;
				foodPos.Y = rand() % ScreenHeight;

				for(int ii = 0; ii < 3; ii++)
					snake.push_back({ snake.back().X, snake.back().Y });
			}

			// Snake & World
			if(snake.front().X <= -1 || snake.front().X >= ScreenWidth || snake.front().Y <= 3 || snake.front().Y >= ScreenHeight)
			{
				dead = true;
			}

			// Snake & Snake
			for(std::list<Vector2>::iterator it = snake.begin(); it != snake.end(); it++)
			{
				if(it != snake.begin() && it->X == snake.front().X && it->Y == snake.front().Y)
				{
					dead = true;
				}
			}

			snake.pop_back();

			//========[DISPLAY]========//

			// Clear Screen
			for(int px = 0; px < ScreenWidth * ScreenHeight; px++)
				screen[px] = L' ';

			// Draw Separator
			for(int xx = 0; xx < ScreenWidth; xx++)
			{
				wsprintf(&screen[0 * ScreenWidth + xx], L"=");
				wsprintf(&screen[2 * ScreenWidth + xx], L"=");
			}

			// Draw Score
			wsprintf(&screen[1 * ScreenWidth + (ScreenWidth / 2)], L"%d", score);

			// Body
			for(auto &segment : snake)
			{
				screen[segment.Y * ScreenWidth + segment.X] = dead ? '#' : 'O';
			}

			// Head
			screen[snake.front().Y * ScreenWidth + snake.front().X] = dead ? 'X' : '@';

			// Food
			screen[foodPos.Y * ScreenWidth + foodPos.X] = '$';

			// Draw 'Press Right-Control To Restart'
			if(dead)
				wsprintf(&screen[7 * ScreenWidth + ((ScreenWidth / 2) - 14)], L"Press Right-Ctrl to Restart");

			// Finally Display Game
			WriteConsoleOutputCharacter(console, screen, ScreenWidth * ScreenHeight, { 0, 0 }, &bytesWritten);
		}

		// Wait for Right Control Key Press
		while(GetAsyncKeyState(VK_RCONTROL) == false);
	}

	return 0;
}