//=============================================================================
//
// メイン処理 [main.cpp]
// Author : 井芹亘井芹亘
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
// マクロ定義
//*****************************************************************************
#define CLASS_NAME		"AppClass"			// ウインドウのクラス名
#define WINDOW_NAME		"メッシュ表示"		// ウインドウのキャプション名

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

void CheckHit(void);


//*****************************************************************************
// グローバル変数:
//*****************************************************************************
long g_MouseX = 0;
long g_MouseY = 0;


#ifdef _DEBUG
int		g_CountFPS;							// FPSカウンタ
char	g_DebugStr[2048] = WINDOW_NAME;		// デバッグ文字表示用

#endif

int	g_Mode = MODE_TITLE;// 起動時の画面を設定

//=============================================================================
// メイン関数
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	UNREFERENCED_PARAMETER(hPrevInstance);	// 無くても良いけど、警告が出る（未使用宣言）
	UNREFERENCED_PARAMETER(lpCmdLine);		// 無くても良いけど、警告が出る（未使用宣言）

	// 時間計測用
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
	
	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// ウィンドウの作成
	hWnd = CreateWindow(CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,																		// ウィンドウの左座標
		CW_USEDEFAULT,																		// ウィンドウの上座標
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,									// ウィンドウ横幅
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),	// ウィンドウ縦幅
		NULL,
		NULL,
		hInstance,
		NULL);

	// 初期化処理(ウィンドウを作成してから行う)
	if(FAILED(Init(hInstance, hWnd, TRUE)))
	{
		return -1;
	}

	// フレームカウント初期化
	timeBeginPeriod(1);	// 分解能を設定
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// システム時刻をミリ秒単位で取得
	dwCurrentTime = dwFrameCount = 0;

	// ウインドウの表示(初期化処理の後に呼ばないと駄目)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// メッセージループ
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()が呼ばれたらループ終了
				break;
			}
			else
			{
				// メッセージの翻訳と送出
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
        }
		else
		{
			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1秒ごとに実行
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPSを測定した時刻を保存
				dwFrameCount = 0;							// カウントをクリア
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))	// 1/60秒ごとに実行
			{
				dwExecLastTime = dwCurrentTime;	// 処理した時刻を保存

#ifdef _DEBUG	// デバッグ版の時だけFPSを表示する
				wsprintf(g_DebugStr, WINDOW_NAME);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
#endif

				Update();			// 更新処理
				Draw();				// 描画処理

#ifdef _DEBUG	// デバッグ版の時だけ表示する
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, g_DebugStr);
#endif

				dwFrameCount++;
			}
		}
	}

	timeEndPeriod(1);				// 分解能を戻す

	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// 終了処理
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// プロシージャ
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
// 初期化処理
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	InitRenderer(hInstance, hWnd, bWindow);

	InitLight();

	InitCamera();

	// 入力処理の初期化
	InitInput(hInstance, hWnd);

	//InitSound(hWnd);

	InitFade();

	SetMode(g_Mode);	// ここはSetModeのままで！

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void Uninit(void)
{
	SetMode(MODE_MAX);

	// フェードの終了処理
	UninitFade();

	// カメラの終了処理
	UninitCamera();

	//入力の終了処理
	UninitInput();

	// レンダラーの終了処理
	UninitRenderer();


}

//=============================================================================
// 更新処理
//=============================================================================
void Update(void)
{

	// 入力の更新処理
	UpdateInput();

	switch (g_Mode)
	{
		// タイトル画面の更新
	case MODE_TITLE:		

		//タイトルの更新
		UpdateTitle();
		break;

		//チュートリアル
	case MODE_TUTORIAL:
		UpdateTutorial();
		break;

		//ステージセレクト
	case MODE_SELECT:

		// カメラ更新
		UpdateCamera();

		// スコアの更新処理
		UpdateScore();

		//ステージセレクトの更新
		UpdateSelect();

		//パーティクルの更新
		UpdateParticle();			
		break;

	case MODE_GAME:

		// カメラ更新
		UpdateCamera();

		// 地面処理の更新
		UpdateMeshField();

		// 壁処理の更新
		UpdateMeshWall();

		//霧の更新
		UpdateMeshFog();

		// プレイヤーの更新処理
		UpdatePlayer();

		// エネミーの更新処理
		UpdateEnemy();

		// エネミーの更新処理
		UpdateEnemyAnime();

		// 木の更新処理
		UpdateTree();

		// 弾の更新処理
		UpdateBullet();

		// 影の更新処理
		UpdateShadow();

		//パーティクルの更新
		UpdateParticle();


		// 当たり判定
		CheckHit();


		// スコアの更新処理
		UpdateScore();

		break;

	case MODE_RESULT:
		UpdateResult();
		break;
	}

	UpdateFade();			// フェードの更新処理

}

