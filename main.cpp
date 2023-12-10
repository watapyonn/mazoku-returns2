//=============================================================================
//
// ���C������ [main.cpp]
// Author : ��ژj��ژj
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "player.h"
#include "enemy.h"
#include "animation.h"
#include "shadow.h"
#include "light.h"
#include "meshfield.h"
#include "meshwall.h"
#include "meshfog.h"
#include "tree.h"
#include "collision.h"
#include "bullet.h"
#include "score.h"
#include "sound.h"
#include "particle.h"
#include "fade.h"
#include "title.h"
#include "select.h"
#include "result.h"
#include "tutorial.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define CLASS_NAME		"AppClass"			// �E�C���h�E�̃N���X��
#define WINDOW_NAME		"���b�V���\��"		// �E�C���h�E�̃L���v�V������

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

void CheckHit(void);


//*****************************************************************************
// �O���[�o���ϐ�:
//*****************************************************************************
long g_MouseX = 0;
long g_MouseY = 0;


#ifdef _DEBUG
int		g_CountFPS;							// FPS�J�E���^
char	g_DebugStr[2048] = WINDOW_NAME;		// �f�o�b�O�����\���p

#endif

int	g_Mode = MODE_TITLE;// �N�����̉�ʂ�ݒ�

//=============================================================================
// ���C���֐�
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	UNREFERENCED_PARAMETER(hPrevInstance);	// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j
	UNREFERENCED_PARAMETER(lpCmdLine);		// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j

	// ���Ԍv���p
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	WNDCLASSEX	wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		NULL,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		//(HBRUSH)(20),
		NULL,
		CLASS_NAME,
		NULL
	};
	HWND		hWnd;
	MSG			msg;
	
	// �E�B���h�E�N���X�̓o�^
	RegisterClassEx(&wcex);

	// �E�B���h�E�̍쐬
	hWnd = CreateWindow(CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,																		// �E�B���h�E�̍����W
		CW_USEDEFAULT,																		// �E�B���h�E�̏���W
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,									// �E�B���h�E����
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),	// �E�B���h�E�c��
		NULL,
		NULL,
		hInstance,
		NULL);

	// ����������(�E�B���h�E���쐬���Ă���s��)
	if(FAILED(Init(hInstance, hWnd, TRUE)))
	{
		return -1;
	}

	// �t���[���J�E���g������
	timeBeginPeriod(1);	// ����\��ݒ�
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// �V�X�e���������~���b�P�ʂŎ擾
	dwCurrentTime = dwFrameCount = 0;

	// �E�C���h�E�̕\��(�����������̌�ɌĂ΂Ȃ��Ƒʖ�)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// ���b�Z�[�W���[�v
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()���Ă΂ꂽ�烋�[�v�I��
				break;
			}
			else
			{
				// ���b�Z�[�W�̖|��Ƒ��o
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
        }
		else
		{
			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1�b���ƂɎ��s
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPS�𑪒肵��������ۑ�
				dwFrameCount = 0;							// �J�E���g���N���A
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))	// 1/60�b���ƂɎ��s
			{
				dwExecLastTime = dwCurrentTime;	// ��������������ۑ�

#ifdef _DEBUG	// �f�o�b�O�ł̎�����FPS��\������
				wsprintf(g_DebugStr, WINDOW_NAME);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
#endif

				Update();			// �X�V����
				Draw();				// �`�揈��

#ifdef _DEBUG	// �f�o�b�O�ł̎������\������
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, g_DebugStr);
#endif

				dwFrameCount++;
			}
		}
	}

	timeEndPeriod(1);				// ����\��߂�

	// �E�B���h�E�N���X�̓o�^������
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// �I������
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// �v���V�[�W��
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		break;

	case WM_MOUSEMOVE:
		g_MouseX = LOWORD(lParam);
		g_MouseY = HIWORD(lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//=============================================================================
// ����������
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	InitRenderer(hInstance, hWnd, bWindow);

	InitLight();

	InitCamera();

	// ���͏����̏�����
	InitInput(hInstance, hWnd);

	//InitSound(hWnd);

	InitFade();

	SetMode(g_Mode);	// ������SetMode�̂܂܂ŁI

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void Uninit(void)
{
	SetMode(MODE_MAX);

	// �t�F�[�h�̏I������
	UninitFade();

	// �J�����̏I������
	UninitCamera();

	//���͂̏I������
	UninitInput();

	// �����_���[�̏I������
	UninitRenderer();


}

//=============================================================================
// �X�V����
//=============================================================================
void Update(void)
{

	// ���͂̍X�V����
	UpdateInput();

	switch (g_Mode)
	{
		// �^�C�g����ʂ̍X�V
	case MODE_TITLE:		

		//�^�C�g���̍X�V
		UpdateTitle();
		break;

		//�`���[�g���A��
	case MODE_TUTORIAL:
		UpdateTutorial();
		break;

		//�X�e�[�W�Z���N�g
	case MODE_SELECT:

		// �J�����X�V
		UpdateCamera();

		// �X�R�A�̍X�V����
		UpdateScore();

		//�X�e�[�W�Z���N�g�̍X�V
		UpdateSelect();

		//�p�[�e�B�N���̍X�V
		UpdateParticle();			
		break;

	case MODE_GAME:

		// �J�����X�V
		UpdateCamera();

		// �n�ʏ����̍X�V
		UpdateMeshField();

		// �Ǐ����̍X�V
		UpdateMeshWall();

		//���̍X�V
		UpdateMeshFog();

		// �v���C���[�̍X�V����
		UpdatePlayer();

		// �G�l�~�[�̍X�V����
		UpdateEnemy();

		// �G�l�~�[�̍X�V����
		UpdateEnemyAnime();

		// �؂̍X�V����
		UpdateTree();

		// �e�̍X�V����
		UpdateBullet();

		// �e�̍X�V����
		UpdateShadow();

		//�p�[�e�B�N���̍X�V
		UpdateParticle();


		// �����蔻��
		CheckHit();


		// �X�R�A�̍X�V����
		UpdateScore();

		break;

	case MODE_RESULT:
		UpdateResult();
		break;
	}

	UpdateFade();			// �t�F�[�h�̍X�V����

}

//=============================================================================
// �`�揈��
//=============================================================================

// 3D�`�敨�̊�{�Z�b�g
void Draw0(void)
{
	// �n�ʂ̕`�揈��
	DrawMeshField();

	// �e�̕`�揈��
	DrawShadow();

	// �v���C���[�̕`�揈��
	DrawPlayer();

	// �G�l�~�[�̕`�揈��
	DrawEnemy();

	// �G�l�~�[�̕`�揈��
	//DrawEnemyAnime();

	// �e�̕`�揈��
	DrawBullet();

	// �ǂ̕`�揈��
	DrawMeshWall();

	// �؂̕`�揈��
	DrawTree();
	//�p�[�e�B�N���̕`�揈��
	DrawParticle();
	SetFillEnable(FALSE);
}


// �`�揈��
void Draw(void)
{
	// �o�b�N�o�b�t�@�N���A
	Clear();
	// ���C�e�B���O��L����
	SetLightEnable(TRUE);
	SetCamera(0);

	// Z��r����
	SetDepthEnable(TRUE);

		// ���[�h�ɂ���ď����𕪂���
	switch (g_Mode)
	{
	case MODE_TITLE:		// �^�C�g����ʂ̕`��

		//3D





		//2D
		// Z��r�Ȃ�
		SetDepthEnable(FALSE);
		// ���C�e�B���O�𖳌�
		SetLightEnable(FALSE);

		DrawTitle();
		// ���ɉ����Q�c������΂��̒��ɏ���


		// ���C�e�B���O��L����
		SetLightEnable(TRUE);
		// Z��r����
		SetDepthEnable(TRUE);
		break;



	case MODE_TUTORIAL:
		//3D

		//2D
				// Z��r�Ȃ�
		SetDepthEnable(FALSE);
		// ���C�e�B���O�𖳌�
		SetLightEnable(FALSE);


		DrawTutorial();

		// ���C�e�B���O��L����
		SetLightEnable(TRUE);
		// Z��r����
		SetDepthEnable(TRUE);

		break;



	case MODE_SELECT:
		//3D

		// ���C�e�B���O��L����
		SetLightEnable(TRUE);

		// Z��r����
		SetDepthEnable(TRUE);

		//SetViewPort(TYPE_FULL_SCREEN);

		//�X�e�[�W�Z���N�g
		DrawSelect();
		//�p�[�e�B�N��
		DrawParticle();

		//2D
		 //Z��r�Ȃ�
		SetDepthEnable(FALSE);
		// ���C�e�B���O�𖳌�
		SetLightEnable(FALSE);

		// �}�g���N�X�ݒ�
		SetWorldViewProjection2D();


		// 2D�X�R�A�̕`�揈��
		DrawScore();
		// ���ɉ����Q�c������΂��̒��ɏ���


		// ���C�e�B���O��L����
		SetLightEnable(TRUE);
		// Z��r����
		SetDepthEnable(TRUE);



		break;

	case MODE_GAME:

		// 3D


		// �v���C���[���_
		XMFLOAT3 pos = GetPlayer()->pos;
		pos.y = 0.0f;			// �J����������h�����߂ɃN���A���Ă���
		SetCameraAT(pos, 0);
		SetCamera(0);

		Draw0();				// 3D�`�敨�̊�{�Z�b�g




		//�QD
		// Z��r�Ȃ�
		SetDepthEnable(FALSE);
		// ���C�e�B���O�𖳌�
		SetLightEnable(FALSE);

		// �}�g���N�X�ݒ�
		SetWorldViewProjection2D();

		// ���ɉ����Q�c������΂��̒��ɏ���
					
		DrawScore();		// 2D�X�R�A�̕`�揈��	

		// ���C�e�B���O��L����
		SetLightEnable(TRUE);
		// Z��r����
		SetDepthEnable(TRUE);

		

			break;
		case MODE_RESULT:
			SetViewPort(TYPE_FULL_SCREEN);

			// Z��r�Ȃ�
			SetDepthEnable(FALSE);

			// ���C�e�B���O�𖳌�
			SetLightEnable(FALSE);

			// �}�g���N�X�ݒ�
			SetWorldViewProjection2D();

			// ���ɉ����Q�c������΂��̒��ɏ���
			DrawResult();

			// ���C�e�B���O��L����
			SetLightEnable(TRUE);

			// Z��r����
			SetDepthEnable(TRUE);



		break;

	}

	// Z��r�Ȃ�
	SetDepthEnable(FALSE);

	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);


	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();


		DrawFade();				// �t�F�[�h��ʂ̕`��

	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

	// Z��r����
	SetDepthEnable(TRUE);




#ifdef _DEBUG
	// �f�o�b�O�\��
	DrawDebugProc();
#endif

	// �o�b�N�o�b�t�@�A�t�����g�o�b�t�@����ւ�
	Present();
}


