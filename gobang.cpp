#undef UNICODE
#undef _UNICODE


#include <string>
#include <Windows.h>
#include <graphics.h>
#include <mmsystem.h>
#include <chrono>
#include <thread>
#include <lua.hpp>
#include "GLOBAL_VARS.h"

#pragma comment(lib, "winmm.lib")

bool PLAYED{ false };


class GoBang {

private:
	IMAGE IMG;

	int coord[2]{},
		  board[16][16];	// -1为无，0为白

	int step{ 0 };	// 走棋数

	int side{ 1 };		// 下棋方（1为黑色）
	int overflag{ 0 };	// 结束标志

	// 对局日志
	struct chess {
		int color{ 0 };
		int coord[2]{};
		void initChess() { color = 0; coord[0] = 0; coord[1] = 0; }
	} record[225];

public:
	MOUSEMSG mevent;
	int chessmode{ 0 };	// 对局模式（1为人人，2为人机）

	// 构造函数指定背景图片
	GoBang() { std::fill(&this->board[0][0], &this->board[15][15], -1); };

	// 点击事件
	void onClick();
	void overClick();

	// 显示菜单及侧边栏
	void showMenu();
	void showTools();

	// 游戏模式
	void playWithHuman();
	void playWithRobot();

	// 初始化棋盘
	void initLine();
	void initPoint();
	void initBackground();

	// 下棋并绘制
	int drawChessR();
	int drawChessP(int, int);
	int drawBack(int step, bool delay=false);	// 悔棋

	// 回放
	int playback(int);
	// 重新开始
	void playAgain();
	// 检查是否结束
	int checkOver(int x, int y);
	// 估值函数
	void evaluate(int, int, int&, int&, int&, int&, int m, int n, bool first=true, bool attack=false);
	int score(int, int);
	// 检测是否在范围内
	int judgeRegion(int, int, int x1, int y1, int x2, int y2);
	// 禁手检测
	// bool checkForbiddenMoves(int x, int y);
	// bool checkPattern1(int x, int y);
};

void setWindow();
void readConfig(bool init=false);

// 1.音乐文件改变	2.音量改变
void playMusic(bool change1 = false, bool change2 = false);
void playEffect(int mode = 1, bool init = false);

int main() {
	if (!fs::exists(fileData) || !fs::is_regular_file(fileData)) {
		inUse = false;
	}
	else {
		lastTime = fs::last_write_time(fileData);
	}

	readConfig(true);
	playEffect(1, true);
	auto program = GoBang();

	setWindow();
	cleardevice();

	program.showMenu();
	
	std::string musicFile{ MUSIC };
	std::string command1 = "open " + musicFile + " alias " + musicFile;
	std::string command2 = "set bkmusic volume to " + std::to_string(MUSIC_VOLUME);
	mciSendString(command1.c_str(), nullptr, 0, nullptr);
	mciSendString(command2.c_str(), nullptr, 0, nullptr);
	mciSendString("", nullptr, 0, nullptr);
	mciSendString("play bkmusic repeat", nullptr, 0, nullptr);
	
	while (true)
	{
		program.chessmode = 0;
		program.mevent = GetMouseMsg();

		if (program.mevent.uMsg == WM_LBUTTONDOWN)
		{
			if ((program.mevent.x > 100 && program.mevent.x < 400) &&
				(program.mevent.y > 150 && program.mevent.y < 250))
				program.chessmode = 1;
			else if ((program.mevent.x > 100 && program.mevent.x < 400) &&
				(program.mevent.y > 350 && program.mevent.y < 450))
				program.chessmode = 2;
		}

		switch (program.chessmode)
		{
		case 1: program.playWithHuman(); break;

		case 2: program.playWithRobot(); break;
		}

	}

	return 0;

}

void playMusic(bool change1, bool change2) {
	static std::string command2 = "setaudio bkmusic volume to " + std::to_string(MUSIC_VOLUME);
	
	if (change1) {
		command2 = "setaudio bkmusic volume to " + std::to_string(MUSIC_VOLUME);
		mciSendString("stop bkmusic", nullptr, 0, nullptr);
		mciSendString("close bkmusic", nullptr, 0, nullptr);
	}
	else if (change2) {
		command2 = "setaudio bkmusic volume to " + std::to_string(MUSIC_VOLUME);
		mciSendString(command2.c_str(), nullptr, 0, nullptr);
		return;
	}

	std::string musicFile{ MUSIC };
	std::string command1 = "open " + musicFile + " alias bkmusic";

	mciSendString(command1.c_str(), nullptr, 0, nullptr);
	mciSendString(command2.c_str(), nullptr, 0, nullptr);
	mciSendString("play bkmusic repeat", nullptr, 0, nullptr);
}

