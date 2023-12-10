

//*****************************************************************************
// 定数バッファ
//*****************************************************************************
//レジスタを登録して使用する「b●」
//バッファの番号を登録していく

//ばっふぁはｂ
//テクスチャはｔ
//サンプルステートはｓ

// マトリクスバッファ
cbuffer WorldBuffer : register( b0 )
{
	matrix World;
}

cbuffer ViewBuffer : register( b1 )
{
	matrix View;
}

cbuffer ProjectionBuffer : register( b2 )
{
	matrix Projection;
}

// マテリアルバッファ
struct MATERIAL
{
	float4		Ambient;
	float4		Diffuse;
	float4		Specular;
	float4		Emission;
	float		Shininess;
	int			noTexSampling;
	float		Dummy[2];//16byte境界用
};

cbuffer MaterialBuffer : register( b3 )
{
	MATERIAL	Material;
}

// ライト用バッファ
struct LIGHT
{
	float4		Direction[5];
	float4		Position[5];
	float4		Diffuse[5];
	float4		Ambient[5];
	float4		Attenuation[5];
	int4		Flags[5];
	int			Enable;
	int			Dummy[3];//16byte境界用
};

cbuffer LightBuffer : register( b4 )
{
	LIGHT		Light;
}

struct FOG
{
	float4		Distance;
	float4		FogColor;
	int			Enable;
	float		Dummy[3];//16byte境界用
};

// フォグ用バッファ
cbuffer FogBuffer : register( b5 )
{
	FOG			Fog;
};

// 縁取り用バッファ
cbuffer Fuchi : register(b6)
{
	int			fuchi;
	int			fill[3];
};


cbuffer CameraBuffer : register(b7)
{
	float4 Camera;
}


// 塗りつぶしようバッファ（ここでもらっている）
struct FILL
{
	float4			Color;
	int				Enable;
	int				Dummy[3];
};

cbuffer FillBuffer : register(b8)
{
	FILL			Fill;
};



//=============================================================================
// 頂点シェーダ
//=============================================================================
void VertexShaderPolygon( in  float4 inPosition		: POSITION0,
						  in  float4 inNormal		: NORMAL0,
						  in  float4 inDiffuse		: COLOR0,
						  in  float2 inTexCoord		: TEXCOORD0,

						  out float4 outPosition	: SV_POSITION,
						  out float4 outNormal		: NORMAL0,
						  out float2 outTexCoord	: TEXCOORD0,
						  out float4 outDiffuse		: COLOR0,
						  out float4 outWorldPos    : POSITION0)
{
	matrix wvp;
	wvp = mul(World, View);
	wvp = mul(wvp, Projection);
	outPosition = mul(inPosition, wvp);

	outNormal = normalize(mul(float4(inNormal.xyz, 0.0f), World));

	outTexCoord = inTexCoord;

	outWorldPos = mul(inPosition, World);

	outDiffuse = inDiffuse;
}



//*****************************************************************************
// グローバル変数
//*****************************************************************************
Texture2D		g_Texture : register( t0 );
SamplerState	g_SamplerState : register( s0 );


//=============================================================================
// ピクセルシェーダ
//=============================================================================
							//セメンティクス
void PixelShaderPolygon(in  float4 inPosition		: SV_POSITION,
						in  float4 inNormal			: NORMAL0,
						in  float2 inTexCoord		: TEXCOORD0,
						in  float4 inDiffuse		: COLOR0,
						in  float4 inWorldPos		: POSITION0,

						out float4 outDiffuse		: SV_Target)
{
	float4 color;
	//塗りつぶし

	//縁取り
	if (fuchi == 1)
	{
		float angle = dot(normalize(inWorldPos.xyz - Camera.xyz), normalize(inNormal));
		if ((angle < 1.0f) && (angle > -0.5f))
			//if (angle > -0.3f)
		{
			outDiffuse.r = 1.0f;
			outDiffuse.gb = 0.0f;
		}
		//return;
	}

	if (Material.noTexSampling == 0)
	{
		color = g_Texture.Sample(g_SamplerState, inTexCoord);

		color *= inDiffuse;
	}
	else
	{
		color = inDiffuse;
	}

	if (Light.Enable == 0)
	{
		color = color * Material.Diffuse;
	}
	else
	{
		float4 tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 outColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

		for (int i = 0; i < 5; i++)
		{
			float3 lightDir;
			float light;

			if (Light.Flags[i].y == 1)
			{
				if (Light.Flags[i].x == 1)
				{
					lightDir = normalize(Light.Direction[i].xyz);
					light = dot(lightDir, inNormal.xyz);

					light = 0.5 - 0.5 * light;
					tempColor = color * Material.Diffuse * light * Light.Diffuse[i];
				}
				else if (Light.Flags[i].x == 2)
				{
					lightDir = normalize(Light.Position[i].xyz - inWorldPos.xyz);
					light = dot(lightDir, inNormal.xyz);

					tempColor = color * Material.Diffuse * light * Light.Diffuse[i];

					float distance = length(inWorldPos - Light.Position[i]);

					float att = saturate((Light.Attenuation[i].x - distance) / Light.Attenuation[i].x);
					tempColor *= att;
				}
				else
				{
					tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
				}

				outColor += tempColor;
			}
		}

		color = outColor;
		color.a = inDiffuse.a * Material.Diffuse.a;
	}

	//フォグ
	if (Fog.Enable == 1)
	{
		float z = inPosition.z * inPosition.w;
		float f = (Fog.Distance.y - z) / (Fog.Distance.y - Fog.Distance.x);
		f = saturate(f);
		outDiffuse = f * color + (1 - f) * Fog.FogColor;
		outDiffuse.a = color.a;
	}
	else
	{
		outDiffuse = color;
	}

	if (Fill.Enable == 1)
	{
		//塗りつぶし
		//outDiffuse = Fill.Color;
		 

		//// 座標系?
		//if ((inTexCoord.x < 0.10) && (inTexCoord.y < 0.10))
		//	color.rgb = float4(1, 0, 0, 1); // red
		//else if ((inTexCoord.x > 0.90) && (inTexCoord.y < 0.10))
		//	color.rgb = float4(0, 1, 0, 1); // green
		//else if ((inTexCoord.x > 0.90) && (inTexCoord.y > 0.9))
		//	color.rgb = float4(0, 0, 1, 1); // blue
		//else if ((inTexCoord.x < 0.10) && (inTexCoord.y > 0.9))
		//	color.rgb = float4(1, 1, 0, 1); // yellow
		//else
		//	color.rgb = float4(0.5, 0.5, 0.5, 1); // silver

		//outDiffuse = color;
		

		//モザイク
		//ここサイズでモザイクの大きさを変える
		float size1 = 40;
		float2 uv1 = floor(inTexCoord * size1 + 0.5) / size1;


		outDiffuse = g_Texture.Sample(g_SamplerState, uv1);


	}

	



}

