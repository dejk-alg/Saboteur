#pragma once

#include "resource.h"
#include <string>
#include <iostream>
#include <sstream>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

#define CELL_WIDTH 60
#define CELL_HEIGHT 30

#define CELL_NOT_DISPLAYED 0
#define CELL_SELECTED 1
#define CELL_DISPLAYED 2

#pragma comment (lib,"Gdiplus.lib")

namespace Saboteur {
	class Cell {
	public:
		static const int width = CELL_WIDTH, height = CELL_HEIGHT;
		int leftX, upperY;
		char cont;
		char disp;

		Cell();
		Cell(int x, int y);
		Cell(int x, int y, char cont, char disp);
		void draw(HDC hdc);
		void select(HDC hdc);
		void activate(HDC hdc);
	};

	Cell::Cell()
		: leftX(0), upperY(0), cont(0), disp(0) {};

	Cell::Cell(int inX, int inY)
		: leftX(inX), upperY(inY), cont(0), disp(0) {};

	Cell::Cell(int inX, int inY, char inCont, char inDisp)
		: leftX(inX), upperY(inY), cont(inCont), disp(inDisp) {};

	void Cell::draw(HDC hdc) {

		using namespace Gdiplus;

		Graphics graphics(hdc);

		Pen borderPen(Color(255, 0, 0, 255), 2);

		SolidBrush innerBrush(Color(15, 100, 100, 0));
		SolidBrush disclosedBrush(Color(60, 100, 200, 50));

		Rect rInnerCell(this->leftX + 1, this->upperY + 1, Cell::width - 1, Cell::height - 1);
		Rect rCell(this->leftX, this->upperY, Cell::width, Cell::height);

		if (disp == CELL_DISPLAYED)
			graphics.FillRectangle(&disclosedBrush, rInnerCell);
		else
			graphics.FillRectangle(&innerBrush, rInnerCell);
		graphics.DrawRectangle(&borderPen, rCell);
	}

	void Cell::select(HDC hdc) {

		using namespace Gdiplus;

		if (disp == CELL_NOT_DISPLAYED) disp = CELL_SELECTED;

		Graphics graphics(hdc);
		Pen      selectPen(Color(255, 255, 0, 0), 2);

		Rect rCell(this->leftX, this->upperY, Cell::width, Cell::height);
		graphics.DrawRectangle(&selectPen, rCell);

	}

	void Cell::activate(HDC hdc) {
		disp = CELL_DISPLAYED;
	}

	class Field {
	public:
		int fHeight;
		int fWidth;
		Cell* fCell;

		Field();
		Field(int _fHeight, int _fWidth);
		void generate(int _fHeight, int _fWidth);
		void draw(HDC hdc);
		Cell* getCell(int x, int y);
	};

	void Field::generate(int _fHeight, int _fWidth) {
		fHeight = _fHeight;
		fWidth = _fWidth;

		fCell = new Cell[fHeight * fWidth];
		for (int x = 0; x < fWidth; x++)
			for (int y = 0; y < fHeight; y++) {
				fCell[y*fWidth + x] = { 50 + Cell::width * x, 50 + Cell::height * y, 0, 0 };
			}
	}

	Field::Field() {
		fHeight = 0;
		fWidth = 0;
		fCell = NULL;
	}

	Field::Field(int _fHeight, int _fWidth) {
		fHeight = _fHeight;
		fWidth = _fWidth;
		fCell = NULL;
	}

	void Field::draw(HDC hdc) {


		for (int x = 0; x < fWidth; x++)
			for (int y = 0; y < fHeight; y++) {
				fCell[y*fWidth + x].draw(hdc);
			}

	}

	Cell* Field::getCell(int x, int y) {
		return &fCell[x + y * fWidth];
	}

	class Sabouter_Field : public Field {
		
	};

	class Game {

	public:

		HWND gameWindow;
		Sabouter_Field field;
		int curX, curY;
		//int prevX, prevY;


		Game(): curX(0), curY(0) {}
		Game(HWND _gameWindow) : gameWindow(_gameWindow), curX(0), curY(0) {}

		void start();
		Cell* getCell(int x, int y);
		Cell* getCurrentCell();
		void drawSelected(HDC hdc);
		void draw(HDC hdc);
		void attachWindow(HWND _gameWindow);
		void createConsole();
		void processKey(int keyCode);

