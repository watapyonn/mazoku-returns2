//=============================================================================
//
// �G�l�~�[���f������ [enemy.cpp]
// Author : ��ژj
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "input.h"
#include "debugproc.h"
#include "select.h"
#include "shadow.h"
#include "sound.h"
#include "fade.h"
#include "particle.h"
#include "collision.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_SELECT_BOLL			"data/MODEL/pokemonboll.obj"		// �ǂݍ��ރ��f����
#define	MODEL_SELECT_SPHERE			"data/MODEL/sphere.obj"		// �ǂݍ��ރ��f����
#define	MODEL_SELECT_TAMA			"data/MODEL/sphere2.obj"		// �ǂݍ��ރ��f����

#define	VALUE_MOVE			(5.0f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��

#define ENEMY_SHADOW_SIZE	(0.4f)						// �e�̑傫��
#define ENEMY_OFFSET_Y		(7.0f)						// �G�l�~�[�̑��������킹��


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static SELECT			g_Select[MAX_SELECT];				// �G�l�~�[

int g_Sleect_load = 0;

BOOL		g_isTriggerslinear = TRUE;

static INTERPOLATION_DATA g_MoveTbl0[] = {
	//���W									��]��							�g�嗦							����
	{ XMFLOAT3(  0.0f,   0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(2.0f, 2.0f, 2.0f),	120 },
	{ XMFLOAT3( 100.0f,  0.0f, 100.0f),	XMFLOAT3(0.0f, 0.0f, 3.14f),XMFLOAT3(1.0f, 1.0f, 1.0f),	120 },
	{ XMFLOAT3(-100.0f,  0.0f, 100.0f),	XMFLOAT3(0.0f, 0.0f, 6.28f),XMFLOAT3(1.0f, 1.0f, 1.0f),	120 },

};


static INTERPOLATION_DATA g_MoveTbl1[] = {
	//���W									��]��							�g�嗦							����
	{ XMFLOAT3( 100.0f,  0.0f, 100.0f),	XMFLOAT3(0.0f, 0.0f, 3.14f),XMFLOAT3(1.0f, 1.0f, 1.0f),	120 },
	{ XMFLOAT3(-100.0f,  0.0f, 100.0f),	XMFLOAT3(0.0f, 0.0f, 6.28f),XMFLOAT3(1.0f, 1.0f, 1.0f),	120 },
	{ XMFLOAT3(   0.0f,   0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(2.0f, 2.0f, 2.0f),	120 },
};


static INTERPOLATION_DATA g_MoveTbl2[] = {
	//���W									��]��							�g�嗦							����
	{ XMFLOAT3(-100.0f,0.0f,100.0f),	XMFLOAT3(0.0f, 0.0f, 6.28f),XMFLOAT3(1.0f, 1.0f, 1.0f),	120 },
	{ XMFLOAT3(   0.0f, 0.0f,  0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(2.0f, 2.0f, 2.0f),	120 },
	{ XMFLOAT3( 100.0f, 0.0f,100.0f),	XMFLOAT3(0.0f, 0.0f, 3.14f),XMFLOAT3(1.0f, 1.0f, 1.0f),	120 },
};

static INTERPOLATION_DATA* g_MoveTblAdr[] =
{
	g_MoveTbl0,
	g_MoveTbl1,
	g_MoveTbl2,

};


//=============================================================================
// ����������
//=============================================================================
HRESULT InitSelect(void)
{
	for (int i = 0; i < MAX_SELECT; i++)
	{
		
		g_Select[i].load = TRUE;

		g_Select[i].pos = XMFLOAT3(-50.0f + i * 30.0f, 7.0f, 20.0f);
		g_Select[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Select[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Select[i].model, &g_Select[i].diffuse[0]);

		//XMFLOAT3 pos = g_Select[i].pos;
		//g_Select[i].shadowIdx = CreateShadow(pos, SELECT_SHADOW_SIZE, SELECT_SHADOW_SIZE);
		
		g_Select[i].use = TRUE;		// TRUE:�����Ă�


	}
	LoadModel(MODEL_SELECT_BOLL		, &g_Select[0].model);
	LoadModel(MODEL_SELECT_SPHERE	, &g_Select[1].model);
	LoadModel(MODEL_SELECT_TAMA		, &g_Select[2].model);



	// 0�Ԃ������`��Ԃœ������Ă݂�
	g_Select[0].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_Select[0].tblNo = 0;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Select[0].tblMax = sizeof(g_MoveTbl0) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	// 1�Ԃ������`��Ԃœ������Ă݂�
	g_Select[1].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_Select[1].tblNo = 1;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Select[1].tblMax = sizeof(g_MoveTbl1) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	// 2�Ԃ������`��Ԃœ������Ă݂�
	g_Select[2].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_Select[2].tblNo = 2;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Select[2].tblMax = sizeof(g_MoveTbl2) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitSelect(void)
{

	for (int i = 0; i < MAX_SELECT; i++)
	{
		if (g_Select[i].load)
		{
			UnloadModel(&g_Select[i].model);
			g_Select[i].load = FALSE;
		}
	}

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateSelect(void)
{
				// �G�l�~�[�𓮂����ꍇ(XMFLOAT3(0.0f, 0.0f, -200.0f),�́A�e�����킹�ē���������Y��Ȃ��悤�ɂˁI
	for (int i = 0; i < MAX_SELECT; i++)
	{
		if (g_Select[i].use == TRUE)		// ���̃G�l�~�[���g���Ă���H
		{								// Yes
			if (GetKeyboardPress(DIK_LEFT))
			{

			// �ړ�����
				if (g_Select[i].tblMax > 0)	// ���`��Ԃ����s����H
				{	// ���`��Ԃ̏���
					int nowNo = (int)g_Select[i].time;			// �������ł���e�[�u���ԍ������o���Ă���
					int maxNo = g_Select[i].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
					int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
					INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Select[i].tblNo];	// �s���e�[�u���̃A�h���X���擾

					XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
					XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
					XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

					XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
					XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
					XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

					float nowTime = g_Select[i].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

					Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
					Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
					Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

					// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
					XMStoreFloat3(&g_Select[i].pos, nowPos + Pos);

					// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
					XMStoreFloat3(&g_Select[i].rot, nowRot + Rot);

					// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
					XMStoreFloat3(&g_Select[i].scl, nowScl + Scl);


					// frame���g�Ď��Ԍo�ߏ���������
					g_Select[i].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
					if ((int)g_Select[i].time >= (float)maxNo - 2.0f)			// �o�^�e�[�u���Ō�܂ňړ��������H
					{
						g_Select[i].time = (float)maxNo - 2.0f;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
						maxNo -= 1;

					}
					else
					{
						maxNo+=1 ;
					}
				
				}
				//if (GetKeyboardPress(DIK_LEFT))
				//{
				//	g_Select[i].time = maxNo - 1;
				//	g_isTriggerslinear = TRUE;
				//}
				//if (GetKeyboardPress(DIK_RIGHT))
				//{
				//	g_Select[i].time = maxNo - 2;

				//}


				//// ��񂾂��̃A�j���[�V������������
				//g_Select[i].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
				//if ((int)g_Select[i].time >= maxNo-1 )			// �o�^�e�[�u���Ō�܂ňړ��������H
				//{
				//	g_Select[i].time = maxNo-1;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
				//	int timeNo= g_Select[i].time;

				//}

			}

			

			BOOL ans = CollisionBC(g_Select[i].pos, XMFLOAT3(0.0f, 0.0f, 0.0f), 100.0f, 10.0f);
			if (ans)
			{

				XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
				pos.x = (float)(rand() % 100-100);
				pos.y = (float)(rand() % 100-100);
				pos.z = 0.0f;



				SetParticle(g_Select[i].pos, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), 100, 100, 100, 0);
			}



		}
	}





	//�x�W�F�Ȑ������H
	//// �e�̈ړ����� �x�W�F�Ȑ�
	//g_Bullet[i].s += 0.02f;

	//if (g_Bullet[i].s > 1.0f)
	//{
	//	g_Bullet[i].use = FALSE;
	//	g_Bullet[i].s = 0.0f;
	//	ReleaseShadow(g_Bullet[i].shadowIdx);
	//}


	//XMVECTOR vP0 = XMLoadFloat3(&g_Bullet[i].p0);
	//XMVECTOR vP1 = XMLoadFloat3(&g_Bullet[i].p1);
	//XMVECTOR vP2 = XMLoadFloat3(&g_Bullet[i].p2t);


	//float s = g_Bullet[i].s * g_Bullet[i].s;
	////float s = g_Bullet[i].s;
	////float s = g_Bullet[i].s * (2 - g_Bullet[i].s);
	////float s = g_Bullet[i].s * (g_Bullet[i].s * (3 - 2 * g_Bullet[i].s));

	//XMVECTOR vAns = QuadraticBezierCurve(vP0, vP1, vP2, s);

	//XMStoreFloat3(&g_Bullet[i].pos, vAns);
	//XMVECTOR vold_pos = XMLoadFloat3(&g_Bullet[i].pos);

	//XMVECTOR vec = vAns - vold_pos;        //�ړ��O�Ƃ��Ƃ̍��W�̍����i�p�x�j
	//float angle = atan2f(vec.m128_f32[1], vec.m128_f32[0]);        //���̍������g���Ċp�x�����߂Ă���




	//�p�[�e�B�N��������
	XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	pos.x = (float)(rand() % 960);
	pos.y = (float)(rand() % 540);
	pos.z = 0.0f;

	XMFLOAT3 move = XMFLOAT3(0.0f,0.0f, 0.0f);
	move.x -= 10.0f;
	move.y -= 10.0f;
	move.z = 0.0f;

	XMFLOAT4 col = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	col.x = (float)(rand() % 10);
	col.x /= 10;
	col.y = (float)(rand() % 10);
	col.y /= 10;


	float fSizeX = 100.0f;
	float fSizeY = 100.0f;

	SetParticle(pos, move, col, fSizeX, fSizeY, 100, 0);

	SetParticle(pos, XMFLOAT3(0.0f, 0.0f, 0.0f), col, fSizeX, fSizeY, 100, 0);


#ifdef _DEBUG

	if (GetKeyboardTrigger(DIK_RETURN) || (IsButtonTriggered(0, BUTTON_B)))
	{// Enter��������A�X�e�[�W��؂�ւ���
		
		SetFade(FADE_OUT, MODE_GAME);

	}

#endif


}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawSelect(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_BACK);

	for (int i = 0; i < MAX_SELECT; i++)
	{
		if (g_Select[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Select[i].scl.x, g_Select[i].scl.y, g_Select[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Select[i].rot.x, g_Select[i].rot.y + XM_PI, g_Select[i].rot.z+0.01f);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Select[i].pos.x, g_Select[i].pos.y, g_Select[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Select[i].mtxWorld, mtxWorld);


		// ���f���`��
		DrawModel(&g_Select[i].model);
	}

//	// �J�����O�ݒ��߂�
//	SetCullingMode(CULL_MODE_BACK);
}
//
////=============================================================================
//// �G�l�~�[�̎擾
////=============================================================================
//SELECT *GetSELECT()
//{
//	return &g_Select[0];
//}
