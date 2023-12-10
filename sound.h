//=============================================================================
//
// サウンド処理 [sound.h]
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// サウンド処理で必要

//*****************************************************************************
// サウンドファイル
//*****************************************************************************
enum 
{
	//サウンドcppと順番をそろえる※これ絶対


	SOUND_LABEL_BGM_TITLE,
	SOUND_LABEL_BGM_ed,	// BGM0タイトル
	SOUND_LABEL_SE_gameover,	// BGM1ゲーム
	SOUND_LABEL_BGM_sample002,	// BGM2リザルト
	SOUND_LABEL_BGM_stageselect,		// 爆発音
	SOUND_LABEL_BGMoption,	// 
	SOUND_LABEL_BGMstage1,	// 

	SOUND_LABEL_BGMstage2,	// 



	SOUND_LABEL_SE_bomb000,		// 爆発音
	SOUND_LABEL_SE_title1,	// 
	SOUND_LABEL_SE_title2,	// 
	SOUND_LABEL_SE_bullet,		// 
	SOUND_LABEL_SE_result,	// 
	SOUND_LABEL_SE_majic,	// 
	SOUND_LABEL_SE_tuta,		// 
	SOUND_LABEL_SE_warp,		// 

	SOUND_LABEL_BGM_maou,		// BGM Maou
	SOUND_LABEL_BOSS_BGM, 
	//SOUND_LABEL_BOSS_SINKA,

	SOUND_LABEL_MAX,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
bool InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);


void SetVolume(int label, float volume);					//個別の音量調整
void SetVolume(float volume);

void SetFrequencyRatio(int label, float pitch);				//全部の音量調整
void SetFrequencyRatio(float pitch);						//全部の速度調整

void SetOutputMatrix(int label, float left, float right);	//左右振り分け