		void move(char dir);
		void activateCell();
		void updateField(Cell* prevCell, Cell* curCell);
	} game;

	void Game::createConsole() {
		AllocConsole();
		FILE *streamIn, *streamOut, *streamErr;
		errno_t err;
		int out, in;
		in = freopen_s(&streamIn, "CONIN$", "r", stdin);
		out = freopen_s(&streamOut, "CONOUT$", "w", stdout);
		err = freopen_s(&streamErr, "CONOUT$", "w", stderr);
	}

	void Game::attachWindow(HWND _gameWindow) {
		//attaching main game window to the "game" object
		gameWindow = _gameWindow;
	}

	Cell* Game::getCell(int x, int y) {
		return field.getCell(x, y);
	}

	Cell* Game::getCurrentCell() {
		return field.getCell(curX, curY);
	}

	void Game::drawSelected(HDC hdc) {

		Cell* curCell = getCell(curX, curY);
		RECT rCell = { curCell->leftX - 1, curCell->upperY - 1, curCell->leftX + Cell::width + 1, curCell->upperY + Cell::height + 1 };
		curCell->select(hdc);

	}

	void Game::draw(HDC hdc) {
		field.draw(hdc);
		drawSelected(hdc);
	}

	void Game::start() {

		// function run when the program starts

		curX = 0;
		curY = 0;

		int fieldHeight = 7;
		int fieldWidth = 10;

		field.generate(fieldHeight, fieldWidth);

	}

	void Game::processKey(int keyCode) {
		switch (keyCode) {

			/*
			when 'move' characters are being processed,
			first line is used for "wasd" characters,
			and second one is used for arrows
			*/

		case 0x41:
		case 0x25: move(0);
			break;
		case 0x53:
		case 0x28: move(1);
			break;
		case 0x44:
		case 0x27: move(2);
			break;
		case 0x57:
		case 0x26: move(3);
			break;

			/* 'Enter' key */
		case 0xd: activateCell();
			break;

		default: break;
		}

	}

	void Game::activateCell() {
		if (getCell(curX, curY)->disp == CELL_DISPLAYED) return;

		getCell(curX, curY)->disp = CELL_DISPLAYED;

		Cell* curCell = getCell(curX, curY);
		RECT curRect = { curCell->leftX - 1, curCell->upperY - 1, curCell->leftX + Cell::width + 1, curCell->upperY + Cell::height + 1 };
		ValidateRect(gameWindow, NULL);
		InvalidateRect(gameWindow, &curRect, TRUE);
	}

	void Game::move(char dir) {

		/*
		0 - move left
		1 - move down
		2 - move right
		3 - move up
		*/
		int prevX = curX;
		int prevY = curY;

		Cell* prevCell = getCurrentCell();

		switch (dir) {

		case 0: if (curX != 0) curX--;
			break;

		case 1: if (curY != field.fHeight - 1) curY++;
			break;

		case 2: if (curX != field.fWidth - 1) curX++;
			break;

		case 3: if (curY != 0) curY--;
			break;

		}

		if (curX == prevX && curY == prevY) return;

		prevCell->disp = CELL_NOT_DISPLAYED;
		getCurrentCell()->disp = CELL_SELECTED;


		Cell* curCell = field.getCell(curX, curY);

		updateField(prevCell, curCell);
	}

	void Game::updateField(Cell* prevCell, Cell* curCell) {
		RECT curRect;
		curRect.left = (prevCell->leftX < curCell->leftX) ? prevCell->leftX : curCell->leftX;
		curRect.left--;

		curRect.top = (prevCell->upperY < curCell->upperY) ? prevCell->upperY : curCell->upperY;
		curRect.top--;

		curRect.right = (prevCell->leftX > curCell->leftX) ? prevCell->leftX : curCell->leftX;
		curRect.right += Cell::width + 1;

		curRect.bottom = (prevCell->upperY > curCell->upperY) ? prevCell->upperY : curCell->upperY;
		curRect.bottom += Cell::height + 1;

		ValidateRect(gameWindow, NULL);
		InvalidateRect(gameWindow, &curRect, TRUE);
	
	}
}