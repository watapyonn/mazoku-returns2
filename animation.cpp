//=============================================================================
//
// エネミーモデル処理 [enemy.cpp]
// Author : 井芹亘
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "input.h"
#include "debugproc.h"
#include "animation.h"
#include "shadow.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_ENEMY0			"data/MODEL/bade1.obj"		// 読み込むモデル名
#define	MODEL_ENEMY1			"data/MODEL/bade2.obj"		// 読み込むモデル名
#define	MODEL_ENEMY2			"data/MODEL/bade3.obj"		// 読み込むモデル名

#define	VALUE_MOVE			(5.0f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量

#define ENEMY_SHADOW_SIZE	(0.4f)						// 影の大きさ
#define ENEMY_OFFSET_Y		(7.0f)						// エネミーの足元をあわせる


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ENEMY_ANIME			g_Enemy[MAX_ENEM_ANIME];				// エネミー

int g_Enemy_loadanime = 0;
int g_animecnt = 0;		//アニメーションのカウント
BOOL g_animefrg = TRUE;
static INTERPOLATION_DATA g_MoveTbl0[5];// = {
	////座標									回転率							拡大率							時間
	//{ XMFLOAT3(50.0f,  0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(30.0f, 30.0f, 30.0f),	60 },
	//{ XMFLOAT3(250.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 3.14f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
	//{ XMFLOAT3(250.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 6.28f),	XMFLOAT3(2.0f, 2.0f, 2.0f),	60 },
//};