void playEffect(int mode, bool init) {
	if (init) {
		auto command = "open " + EFFECT1 + " alias effect1";
		mciSendString(command.c_str(), nullptr, 0, nullptr);
		
		command = "open " + EFFECT2 + " alias effect2";
		mciSendString(command.c_str(), nullptr, 0, nullptr);
		
		command = "open " + EFFECT3 + " alias effect3";
		mciSendString(command.c_str(), nullptr, 0, nullptr);
		
		command = "setaudio effect1 volume to " + std::to_string(EFFECT_VOLUME);
		mciSendString(command.c_str(), nullptr, 0, nullptr);
		
		command = "setaudio effect2 volume to " + std::to_string(EFFECT_VOLUME);
		mciSendString(command.c_str(), nullptr, 0, nullptr);
		
		command = "setaudio effect3 volume to " + std::to_string(EFFECT_VOLUME);
		mciSendString(command.c_str(), nullptr, 0, nullptr);

		return;
	}
	
	std::string effect;
	switch (mode)
	{
	case 1:
		effect = "effect1";
		break;

	case 2:
		effect = "effect2";
		break;
	case 3:
		effect = "effect3";
		break;
	default:
		return;
	}
	/*
	if (effect != "effect1") {
		std::string command = "status " + effect + " mode";
		char response[32];
		mciSendStringA(command.c_str(), response, sizeof(response), NULL);
		if (strcmp(response, "playing") == 0) { return; }
	}
	*/

	std::string command = "play " + effect + " from 0";
	mciSendString(command.c_str(), nullptr, 0, nullptr);
}

void readConfig(bool init) {
	if (!inUse) {
		return;
	}
	
	auto currentTime{ fs::last_write_time(fileData)};

	if (init || lastTime != currentTime) {
		lastTime = currentTime;
	}	
	else {
		return;
	}

	
	// 初始化配置文件
	lua_State* L{ luaL_newstate() };
	luaL_openlibs(L);

	if (luaL_dofile(L, ".\\config.lua") && luaL_dofile(L, ".\\defaults.lua")) {
		// 打开失败则打开默认文件
		return;
	}

	lua_getglobal(L, "EFFECT1");
	lua_getglobal(L, "EFFECT2");
	lua_getglobal(L, "EFFECT3");

	lua_getglobal(L, "PLAYBACK_SPEED");

	lua_getglobal(L, "FONT_COLOR1");
	lua_getglobal(L, "FONT_COLOR2");
	lua_getglobal(L, "FONT_COLOR3");

	lua_getglobal(L, "FONT1");
	lua_getglobal(L, "FONT2");
	lua_getglobal(L, "FONT3");

	lua_getglobal(L, "WIDTH");
	lua_getglobal(L, "HEIGHT");
	
	lua_getglobal(L, "MUSIC");
	
	lua_getglobal(L, "MUSIC_VOLUME");
	lua_getglobal(L, "EFFECT_VOLUME");
	
	lua_getglobal(L, "BACKGROUND_IMAGE1");
	lua_getglobal(L, "BACKGROUND_IMAGE2");

	lua_getglobal(L, "FIVE_COLOR");
	lua_getglobal(L, "LINE_COLOR");
	lua_getglobal(L, "BLACK_COLOR");
	lua_getglobal(L, "WHITE_COLOR");

	lua_getglobal(L, "DISPLAY");

	// 读取display
	if (lua_istable(L, -1)) {
		for (int i = 1; i < 3; i++) {
			lua_rawgeti(L, -1, i);

			if (!lua_isnumber(L, -1)) { break; }
			DISPLAY[i++ - 1] = static_cast<int>(lua_tonumber(L, -1));
			lua_pop(L, 1);
		}

	}
	if (lua_isnumber(L, -2)) { WHITE_COLOR = static_cast<int>(lua_tonumber(L, -2)); }
	if (lua_isnumber(L, -3)) { BLACK_COLOR = static_cast<int>(lua_tonumber(L, -3)); }
	if (lua_isnumber(L, -4)) { LINE_COLOR = static_cast<int>(lua_tonumber(L, -4)); }
	if (lua_isnumber(L, -5)) { FIVE_COLOR = static_cast<int>(lua_tonumber(L, -5)); }
	
	if (lua_isstring(L, -6)) { BACKGROUND_IMAGE1 = (LPCTSTR) lua_tostring(L, -6); }
	if (lua_isstring(L, -7)) { BACKGROUND_IMAGE2 = (LPCTSTR) lua_tostring(L, -7); }
	
	if (lua_isnumber(L, -8)) { EFFECT_VOLUME = static_cast<int>(lua_tonumber(L, -8)); }
	if (lua_isnumber(L, -9)) { MUSIC_VOLUME = static_cast<int>(lua_tonumber(L, -9)); }
	if (lua_isstring(L, -10)) { MUSIC = lua_tostring(L, -10); }
	
	if (lua_isnumber(L, -11)) { WIDTH = static_cast<int>(lua_tonumber(L, -11)); }
	if (lua_isnumber(L, -12)) { HEIGHT= static_cast<int>(lua_tonumber(L, -12)); }
	
	if (lua_isstring(L, -13)) { FONT3 = lua_tostring(L, -13); }
	if (lua_isstring(L, -14)) { FONT2 = lua_tostring(L, -14); }
	if (lua_isstring(L, -15)) { FONT1 = lua_tostring(L, -15); }

	if (lua_isnumber(L, -16)) { FONT_COLOR3 = static_cast<int>(lua_tonumber(L, -16)); }
	if (lua_isnumber(L, -17)) { FONT_COLOR2 = static_cast<int>(lua_tonumber(L, -17)); }
	if (lua_isnumber(L, -18)) { FONT_COLOR1 = static_cast<int>(lua_tonumber(L, -18)); }
	if (lua_isnumber(L, -19)) { PLAYBACK_SPEED = static_cast<int>(lua_tonumber(L, -19)); }

	if (lua_isstring(L, -20)) { EFFECT3 = lua_tostring(L, -20); }
	if (lua_isstring(L, -21)) { EFFECT2 = lua_tostring(L, -21); }
	if (lua_isstring(L, -22)) { EFFECT1 = lua_tostring(L, -22); }

	if (prevMusic != MUSIC) {
		prevMusic = MUSIC;
		playMusic(true);
	}
	else if (prevVolume1 != MUSIC_VOLUME) {
		prevVolume1 = MUSIC_VOLUME;
		playMusic(false, true);
	}

	lua_pop(L, 22);
	lua_close(L);

	std::this_thread::sleep_for(std::chrono::seconds(1));
}

