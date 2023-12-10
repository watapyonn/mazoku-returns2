//=============================================================================
//
// �J�������� [camera.cpp]
// Author : ��ژj
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define CAMERA_MAX			(2)
#define	POS_X_CAM			(0.0f)			// �J�����̏����ʒu(X���W)
#define	POS_Y_CAM			(70.0f)			// �J�����̏����ʒu(Y���W)
#define	POS_Z_CAM			(-250.0f)		// �J�����̏����ʒu(Z���W)

//#define	POS_X_CAM		(0.0f)			// �J�����̏����ʒu(X���W)
//#define	POS_Y_CAM		(200.0f)		// �J�����̏����ʒu(Y���W)
//#define	POS_Z_CAM		(-400.0f)		// �J�����̏����ʒu(Z���W)


#define	VIEW_ANGLE		(XMConvertToRadians(45.0f))						// �r���[���ʂ̎���p
#define	VIEW_ASPECT		((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)	// �r���[���ʂ̃A�X�y�N�g��	
#define	VIEW_NEAR_Z		(10.0f)											// �r���[���ʂ�NearZ�l
#define	VIEW_FAR_Z		(10000.0f)										// �r���[���ʂ�FarZ�l

#define	VALUE_MOVE_CAMERA	(2.0f)										// �J�����̈ړ���
#define	VALUE_ROTATE_CAMERA	(XM_PI * 0.01f)								// �J�����̉�]��

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static CAMERA			g_Camera[CAMERA_MAX];		// �J�����f�[�^

static int				g_ViewPortType = TYPE_FULL_SCREEN;



//���`�ۊǗp

static INTERPOLATION_DATA_CAMERA g_MoveTbl0[] = {
	//pos									at							rot						����
	{ XMFLOAT3(POS_X_CAM,POS_Y_CAM,POS_Z_CAM),XMFLOAT3(0.0f, 0.0f, 0.0f)   ,XMFLOAT3(0.0f, 0.0f, 0.0f),60 },
	//{ XMFLOAT3(POS_X_CAM,POS_Y_CAM,POS_Z_CAM),XMFLOAT3(0.0f, 0.0f, 0.0f)   ,XMFLOAT3(0.0f, 0.0f, 0.0f),60 },
	//{ XMFLOAT3(POS_X_CAM,POS_Y_CAM,POS_Z_CAM),XMFLOAT3(200.0f, 0.0f, 0.0f) ,XMFLOAT3(0.0f, 0.0f, 0.0f),60 },
	//{ XMFLOAT3(POS_X_CAM,POS_Y_CAM,POS_Z_CAM),XMFLOAT3(-200.0f, 0.0f, 0.0f),XMFLOAT3(0.0f, 0.0f, 0.0f),1 },
	//{ XMFLOAT3(POS_X_CAM,POS_Y_CAM,POS_Z_CAM),XMFLOAT3(0.0f, 0.0f, 200.0f) ,XMFLOAT3(0.0f, 0.0f, 0.0f),60 },
	//{ XMFLOAT3(POS_X_CAM,POS_Y_CAM,POS_Z_CAM),XMFLOAT3(0.0f, 0.0f, -200.0f),XMFLOAT3(0.0f, 0.0f, 0.0f),1 },
};


static INTERPOLATION_DATA_CAMERA* g_MoveTblAdr[] =
{
	g_MoveTbl0,

};


//=============================================================================
// ����������
//=============================================================================
void InitCamera(void)
{
	//���ׂẴJ������������
	for (int i = 0; i < CAMERA_MAX; i++)
	{
		g_Camera[i].pos = { POS_X_CAM, POS_Y_CAM, POS_Z_CAM };
		g_Camera[i].at  = { 0.0f, 0.0f, 0.0f };
		g_Camera[i].up  = { 0.0f, 1.0f, 0.0f };
		g_Camera[i].rot = { 0.0f, 0.0f, 0.0f };

		// ���_�ƒ����_�̋������v�Z
		float vx, vz;
		vx = g_Camera[i].pos.x - g_Camera[i].at.x;
		vz = g_Camera[i].pos.z - g_Camera[i].at.z;
		g_Camera[i].len = sqrtf(vx * vx + vz * vz);

		// �r���[�|�[�g�^�C�v�̏�����
		SetViewPort(g_ViewPortType);



	}
	// 0�Ԃ������`��Ԃœ������Ă݂�
	g_Camera[LINEAR_INTERPOLATION_CAMERA].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_Camera[LINEAR_INTERPOLATION_CAMERA].tblNo = 0;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Camera[LINEAR_INTERPOLATION_CAMERA].tblMax = sizeof(g_MoveTbl0) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

}


