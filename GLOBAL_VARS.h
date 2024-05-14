#pragma once
#include <filesystem>

namespace fs = std::filesystem;

// …Ë÷√
int WIDTH = 800;
int HEIGHT = 630;

int DISPLAY[] = { -1,-1 };

std::string MUSIC;

int MUSIC_VOLUME{ 500 };
int EFFECT_VOLUME{ 500 };

std::string BACKGROUND_IMAGE1{ ".\\bgp.jpg" };
std::string BACKGROUND_IMAGE2{ ".\\bgp.jpg" };

std::string FONT1{ "∫⁄ÃÂ" };
std::string FONT2{ "Œ¢»Ì—≈∫⁄" };
std::string FONT3{ "ø¨ÃÂ" };

int FONT_COLOR1{ RED };
int FONT_COLOR2{ RED };
int FONT_COLOR3{ RED };

int FIVE_COLOR{ 0x000000 };
int LINE_COLOR{ 0xffffff };
int BLACK_COLOR{ FIVE_COLOR };
int WHITE_COLOR{ LINE_COLOR };

int PLAYBACK_SPEED{ 500 };

bool inUse{ true };
fs::path fileData{ ".\\config.lua" };
std::filesystem::file_time_type lastTime;

std::string prevMusic{ MUSIC };
int prevVolume1{ MUSIC_VOLUME };
int prevVolume2{ EFFECT_VOLUME };