//=============================================================================
// 描画処理
//=============================================================================

// 3D描画物の基本セット
void Draw0(void)
{
	// 地面の描画処理
	DrawMeshField();

	// 影の描画処理
	DrawShadow();

	// プレイヤーの描画処理
	DrawPlayer();

	// エネミーの描画処理
	DrawEnemy();

	// エネミーの描画処理
	//DrawEnemyAnime();

	// 弾の描画処理
	DrawBullet();

	// 壁の描画処理
	DrawMeshWall();

	// 木の描画処理
	DrawTree();
	//パーティクルの描画処理
	DrawParticle();
	SetFillEnable(FALSE);
}


// 描画処理
void Draw(void)
{
	// バックバッファクリア
	Clear();
	// ライティングを有効に
	SetLightEnable(TRUE);
	SetCamera(0);

	// Z比較あり
	SetDepthEnable(TRUE);

		// モードによって処理を分ける
	switch (g_Mode)
	{
	case MODE_TITLE:		// タイトル画面の描画

		//3D





		//2D
		// Z比較なし
		SetDepthEnable(FALSE);
		// ライティングを無効
		SetLightEnable(FALSE);

		DrawTitle();
		// 他に何か２Ｄ物あればこの中に書く


		// ライティングを有効に
		SetLightEnable(TRUE);
		// Z比較あり
		SetDepthEnable(TRUE);
		break;



	case MODE_TUTORIAL:
		//3D

		//2D
				// Z比較なし
		SetDepthEnable(FALSE);
		// ライティングを無効
		SetLightEnable(FALSE);


		DrawTutorial();

		// ライティングを有効に
		SetLightEnable(TRUE);
		// Z比較あり
		SetDepthEnable(TRUE);

		break;



	case MODE_SELECT:
		//3D

		// ライティングを有効に
		SetLightEnable(TRUE);

		// Z比較あり
		SetDepthEnable(TRUE);

		//SetViewPort(TYPE_FULL_SCREEN);

		//ステージセレクト
		DrawSelect();
		//パーティクル
		DrawParticle();

		//2D
		 //Z比較なし
		SetDepthEnable(FALSE);
		// ライティングを無効
		SetLightEnable(FALSE);

		// マトリクス設定
		SetWorldViewProjection2D();


		// 2Dスコアの描画処理
		DrawScore();
		// 他に何か２Ｄ物あればこの中に書く


		// ライティングを有効に
		SetLightEnable(TRUE);
		// Z比較あり
		SetDepthEnable(TRUE);



		break;

	case MODE_GAME:

		// 3D


		// プレイヤー視点
		XMFLOAT3 pos = GetPlayer()->pos;
		pos.y = 0.0f;			// カメラ酔いを防ぐためにクリアしている
		SetCameraAT(pos, 0);
		SetCamera(0);

		Draw0();				// 3D描画物の基本セット




		//２D
		// Z比較なし
		SetDepthEnable(FALSE);
		// ライティングを無効
		SetLightEnable(FALSE);

		// マトリクス設定
		SetWorldViewProjection2D();

		// 他に何か２Ｄ物あればこの中に書く
					
		DrawScore();		// 2Dスコアの描画処理	

		// ライティングを有効に
		SetLightEnable(TRUE);
		// Z比較あり
		SetDepthEnable(TRUE);

		

			break;
		case MODE_RESULT:
			SetViewPort(TYPE_FULL_SCREEN);

			// Z比較なし
			SetDepthEnable(FALSE);

			// ライティングを無効
			SetLightEnable(FALSE);

			// マトリクス設定
			SetWorldViewProjection2D();

			// 他に何か２Ｄ物あればこの中に書く
			DrawResult();

			// ライティングを有効に
			SetLightEnable(TRUE);

			// Z比較あり
			SetDepthEnable(TRUE);



		break;

	}

	// Z比較なし
	SetDepthEnable(FALSE);

	// ライティングを無効
	SetLightEnable(FALSE);


	// マトリクス設定
	SetWorldViewProjection2D();


		DrawFade();				// フェード画面の描画

	// ライティングを有効に
	SetLightEnable(TRUE);

	// Z比較あり
	SetDepthEnable(TRUE);




#ifdef _DEBUG
	// デバッグ表示
	DrawDebugProc();
#endif

	// バックバッファ、フロントバッファ入れ替え
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
// 当たり判定処理
//=============================================================================
void CheckHit(void)
{
	ENEMY  *enemy  = GetEnemy();	// エネミーのポインターを初期化
	PLAYER *player = GetPlayer();	// プレイヤーのポインターを初期化
	PLAYER *parts  = GetParts(0);	//プレイヤーパーツ
	BULLET *bullet = GetBullet();	// 弾のポインターを初期化

	// 敵とプレイヤーキャラ
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		// プレイヤーが死んでたら次の奴へ
		if (player[i].use == FALSE) continue;

		for (int j = 0; j < MAX_ENEMY; j++)
		{
			//敵が死んでたら次の奴へ
			if (enemy[j].use == FALSE) continue;

			//BCの当たり判定
			if (CollisionBC(player[i].pos, enemy[j].pos,player[0].size*1.5f, enemy[j].size))
			{
				// 敵キャラクターは倒される
				enemy[j].use = FALSE;
				ReleaseShadow(enemy[j].shadowIdx);

				// スコアを足す
				AddScore(ADD_SCORE_HITOBJ);

			}
		}
	}


	// プレイヤーの弾と敵
	for (int i = 0; i < MAX_BULLET; i++)
	{
		//弾の有効フラグをチェックする
		if (bullet[i].use == FALSE) continue;

		// 敵と当たってるか調べる
		for (int j = 0; j < MAX_ENEMY; j++)
		{
			//敵の有効フラグをチェックする
			if (enemy[j].use == FALSE) continue;

			//BCの当たり判定
			if (CollisionBC(bullet[i].pos, enemy[j].pos, bullet[i].fWidth, enemy[j].size))
			{
				// 当たったから未使用に戻す
				bullet[i].use = FALSE;
				ReleaseShadow(bullet[i].shadowIdx);

				// 敵キャラクターは倒される
				enemy[j].use = FALSE;
				ReleaseShadow(enemy[j].shadowIdx);

				// スコアを足す
				AddScore(ADD_SCORE_HITBULLET);
			}
		}

	}


	// エネミーが全部死亡したら状態遷移
	int enemy_count = 0;
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemy[i].use == FALSE) continue;
		enemy_count++;
	}

	// エネミーが０匹？
	if (enemy_count == 0)
	{
		// SetFade(FADE_OUT, MODE_RESULT);

	}

}
//=============================================================================
// モードの設定
//=============================================================================
void SetMode(int mode)
{
	//// モードを変える前に全部メモリを解放しちゃう
	//StopSound();			// まず曲を止める
	// タイトルの初期化
	UninitTitle();
	// チュートリアルの終了処理
	UninitTutorial();

	//リザルト終了処理
	UninitResult();

	// スコアの終了処理
	UninitScore();

	// 弾の終了処理
	UninitBullet();

	// 木の終了処理
	UninitTree();

	// エネミーの終了処理
	UninitEnemy();
	// エネミーの終了処理
	UninitEnemyAnime();

	// プレイヤーの終了処理
	UninitPlayer();

	// 影の終了処理
	UninitShadow();

	// 壁の終了処理
	UninitMeshWall();
	// 地面の終了処理
	UninitMeshField();
	UninitMeshFog();
	//サウンドの終了処理
	//UninitSound();
	// 
	//パーティクルの終了処理
	UninitParticle();

	//ステージセレクト終了処理
	UninitSelect();

	g_Mode = mode;	// 次のモードをセットしている

	switch (g_Mode)
	{
	case MODE_TITLE:
		// タイトル画面の初期化
		InitTitle();
		//PlaySound(SOUND_LABEL_BGM_TITLE);

		break;


	case MODE_TUTORIAL:
		// チュートリアルの初期化
		InitTutorial();

		break;

	case MODE_SELECT:
		// スコアの初期化
		InitScore();
		InitParticle();

		InitSelect();
		break;

	case MODE_GAME:
		//PlaySound(SOUND_LABEL_BOSS_SINKA);
		//SetVolume(SOUND_LABEL_BOSS_SINKA, 1.0f);
		// フィールドの初期化
		InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4 (1.0f, 1.0f, 1.0f, 1.0f),100, 100, 13.0f, 13.0f);



		// 壁の初期化
		InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
		InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
		InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
		InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, XM_PI, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);

		// 壁(裏側用の半透明)
		InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, XM_PI, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
		InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
		InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
		InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);


		InitMeshFog(XMFLOAT3(0.0f, 10.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 100, 100, 13.0f, 13.0f);
		// 影の初期化処理
		InitShadow();

		// プレイヤーの初期化
		InitPlayer();

		// エネミーの初期化
		InitEnemy();
		// エネミーの初期化
		InitEnemyAnime();



		// 木を生やす
		InitTree();

		// 弾の初期化
		InitBullet();

		// スコアの初期化
		InitScore();

		SetVolume(1.0f);
		// ライトを有効化
		SetLightEnable(TRUE);
		InitParticle();


		// 背面ポリゴンをカリング
		SetCullingMode(CULL_MODE_BACK);

		break;

	case MODE_RESULT:
		InitResult();
		break;

	}
}

//=============================================================================
// モードの取得
//=============================================================================
int GetMode(void)
{
	return g_Mode;
}