//=============================================================================
// �J�����̏I������
//=============================================================================
void UninitCamera(void)
{

}


//=============================================================================
// �J�����̍X�V����
//=============================================================================
void UpdateCamera(void)
{

#ifdef _DEBUG

	if (GetKeyboardPress(DIK_Z))
	{// ���_����u���v
		g_Camera[DEFAULT_CAMERA].rot.y += VALUE_ROTATE_CAMERA;
		if (g_Camera[DEFAULT_CAMERA].rot.y > XM_PI)
		{
			g_Camera[DEFAULT_CAMERA].rot.y -= XM_PI * 2.0f;
		}

		g_Camera[DEFAULT_CAMERA].pos.x = g_Camera[DEFAULT_CAMERA].at.x - sinf(g_Camera[DEFAULT_CAMERA].rot.y) * g_Camera[DEFAULT_CAMERA].len;
		g_Camera[DEFAULT_CAMERA].pos.z = g_Camera[DEFAULT_CAMERA].at.z - cosf(g_Camera[DEFAULT_CAMERA].rot.y) * g_Camera[DEFAULT_CAMERA].len;
	}

	if (GetKeyboardPress(DIK_C))
	{// ���_����u�E�v
		g_Camera[DEFAULT_CAMERA].rot.y -= VALUE_ROTATE_CAMERA;
		if (g_Camera[DEFAULT_CAMERA].rot.y < -XM_PI)
		{
			g_Camera[DEFAULT_CAMERA].rot.y += XM_PI * 2.0f;
		}

		g_Camera[DEFAULT_CAMERA].pos.x = g_Camera[DEFAULT_CAMERA].at.x - sinf(g_Camera[DEFAULT_CAMERA].rot.y) * g_Camera[DEFAULT_CAMERA].len;
		g_Camera[DEFAULT_CAMERA].pos.z = g_Camera[DEFAULT_CAMERA].at.z - cosf(g_Camera[DEFAULT_CAMERA].rot.y) * g_Camera[DEFAULT_CAMERA].len;
	}

	if (GetKeyboardPress(DIK_Y))
	{// ���_�ړ��u��v
		g_Camera[DEFAULT_CAMERA].pos.y += VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_N))
	{// ���_�ړ��u���v
		g_Camera[DEFAULT_CAMERA].pos.y -= VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_Q))
	{// �����_����u���v
		g_Camera[DEFAULT_CAMERA].rot.y -= VALUE_ROTATE_CAMERA;
		if (g_Camera[DEFAULT_CAMERA].rot.y < -XM_PI)
		{
			g_Camera[DEFAULT_CAMERA].rot.y += XM_PI * 2.0f;
		}

		g_Camera[DEFAULT_CAMERA].at.x = g_Camera[DEFAULT_CAMERA].pos.x + sinf(g_Camera[DEFAULT_CAMERA].rot.y) * g_Camera[DEFAULT_CAMERA].len;
		g_Camera[DEFAULT_CAMERA].at.z = g_Camera[DEFAULT_CAMERA].pos.z + cosf(g_Camera[DEFAULT_CAMERA].rot.y) * g_Camera[DEFAULT_CAMERA].len;
	}

	if (GetKeyboardPress(DIK_E))
	{// �����_����u�E�v
		g_Camera[DEFAULT_CAMERA].rot.y += VALUE_ROTATE_CAMERA;
		if (g_Camera[DEFAULT_CAMERA].rot.y > XM_PI)
		{
			g_Camera[DEFAULT_CAMERA].rot.y -= XM_PI * 2.0f;
		}

		g_Camera[DEFAULT_CAMERA].at.x = g_Camera[DEFAULT_CAMERA].pos.x + sinf(g_Camera[DEFAULT_CAMERA].rot.y) * g_Camera[DEFAULT_CAMERA].len;
		g_Camera[DEFAULT_CAMERA].at.z = g_Camera[DEFAULT_CAMERA].pos.z + cosf(g_Camera[DEFAULT_CAMERA].rot.y) * g_Camera[DEFAULT_CAMERA].len;
	}

	if (GetKeyboardPress(DIK_T))
	{// �����_�ړ��u��v
		g_Camera[DEFAULT_CAMERA].at.y += VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_B))
	{// �����_�ړ��u���v
		g_Camera[DEFAULT_CAMERA].at.y -= VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_U))
	{// �߂Â�
		g_Camera[DEFAULT_CAMERA].len -= VALUE_MOVE_CAMERA;
		g_Camera[DEFAULT_CAMERA].pos.x = g_Camera[DEFAULT_CAMERA].at.x - sinf(g_Camera[DEFAULT_CAMERA].rot.y) * g_Camera[DEFAULT_CAMERA].len;
		g_Camera[DEFAULT_CAMERA].pos.z = g_Camera[DEFAULT_CAMERA].at.z - cosf(g_Camera[DEFAULT_CAMERA].rot.y) * g_Camera[DEFAULT_CAMERA].len;
	}

	if (GetKeyboardPress(DIK_M))
	{// �����
		g_Camera[DEFAULT_CAMERA].len += VALUE_MOVE_CAMERA;
		g_Camera[DEFAULT_CAMERA].pos.x = g_Camera[DEFAULT_CAMERA].at.x - sinf(g_Camera[DEFAULT_CAMERA].rot.y) * g_Camera[DEFAULT_CAMERA].len;
		g_Camera[DEFAULT_CAMERA].pos.z = g_Camera[DEFAULT_CAMERA].at.z - cosf(g_Camera[DEFAULT_CAMERA].rot.y) * g_Camera[DEFAULT_CAMERA].len;
	}

	// �J�����������ɖ߂�
	if (GetKeyboardPress(DIK_R))
	{
		UninitCamera();
		InitCamera();
	}


		//���`�ۊǂœ�����
		// �ړ�����
		if (g_Camera[LINEAR_INTERPOLATION_CAMERA].tblMax > LINEAR_INTERPOLATION_CAMERA)	// ���`��Ԃ����s����H
		{	// ���`��Ԃ̏���
			int nowNo = (int)g_Camera[LINEAR_INTERPOLATION_CAMERA].time;			// �������ł���e�[�u���ԍ������o���Ă���
			int maxNo = g_Camera[LINEAR_INTERPOLATION_CAMERA].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
			int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
			INTERPOLATION_DATA_CAMERA* tbl = g_MoveTblAdr[g_Camera[LINEAR_INTERPOLATION_CAMERA].tblNo];	// �s���e�[�u���̃A�h���X���擾

			XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
			XMVECTOR nowAt = XMLoadFloat3(&tbl[nowNo].at);	// XMVECTOR�֕ϊ�
			XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�

			XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
			XMVECTOR At  = XMLoadFloat3(&tbl[nextNo].at) - nowAt;	// XYZ��]�ʂ��v�Z���Ă���
			XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ�g�嗦���v�Z���Ă���

			float nowTime = g_Camera[LINEAR_INTERPOLATION_CAMERA].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

			Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
			At *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
			Rot *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

			// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
			XMStoreFloat3(&g_Camera[LINEAR_INTERPOLATION_CAMERA].pos, nowPos + Pos);

			// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
			XMStoreFloat3(&g_Camera[LINEAR_INTERPOLATION_CAMERA].at, nowAt + At);

			// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
			XMStoreFloat3(&g_Camera[LINEAR_INTERPOLATION_CAMERA].rot, nowRot + Rot);
			//g_Enemy[DEFAULT_CAMERA].w = TEXTURE_WIDTH * g_Enemy[DEFAULT_CAMERA].scl.x;
			//g_Enemy[DEFAULT_CAMERA].h = TEXTURE_HEIGHT * g_Enemy[DEFAULT_CAMERA].scl.y;
			// frame���g�Ď��Ԍo�ߏ���������
			g_Camera[LINEAR_INTERPOLATION_CAMERA].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
			if ((int)g_Camera[LINEAR_INTERPOLATION_CAMERA].time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
			{
				g_Camera[LINEAR_INTERPOLATION_CAMERA].time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
			}






		}

	






#endif



#ifdef _DEBUG	// �f�o�b�O����\������
	PrintDebugProc("Camera:ZC QE TB YN UM R\n");
#endif
}


