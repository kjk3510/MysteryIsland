#pragma once
#include <d3dUtil.h>

class CCamera
{
private:
	XMFLOAT4X4 m_View;
	XMFLOAT4X4 m_Proj;

	float m_Theta;
	float m_Phi;
	float m_Radius;

	POINT m_LastMousePos;

public:
	CCamera();
	~CCamera();
	void GenerateProjectionMatrix(float fFOVAngle, float fAspectRatio, float fNearPlaneDistance, float fFarPlaneDistance);
	void GenerateViewMatrix();
	void OnMouseDown(HWND* phMainWnd, WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

	XMFLOAT4X4 GetViewMatrix() { return m_View; }
	XMFLOAT4X4 GetProjecMatirx() { return m_Proj; }
};