void setWindow() {

	int posX;
	int posY;

	// 获取屏幕尺寸
	int screenWidth = GetSystemMetrics(SM_CXSCREEN); // 获取屏幕宽度
	int screenHeight = GetSystemMetrics(SM_CYSCREEN); // 获取屏幕高度


	int windowWidth{ WIDTH };
	int windowHeight{ HEIGHT };

	
	if (DISPLAY[0] < 0 || DISPLAY[1] < 0) {
		// 计算窗口在屏幕中心的位置
		posX = (screenWidth - windowWidth) / 2;
		posY = (screenHeight - windowHeight) / 2;
	}
	else {
		posX = DISPLAY[0];
		posY = DISPLAY[1];
	}

	// 初始化图形窗口，设置窗口大小为 windowWidth x windowHeight，并将窗口放置在屏幕中心
	initgraph(windowWidth, windowHeight);
	HWND hwnd = GetHWnd(); // 获取窗口句柄
	SetWindowPos(hwnd, HWND_TOP, posX, posY, 0, 0, SWP_NOSIZE); // 设置窗口位置为屏幕中心
}

void GoBang::onClick() {
	if (this->mevent.uMsg != WM_LBUTTONDOWN ||
		this->overflag != 0) {
		return;
	}
	readConfig();

	auto x{ this->mevent.x },
		 y{ this->mevent.y };

	// 获取坐标值
	this->coord[0] = x / 40 + 1;
	this->coord[1] = y / 40 + 1;
	// 点击棋盘
	if (drawChessP(this->coord[0], this->coord[1])) { this->step++; }
	// 点击提示
	else if (judgeRegion(x, y, 620, 100, 800, 200) == 1) {
		drawChessR();
		this->step++;
	}
	// 悔棋
	else if (judgeRegion(x, y, 620, 200, 800, 300) == 1 && this->step > 0) {
		drawBack(this->step--);
		if (this->chessmode == 2) { drawBack(this->step--); }
	}
	// 重开
	else if (judgeRegion(x, y, 620, 300, 800, 400) == 1) { playAgain(); }
	// 返回菜单
	else if (judgeRegion(x, y, 620, 400, 800, 500) == 1) {
		for (int i = 0; i < 225; i++) { this->record[i].initChess(); }
		std::fill(&this->board[0][0], &this->board[15][15], -1);
		cleardevice();
		showMenu();
		this->chessmode = 0;
	}
	// 回放
	else if (judgeRegion(x, y, 620, 500, 800, 600) == 1) { playback(this->step); }

	return;

}

