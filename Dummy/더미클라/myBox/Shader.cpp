#include "Shader.h"


CShader::CShader()
{
	m_pCube = nullptr;
}

CShader::~CShader()
{

}

void CShader::BuildObjects(ID3D11Device *pd3dDevice)
{
	m_pCube = new CGameObject(pd3dDevice);
}

void CShader::BuildFX(ID3D11Device *pd3dDevice)
{
	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3D10Blob* compiledShader = 0;
	ID3D10Blob* compilationMsgs = 0;
	HRESULT hr = D3DX11CompileFromFile(L"FX/color.fx", 0, 0, 0, "fx_5_0", shaderFlags,
		0, 0, &compiledShader, &compilationMsgs, 0);

	// compilationMsgs can store errors or warnings.
	if (compilationMsgs != 0)
	{
		MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
		ReleaseCOM(compilationMsgs);
	}

	// Even if there are no compilationMsgs, check to make sure there were no other errors.
	if (FAILED(hr))
	{
		DXTrace(__FILEW__, (DWORD)__LINE__, hr, L"D3DX11CompileFromFile", true);
	}

	HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(),
		0, pd3dDevice, &m_FX));

	// Done with compiled shader.
	ReleaseCOM(compiledShader);

	m_Tech = m_FX->GetTechniqueByName("ColorTech");
	m_WorldViewProj = m_FX->GetVariableByName("gWorldViewProj")->AsMatrix();
}

void CShader::BuildVertexLayout(ID3D11Device *pd3dDevice)
{
	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Create the input layout
	D3DX11_PASS_DESC passDesc;
	m_Tech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(pd3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &m_InputLayout));
}

void CShader::DrawScene(ID3D11DeviceContext* pd3dImmediateContext, CCamera* pCamera)
{
	pd3dImmediateContext->IASetInputLayout(m_InputLayout);
	pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX world = XMLoadFloat4x4(&m_pCube->GetWorldMatrix());
	XMMATRIX view = XMLoadFloat4x4(&pCamera->GetViewMatrix());
	XMMATRIX proj = XMLoadFloat4x4(&pCamera->GetProjecMatirx());
	XMMATRIX worldViewProj = world*view*proj;

	m_WorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));


	D3DX11_TECHNIQUE_DESC techDesc;
	m_Tech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_Tech->GetPassByIndex(p)->Apply(0, pd3dImmediateContext);

		// 36 indices for the box.
		pd3dImmediateContext->DrawIndexed(36, 0, 0);
	}
	m_pCube->DrawObject(pd3dImmediateContext);
}

void CShader::ReleaseShader()
{
	ReleaseCOM(m_FX);
	ReleaseCOM(m_InputLayout);
	m_pCube->ReleaseObject();
	delete m_pCube;
}