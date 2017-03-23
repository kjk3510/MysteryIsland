#include "Light.fx"
//ī�޶� ��ȯ ��İ� ���� ��ȯ ����� ���� ���̴� ������ �����Ѵ�(���� 0�� ���).
cbuffer cbViewProjectionMatrix : register(b0)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
};

//���� ��ȯ ����� ���� ���̴� ������ �����Ѵ�(���� 1�� ���). 
cbuffer cbWorldMatrix : register(b1)
{
	matrix gmtxWorld : packoffset(c0);
};

//---------------------------------------������ ���� ���� ����� ���� ���̴� ����� ����ü
//������ ������ ���� ��� ���� ���̴��� �Է��� ���� ����ü�̴�.
struct VS_DIFFUSED_COLOR_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR0;
};

//������ ������ ���� ��� ���� ���̴��� ����� ���� ����ü�̴�.
struct VS_DIFFUSED_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
};
//---------------------------------------������ ���� ���� ����� ���� ���̴� ����� ����ü

//------------------------------------------�ν��Ͻ��� �ϸ鼭 ������ ���� ���� ��� ���� ���̴��� ����� ����ü
//�ν��Ͻ��� �ϸ鼭 ������ ������ ���� ��� ���� ���̴��� �Է��� ���� ����ü�̴�.
struct VS_INSTANCED_DIFFUSED_COLOR_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR0;
	float4x4 mtxTransform : INSTANCEPOS;
};

//�ν��Ͻ��� �ϸ鼭 ������ ������ ���� ��� ���� ���̴��� ����� ���� ����ü�̴�.
struct VS_INSTANCED_DIFFUSED_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
}; 
//------------------------------------------�ν��Ͻ��� �ϸ鼭 ������ ���� ���� ��� ���� ���̴��� ����� ����ü

//-------------------------------������ ����ϴ� ���� ���̴� ����� ����ü-------------------------
//������ ����ϴ� ��� ���� ���̴��� �Է��� ���� ����ü�̴�.
struct VS_LIGHTING_COLOR_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
};

//������ ����ϴ� ��� ���� ���̴��� ����� ���� ����ü�̴�.
struct VS_LIGHTING_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	//������ǥ�迡�� ������ ��ġ�� ���� ���͸� ��Ÿ����.
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
};

//�ν��Ͻ��� �ϸ鼭 ������ ����ϴ� ��� ���� ���̴��� �Է��� ���� ����ü�̴�.
struct VS_INSTANCED_LIGHTING_COLOR_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float4x4 mtxTransform : INSTANCEPOS;
};

//�ν��Ͻ��� �ϸ鼭 ������ ����ϴ� ��� ���� ���̴��� ����� ���� ����ü�̴�.
struct VS_INSTANCED_LIGHTING_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
};
//-------------------------------������ ����ϴ� ���� ���̴� ����� ����ü-----------------------------


//�÷��� ������ ���� ����ϴ� ���
struct VS_TERRAIN_LIGHTING_COLOR_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR0;
};

struct VS_TERRAIN_LIGHTING_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR0;
};

//---------------
//------------------------������ ���� ���� ����� VS PS
VS_DIFFUSED_COLOR_OUTPUT VSDiffusedColor(VS_DIFFUSED_COLOR_INPUT input)
{
	VS_DIFFUSED_COLOR_OUTPUT output = (VS_DIFFUSED_COLOR_OUTPUT)0;
	output.position = mul(float4(input.position, 1.0f), mul(mul(gmtxWorld, gmtxView), gmtxProjection));
	output.color = input.color;

	return(output);
}

float4 PSDiffusedColor(VS_DIFFUSED_COLOR_OUTPUT input) : SV_Target
{
	return(input.color);
}
//instance
VS_INSTANCED_DIFFUSED_COLOR_OUTPUT VSInstancedDiffusedColor(VS_INSTANCED_DIFFUSED_COLOR_INPUT input)
{
	VS_INSTANCED_DIFFUSED_COLOR_OUTPUT output = (VS_INSTANCED_DIFFUSED_COLOR_OUTPUT)0;
	output.position = mul(mul(mul(float4(input.position, 1.0f), input.mtxTransform), gmtxView), gmtxProjection);
	output.color = input.color;

	return(output);
}

float4 PSInstancedDiffusedColor(VS_INSTANCED_DIFFUSED_COLOR_OUTPUT input) : SV_Target
{
	return(input.color);
}
//instance
//---------------
//------------------------������ ���� ���� ����� VS PS

//-----------------------
//--------------------------���� ������ �޴� ���̴� -----------------
//������ ������ ����ϱ� ���� ������ ���� ���Ϳ� ������ ��ġ�� ����ϴ� ���� ���̴� �Լ��̴�.
VS_LIGHTING_COLOR_OUTPUT VSLightingColor(VS_LIGHTING_COLOR_INPUT input)
{
	VS_LIGHTING_COLOR_OUTPUT output = (VS_LIGHTING_COLOR_OUTPUT)0;
	//������ ������ ����ϱ� ���Ͽ� ������ǥ�迡�� ������ ��ġ�� ���� ���͸� ���Ѵ�.
	output.normalW = mul(input.normal, (float3x3)gmtxWorld);
	output.positionW = mul(float4(input.position, 1.0f), gmtxWorld).xyz;
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);

	return(output);
}