void GoBang::overClick() {
	if (!PLAYED) {
		if (this->chessmode == 2 && this->side == 1) { playEffect(3); }
		else { playEffect(2); }
		PLAYED = true;
	}


	settextcolor(FONT_COLOR3);
	settextstyle(80, 0, _T(FONT3.c_str()));
	
	outtextxy(150, 150, _T("游戏结束"));
	if (this->side == 1) { outtextxy(150, 350, _T("白子获胜")); }
	else { outtextxy(150, 350, _T("黑子获胜")); }

	if (this->mevent.uMsg != WM_LBUTTONDOWN) { return; }
	

	auto x{ this->mevent.x },
		 y{ this->mevent.y };

	// 重开
	if (judgeRegion(x, y, 620, 300, 800, 400) == 1) { playAgain(); PLAYED = false; }
	// 返回菜单
	else if (judgeRegion(x, y, 620, 400, 800, 500) == 1) {
		PLAYED = false;
		for (int i = 0; i < 225; i++) { this->record[i].initChess(); }
		std::fill(&this->board[0][0], &this->board[15][15], -1);
		cleardevice();
		showMenu();
		this->chessmode = 0;
	}
	// 回放
	else if (judgeRegion(x, y, 620, 500, 800, 600) == 1) { playback(this->step); }

	return;
}

void GoBang::showMenu() {
	loadimage(&this->IMG, BACKGROUND_IMAGE1.c_str());
	putimage(0, 0, 800, 630, &this->IMG, 0, 0);
	
	settextcolor(FONT_COLOR1);
	settextstyle(50, 0, _T(FONT1.c_str()));
	setbkmode(TRANSPARENT);

	outtextxy(100, 150, _T("人人对战"));
	outtextxy(100, 350, _T("人机对战"));

	return;

}

void GoBang::showTools() {
	settextcolor(FONT_COLOR2);
	settextstyle(30, 15, _T(FONT2.c_str()));
	setbkmode(TRANSPARENT);

	outtextxy(630, 100, _T("提示"));
	outtextxy(630, 200, _T("悔棋"));
	outtextxy(630, 300, _T("重新开始"));
	outtextxy(630, 500, _T("回放"));
	outtextxy(630, 400, _T("返回菜单"));
}

void GoBang::playWithHuman() {
	this->side = 1;
	this->step = 0;
	this->overflag = 0;

	setWindow();
	initBackground();

	while (true) {
		showTools();
		
		this->mevent = GetMouseMsg();
		onClick();

		this->overflag = checkOver(this->coord[0], this->coord[1]);
		if (this->overflag == 1) { overClick(); }
		if (this->chessmode == 0) { break; }
	}
}

void GoBang::playWithRobot() {
	this->side = 1;
	this->step = 0;
	this->overflag = 0;

	setWindow();
	initBackground();

	while (true) {
		showTools();

		this->mevent = GetMouseMsg();

		if (this->side == 0 && this->overflag == 0) {
			drawChessR();
			this->step++;
		}
		onClick();

		this->overflag = checkOver(this->coord[0], this->coord[1]);
		if (this->overflag == 1) { overClick(); }
		if (this->chessmode == 0) { break; }
	}
}

void GoBang::initPoint() {
	setfillcolor(FIVE_COLOR);

	// 五个点
	fillcircle(4 * 40 - 20, 4 * 40 - 20, 3);
	fillcircle(4 * 40 - 20, 12 * 40 - 20, 3);
	fillcircle(8 * 40 - 20, 8 * 40 - 20, 3);
	fillcircle(12 * 40 - 20, 4 * 40 - 20, 3);
	fillcircle(12 * 40 - 20, 12 * 40 - 20, 3);
}

void GoBang::initLine() {
	setlinecolor(LINE_COLOR);
	for (int x = 20; x < 600; x += 40) { line(x, 20, x, 580); }
	for (int y = 20; y < 600; y += 40) { line(20, y, 580, y); }
}

