#ifndef UNICODE
#define UNICODE
#endif // !UNICODE

#include <Windows.h>

#include <string>
#include <chrono>
#include <thread>

namespace MATH
{
	float ToRadians(float degree_) { return degree_ * acosf(-1.0f) / 180.0f; }

	struct POINT
	{
		float x = 0.0f;
		float y = 0.0f;

		POINT(float x_, float y_) : x(x_), y(y_) {}
	};
}

struct WINDOW_DATA
{
	std::wstring engineProjectName;
	HINSTANCE hInstance;
	int nCmdShow;
	size_t width = 0;
	size_t height = 0;
	size_t* screenBuffer;

	HWND hwnd;
	HDC device;

	double elapsedTime = 0.0;
	double fps = 60.0;
	std::chrono::steady_clock::time_point timePointOne = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point timePointTwo = std::chrono::steady_clock::now();
	std::chrono::duration<double, std::milli> elapse;
	std::chrono::duration<double, std::milli> spf = (std::chrono::duration<double, std::milli>)(1000.0 / fps);

	bool keyBuffer[256] = { false };
	bool KeyPress(size_t key_code_) { return keyBuffer[key_code_]; }

	WINDOW_DATA(std::wstring projectName_, size_t width_, size_t height_)
	{
		engineProjectName = projectName_;
		width = width_;
		height = height_;
	}

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM w_param_, LPARAM l_param_)
	{
		switch (uMsg)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			break;
		}

		return DefWindowProcW(hwnd, uMsg, w_param_, l_param_);
	}

	ATOM RegisterNewClass()
	{
		WNDCLASSEX wc = { };

		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = (WNDPROC)WindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hIcon = NULL;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = L"Class Name";
		wc.hIconSm = NULL;

		return RegisterClassEx(&wc);
	}

	BOOL CreateNewWindow()
	{
		RECT screenRect = { 100, 100, width + 100 , height + 100 };
		if (!AdjustWindowRectEx(&screenRect, WS_CAPTION | WS_VISIBLE, false, WS_EX_LEFT)) return false;

		if ((hwnd = CreateWindowEx(
			0,
			L"Class Name",
			engineProjectName.c_str(),
			WS_OVERLAPPEDWINDOW,
			screenRect.left,
			screenRect.top,
			screenRect.right - screenRect.left,
			screenRect.bottom - screenRect.top,
			NULL,
			NULL,
			hInstance,
			NULL
		)) == NULL) return false;

		ShowWindow(hwnd, nCmdShow);
		UpdateWindow(hwnd);

		if ((device = GetDC(hwnd)) == NULL) return false;

		return true;
	}

	void UpdateElapsedTime()
	{
		std::this_thread::sleep_until(timePointOne + spf);
		timePointTwo = std::chrono::steady_clock::now();
		elapse = timePointTwo - timePointOne;
		elapsedTime = elapse.count() / 1000.0;
		timePointOne = std::chrono::steady_clock::now();
	}

	void CleerScreen(size_t color_) { for (size_t i = 0; i < width * height; i++) screenBuffer[i] = color_; }
	void DrawLine(size_t color_, size_t x_, size_t y_one_, size_t y_two_) { for (size_t i = y_one_; i < y_two_ + 1u; i++) screenBuffer[i * width + x_] = color_; }
	void DrawScreen()
	{
		HBITMAP image = CreateBitmap(width, height, 1, 8 * 4, (void*)screenBuffer);

		HDC hdcImage = CreateCompatibleDC(device);

		SelectObject(hdcImage, image);

		BitBlt(device, 0, 0, width, height, hdcImage, 0, 0, SRCCOPY);

		DeleteObject((HBITMAP)image);
		DeleteDC(hdcImage);
	}

	template<class T>
	int Start(HINSTANCE hInstance_, int nCmdShow_, T app_loop_)
	{
		hInstance = hInstance_;
		nCmdShow = nCmdShow_;

		MSG msg = { };

		RegisterNewClass();
		if (!CreateNewWindow()) return 0;

		screenBuffer = (size_t*)calloc(width * height, sizeof(size_t));

		while (true)
		{
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				if (msg.message == WM_KEYDOWN) keyBuffer[msg.wParam] = true;
				if (msg.message == WM_KEYUP) keyBuffer[msg.wParam] = false;
				if (msg.message == WM_QUIT) goto loopEnded;
			}

			app_loop_();

			UpdateElapsedTime();
		}
	loopEnded:

		free(screenBuffer);
		ReleaseDC(hwnd, device);

		return msg.wParam;
	}
};

struct PLAYER_DATA
{
	MATH::POINT position = { 0, 0 };
	float lookAt = 0;
	float fieldOfView = 0;
	float screenHalfToDistanceToScreen = 0;
	float distanceToScreen = 0;
	float rotateVelocity = 0;
	float moveVelocity = 0;

	PLAYER_DATA(MATH::POINT position_, float look_at_, float field_of_view_, float rotate_velocity_, float move_velocity_, size_t window_width_)
	{
		position = position_;
		lookAt = look_at_;
		fieldOfView = field_of_view_;
		screenHalfToDistanceToScreen = sin(MATH::ToRadians(field_of_view_ / 2)) / cos(MATH::ToRadians(field_of_view_ / 2));
		distanceToScreen = window_width_ / 2.0 / screenHalfToDistanceToScreen;
		rotateVelocity = rotate_velocity_;
		moveVelocity = move_velocity_;
	}
};

struct GAME_DATA
{
	std::string map = "";
	size_t mapWidth = 0u;

	GAME_DATA(std::string map_, size_t map_width_) : map(map_), mapWidth(map_width_) {}

