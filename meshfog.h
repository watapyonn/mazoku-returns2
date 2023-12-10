//=============================================================================
//
// メッシュ地面の処理 [meshfield.h]
// Author : 井芹亘
//
//=============================================================================
#pragma once

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitMeshFog(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT4 col,
	int nNumBlockX, int nNumBlockZ, float fBlockSizeX, float fBlockSizeZ);

void UninitMeshFog(void);
void UpdateMeshFog(void);
void DrawMeshFog(void);

BOOL RayHitFog(XMFLOAT3 pos, XMFLOAT3 *HitPosition, XMFLOAT3 *Normal);

