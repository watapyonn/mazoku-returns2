//=============================================================================
//
// カメラ処理 [camera.cpp]
// Author : 井芹亘
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CAMERA_MAX			(2)
#define	POS_X_CAM			(0.0f)			// カメラの初期位置(X座標)
#define	POS_Y_CAM			(70.0f)			// カメラの初期位置(Y座標)
#define	POS_Z_CAM			(-250.0f)		// カメラの初期位置(Z座標)

//#define	POS_X_CAM		(0.0f)			// カメラの初期位置(X座標)
//#define	POS_Y_CAM		(200.0f)		// カメラの初期位置(Y座標)
//#define	POS_Z_CAM		(-400.0f)		// カメラの初期位置(Z座標)


#define	VIEW_ANGLE		(XMConvertToRadians(45.0f))						// ビュー平面の視野角
#define	VIEW_ASPECT		((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)	// ビュー平面のアスペクト比	
#define	VIEW_NEAR_Z		(10.0f)											// ビュー平面のNearZ値
#define	VIEW_FAR_Z		(10000.0f)										// ビュー平面のFarZ値

#define	VALUE_MOVE_CAMERA	(2.0f)										// カメラの移動量
#define	VALUE_ROTATE_CAMERA	(XM_PI * 0.01f)								// カメラの回転量

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static CAMERA			g_Camera[CAMERA_MAX];		// カメラデータ

static int				g_ViewPortType = TYPE_FULL_SCREEN;



//線形保管用