void GoBang::initBackground() {
	loadimage(&this->IMG, BACKGROUND_IMAGE2.c_str());
	putimage(0, 0, 800, 630, &this->IMG, 0, 0);

	initLine();
	initPoint();
}

int GoBang::drawChessR() {
	int sc{ 0 },
		x{ 0 },
		y{ 0 };

	for (int i = 1; i < 16; i++) {
		for (int j = 1; j < 16; j++) {
			int tmp = score(i, j);
			if (this->board[i][j] == -1 && tmp > sc) {
				sc = tmp;
				x = i; 
				y = j;
			}
		}
	}

	if (this->step <= 0) { x = 7, y = 7; }
	// x++; y++;
	
	auto color = this->side == 1 ? BLACK_COLOR : WHITE_COLOR;
	setfillcolor(color);
	
	this->board[x][y] = this->side;
	this->record[this->step].color = this->side;
	this->side = this->side ?  0 : 1;

	this->record[this->step].coord[0] = x;
	this->record[this->step].coord[1] = y;
	this->coord[0] = x;
	this->coord[1] = y;

	playEffect();
	fillcircle(x * 40 - 20, y * 40 - 20, 17);

	return 1;
}

int GoBang::drawChessP(int x, int y) {
	int color = this->side == 1 ? BLACK_COLOR : WHITE_COLOR;
	setfillcolor(color);

	if (this->board[x][y] != -1 || judgeRegion(x, y, 1, 1, 16, 16) != 1) {
		return 0;
	}

	playEffect();
	fillcircle(x * 40 - 20, y * 40 - 20, 17);

	this->board[x][y] = this->side;
	this->record[this->step].color = this->side;
	this->record[this->step].coord[0] = x;
	this->record[this->step].coord[1] = y;
	this->side = this->side ? 0 : 1;

	return 1;
}

int GoBang::drawBack(int i, bool delay) {
	cleardevice();
	initBackground();

	--i;
	for (int j = 0; j < i; j++) {
		auto color = this->record[j].color == 1 ? BLACK_COLOR : WHITE_COLOR;
		setfillcolor(color);
		fillcircle(this->record[j].coord[0] * 40 - 20, 
			this->record[j].coord[1] * 40 - 20, 17);
		if (delay) { Sleep(PLAYBACK_SPEED); }
	}

	if (delay) { return 1; }

	this->record[i].initChess();
	this->record[i].color = 0;
	this->side = this->side ?  0 : 1;

	return 1;
}

int GoBang::playback(int i) {
	if (drawBack(i + 1, true) != 1) { return 0; }
	
	flushmessage(EX_MOUSE);
	if (this->overflag == 1) {
		this->mevent.uMsg = 0x0053;
		overClick();
	}

	return 1;
}

void GoBang::playAgain() {
	this->side = 1;
	cleardevice();
	initBackground();
	for (int i = 0; i < 225; i++) { this->record[i].initChess(); }
	std::fill(&this->board[0][0], &this->board[15][15], -1);
	this->overflag = 0;
	this->step = 0;
}

int GoBang::checkOver(int x, int y) {
	int i{ 0 },
		j{ 0 },
		tmp{ 0 };

	for (i = x - 3; y >= 0 && y <= 15 && i <= x + 4; i++) {
		tmp = (
			i >= 1 && i <= 15 && 
			this->board[i][y] == this->board[i - 1][y] &&
			this->board[i][y] != -1
			) ? tmp + 1 : 0;
		if (tmp == 4) { return 1; }
	}

	for (j = y - 3, tmp = 0; x >= 0 && x <= 15 && j <= y + 4; j++) {
		tmp = (j >= 1 && j <= 15 && this->board[x][j] == this->board[x][j - 1] &&
			this->board[x][j] != -1) ? tmp + 1 : 0;
		if (tmp == 4) { return 1; }
	}

	for (i = x - 3, j = y - 3, tmp = 0; i <= x + 4 && j <= y + 4; i++, j++)
	{
		if (judgeRegion(i, j, 0, 0, 16, 16) != 1) { continue; }
		tmp = (this->board[i][j] == this->board[i - 1][j - 1] &&
			this->board[i][j] != -1) ? tmp + 1 : 0;
		if (tmp == 4) { return 1; }
	}

	for (i = x - 3, j = y + 3, tmp = 0; i <= x + 4 && j >= y - 4; i++, j--)
	{
		if (judgeRegion(i, j, 0, 0, 16, 16) != 1) { continue; }
		tmp = (this->board[i][j] == this->board[i - 1][j + 1] &&
			this->board[i][j] != -1) ? tmp + 1 : 0;
		if (tmp == 4) { return 1; }
	}

	return 0;
}