//=============================================================================
// �J�����̍X�V
//=============================================================================
void SetCamera(int index) 
{
	// �r���[�}�g���b�N�X�ݒ�
	XMMATRIX mtxView;
	mtxView = XMMatrixLookAtLH(XMLoadFloat3(&g_Camera[index].pos), XMLoadFloat3(&g_Camera[index].at), XMLoadFloat3(&g_Camera[index].up));
	SetViewMatrix(&mtxView);
	XMStoreFloat4x4(&g_Camera[index].mtxView, mtxView);

	XMMATRIX mtxInvView;
	mtxInvView = XMMatrixInverse(nullptr, mtxView);
	XMStoreFloat4x4(&g_Camera[index].mtxInvView, mtxInvView);


	// �v���W�F�N�V�����}�g���b�N�X�ݒ�
	XMMATRIX mtxProjection;
	mtxProjection = XMMatrixPerspectiveFovLH(VIEW_ANGLE, VIEW_ASPECT, VIEW_NEAR_Z, VIEW_FAR_Z);

	SetProjectionMatrix(&mtxProjection);
	XMStoreFloat4x4(&g_Camera[index].mtxProjection, mtxProjection);

	SetShaderCamera(g_Camera[index].pos);
}


//=============================================================================
// �J�����̎擾
//=============================================================================
CAMERA *GetCamera(void) 
{
	return &g_Camera[DEFAULT_CAMERA];
}