static INTERPOLATION_DATA_CAMERA g_MoveTbl0[] = {
	//pos									at							rot						時間
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
// 初期化処理
//=============================================================================
void InitCamera(void)
{
	//すべてのカメラを初期化
	for (int i = 0; i < CAMERA_MAX; i++)
	{
		g_Camera[i].pos = { POS_X_CAM, POS_Y_CAM, POS_Z_CAM };
		g_Camera[i].at  = { 0.0f, 0.0f, 0.0f };
		g_Camera[i].up  = { 0.0f, 1.0f, 0.0f };
		g_Camera[i].rot = { 0.0f, 0.0f, 0.0f };

		// 視点と注視点の距離を計算
		float vx, vz;
		vx = g_Camera[i].pos.x - g_Camera[i].at.x;
		vz = g_Camera[i].pos.z - g_Camera[i].at.z;
		g_Camera[i].len = sqrtf(vx * vx + vz * vz);

		// ビューポートタイプの初期化
		SetViewPort(g_ViewPortType);



	}
	// 0番だけ線形補間で動かしてみる
	g_Camera[LINEAR_INTERPOLATION_CAMERA].time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Camera[LINEAR_INTERPOLATION_CAMERA].tblNo = 0;		// 再生するアニメデータの先頭アドレスをセット
	g_Camera[LINEAR_INTERPOLATION_CAMERA].tblMax = sizeof(g_MoveTbl0) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

}


//=============================================================================
// カメラの終了処理
//=============================================================================
void UninitCamera(void)
{

}


//=============================================================================
// カメラの更新処理
//=============================================================================
void UpdateCamera(void)
{

#ifdef _DEBUG

	if (GetKeyboardPress(DIK_Z))
	{// 視点旋回「左」
		g_Camera[DEFAULT_CAMERA].rot.y += VALUE_ROTATE_CAMERA;
		if (g_Camera[DEFAULT_CAMERA].rot.y > XM_PI)
		{
			g_Camera[DEFAULT_CAMERA].rot.y -= XM_PI * 2.0f;
		}

		g_Camera[DEFAULT_CAMERA].pos.x = g_Camera[DEFAULT_CAMERA].at.x - sinf(g_Camera[DEFAULT_CAMERA].rot.y) * g_Camera[DEFAULT_CAMERA].len;
		g_Camera[DEFAULT_CAMERA].pos.z = g_Camera[DEFAULT_CAMERA].at.z - cosf(g_Camera[DEFAULT_CAMERA].rot.y) * g_Camera[DEFAULT_CAMERA].len;
	}

	if (GetKeyboardPress(DIK_C))
	{// 視点旋回「右」
		g_Camera[DEFAULT_CAMERA].rot.y -= VALUE_ROTATE_CAMERA;
		if (g_Camera[DEFAULT_CAMERA].rot.y < -XM_PI)
		{
			g_Camera[DEFAULT_CAMERA].rot.y += XM_PI * 2.0f;
		}

		g_Camera[DEFAULT_CAMERA].pos.x = g_Camera[DEFAULT_CAMERA].at.x - sinf(g_Camera[DEFAULT_CAMERA].rot.y) * g_Camera[DEFAULT_CAMERA].len;
		g_Camera[DEFAULT_CAMERA].pos.z = g_Camera[DEFAULT_CAMERA].at.z - cosf(g_Camera[DEFAULT_CAMERA].rot.y) * g_Camera[DEFAULT_CAMERA].len;
	}

	if (GetKeyboardPress(DIK_Y))
	{// 視点移動「上」
		g_Camera[DEFAULT_CAMERA].pos.y += VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_N))
	{// 視点移動「下」
		g_Camera[DEFAULT_CAMERA].pos.y -= VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_Q))
	{// 注視点旋回「左」
		g_Camera[DEFAULT_CAMERA].rot.y -= VALUE_ROTATE_CAMERA;
		if (g_Camera[DEFAULT_CAMERA].rot.y < -XM_PI)
		{
			g_Camera[DEFAULT_CAMERA].rot.y += XM_PI * 2.0f;
		}

		g_Camera[DEFAULT_CAMERA].at.x = g_Camera[DEFAULT_CAMERA].pos.x + sinf(g_Camera[DEFAULT_CAMERA].rot.y) * g_Camera[DEFAULT_CAMERA].len;
		g_Camera[DEFAULT_CAMERA].at.z = g_Camera[DEFAULT_CAMERA].pos.z + cosf(g_Camera[DEFAULT_CAMERA].rot.y) * g_Camera[DEFAULT_CAMERA].len;
	}

	if (GetKeyboardPress(DIK_E))
	{// 注視点旋回「右」
		g_Camera[DEFAULT_CAMERA].rot.y += VALUE_ROTATE_CAMERA;
		if (g_Camera[DEFAULT_CAMERA].rot.y > XM_PI)
		{
			g_Camera[DEFAULT_CAMERA].rot.y -= XM_PI * 2.0f;
		}

		g_Camera[DEFAULT_CAMERA].at.x = g_Camera[DEFAULT_CAMERA].pos.x + sinf(g_Camera[DEFAULT_CAMERA].rot.y) * g_Camera[DEFAULT_CAMERA].len;
		g_Camera[DEFAULT_CAMERA].at.z = g_Camera[DEFAULT_CAMERA].pos.z + cosf(g_Camera[DEFAULT_CAMERA].rot.y) * g_Camera[DEFAULT_CAMERA].len;
	}

	if (GetKeyboardPress(DIK_T))
	{// 注視点移動「上」
		g_Camera[DEFAULT_CAMERA].at.y += VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_B))
	{// 注視点移動「下」
		g_Camera[DEFAULT_CAMERA].at.y -= VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_U))
	{// 近づく
		g_Camera[DEFAULT_CAMERA].len -= VALUE_MOVE_CAMERA;
		g_Camera[DEFAULT_CAMERA].pos.x = g_Camera[DEFAULT_CAMERA].at.x - sinf(g_Camera[DEFAULT_CAMERA].rot.y) * g_Camera[DEFAULT_CAMERA].len;
		g_Camera[DEFAULT_CAMERA].pos.z = g_Camera[DEFAULT_CAMERA].at.z - cosf(g_Camera[DEFAULT_CAMERA].rot.y) * g_Camera[DEFAULT_CAMERA].len;
	}

	if (GetKeyboardPress(DIK_M))
	{// 離れる
		g_Camera[DEFAULT_CAMERA].len += VALUE_MOVE_CAMERA;
		g_Camera[DEFAULT_CAMERA].pos.x = g_Camera[DEFAULT_CAMERA].at.x - sinf(g_Camera[DEFAULT_CAMERA].rot.y) * g_Camera[DEFAULT_CAMERA].len;
		g_Camera[DEFAULT_CAMERA].pos.z = g_Camera[DEFAULT_CAMERA].at.z - cosf(g_Camera[DEFAULT_CAMERA].rot.y) * g_Camera[DEFAULT_CAMERA].len;
	}

	// カメラを初期に戻す
	if (GetKeyboardPress(DIK_R))
	{
		UninitCamera();
		InitCamera();
	}


		//線形保管で動かす
		// 移動処理
		if (g_Camera[LINEAR_INTERPOLATION_CAMERA].tblMax > LINEAR_INTERPOLATION_CAMERA)	// 線形補間を実行する？
		{	// 線形補間の処理
			int nowNo = (int)g_Camera[LINEAR_INTERPOLATION_CAMERA].time;			// 整数分であるテーブル番号を取り出している
			int maxNo = g_Camera[LINEAR_INTERPOLATION_CAMERA].tblMax;				// 登録テーブル数を数えている
			int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
			INTERPOLATION_DATA_CAMERA* tbl = g_MoveTblAdr[g_Camera[LINEAR_INTERPOLATION_CAMERA].tblNo];	// 行動テーブルのアドレスを取得

			XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
			XMVECTOR nowAt = XMLoadFloat3(&tbl[nowNo].at);	// XMVECTORへ変換
			XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換

			XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
			XMVECTOR At  = XMLoadFloat3(&tbl[nextNo].at) - nowAt;	// XYZ回転量を計算している
			XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ拡大率を計算している

			float nowTime = g_Camera[LINEAR_INTERPOLATION_CAMERA].time - nowNo;	// 時間部分である少数を取り出している

			Pos *= nowTime;								// 現在の移動量を計算している
			At *= nowTime;								// 現在の回転量を計算している
			Rot *= nowTime;								// 現在の拡大率を計算している

			// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
			XMStoreFloat3(&g_Camera[LINEAR_INTERPOLATION_CAMERA].pos, nowPos + Pos);

			// 計算して求めた回転量を現在の移動テーブルに足している
			XMStoreFloat3(&g_Camera[LINEAR_INTERPOLATION_CAMERA].at, nowAt + At);

			// 計算して求めた拡大率を現在の移動テーブルに足している
			XMStoreFloat3(&g_Camera[LINEAR_INTERPOLATION_CAMERA].rot, nowRot + Rot);
			//g_Enemy[DEFAULT_CAMERA].w = TEXTURE_WIDTH * g_Enemy[DEFAULT_CAMERA].scl.x;
			//g_Enemy[DEFAULT_CAMERA].h = TEXTURE_HEIGHT * g_Enemy[DEFAULT_CAMERA].scl.y;
			// frameを使て時間経過処理をする
			g_Camera[LINEAR_INTERPOLATION_CAMERA].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
			if ((int)g_Camera[LINEAR_INTERPOLATION_CAMERA].time >= maxNo)			// 登録テーブル最後まで移動したか？
			{
				g_Camera[LINEAR_INTERPOLATION_CAMERA].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
			}






		}

	






