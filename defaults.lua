 -- gobang项目的默认配置文件
 
-- 游戏界面大小，不建议更改，因为目前界面还未实现自适应缩放，如需更改请等比例更改
-- 如：WIDTH = 630 * 2
--    HEIGTH = 800 * 2	
WIDTH = 630 -- 窗口宽度 
HEIGHT = 800 -- 窗口高度

-- 窗口出现位置，设置为负值则自动居中，第一个参数为x坐标，第二个参数为y坐标
DISPLAY = {-1, -1}

-- 背景音乐，设置为空字符串或无效地址则不播放
MUSIC = ".\\assets\\1.mp3"
EFFECT1 = ".\\assets\\sound_put_chess.mp3"
EFFECT2 = ".\\assets\\sound_win_game.mp3"
EFFECT3 = ".\\assets\\sound_lose_game.mp3"

-- 音量设置，值的取值范围为 0~1000
MUSIC_VOLUME = 1000  -- 音乐音量
EFFECT_VOLUME = 1000 -- 效果音量

-- 背景图片，设置为nil则不显示，1为菜单背景，2为游戏背景
BACKGROUND_IMAGE1 = ".\\assets\\bgp.jpg"
BACKGROUND_IMAGE2 = ".\\assets\\bgp.jpg"

-- 字体，1为菜单字体，2为游戏时侧边栏字体，3为胜利字体
FONT1 = "黑体"
FONT2 = "微软雅黑"
FONT3 = "楷体"

-- 默认为红色
FONT_COLOR1 = 0x0000AA
FONT_COLOR2 = 0x0000AA
FONT_COLOR3 = 0x0000AA


-- 自定义棋盘

--[[
常用颜色对应的十六进制如下
	BLACK			0
	BLUE			0xAA0000
	GREEN			0x00AA00
	CYAN			0xAAAA00
	RED				0x0000AA
	MAGENTA			0xAA00AA
	BROWN			0x0055AA
	LIGHTGRAY		0xAAAAAA
	DARKGRAY		0x555555
	LIGHTBLUE		0xFF5555
	LIGHTGREEN		0x55FF55
	LIGHTCYAN		0xFFFF55
	LIGHTRED		0x5555FF
	LIGHTMAGENTA	0xFF55FF
	YELLOW			0x55FFFF
	WHITE			0xFFFFFF

    也可以自行输入颜色对应的数值（十六进制形式）
]]
FIVE_COLOR = 0x000000   -- 五基准点的颜色
LINE_COLOR = 0xffffff   -- 棋盘线条颜色

-- 更改棋子颜色只能更改之后下的棋子的值，已经下的棋子颜色不会更改 
BLACK_COLOR = 0x000000 -- 黑子颜色
WHITE_COLOR = 0xffffff  -- 白子颜色

-- 回放速度，以毫秒计
PLAYBACK_SPEED = 200

