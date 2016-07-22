#pragma once

#define REEL_SIZE_X		"ReelSizeX"
#define REEL_SIZE_Y		"ReelSizeY"
#define STRIPS_NAME		"StripsName"
#define REELS_NAME		"ReelsName"
#define STRIP_COUNT_NG	"NgStripCount"
#define STRIP_COUNT_FG	"FgStripCount"
#define NG_SPIN			"CmdIn_NgSpin"
#define FG_SPIN			"CmdIn_FgSpin"
#define NG_OUT			"CmdOut_NgSpin"
#define FG_OUT			"CmdOut_FgSpin"
#define PROB_REELS_NAME	"reels"

// SetInfo 種類
enum Type_SetInfo {
	T_Con = 0,
	T_Ng = 1,
	T_Fg = 2,

	T_count = 3
};

// Spin 種類
enum Type_Spin {
	S_Ng = 0,
	S_Fg = 1,

	S_count = 2
};