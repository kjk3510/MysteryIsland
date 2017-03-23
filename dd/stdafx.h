// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

//카메라 변환 행렬과 투영 변환 행렬 그리고 월드 변환 행렬을 위한 상수 버퍼를 연결할 슬롯 번호이다.
//카메라 변환 행렬과 투영 변환 행렬 그리고 월드 변환 행렬을 위한 상수 버퍼를 연결할 슬롯 번호이다.
#define VS_SLOT_CAMERA				0x00
#define VS_SLOT_WORLD_MATRIX			0x01
//카메라 변환 행렬과 투영 변환 행렬 그리고 월드 변환 행렬을 위한 상수 버퍼를 연결할 슬롯 번호이다.
//카메라 변환 행렬과 투영 변환 행렬 그리고 월드 변환 행렬을 위한 상수 버퍼를 연결할
//슬롯 번호이다.


//조명 재질 
//조명과 재질을 설정하기 위한 상수 버퍼의 슬롯 번호를 정의한다. 
#define PS_SLOT_LIGHT			0x00
#define PS_SLOT_MATERIAL		0x01
//조명 재질 
//조명과 재질을 설정하기 위한 상수 버퍼의 슬롯 번호를 정의한다. 


#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:

// 버퍼의 크기를 나타내는 상수
#define FRAME_BUFFER_WIDTH	640
#define FRAME_BUFFER_HEIGHT	480
// 버퍼의 크기를 나타내는 상수


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


//DirectX SDK 라이브러리 헤더 추가
#include <d3d11.h>
#include <d3dx11.h>

#include <D3D9Types.h>
/*
d3d11.h d3dx11.h 헤더파일은 DirectX API함수를 사용하기 위해 반드시 포함시켜야하는 헤더이다.
stdafx.h 파일에 헤더 파일을 추가하는 이유는 Visual Studio 에서는 PCH(Precompiled Header)를 통하여
자주 변경되지 않느 ㄴ헤더 파일에 대한 컴팡ㄹ을 매번 하지 않아도 되도록 하는 기능을 제공하기 때문이다.
그리고 프로젝트 마법사가 만들어준 코드에는 stdafx.h 파일이 포함되도록 되어있다.
*/
//DirectX SDK 라이브러리 헤더 추가

//시간과 관련된 멀티미디어 라이브러리 함수를 사용하기 위하여 다음 헤더 파일을 추가한다.
#include <Mmsystem.h>

//수학 라이브러리 함수를 사용하기 위하여 다음 헤더 파일을 추가한다.
#include <math.h>


// TODO: reference additional headers your program requires here

#include <d3dcompiler.h>  	//쉐이더 컴파일 함수를 사용하기 위한 헤더 파일
#include <D3DX10Math.h>	//Direct3D 수학 함수를 사용하기 위한 헤더 파일