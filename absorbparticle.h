//=============================================================================
//
// �z���p�[�e�B�N������ [absorbparticle.cpp]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitAbsorbParticle(void);
void UninitAbsorbParticle(void);
void UpdateAbsorbParticle(void);
void DrawAbsorbParticle(void);

void SetColorAbsorbParticle(int nIdxParticle, XMFLOAT4 col);
int SetAbsorbParticle(XMFLOAT3 localPos, XMFLOAT3 move, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife, int group);