static INTERPOLATION_DATA g_MoveTbl1[] = {
	//座標									回転率							拡大率							時間
	{ XMFLOAT3(500.0f,   0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
	{ XMFLOAT3(500.0f,  0.0f, 0.0f),XMFLOAT3(0.0f, 0.0f, 6.28f),	XMFLOAT3(2.0f, 2.0f, 1.0f),	60 },
};


static INTERPOLATION_DATA g_MoveTbl2[] = {
	//座標									回転率							拡大率							時間
	{ XMFLOAT3(300.0f, 100.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
	{ XMFLOAT3(300.0, 100.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 6.28f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
};


static INTERPOLATION_DATA* g_MoveTblAdr[] =
{
	g_MoveTbl0,
	g_MoveTbl1,
	g_MoveTbl2,

};


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemyAnime(void)
{
	for (int i = 0; i < MAX_ENEM_ANIME; i++)
	{

		g_Enemy[i].load = TRUE;

		g_Enemy[i].pos = XMFLOAT3(-50.0f + i * 30.0f, 7.0f, 20.0f);
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Enemy[i].spd  = 0.0f;			// 移動スピードクリア
		g_Enemy[i].size = ENEMY_SIZE;	// 当たり判定の大きさ

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Enemy[0].model[0], &g_Enemy[0].diffuse[0]);
		GetModelDiffuse(&g_Enemy[1].model[1], &g_Enemy[1].diffuse[1]);
		GetModelDiffuse(&g_Enemy[2].model[2], &g_Enemy[2].diffuse[2]);

		XMFLOAT3 pos = g_Enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		//g_Enemy[0].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);
		
		g_Enemy[0].use = TRUE;		// TRUE:生きてる

	// 0番だけ線形補間で動かしてみる
		g_Enemy[i].time =(float) 0.0f+0.1f*i;		// 線形補間用のタイマーをクリア
		g_Enemy[i].tblNo = 0;		// 再生するアニメデータの先頭アドレスをセット
		g_Enemy[i].tblMax = sizeof(g_MoveTbl0) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット


	}
	LoadModel(MODEL_ENEMY0, &g_Enemy[0].model[0]);
	LoadModel(MODEL_ENEMY1, &g_Enemy[1].model[1]);
	LoadModel(MODEL_ENEMY2, &g_Enemy[2].model[2]);



	// 0番だけ線形補間で動かしてみる
	g_Enemy[0].time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Enemy[0].tblNo = 0;		// 再生するアニメデータの先頭アドレスをセット
	g_Enemy[0].tblMax = sizeof(g_MoveTbl0) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	//// 1番だけ線形補間で動かしてみる
	//g_Enemy[1].time = 0.0f;		// 線形補間用のタイマーをクリア
	//g_Enemy[1].tblNo = 1;		// 再生するアニメデータの先頭アドレスをセット
	//g_Enemy[1].tblMax = sizeof(g_MoveTbl1) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	//// 2番だけ線形補間で動かしてみる
	//g_Enemy[2].time = 0.0f;		// 線形補間用のタイマーをクリア
	//g_Enemy[2].tblNo = 2;		// 再生するアニメデータの先頭アドレスをセット
	//g_Enemy[2].tblMax = sizeof(g_MoveTbl2) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット


	//// 2番だけ線形補間で動かしてみる
	//g_Enemy[2].time = 0.0f;		// 線形補間用のタイマーをクリア
	//g_Enemy[2].tblNo = 2;		// 再生するアニメデータの先頭アドレスをセット
	//g_Enemy[2].tblMax = sizeof(g_MoveTbl2) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット


	for (int tree = 0; tree < 5; tree++)
	{

		g_MoveTbl0[0] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 };
		g_MoveTbl0[1] = { XMFLOAT3(200.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*2 };
		g_MoveTbl0[2] = { XMFLOAT3(-200.0f, 0.0f, 00.0f),XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), (int)60*1.4 };
		g_MoveTbl0[3] = { XMFLOAT3(0.0f, 0.0f, 200.0f),XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*2 };
		g_MoveTbl0[4] = { XMFLOAT3(0.0f, 0.0f, -200.0f),XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 };



	}



	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemyAnime(void)



{

	for (int i = 0; i < MAX_ENEM_ANIME; i++)
	{
		if (g_Enemy[i].load)
		{
			UnloadModel(&g_Enemy[i].model[i]);

			g_Enemy[i].load = FALSE;
		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEnemyAnime(void)
{
				// エネミーを動かく場合(XMFLOAT3(0.0f, 0.0f, -200.0f),は、影も合わせて動かす事を忘れないようにね！
	for (int i = 0; i < MAX_ENEM_ANIME; i++)
	{
		if (g_Enemy[0].use == TRUE)		// このエネミーが使われている？
		{								// Yes



			// 影もプレイヤーの位置に合わせる
			XMFLOAT3 pos = g_Enemy[0].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			SetPositionShadow(g_Enemy[0].shadowIdx, pos);






			//// 移動処理
			//if (g_Enemy[i].tblMax > 0)	// 線形補間を実行する？
			//{	// 線形補間の処理
			//	int nowNo = (int)g_Enemy[i].time;			// 整数分であるテーブル番号を取り出している
			//	int maxNo = g_Enemy[i].tblMax;				// 登録テーブル数を数えている
			//	int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
			//	INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Enemy[i].tblNo];	// 行動テーブルのアドレスを取得

			//	XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
			//	XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
			//	XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

			//	XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
			//	XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
			//	XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

			//	float nowTime = g_Enemy[i].time - nowNo;	// 時間部分である少数を取り出している

			//	Pos *= nowTime;								// 現在の移動量を計算している
			//	Rot *= nowTime;								// 現在の回転量を計算している
			//	Scl *= nowTime;								// 現在の拡大率を計算している

			//	// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
			//	XMStoreFloat3(&g_Enemy[i].pos, nowPos + Pos);

			//	// 計算して求めた回転量を現在の移動テーブルに足している
			//	XMStoreFloat3(&g_Enemy[i].rot, nowRot + Rot);

			//	// 計算して求めた拡大率を現在の移動テーブルに足している
			//	XMStoreFloat3(&g_Enemy[i].scl, nowScl + Scl);
			//	//g_Enemy[i].w = TEXTURE_WIDTH * g_Enemy[i].scl.x;
			//	//g_Enemy[i].h = TEXTURE_HEIGHT * g_Enemy[i].scl.y;
			//	// frameを使て時間経過処理をする
			//	g_Enemy[i].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
			//	if ((int)g_Enemy[i].time >= maxNo)			// 登録テーブル最後まで移動したか？
			//	{
			//		g_Enemy[i].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
			//	}


			//}



		}
	}




#ifdef _DEBUG

	if (GetKeyboardTrigger(DIK_F3))
	{
		g_animefrg = (g_animefrg) ? FALSE : TRUE;
			//g_Light[0].Enable = (g_Light[0].Enable) ? false : true;

	}


	//if (GetKeyboardTrigger(DIK_P))
	//{
	//	// モデルの色を変更できるよ！半透明にもできるよ。
	//	for (int j = 0; j < g_Enemy[0].model.SubsetNum; j++)
	//	{
	//		SetModelDiffuse(&g_Enemy[0].model, j, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.5f));
	//	}
	//}

	//if (GetKeyboardTrigger(DIK_L))
	//{
	//	// モデルの色を元に戻している
	//	for (int j = 0; j < g_Enemy[0].model.SubsetNum; j++)
	//	{
	//		SetModelDiffuse(&g_Enemy[0].model, j, g_Enemy[0].diffuse[j]);
	//	}
	//}
#endif


}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemyAnime(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	//for (int i = 0; i < MAX_ENEMY; i++)
	//{
	if (g_Enemy[0].use == TRUE) //continue;
	{
		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Enemy[0].scl.x, g_Enemy[0].scl.y, g_Enemy[0].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[0].rot.x, g_Enemy[0].rot.y + XM_PI, g_Enemy[0].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Enemy[0].pos.x, g_Enemy[0].pos.y, g_Enemy[0].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[0].mtxWorld, mtxWorld);
		DrawModel(&g_Enemy[0].model[0]);

		g_animecnt++;
		if (g_animefrg == TRUE)
		{
			// モデル描画(ここでアニメーションをやっている能近)
			// 
			//DrawModel(&g_Enemy[i].model);
			switch (g_animecnt % 9)
			{
			case 0:
			case 1:
			case 2:
				DrawModel(&g_Enemy[0].model[0]);
				break;
			case 3:
			case 4:
			case 5:

				DrawModel(&g_Enemy[1].model[1]);
				break;
			case 6:
			case 7:
			case 8:

				DrawModel(&g_Enemy[2].model[2]);
				break;

			}
		}
	}
//	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// エネミーの取得
//=============================================================================
ENEMY_ANIME*GetEnemyAnime()
{
	return &g_Enemy[0];
}
