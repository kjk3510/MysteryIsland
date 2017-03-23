// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

//ī�޶� ��ȯ ��İ� ���� ��ȯ ��� �׸��� ���� ��ȯ ����� ���� ��� ���۸� ������ ���� ��ȣ�̴�.
//ī�޶� ��ȯ ��İ� ���� ��ȯ ��� �׸��� ���� ��ȯ ����� ���� ��� ���۸� ������ ���� ��ȣ�̴�.
#define VS_SLOT_CAMERA				0x00
#define VS_SLOT_WORLD_MATRIX			0x01
//ī�޶� ��ȯ ��İ� ���� ��ȯ ��� �׸��� ���� ��ȯ ����� ���� ��� ���۸� ������ ���� ��ȣ�̴�.
//ī�޶� ��ȯ ��İ� ���� ��ȯ ��� �׸��� ���� ��ȯ ����� ���� ��� ���۸� ������
//���� ��ȣ�̴�.


//���� ���� 
//����� ������ �����ϱ� ���� ��� ������ ���� ��ȣ�� �����Ѵ�. 
#define PS_SLOT_LIGHT			0x00
#define PS_SLOT_MATERIAL		0x01
//���� ���� 
//����� ������ �����ϱ� ���� ��� ������ ���� ��ȣ�� �����Ѵ�. 


#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:

// ������ ũ�⸦ ��Ÿ���� ���
#define FRAME_BUFFER_WIDTH	640
#define FRAME_BUFFER_HEIGHT	480
// ������ ũ�⸦ ��Ÿ���� ���


#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <iostream>
#include <random>
#include <time.h>

using namespace std;


//DirectX SDK ���̺귯�� ��� �߰�
#include <d3d11.h>
#include <d3dx11.h>

#include <D3D9Types.h>
/*
d3d11.h d3dx11.h ��������� DirectX API�Լ��� ����ϱ� ���� �ݵ�� ���Խ��Ѿ��ϴ� ����̴�.
stdafx.h ���Ͽ� ��� ������ �߰��ϴ� ������ Visual Studio ������ PCH(Precompiled Header)�� ���Ͽ�
���� ������� �ʴ� ����� ���Ͽ� ���� ���Τ��� �Ź� ���� �ʾƵ� �ǵ��� �ϴ� ����� �����ϱ� �����̴�.
�׸��� ������Ʈ �����簡 ������� �ڵ忡�� stdafx.h ������ ���Եǵ��� �Ǿ��ִ�.
*/
//DirectX SDK ���̺귯�� ��� �߰�

//�ð��� ���õ� ��Ƽ�̵�� ���̺귯�� �Լ��� ����ϱ� ���Ͽ� ���� ��� ������ �߰��Ѵ�.
#include <Mmsystem.h>

//���� ���̺귯�� �Լ��� ����ϱ� ���Ͽ� ���� ��� ������ �߰��Ѵ�.
#include <math.h>


// TODO: reference additional headers your program requires here

#include <d3dcompiler.h>  	//���̴� ������ �Լ��� ����ϱ� ���� ��� ����
#include <D3DX10Math.h>	//Direct3D ���� �Լ��� ����ϱ� ���� ��� ����