//�� �ȼ��� ���Ͽ� ������ ������ �ݿ��� ������ ����ϱ� ���� �ȼ� ���̴� �Լ��̴�.
float4 PSLightingColor(VS_LIGHTING_COLOR_OUTPUT input) : SV_Target
{
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW);

		return(cIllumination);
}
VS_INSTANCED_LIGHTING_COLOR_OUTPUT VSInstancedLightingColor(VS_INSTANCED_LIGHTING_COLOR_INPUT input)
{
	VS_INSTANCED_LIGHTING_COLOR_OUTPUT output = (VS_INSTANCED_LIGHTING_COLOR_OUTPUT)0;
	output.normalW = mul(input.normal, (float3x3)input.mtxTransform);
	output.positionW = mul(float4(input.position, 1.0f), input.mtxTransform).xyz;
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);

	return(output);
}

float4 PSInstancedLightingColor(VS_INSTANCED_LIGHTING_COLOR_OUTPUT input) : SV_Target
{
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW);

		return(cIllumination);
}
//-----------------------
//--------------------------���� ������ �޴� ���̴� -----------------
struct VS_INPUT
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
};
/*����-���̴��̴�. ������ ��ġ ���͸� ���� ��ȯ, ī�޶� ��ȯ, ���� ��ȯ�� ������� �����Ѵ�. ���� �ﰢ���� �� ������ y-�������� ȸ���� ��Ÿ���� ��Ŀ� ���� ��ȯ�Ѵ�. �׷��Ƿ� �ﰢ���� ȸ���ϰ� �ȴ�.*/
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.position = mul(input.position, gmtxWorld);
	output.position = mul(output.position, gmtxView);
	output.position = mul(output.position, gmtxProjection);
	output.color = input.color;
	//�ԷµǴ� ������ ������ �״�� ����Ѵ�. 
	return output;
}


//���̴� ����("Effect.fx")�� ������ ���� �ȼ� - ���̴� �Լ��� �߰��Ѵ�.
float4 PS(VS_OUTPUT input) : SV_Target
{
	return input.color;
	//�ԷµǴ� ������ ������ �״�� ����Ѵ�. 
}
/*(����) register(b0)���� b�� �������Ͱ� ��� ���۸� ���� ���Ǵ� ���� �ǹ��Ѵ�. 0�� ���������� ��ȣ�̸� ���� ���α׷����� ��� ���۸� 
����̽� ���ؽ�Ʈ�� ������ ���� ���� ��ȣ�� ��ġ�ϵ��� �ؾ� �Ѵ�.

pd3dDeviceContext->VSSetConstantBuffers(VS_SLOT_WORLD_MATRIX, 1, &m_pd3dcbWorldMatrix);
*/
//����-���̴��� ����� ���� ����ü�̴�.
/*struct VS_INSTANCED_COLOR_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR0;
	column_major float4x4 mtxTransform : INSTANCEPOS;
	float4 instanceColor : INSTANCECOLOR;
};

struct VS_INSTANCED_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
	//�ý��� ���� ������ ���� ���̴��� ���޵Ǵ� ��ü �ν��Ͻ��� ID�� �ȼ� ���̴��� �����Ѵ�.
	float4 instanceID : INDEX;
};*/

/*����-���̴��̴�. ������ ��ġ ���͸� ���� ��ȯ, ī�޶� ��ȯ, ���� ��ȯ�� ������� �����Ѵ�. ���� �ﰢ���� �� ������ y-�������� ȸ���� ��Ÿ���� ��Ŀ� ���� ��ȯ�Ѵ�. �׷��Ƿ� �ﰢ���� ȸ���ϰ� �ȴ�.*/
/*VS_INSTANCED_COLOR_OUTPUT VSInstancedDiffusedColor(VS_INSTANCED_COLOR_INPUT input, uint instanceID : SV_InstanceID)
{
	VS_INSTANCED_COLOR_OUTPUT output = (VS_INSTANCED_COLOR_OUTPUT)0;
	//������ ��ġ ���Ϳ� �ν��Ͻ��� ��ġ ����(���� ��ǥ)�� ���Ѵ�.
	//output.position = input.position + float4(input.instancePos, 0.0f); //OK
	//output.position = float4(input.position, 1.0f) + input.instancePos; //OK
	//output.position = input.position + float4(input.instancePos, 1.0f); //OK
	//output.position = float4(input.position, 0.0f) + float4(input.instancePos, 1.0f); //OK
	output.position = float4(input.position, 1.0f) + float4(input.instancePos, 0.0f); //OK
	output.position = mul(output.position, gmtxView);
	output.position = mul(output.position, gmtxProjection);
	output.color = input.color;
	//�ν��Ͻ� ID�� �ȼ� ���̴��� �����Ѵ�.
	output.instanceID = instanceID;
	return output;
}*/
/*VS_INSTANCED_COLOR_OUTPUT VSInstancedDiffusedColor(VS_INSTANCED_COLOR_INPUT input, uint instanceID : SV_InstanceID)
{
	VS_INSTANCED_COLOR_OUTPUT output = (VS_INSTANCED_COLOR_OUTPUT)0;
	output.position = mul(float4(input.position, 1), input.mtxTransform);
	output.position = mul(output.position, gmtxView);
	output.position = mul(output.position, gmtxProjection);
	output.color = (instanceID % 5) ? input.color : (input.color * 0.3f + input.instanceColor * 0.7f);
	output.instanceID = instanceID;
	return output;
}

//���̴� ����("Effect.fx")�� ������ ���� �ȼ� - ���̴� �Լ��� �߰��Ѵ�.
float4 PSInstancedDiffusedColor(VS_INSTANCED_COLOR_OUTPUT input) : SV_Target
{
	return input.color;
}*/