//=============================================================================
// �r���[�|�[�g�̐ݒ�
//=============================================================================
void SetViewPort(int type)
{
	ID3D11DeviceContext *g_ImmediateContext = GetDeviceContext();
	D3D11_VIEWPORT vp;

	g_ViewPortType = type;

	// �r���[�|�[�g�ݒ�
	switch (g_ViewPortType)
	{
	case TYPE_FULL_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH;
		vp.Height = (FLOAT)SCREEN_HEIGHT;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		break;

	case TYPE_LEFT_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH / 2;
		vp.Height = (FLOAT)SCREEN_HEIGHT;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		break;

	case TYPE_RIGHT_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH / 2;
		vp.Height = (FLOAT)SCREEN_HEIGHT;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = (FLOAT)SCREEN_WIDTH / 2;
		vp.TopLeftY = 0;
		break;

	case TYPE_UP_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH;
		vp.Height = (FLOAT)SCREEN_HEIGHT / 2;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		break;

	case TYPE_DOWN_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH;
		vp.Height = (FLOAT)SCREEN_HEIGHT / 2;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = (FLOAT)SCREEN_HEIGHT / 2;
		break;




	case TYPE_UP_LFET:

		vp.Width = (FLOAT)SCREEN_WIDTH / 2-10;
		vp.Height = (FLOAT)SCREEN_HEIGHT / 2-10;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		break;

	case TYPE_UP_RIGHT:

		vp.Width = (FLOAT)SCREEN_WIDTH / 2-10;
		vp.Height = (FLOAT)SCREEN_HEIGHT / 2-10;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = (FLOAT)SCREEN_WIDTH / 2+10;
		vp.TopLeftY = 0;
		break;

	case TYPE_DOMN_LFET:
		vp.Width = (FLOAT)SCREEN_WIDTH / 2-10;
		vp.Height = (FLOAT)SCREEN_HEIGHT / 2-10;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = (FLOAT)SCREEN_HEIGHT / 2+10;
		break;

	case TYPE_DOMN_RIGHT:
		vp.Width = (FLOAT)SCREEN_WIDTH / 2-10;
		vp.Height = (FLOAT)SCREEN_HEIGHT / 2-10;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = (FLOAT)SCREEN_WIDTH / 2+ 10;
		vp.TopLeftY = (FLOAT)SCREEN_HEIGHT / 2+10;
		break;	


	}
	g_ImmediateContext->RSSetViewports(1, &vp);

}


int GetViewPortType(void)
{
	return g_ViewPortType;
}



// �J�����̎��_�ƒ����_���Z�b�g
void SetCameraAT(XMFLOAT3 pos ,int index)
{
	// �J�����̒����_�������̍��W�ɂ��Ă݂�
	g_Camera[index].at = pos;

	// �J�����̎��_���J������Y����]�ɑΉ������Ă���
	g_Camera[index].pos.x = g_Camera[index].at.x - sinf(g_Camera[index].rot.y) * g_Camera[index].len;
	g_Camera[index].pos.z = g_Camera[index].at.z - cosf(g_Camera[index].rot.y) * g_Camera[index].len;

}

