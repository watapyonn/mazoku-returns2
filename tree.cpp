//=============================================================================
//
// �؏��� [tree.cpp]
// Author : ��ژj
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "tree.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(5)				// �e�N�X�`���̐�

#define	TREE_WIDTH			(50.0f)			// ���_�T�C�Y
#define	TREE_HEIGHT			(80.0f)			// ���_�T�C�Y

#define	MAX_TREE			(256)			// �؍ő吔


#define TEXTURE_PATTERN_DIVIDE_X	(5)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(4)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(16)		// �A�j���[�V�����̐؂�ւ��Wait�l


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	XMFLOAT3	pos;			// �ʒu
	XMFLOAT3	scl;			// �X�P�[��
	MATERIAL	material;		// �}�e���A��
	float		fWidth;			// ��
	float		fHeight;		// ����
	int			nIdxShadow;		// �eID
	BOOL		use;			// �g�p���Ă��邩�ǂ���

} TREE;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexTree(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// ���_�o�b�t�@
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static TREE					g_aTree[MAX_TREE];	// �؃��[�N
static int					g_TexNo;			// �e�N�X�`���ԍ�
static BOOL					g_bAlpaTest;		// �A���t�@�e�X�gON/OFF
//static int					g_nAlpha;			// �A���t�@�e�X�g��臒l


VERTEX_3D* vertex = NULL;
int	g_countAnim;
int	g_patternAnim;
int	g_texNo;

static char *g_TextureName[] =
{
	"data/TEXTURE/item/crystalpurple.png",
	"data/TEXTURE/item/crystalwhite.png",
	"data/TEXTURE/tree003.png",
	"data/TEXTURE/tree004.png",
	"data/TEXTURE/tree005.png",

};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitTree(void)
{

	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	// �؃��[�N�̏�����
	for(int nCntTree = 0; nCntTree < MAX_TREE; nCntTree++)
	{
		ZeroMemory(&g_aTree[nCntTree].material, sizeof(g_aTree[nCntTree].material));
		g_aTree[nCntTree].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_aTree[nCntTree].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_aTree[nCntTree].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_aTree[nCntTree].fWidth = TREE_WIDTH;
		g_aTree[nCntTree].fHeight = TREE_HEIGHT;
		g_aTree[nCntTree].use = FALSE;
	}

	g_countAnim	  = 0;;			// �A�j���[�V�����J�E���g
	g_patternAnim = 0;;			// �A�j���[�V�����p�^�[���i���o�[
	g_texNo		  = 0;;			// ���Ԗڂ̃e�N�X�`���[���g�p����̂�



	g_bAlpaTest = TRUE;
	//g_nAlpha = 0x0;

	// �؂̐ݒ�
	SetTree(XMFLOAT3(-200.0f, 0.0f, 200.0f)   , 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetTree(XMFLOAT3(200.0f, 0.0f, 0.0f) , 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	//SetTree(XMFLOAT3(-200.0f, 0.0f, 0.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	//SetTree(XMFLOAT3(0.0f, 0.0f, 200.0f) , 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	//SetTree(XMFLOAT3(0.0f, 0.0f, -200.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTree(void)
{
	for(int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if(g_Texture[nCntTex] != NULL)
		{// �e�N�X�`���̉��
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if(g_VertexBuffer != NULL)
	{// ���_�o�b�t�@�̉��
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateTree(void)
{

	for(int nCntTree = 0; nCntTree < MAX_TREE; nCntTree++)
	{
		if(g_aTree[nCntTree].use)
		{
			// �e�̈ʒu�ݒ�
			SetPositionShadow(g_aTree[nCntTree].nIdxShadow, XMFLOAT3(g_aTree[nCntTree].pos.x, 0.1f, g_aTree[nCntTree].pos.z));

			// �A�j���[�V����  
			g_countAnim++;
			if ((g_countAnim % ANIM_WAIT) == 0)
			{
				// �p�^�[���̐؂�ւ�
				g_patternAnim = (g_patternAnim + 1) % ANIM_PATTERN_NUM;
				g_patternAnim = (g_patternAnim + 1) % ANIM_PATTERN_NUM;
			}



			// ���_�o�b�t�@����
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.ByteWidth = sizeof(VERTEX_3D) * 4;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

			// ���_���W�̐ݒ�
			// ���_�o�b�t�@�ɒl���Z�b�g����
			D3D11_MAPPED_SUBRESOURCE msr;
			GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

			VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

			float fWidth = 60.0f;
			float fHeight = 90.0f;

			vertex[0].Position = XMFLOAT3(-fWidth , fHeight, 0.0f);
			vertex[1].Position = XMFLOAT3(fWidth, fHeight, 0.0f);
			vertex[2].Position = XMFLOAT3(-fWidth, 0.0f, 0.0f);
			vertex[3].Position = XMFLOAT3(fWidth, 0.0f, 0.0f);

			// �@���̐ݒ�
			vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
			vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
			vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
			vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

			// �g�U���̐ݒ�
			vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			//�A�j���[�V������������
			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = (float)(g_patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(g_patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

			// �e�N�X�`�����W�̐ݒ�
			vertex[0].TexCoord = XMFLOAT2(tx, ty);
			vertex[1].TexCoord = XMFLOAT2(tx + tw, ty);
			vertex[2].TexCoord = XMFLOAT2(tx, ty + th);
			vertex[3].TexCoord = XMFLOAT2(tx + tw, ty + th);

			GetDeviceContext()->Unmap(g_VertexBuffer, 0);

		}
	}


#ifdef _DEBUG
	// �A���t�@�e�X�gON/OFF
	if(GetKeyboardTrigger(DIK_F1))
	{
		g_bAlpaTest = g_bAlpaTest ? FALSE: TRUE;
	}

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTree(void)
{
	// ���e�X�g�ݒ�
	if (g_bAlpaTest == TRUE)
	{
		// ���e�X�g��L����
		SetAlphaTestEnable(TRUE);
	}


	SetFillEnable(TRUE);
	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for(int i = 0; i < MAX_TREE; i++)
	{
		if(g_aTree[i].use)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �r���[�}�g���b�N�X���擾
			mtxView = XMLoadFloat4x4(&cam->mtxView);


			// �֐��g������
			//mtxWorld = XMMatrixInverse(nullptr, mtxView);
			//mtxWorld.r[3].m128_f32[0] = 0.0f;
			//mtxWorld.r[3].m128_f32[1] = 0.0f;
			//mtxWorld.r[3].m128_f32[2] = 0.0f;


			// �����s��i�����s��j��]�u�s�񂳂��ċt�s�������Ă��(����)
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
			mtxScl = XMMatrixScaling(g_aTree[i].scl.x, g_aTree[i].scl.y, g_aTree[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_aTree[i].pos.x, g_aTree[i].pos.y, g_aTree[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);


			// �}�e���A���ݒ�
			SetMaterial(g_aTree[i].material);

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i % TEXTURE_MAX]);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
	SetFillEnable(FALSE);

	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

	// ���e�X�g�𖳌���
	SetAlphaTestEnable(FALSE);
}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexTree(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// ���_�o�b�t�@�ɒl���Z�b�g����
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = 60.0f;
	float fHeight = 90.0f;

	// ���_���W�̐ݒ�
	vertex[0].Position = XMFLOAT3(-fWidth*3 , fHeight*3, 0.0f);
	vertex[1].Position = XMFLOAT3(fWidth *3, fHeight*3, 0.0f);
	vertex[2].Position = XMFLOAT3(-fWidth *3, -30.0f, 0.0f);
	vertex[3].Position = XMFLOAT3(fWidth *3, -30.0f, 0.0f);

	// �@���̐ݒ�
	vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// �g�U���̐ݒ�
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// �e�N�X�`�����W�̐ݒ�
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// �؂̃p�����[�^���Z�b�g
//=============================================================================
int SetTree(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col)
{
	int nIdxTree = -1;

	for(int nCntTree = 0; nCntTree < MAX_TREE; nCntTree++)
	{
		if(!g_aTree[nCntTree].use)
		{
			g_aTree[nCntTree].pos = pos;
			g_aTree[nCntTree].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			g_aTree[nCntTree].fWidth = fWidth;
			g_aTree[nCntTree].fHeight = fHeight;
			g_aTree[nCntTree].use = TRUE;

			// �e�̐ݒ�
			g_aTree[nCntTree].nIdxShadow = CreateShadow(g_aTree[nCntTree].pos, 0.5f, 0.5f);

			nIdxTree = nCntTree;

			break;
		}
	}

	return nIdxTree;
}