void GoBang::evaluate(int x, int y, int& score, int& chess, int& empty, int& chance, int m, int n, bool first, bool attack) {
	int r{ 0 }, l{ 0 },
		empty_chess{ 1 }, player_chess{ 0 };
	
	int side = !attack ? (this->side ? 0 : 1) : this->side;
	if (!first) { m = -m; n = -n; }

	for (int i = 1; i <= 5; i++) {
		r = x + n * i;
		l = y + m * i;

		if (this->board[r][l] == side) { player_chess++; }
		else if (judgeRegion(r, l, 1, 1, 16 ,16) != 1) { break; }
		else if (this->board[r][l] == -1) { empty_chess++; break; }
		else { break; }
	}

	switch (player_chess) {
	case 1:
		score += 10;
		break;
	case 2:
		if (empty_chess == 1) { score += 20; }
		else if (empty_chess == 2) { score += 30; chance++; }
		break;
	case 3:
		if (empty_chess == 1) { score += attack ? 50 : 40; }
		else if (empty_chess == 2) { score += attack ? 15000 : 2000; }

		chance++;
		break;
	case 4:
		score += attack ? 100000 : 20000;
		break;
	}

	empty += empty_chess;
	chess += player_chess;

	return;
}

int GoBang::score(int x, int y) {
	int score{ 0 }, chess{ 0 }, empty{ 0 },
		chance{ 0 }, robot_chess{ 0 };

	for (int m = -1; m <= 0; m++) {
		for (int n = -1; n <= 1; n++) {
			if (m == 0 && n != 1) { continue; }
			evaluate(x, y, score, chess, empty, chance, m, n);
			evaluate(x, y, score, chess, empty, chance, m, n, false);

			score += (empty == 3 && chess == 2) ? 30 :
				(empty == 3 && chess == 3) ? 2000 :
				(empty == 2 && chess == 2) ? 20 :
				(empty == 2 && chess == 3) ? 40 :
				(empty != 0 && chess == 4) ? 20000 : 0;

			if (chance >= 2) { score += 2000; }
			chance = chess = empty = 0;

			// 攻击
			evaluate(x, y, score, chess, empty, chance, m, n, true, true);
			evaluate(x, y, score, chess, empty, chance, m, n, false, true);

			score += (empty == 3 && chess == 2) ? 30 :
				(empty == 3 && chess == 3) ? 15000 :
				(empty == 2 && chess == 2) ? 20 :
				(empty == 2 && chess == 3) ? 50 :
				(empty != 0 && chess == 4) ? 100000 : 0;

			if (chance >= 2) { score += 15000; }
			chance = chess = empty = 0;
		}
	}

	return score;
}

int GoBang::judgeRegion(int x, int y, int x1, int y1, int x2, int y2) {
	int res = 0;
	if (x >= x1 && x < x2 && y >= y1 && y < y2) { res = 1; }
	return res;
}
/*
bool GoBang::checkForbiddenMoves(int x, int y) {
	// 只有黑子才有禁手
	if (!IS_CHECK_ENABLED || this->side != 1) {
		return false;
	}

	int dr[][2] = {
		{1, 0},		// 0右方
		{0, 1},		// 1上方
		{-1, 0},	// 2左边
		{0, -1},	// 3下方
		{1, 1},		// 4右上
		{-1, 1},	// 5左上
		{1, -1},	// 6右下
		{-1, -1},	// 7左下
	};


	int pattern[3] = {
		0,	// 活三
		0,	// 四连
		0	// 长连
	};
	bool dr_pattern[8][3] = {};

	for (int i = 0; i < 8; i++) {
		auto tmpx{ x };
		auto tmpy{ y };
		auto times{ 0 };
		auto block{ false };	// 白子或空位
		
		while (true) {
			tmpx += dr[i][0];
			tmpy += dr[i][1];

			if (judgeRegion(tmpx, tmpy, 1, 1, 16, 16) != 1) { break; }
			if (board[tmpx][tmpy] == 1) { times++; }
			else if (board[tmpx][tmpy] == 0) { block = true; break; }
			else { break; }
		}

		if (times < 3) { continue; }
		dr_pattern[i][0] = times == 3 && !block;
		dr_pattern[i][1] = times == 4;
		dr_pattern[i][2] = times >= 6;
	}
}
*/