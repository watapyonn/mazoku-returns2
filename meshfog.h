//=============================================================================
//
// ���b�V���n�ʂ̏��� [meshfield.h]
// Author : ��ژj
//
//=============================================================================
#pragma once

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitMeshFog(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT4 col,
	int nNumBlockX, int nNumBlockZ, float fBlockSizeX, float fBlockSizeZ);

void UninitMeshFog(void);
void UpdateMeshFog(void);
void DrawMeshFog(void);

BOOL RayHitFog(XMFLOAT3 pos, XMFLOAT3 *HitPosition, XMFLOAT3 *Normal);

