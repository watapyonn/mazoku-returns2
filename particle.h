//=============================================================================
//
// パーティクル処理 [particle.h]
// Author : 井芹亘
//
//=============================================================================
#pragma once


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitParticle(void);
void UninitParticle(void);
void UpdateParticle(void);
void DrawParticle(void);

int SetParticle(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife,int tex);
void SetColorParticle(int nIdxParticle, XMFLOAT4 col);

