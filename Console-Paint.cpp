#include <stdio.h>
#include <windows.h>
#include <string.h>

#define P_COLOR1 ' '
#define P_COLOR2 (char)176
#define P_COLOR3 (char)177
#define P_COLOR4 (char)178
#define P_COLOR5 (char)219

typedef struct FLOAT_XY
{
	float x, y;
};

class SCREEN
{
private:
	char* buffer;
	int width, height;
	FLOAT_XY mouse_pos, cell_size;
	HWND hwnd = nullptr;
	HANDLE handle = nullptr;

public:
	char pen_color;

	SCREEN(int screenWidth, int screenHeight)
	{
		width = screenWidth;
		height = screenHeight;
		buffer = new char[width * height + 1];
		buffer[width * height] = '\0';

		char command[30] = "mode con cols=", num[4];

		_itoa_s(width, num, 10);
		strcat_s(command, num);
		strcat_s(command, " lines=");
		_itoa_s(height+3, num, 10);
		strcat_s(command, num);
		system(command);
		system("title Console Paint");
		system("color 0F");

		// Gets handle to the console window
		hwnd = GetConsoleWindow();

		// Gets handle to the specified standard device
		handle = GetStdHandle(STD_OUTPUT_HANDLE);

		RECT rect;

		//Gets rectangle of coordinates of the console window
		GetClientRect(hwnd, &rect);

		cell_size = { 
			(rect.right - rect.left) / (float)width,
			(rect.bottom - rect.top) / (float)(height + 3)
		};

		pen_color = P_COLOR4;
	}

	~SCREEN()
	{
		delete[] buffer;
	}

	void clear()
	{
		// Fills the first N chars of buffer
		memset(buffer, P_COLOR1, width*height);
	}

	void print()
	{
		/*
		The same as system("cls"), but faster
		Set the console cursor to the { 0, 0 } position in characters
		*/
		SetConsoleCursorPosition(handle, { 0, 0 });
		
		// Print the screen
		printf("%s\n\nPen color: 1 - [%c], 2 - [%c], 3 - [%c], 4 - [%c], 5 - [%c],   Clear: [C]\nSelect color:  [%c]   ", buffer, P_COLOR1, P_COLOR2, P_COLOR3, P_COLOR4, P_COLOR5, pen_color);
	}

	FLOAT_XY get_mouse_pos()
	{
		POINT t_mouse_pos;

		// Gets the coordinates of a point in the screen
		GetCursorPos(&t_mouse_pos);

		/*
		Converts coordinates of a point in the screen to the coordinates in client-area
		hwnd - handle to the client-area window
		*/
		ScreenToClient(hwnd, &t_mouse_pos);

		return { 
			t_mouse_pos.x / cell_size.x,
			t_mouse_pos.y / cell_size.y
		};
	}

	void set_cell(FLOAT_XY cell_pos)
	{
		if (cell_pos.x >= 0 && cell_pos.y >= 0 && cell_pos.x < width && cell_pos.y < height)
		{
			buffer[(int)(cell_pos.y) * width + (int)(cell_pos.x)] = pen_color;
		}
	}
};

int main()
{
	SCREEN scr(80, 30);

	scr.clear();
	
	while (true)
	{
		if (GetKeyState(VK_LBUTTON) < 0) scr.set_cell(scr.get_mouse_pos());
		else if (GetKeyState('1') < 0) scr.pen_color = P_COLOR1;
		else if (GetKeyState('2') < 0) scr.pen_color = P_COLOR2;
		else if (GetKeyState('3') < 0) scr.pen_color = P_COLOR3;
		else if (GetKeyState('4') < 0) scr.pen_color = P_COLOR4;
		else if (GetKeyState('5') < 0) scr.pen_color = P_COLOR5;
		else if (GetKeyState('C') < 0) scr.clear();
		else if (GetKeyState(VK_ESCAPE) < 0) return 0;

		scr.print();
	}
}