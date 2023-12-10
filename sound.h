//=============================================================================
//
// �T�E���h���� [sound.h]
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// �T�E���h�����ŕK�v

//*****************************************************************************
// �T�E���h�t�@�C��
//*****************************************************************************
enum 
{
	//�T�E���hcpp�Ə��Ԃ����낦�遦������


	SOUND_LABEL_BGM_TITLE,
	SOUND_LABEL_BGM_ed,	// BGM0�^�C�g��
	SOUND_LABEL_SE_gameover,	// BGM1�Q�[��
	SOUND_LABEL_BGM_sample002,	// BGM2���U���g
	SOUND_LABEL_BGM_stageselect,		// ������
	SOUND_LABEL_BGMoption,	// 
	SOUND_LABEL_BGMstage1,	// 

	SOUND_LABEL_BGMstage2,	// 



	SOUND_LABEL_SE_bomb000,		// ������
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
// �v���g�^�C�v�錾
//*****************************************************************************
bool InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);


void SetVolume(int label, float volume);					//�ʂ̉��ʒ���
void SetVolume(float volume);

void SetFrequencyRatio(int label, float pitch);				//�S���̉��ʒ���
void SetFrequencyRatio(float pitch);						//�S���̑��x����

void SetOutputMatrix(int label, float left, float right);	//���E�U�蕪��
