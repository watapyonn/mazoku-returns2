//=============================================================================
//
// �p�[�e�B�N������ [particle.cpp]
// Author : ��ژj
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "shadow.h"
#include "light.h"
#include "particle.h"
#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(2)			// �e�N�X�`���̐�

#define	PARTICLE_SIZE_X		(40.0f)		// ���_�T�C�Y
#define	PARTICLE_SIZE_Y		(40.0f)		// ���_�T�C�Y
#define	VALUE_MOVE_PARTICLE	(1.0f)		// �ړ����x

#define	MAX_PARTICLE		(200)		// �p�[�e�B�N���ő吔

#define	DISP_SHADOW						// �e�̕\��
//#undef DISP_SHADOW

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	XMFLOAT3		pos;			// �ʒu
	XMFLOAT3		rot;			// ��]
	XMFLOAT3		scale;			// �X�P�[��
	XMFLOAT3		move;			// �ړ���
	MATERIAL		material;		// �}�e���A��
	float			fSizeX;			// ��
	float			fSizeY;			// ����
	int				nIdxShadow;		// �eID
	int				nLife;			// ����
	BOOL			use;			// �g�p���Ă��邩�ǂ���
		// �K�w�A�j���[�V�����p�̃����o�[�ϐ�
	float			time;		// ���`��ԗp
	int				tblNo;		// �s���f�[�^�̃e�[�u���ԍ�
	int				tblMax;		// ���̃e�[�u���̃f�[�^��



} PARTICLE;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexParticle(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;		// ���_�o�b�t�@

static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static int							g_TexNo;					// �e�N�X�`���ԍ�

static PARTICLE					g_aParticle[MAX_PARTICLE];		// �p�[�e�B�N�����[�N
static XMFLOAT3					g_posBase;						// �r���{�[�h�����ʒu
static float					g_fWidthBase = 5.0f;			// ��̕�
static float					g_fHeightBase = 10.0f;			// ��̍���
static float					g_roty = 0.0f;					// �ړ�����
static float					g_spd = 0.0f;					// �ړ��X�s�[�h

static char *g_TextureName[] =
{
	"data/TEXTURE/effect000.jpg",
	//pipo_vine001-2.png
	"data/TEXTURE/51505812_p0.png",

	//"data/TEXTURE/kamifubuki002.png",
	//"data/TEXTURE/kamifubuki003.png",
	//"data/TEXTURE/kamifubuki004.png",
	//"data/TEXTURE/kamifubuki005.png",
	//"data/TEXTURE/kamifubuki006.png",
	//"data/TEXTURE/kamifubuki007.png",
	//"data/TEXTURE/kamifubuki008.png",


};

static INTERPOLATION_DATA move_tbl[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0, 0.0f, 0.0f), XMFLOAT3(0, 0.0f, 0.0f),XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0, 0.0f, 0.0f), XMFLOAT3(0, 0.0f, 0.0f),XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0, 0.0f, 0.0f), XMFLOAT3(0.0, 0.0f, 0.0f),XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
};

static INTERPOLATION_DATA* g_MoveTblAdrMove[] =
{
	move_tbl,
	
};


static BOOL							g_Load = FALSE;
static int				stay=1;
static int gCnt = 0;
//=============================================================================
// ����������
//=============================================================================
HRESULT InitParticle(void)
{
	// ���_���̍쐬
	MakeVertexParticle();

	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	// �p�[�e�B�N�����[�N�̏�����
	for(int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		g_aParticle[nCntParticle].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_aParticle[nCntParticle].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_aParticle[nCntParticle].scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_aParticle[nCntParticle].move = XMFLOAT3(1.0f, 1.0f, 1.0f);

		ZeroMemory(&g_aParticle[nCntParticle].material, sizeof(g_aParticle[nCntParticle].material));
		g_aParticle[nCntParticle].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_aParticle[nCntParticle].fSizeX = PARTICLE_SIZE_X;
		g_aParticle[nCntParticle].fSizeY = PARTICLE_SIZE_Y;
		g_aParticle[nCntParticle].nIdxShadow = -1;
		g_aParticle[nCntParticle].nLife = 0;
		g_aParticle[nCntParticle].use = FALSE;

		g_aParticle[nCntParticle].time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N��
		g_aParticle[nCntParticle].tblNo = 0;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_aParticle[nCntParticle].tblMax = sizeof(move_tbl) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	}

	g_posBase = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_roty = 0.0f;
	g_spd = 0.0f;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitParticle(void)
{
	if (g_Load == FALSE) return;

	//�e�N�X�`���̉��
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	// ���_�o�b�t�@�̉��
	if (g_VertexBuffer != NULL)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateParticle(void)
{
	PLAYER *pPlayer = GetPlayer();
	g_posBase = pPlayer->pos;

	{
		for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
		{
			if (g_aParticle[nCntParticle].use)
			{// �g�p��
				g_aParticle[nCntParticle].pos.x += g_aParticle[nCntParticle].move.x;
				g_aParticle[nCntParticle].pos.z += g_aParticle[nCntParticle].move.z;
				g_aParticle[nCntParticle].pos.y += g_aParticle[nCntParticle].move.y;

				//if(g_aParticle[nCntParticle].pos.y <= g_aParticle[nCntParticle].fSizeY / 2)
				//{// ���n����
				//	g_aParticle[nCntParticle].pos.y = g_aParticle[nCntParticle].fSizeY / 2;
				//	g_aParticle[nCntParticle].move.y = -g_aParticle[nCntParticle].move.y * 0.75f;
				//}

				g_aParticle[nCntParticle].move.x += (0.0f - g_aParticle[nCntParticle].move.x) * 0.015f;
				g_aParticle[nCntParticle].move.y += (0.0f - g_aParticle[nCntParticle].move.y) * 0.015f;

				//g_aParticle[nCntParticle].move.y -=  0.25f;
				g_aParticle[nCntParticle].move.z += (0.0f - g_aParticle[nCntParticle].move.z) * 0.015f;

#ifdef DISP_SHADOW
				if (g_aParticle[nCntParticle].nIdxShadow != -1)
				{// �e�g�p��
					float colA;

					// �e�̈ʒu�ݒ�
					SetPositionShadow(g_aParticle[nCntParticle].nIdxShadow, XMFLOAT3(g_aParticle[nCntParticle].pos.x, 0.1f, g_aParticle[nCntParticle].pos.z));

					// �e�̐F�̐ݒ�
					colA = g_aParticle[nCntParticle].material.Diffuse.w;
					SetColorShadow(g_aParticle[nCntParticle].nIdxShadow, XMFLOAT4(0.5f, 0.5f, 0.5f, colA));
				}
#endif

				g_aParticle[nCntParticle].nLife--;
				if (g_aParticle[nCntParticle].nLife <= 0)
				{
					g_aParticle[nCntParticle].use = FALSE;
					ReleaseShadow(g_aParticle[nCntParticle].nIdxShadow);
					g_aParticle[nCntParticle].nIdxShadow = -1;
				}
				else
				{
					//if(g_aParticle[nCntParticle].nLife <= 80)
					//{
					//	g_aParticle[nCntParticle].material.Diffuse.x = 0.8f - (float)(80 - g_aParticle[nCntParticle].nLife) / 80 * 0.8f;
					//	g_aParticle[nCntParticle].material.Diffuse.y = 0.7f - (float)(80 - g_aParticle[nCntParticle].nLife) / 80 * 0.7f;
					//	g_aParticle[nCntParticle].material.Diffuse.z = 0.2f - (float)(80 - g_aParticle[nCntParticle].nLife) / 80 * 0.2f;
					//}

					if (g_aParticle[nCntParticle].nLife <= 30)
					{
						// ���l�ݒ�
						g_aParticle[nCntParticle].material.Diffuse.w -= 0.05f;
						if (g_aParticle[nCntParticle].material.Diffuse.w < 0.0f)
						{
							g_aParticle[nCntParticle].material.Diffuse.w = 0.0f;
						}
					}
				}
			}
		}
	}
	//�搶�̌��̃v���O����
	{
		//fAngle = (float)(rand() % 628 - 314) / 100.0f;
		//fLength = rand() % (int)(g_fWidthBase * 200 ) / 100.0f - g_fWidthBase;
		//move.x = sinf(fAngle) * fLength;
		////move.y = rand() % 300 / 100.0f + g_fHeightBase;
		//move.y = rand() % 10 ;
		//move.z = cosf(fAngle) * fLength;
		//nLife = rand() % 100 +150;  
		//fSize = (float)(rand() % 30 + 20);
		//pos.y = fSize / 2;
	}


	XMFLOAT3 pos;
	XMFLOAT3 move = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float fAngle, fLength;
	int nLife;
	float fSize;

	pos = g_posBase;


	fAngle = (float)(rand() % 628 - 314) / 100.0f;
	fLength = rand() % (int)(g_fWidthBase * 20) / 100.0f - g_fWidthBase;

	move.x = sinf(fAngle) * fLength / 2;
	move.y = 0.0f;
	move.z = cosf(-fAngle) * fLength / 2;

	nLife = rand() % 100;
	fSize = (float)(rand() % 30 + 20);

	pos.x = sinf(fAngle) * 100;
	pos.z = cosf(fAngle) * 100;
	pos.y = rand() % 30 + 10.0f;


	float R = 1.0f;// (float)(rand() % 10);
	float G = 1.0f;//(float)(rand() % 10);
	float B = 1.0f;//(float)(rand() % 10);

	// �r���{�[�h�̐ݒ�
	//SetParticle(pos, move, XMFLOAT4(R, G, B, 1.0f), fSize, fSize, nLife, 0);

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawParticle(void)
{
	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();
	PLAYER* player = GetPlayer();
	// ���C�e�B���O�𖳌���
	SetLightEnable(FALSE);
	SetBlendState(BLEND_MODE_ADD);

	// Z��r����
	SetDepthEnable(FALSE);

	// �t�H�O����
	SetFogEnable(FALSE);

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	gCnt++;
	////// �e�N�X�`���ݒ�
	//GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[gCnt % TEXTURE_MAX]);
	//// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	for(int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{	// �e�N�X�`���ݒ�
		//GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[nCntParticle % TEXTURE_MAX]);

		if(g_aParticle[nCntParticle].use)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �r���[�}�g���b�N�X���擾
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			//mtxWorld = XMMatrixInverse(nullptr, mtxView);
			//mtxWorld.r[3].m128_f32[0] = 0.0f;
			//mtxWorld.r[3].m128_f32[1] = 0.0f;
			//mtxWorld.r[3].m128_f32[2] = 0.0f;

			// ������������������
			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_aParticle[nCntParticle].scale.x, g_aParticle[nCntParticle].scale.y, g_aParticle[nCntParticle].scale.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_aParticle[nCntParticle].pos.x, g_aParticle[nCntParticle].pos.y, g_aParticle[nCntParticle].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			// �}�e���A���ݒ�
			SetMaterial(g_aParticle[nCntParticle].material);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

	// �ʏ�u�����h�ɖ߂�
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z��r�L��
	SetDepthEnable(TRUE);

	// �t�H�O�L��
	SetFogEnable( GetFogEnable() );

}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexParticle(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	{//���_�o�b�t�@�̒��g�𖄂߂�
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		// ���_���W�̐ݒ�
		vertex[0].Position = XMFLOAT3(-PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[1].Position = XMFLOAT3(PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[2].Position = XMFLOAT3(-PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[3].Position = XMFLOAT3(PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);

		// �@���̐ݒ�
		vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

		// ���ˌ��̐ݒ�
		vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		// �e�N�X�`�����W�̐ݒ�
		vertex[0].TexCoord = { 0.0f, 0.0f };
		vertex[1].TexCoord = { 1.0f, 0.0f };
		vertex[2].TexCoord = { 0.0f, 1.0f };
		vertex[3].TexCoord = { 1.0f, 1.0f };

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	return S_OK;
}

//=============================================================================
// �}�e���A���J���[�̐ݒ�
//=============================================================================
void SetColorParticle(int nIdxParticle, XMFLOAT4 col)
{
	g_aParticle[nIdxParticle].material.Diffuse = col;
}

//=============================================================================
// �p�[�e�B�N���̔�������
//=============================================================================
int SetParticle(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife, int tex)
{
	int nIdxParticle = -1;

	for(int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if(!g_aParticle[nCntParticle].use)
		{
			g_aParticle[nCntParticle].pos = pos;
			g_aParticle[nCntParticle].rot   = { 0.0f, 0.0f, 0.0f };
			g_aParticle[nCntParticle].scale = { 0.25f, 0.25f, 0.25f };
			g_aParticle[nCntParticle].move = move;
			g_aParticle[nCntParticle].material.Diffuse = col;
			g_aParticle[nCntParticle].fSizeX = fSizeX;
			g_aParticle[nCntParticle].fSizeY = fSizeY;
			g_aParticle[nCntParticle].nLife = nLife;
			g_aParticle[nCntParticle].use = TRUE;
			g_TexNo = tex;
			nIdxParticle = nCntParticle;
			//SetColorParticle(nCntParticle, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
#ifdef DISP_SHADOW
			// �e�̐ݒ�
			g_aParticle[nCntParticle].nIdxShadow = CreateShadow(XMFLOAT3(pos.x, 0.1f, pos.z), 0.8f, 0.8f);		// �e�̐ݒ�
			if(g_aParticle[nCntParticle].nIdxShadow != -1)
			{
				SetColorShadow(g_aParticle[nCntParticle].nIdxShadow, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f));
			}
#endif

			break;
		}
	}

	return nIdxParticle;
}