	size_t GetMapData(size_t x_, size_t y_) { return map[y_ * mapWidth + x_] - '0'; }
};

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	WINDOW_DATA windowData(L"Ray Caster", 400u, 300u);
	PLAYER_DATA playerData({ 5.0f, 5.0f }, 0.0f, 60.0f, 100.0f, 5.0f, windowData.width);
	GAME_DATA gameData("1111111111100000000110100001011000000001100000000110000000011000000001101000010110000000011111111111", 10u);

	auto Raycaster = [&]()
	{
		windowData.CleerScreen(0xBDBDBDu);

		float playerLookAtX = cos(MATH::ToRadians(playerData.lookAt));
		float playerLookAtY = sin(MATH::ToRadians(playerData.lookAt));
		float playerPlaneX = playerLookAtY * playerData.screenHalfToDistanceToScreen;
		float playerPlaneY = -playerLookAtX * playerData.screenHalfToDistanceToScreen;
		MATH::POINT mapPositionOffset = { fmodf(playerData.position.x, 1.0f), fmodf(playerData.position.y, 1.0f) };
		MATH::POINT mapPosition = { (float)(int)playerData.position.x, (float)(int)playerData.position.y };

		for (size_t i = 0; i < windowData.width; i++)
		{
			float cameraX = 2.0f * i / windowData.width - 1;

			MATH::POINT rayDirection = { playerLookAtX + playerPlaneX * cameraX, playerLookAtY + playerPlaneY * cameraX };
			if (rayDirection.x == 0.0f) rayDirection.x = 0.000001f;
			if (rayDirection.y == 0.0f) rayDirection.y = 0.000001f;

			MATH::POINT playerMapPositionForDDA = { mapPosition.x, mapPosition.y };
			MATH::POINT step = { 0.0f, 0.0f };
			MATH::POINT deltaDistance = { abs(1.0f / rayDirection.x), abs(1.0f / rayDirection.y) };
			MATH::POINT sideDistance = { 0.0f, 0.0f };
			float distanceToWall = 0.0f;
			bool wallSideX = true;

			if (rayDirection.x < 0)
			{
				step.x = -1.0f;
				sideDistance.x = mapPositionOffset.x * deltaDistance.x;
			}
			else
			{
				step.x = 1.0f;
				sideDistance.x = (1.0f - mapPositionOffset.x) * deltaDistance.x;
			}
			if (rayDirection.y < 0)
			{
				step.y = -1.0f;
				sideDistance.y = mapPositionOffset.y * deltaDistance.y;
			}
			else
			{
				step.y = 1.0f;
				sideDistance.y = (1.0f - mapPositionOffset.y) * deltaDistance.y;
			}

			while (true)
			{
				if (sideDistance.x < sideDistance.y)
				{
					sideDistance.x += deltaDistance.x;
					playerMapPositionForDDA.x += step.x;
					wallSideX = true;
				}
				else
				{
					sideDistance.y += deltaDistance.y;
					playerMapPositionForDDA.y += step.y;
					wallSideX = false;
				}

				if (gameData.GetMapData(playerMapPositionForDDA.x, playerMapPositionForDDA.y) == 1u) break;
			}

			if (wallSideX) distanceToWall = sideDistance.x - deltaDistance.x;
			else distanceToWall = sideDistance.y - deltaDistance.y;

			float wallHeight = 1.0f / distanceToWall * playerData.distanceToScreen;

			int ceiling = (int)((float)windowData.height / 2.0f - wallHeight / 2.0f);
			if (ceiling < 0) ceiling = 0;
			int floor = (int)((float)windowData.height / 2.0f + wallHeight / 2.0f);
			if (floor >= (int)windowData.height) floor = (int)windowData.height - 1;

			windowData.DrawLine(0x0000FFu, i, (size_t)ceiling, (size_t)floor);
		}

		float playerRotateVelocity = playerData.rotateVelocity * (float)windowData.elapsedTime;
		float playerMoveVelocity = playerData.moveVelocity * (float)windowData.elapsedTime;
		if (windowData.KeyPress(37))
		{
			playerData.lookAt = playerData.lookAt + playerRotateVelocity;
			if (playerData.lookAt >= 360.0f) playerData.lookAt - 360.0f;
		}
		if (windowData.KeyPress(39))
		{
			playerData.lookAt = playerData.lookAt - playerRotateVelocity;
			if (playerData.lookAt < 0.0f) playerData.lookAt + 360.0f;
		}
		if (windowData.KeyPress(38))
		{
			float newPositionX = playerData.position.x + playerLookAtX * playerMoveVelocity;
			float newPositionY = playerData.position.y + playerLookAtY * playerMoveVelocity;
			if ((size_t)newPositionX >= 0u && (size_t)newPositionX < gameData.mapWidth && (size_t)newPositionY >= 0u && (size_t)newPositionY < gameData.mapWidth)
			{
				if (gameData.GetMapData(newPositionX, newPositionY) == 0u)
				{
					playerData.position.x = newPositionX;
					playerData.position.y = newPositionY;
				}
			}
		}
		if (windowData.KeyPress(40))
		{
			float newPositionX = playerData.position.x - playerLookAtX * playerMoveVelocity;
			float newPositionY = playerData.position.y - playerLookAtY * playerMoveVelocity;
			if ((size_t)newPositionX >= 0u && (size_t)newPositionX < gameData.mapWidth && (size_t)newPositionY >= 0u && (size_t)newPositionY < gameData.mapWidth)
			{
				if (gameData.GetMapData(newPositionX, newPositionY) == 0u)
				{
					playerData.position.x = newPositionX;
					playerData.position.y = newPositionY;
				}
			}
		}

		windowData.DrawScreen();
	};

	return windowData.Start(hInstance, nCmdShow, Raycaster);
}