#endif



#ifdef _DEBUG	// デバッグ情報を表示する
	PrintDebugProc("Camera:ZC QE TB YN UM R\n");
#endif
}


//=============================================================================
// カメラの更新
//=============================================================================
void SetCamera(int index) 
{
	// ビューマトリックス設定
	XMMATRIX mtxView;
	mtxView = XMMatrixLookAtLH(XMLoadFloat3(&g_Camera[index].pos), XMLoadFloat3(&g_Camera[index].at), XMLoadFloat3(&g_Camera[index].up));
	SetViewMatrix(&mtxView);
	XMStoreFloat4x4(&g_Camera[index].mtxView, mtxView);

	XMMATRIX mtxInvView;
	mtxInvView = XMMatrixInverse(nullptr, mtxView);
	XMStoreFloat4x4(&g_Camera[index].mtxInvView, mtxInvView);


	// プロジェクションマトリックス設定
	XMMATRIX mtxProjection;
	mtxProjection = XMMatrixPerspectiveFovLH(VIEW_ANGLE, VIEW_ASPECT, VIEW_NEAR_Z, VIEW_FAR_Z);

	SetProjectionMatrix(&mtxProjection);
	XMStoreFloat4x4(&g_Camera[index].mtxProjection, mtxProjection);

	SetShaderCamera(g_Camera[index].pos);
}


//=============================================================================
// カメラの取得
//=============================================================================
CAMERA *GetCamera(void) 
{
	return &g_Camera[DEFAULT_CAMERA];
}

//=============================================================================
// ビューポートの設定
//=============================================================================
void SetViewPort(int type)
{
	ID3D11DeviceContext *g_ImmediateContext = GetDeviceContext();
	D3D11_VIEWPORT vp;

	g_ViewPortType = type;

	// ビューポート設定
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



// カメラの視点と注視点をセット
void SetCameraAT(XMFLOAT3 pos ,int index)
{
	// カメラの注視点を引数の座標にしてみる
	g_Camera[index].at = pos;

	// カメラの視点をカメラのY軸回転に対応させている
	g_Camera[index].pos.x = g_Camera[index].at.x - sinf(g_Camera[index].rot.y) * g_Camera[index].len;
	g_Camera[index].pos.z = g_Camera[index].at.z - cosf(g_Camera[index].rot.y) * g_Camera[index].len;

}