long GetMousePosX(void)
{
	return g_MouseX;
}


long GetMousePosY(void)
{
	return g_MouseY;
}


#ifdef _DEBUG
char* GetDebugStr(void)
{
	return g_DebugStr;
}
#endif



//=============================================================================
// �����蔻�菈��
//=============================================================================
void CheckHit(void)
{
	ENEMY  *enemy  = GetEnemy();	// �G�l�~�[�̃|�C���^�[��������
	PLAYER *player = GetPlayer();	// �v���C���[�̃|�C���^�[��������
	PLAYER *parts  = GetParts(0);	//�v���C���[�p�[�c
	BULLET *bullet = GetBullet();	// �e�̃|�C���^�[��������

	// �G�ƃv���C���[�L����
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		// �v���C���[������ł��玟�̓z��
		if (player[i].use == FALSE) continue;

		for (int j = 0; j < MAX_ENEMY; j++)
		{
			//�G������ł��玟�̓z��
			if (enemy[j].use == FALSE) continue;

			//BC�̓����蔻��
			if (CollisionBC(player[i].pos, enemy[j].pos,player[0].size*1.5f, enemy[j].size))
			{
				// �G�L�����N�^�[�͓|�����
				enemy[j].use = FALSE;
				ReleaseShadow(enemy[j].shadowIdx);

				// �X�R�A�𑫂�
				AddScore(ADD_SCORE_HITOBJ);

			}
		}
	}


	// �v���C���[�̒e�ƓG
	for (int i = 0; i < MAX_BULLET; i++)
	{
		//�e�̗L���t���O���`�F�b�N����
		if (bullet[i].use == FALSE) continue;

		// �G�Ɠ������Ă邩���ׂ�
		for (int j = 0; j < MAX_ENEMY; j++)
		{
			//�G�̗L���t���O���`�F�b�N����
			if (enemy[j].use == FALSE) continue;

			//BC�̓����蔻��
			if (CollisionBC(bullet[i].pos, enemy[j].pos, bullet[i].fWidth, enemy[j].size))
			{
				// �����������疢�g�p�ɖ߂�
				bullet[i].use = FALSE;
				ReleaseShadow(bullet[i].shadowIdx);

				// �G�L�����N�^�[�͓|�����
				enemy[j].use = FALSE;
				ReleaseShadow(enemy[j].shadowIdx);

				// �X�R�A�𑫂�
				AddScore(ADD_SCORE_HITBULLET);
			}
		}

	}


	// �G�l�~�[���S�����S�������ԑJ��
	int enemy_count = 0;
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemy[i].use == FALSE) continue;
		enemy_count++;
	}

	// �G�l�~�[���O�C�H
	if (enemy_count == 0)
	{
		// SetFade(FADE_OUT, MODE_RESULT);

	}

}
//=============================================================================
// ���[�h�̐ݒ�
//=============================================================================
void SetMode(int mode)
{
	//// ���[�h��ς���O�ɑS������������������Ⴄ
	//StopSound();			// �܂��Ȃ��~�߂�
	// �^�C�g���̏�����
	UninitTitle();
	// �`���[�g���A���̏I������
	UninitTutorial();

	//���U���g�I������
	UninitResult();

	// �X�R�A�̏I������
	UninitScore();

	// �e�̏I������
	UninitBullet();

	// �؂̏I������
	UninitTree();

	// �G�l�~�[�̏I������
	UninitEnemy();
	// �G�l�~�[�̏I������
	UninitEnemyAnime();

	// �v���C���[�̏I������
	UninitPlayer();

	// �e�̏I������
	UninitShadow();

	// �ǂ̏I������
	UninitMeshWall();
	// �n�ʂ̏I������
	UninitMeshField();
	UninitMeshFog();
	//�T�E���h�̏I������
	//UninitSound();
	// 
	//�p�[�e�B�N���̏I������
	UninitParticle();

	//�X�e�[�W�Z���N�g�I������
	UninitSelect();

	g_Mode = mode;	// ���̃��[�h���Z�b�g���Ă���

	switch (g_Mode)
	{
	case MODE_TITLE:
		// �^�C�g����ʂ̏�����
		InitTitle();
		//PlaySound(SOUND_LABEL_BGM_TITLE);

		break;


	case MODE_TUTORIAL:
		// �`���[�g���A���̏�����
		InitTutorial();

		break;

	case MODE_SELECT:
		// �X�R�A�̏�����
		InitScore();
		InitParticle();

		InitSelect();
		break;

	case MODE_GAME:
		//PlaySound(SOUND_LABEL_BOSS_SINKA);
		//SetVolume(SOUND_LABEL_BOSS_SINKA, 1.0f);
		// �t�B�[���h�̏�����
		InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4 (1.0f, 1.0f, 1.0f, 1.0f),100, 100, 13.0f, 13.0f);



		// �ǂ̏�����
		InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
		InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
		InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
		InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, XM_PI, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);

		// ��(�����p�̔�����)
		InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, XM_PI, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
		InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
		InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
		InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);


		InitMeshFog(XMFLOAT3(0.0f, 10.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 100, 100, 13.0f, 13.0f);
		// �e�̏���������
		InitShadow();

		// �v���C���[�̏�����
		InitPlayer();

		// �G�l�~�[�̏�����
		InitEnemy();
		// �G�l�~�[�̏�����
		InitEnemyAnime();



		// �؂𐶂₷
		InitTree();

		// �e�̏�����
		InitBullet();

		// �X�R�A�̏�����
		InitScore();

		SetVolume(1.0f);
		// ���C�g��L����
		SetLightEnable(TRUE);
		InitParticle();


		// �w�ʃ|���S�����J�����O
		SetCullingMode(CULL_MODE_BACK);

		break;

	case MODE_RESULT:
		InitResult();
		break;

	}
}

//=============================================================================
// ���[�h�̎擾
//=============================================================================
int GetMode(void)
{
	return g_Mode;
}
