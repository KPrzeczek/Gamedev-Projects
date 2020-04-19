#include <iostream>
#include <thread>

#include <Windows.h>

int ScreenWidth = 120;
int ScreenHeight = 40;

struct Vector2
{
	int X;
	int Y;

	Vector2 operator+ (Vector2 const &other)
	{
		return { this->X + other.X, this->Y + other.Y };
	}
};

struct PaddleInput
{
	int UpKey;
	int DownKey;
};

class Ball
{
public:
	Ball() = default;
	~Ball() = default;

	void Update(bool hittingPaddle)
	{
		if(Position.X <= 0 || Position.X >= ScreenWidth - 2) m_velocity.X *= -1;
		if(Position.Y <= 0 || Position.Y >= ScreenHeight - 1) m_velocity.Y *= -1;

		if(hittingPaddle) m_velocity.X *= -1;

		Position = Position + m_velocity;
	}

	Vector2 Position { ScreenWidth / 2, ScreenHeight / 2 };

private:
	Vector2 m_velocity { 2, 1 };

};

class Paddle
{
public:
	Paddle() = default;
	~Paddle() = default;

	void Update()
	{
		if(GetAsyncKeyState(Input.UpKey)) {
			m_velocity.Y = -1;
		}
		else if(GetAsyncKeyState(Input.DownKey)) {
			m_velocity.Y = 1;
		}
		else {
			m_velocity.Y = 0;
		}

		Position = Position + m_velocity;
	}

	PaddleInput Input;
	Vector2 Position { 8, ScreenHeight / 2 };

private:
	Vector2 m_velocity { 0, 0 };

};

int main()
{
	wchar_t *screen = new wchar_t[ScreenWidth * ScreenHeight]; // <- Possible byte overflow (* on 4 bytes results in 8 bytes)
	for(int px = 0; px < ScreenWidth * ScreenHeight; px++)
		screen[px] = L' ';
	HANDLE console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(console);
	DWORD bytesWritten = 0;

	Ball ball;

	Paddle leftPaddle;
	Paddle rightPaddle;

	leftPaddle.Input.UpKey = VK_LSHIFT;
	leftPaddle.Input.DownKey = VK_LCONTROL;

	rightPaddle.Input.UpKey = VK_UP;
	rightPaddle.Input.DownKey = VK_DOWN;
	rightPaddle.Position = { ScreenWidth - 8, ScreenHeight / 2 };

	while(true)
	{
		// Timing
		{
			using namespace std;
			this_thread::sleep_for(100ms);
		}

		// Logic
		leftPaddle.Update();
		rightPaddle.Update();

		// Display
		for(int px = 0; px < ScreenWidth * ScreenHeight; px++) {
			screen[px] = L' ';
		}

		for(int yy = -3; yy < 3; yy++) {
			for(int xx = -1; xx < 1; xx++) {
				screen[(leftPaddle.Position.Y + yy) * ScreenWidth + (leftPaddle.Position.X + xx)] = L'#';
				screen[(rightPaddle.Position.Y + yy) * ScreenWidth + (rightPaddle.Position.X + xx)] = L'#';
			}
		}

		bool col = (screen[ball.Position.Y * ScreenWidth + ball.Position.X] == L'#');
		ball.Update(col);

		screen[ball.Position.Y * ScreenWidth + ball.Position.X] = L'@';

		WriteConsoleOutputCharacter(console, screen, ScreenWidth * ScreenHeight, { 0, 0 }, &bytesWritten);
	}

	return 0;
}