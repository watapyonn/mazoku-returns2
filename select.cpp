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
#include "select.h"
#include "shadow.h"
#include "sound.h"
#include "fade.h"
#include "particle.h"
#include "collision.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_SELECT_BOLL			"data/MODEL/pokemonboll.obj"		// 読み込むモデル名
#define	MODEL_SELECT_SPHERE			"data/MODEL/sphere.obj"		// 読み込むモデル名
#define	MODEL_SELECT_TAMA			"data/MODEL/sphere2.obj"		// 読み込むモデル名

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
static SELECT			g_Select[MAX_SELECT];				// エネミー

int g_Sleect_load = 0;

BOOL		g_isTriggerslinear = TRUE;

static INTERPOLATION_DATA g_MoveTbl0[] = {
	//座標									回転率							拡大率							時間
	{ XMFLOAT3(  0.0f,   0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(2.0f, 2.0f, 2.0f),	120 },
	{ XMFLOAT3( 100.0f,  0.0f, 100.0f),	XMFLOAT3(0.0f, 0.0f, 3.14f),XMFLOAT3(1.0f, 1.0f, 1.0f),	120 },
	{ XMFLOAT3(-100.0f,  0.0f, 100.0f),	XMFLOAT3(0.0f, 0.0f, 6.28f),XMFLOAT3(1.0f, 1.0f, 1.0f),	120 },

};


static INTERPOLATION_DATA g_MoveTbl1[] = {
	//座標									回転率							拡大率							時間
	{ XMFLOAT3( 100.0f,  0.0f, 100.0f),	XMFLOAT3(0.0f, 0.0f, 3.14f),XMFLOAT3(1.0f, 1.0f, 1.0f),	120 },
	{ XMFLOAT3(-100.0f,  0.0f, 100.0f),	XMFLOAT3(0.0f, 0.0f, 6.28f),XMFLOAT3(1.0f, 1.0f, 1.0f),	120 },
	{ XMFLOAT3(   0.0f,   0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(2.0f, 2.0f, 2.0f),	120 },
};


static INTERPOLATION_DATA g_MoveTbl2[] = {
	//座標									回転率							拡大率							時間
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
// 初期化処理
//=============================================================================
HRESULT InitSelect(void)
{
	for (int i = 0; i < MAX_SELECT; i++)
	{
		
		g_Select[i].load = TRUE;

		g_Select[i].pos = XMFLOAT3(-50.0f + i * 30.0f, 7.0f, 20.0f);
		g_Select[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Select[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Select[i].model, &g_Select[i].diffuse[0]);

		//XMFLOAT3 pos = g_Select[i].pos;
		//g_Select[i].shadowIdx = CreateShadow(pos, SELECT_SHADOW_SIZE, SELECT_SHADOW_SIZE);
		
		g_Select[i].use = TRUE;		// TRUE:生きてる


	}
	LoadModel(MODEL_SELECT_BOLL		, &g_Select[0].model);
	LoadModel(MODEL_SELECT_SPHERE	, &g_Select[1].model);
	LoadModel(MODEL_SELECT_TAMA		, &g_Select[2].model);



	// 0番だけ線形補間で動かしてみる
	g_Select[0].time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Select[0].tblNo = 0;		// 再生するアニメデータの先頭アドレスをセット
	g_Select[0].tblMax = sizeof(g_MoveTbl0) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	// 1番だけ線形補間で動かしてみる
	g_Select[1].time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Select[1].tblNo = 1;		// 再生するアニメデータの先頭アドレスをセット
	g_Select[1].tblMax = sizeof(g_MoveTbl1) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	// 2番だけ線形補間で動かしてみる
	g_Select[2].time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Select[2].tblNo = 2;		// 再生するアニメデータの先頭アドレスをセット
	g_Select[2].tblMax = sizeof(g_MoveTbl2) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	return S_OK;
}

//=============================================================================
// 終了処理
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
// 更新処理
//=============================================================================
void UpdateSelect(void)
{
				// エネミーを動かく場合(XMFLOAT3(0.0f, 0.0f, -200.0f),は、影も合わせて動かす事を忘れないようにね！
	for (int i = 0; i < MAX_SELECT; i++)
	{
		if (g_Select[i].use == TRUE)		// このエネミーが使われている？
		{								// Yes
			if (GetKeyboardPress(DIK_LEFT))
			{

			// 移動処理
				if (g_Select[i].tblMax > 0)	// 線形補間を実行する？
				{	// 線形補間の処理
					int nowNo = (int)g_Select[i].time;			// 整数分であるテーブル番号を取り出している
					int maxNo = g_Select[i].tblMax;				// 登録テーブル数を数えている
					int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
					INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Select[i].tblNo];	// 行動テーブルのアドレスを取得

					XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
					XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
					XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

					XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
					XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
					XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

					float nowTime = g_Select[i].time - nowNo;	// 時間部分である少数を取り出している

					Pos *= nowTime;								// 現在の移動量を計算している
					Rot *= nowTime;								// 現在の回転量を計算している
					Scl *= nowTime;								// 現在の拡大率を計算している

					// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
					XMStoreFloat3(&g_Select[i].pos, nowPos + Pos);

					// 計算して求めた回転量を現在の移動テーブルに足している
					XMStoreFloat3(&g_Select[i].rot, nowRot + Rot);

					// 計算して求めた拡大率を現在の移動テーブルに足している
					XMStoreFloat3(&g_Select[i].scl, nowScl + Scl);


					// frameを使て時間経過処理をする
					g_Select[i].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
					if ((int)g_Select[i].time >= (float)maxNo - 2.0f)			// 登録テーブル最後まで移動したか？
					{
						g_Select[i].time = (float)maxNo - 2.0f;				// ０番目にリセットしつつも小数部分を引き継いでいる
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


				//// 一回だけのアニメーションをさせる
				//g_Select[i].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
				//if ((int)g_Select[i].time >= maxNo-1 )			// 登録テーブル最後まで移動したか？
				//{
				//	g_Select[i].time = maxNo-1;				// ０番目にリセットしつつも小数部分を引き継いでいる
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





	//ベジェ曲線入れる？
	//// 弾の移動処理 ベジェ曲線
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

	//XMVECTOR vec = vAns - vold_pos;        //移動前とあとの座標の差分（角度）
	//float angle = atan2f(vec.m128_f32[1], vec.m128_f32[0]);        //その差分を使って角度を求めている




	//パーティクルを入れる
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
	{// Enter押したら、ステージを切り替える
		
		SetFade(FADE_OUT, MODE_GAME);

	}

#endif


}

//=============================================================================
// 描画処理
//=============================================================================
void DrawSelect(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_BACK);

	for (int i = 0; i < MAX_SELECT; i++)
	{
		if (g_Select[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Select[i].scl.x, g_Select[i].scl.y, g_Select[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Select[i].rot.x, g_Select[i].rot.y + XM_PI, g_Select[i].rot.z+0.01f);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Select[i].pos.x, g_Select[i].pos.y, g_Select[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Select[i].mtxWorld, mtxWorld);


		// モデル描画
		DrawModel(&g_Select[i].model);
	}

//	// カリング設定を戻す
//	SetCullingMode(CULL_MODE_BACK);
}
//
////=============================================================================
//// エネミーの取得
////=============================================================================
//SELECT *GetSELECT()
//{
//	return &g_Select[0];
//}
