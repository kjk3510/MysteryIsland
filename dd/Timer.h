#pragma once

const ULONG MAX_SAMPLE_COUNT = 50; //50ȸ�� ������ ó���ð��� �����Ͽ� ����Ѵ�.

class CGameTimer
{
public:
	CGameTimer();
	virtual ~CGameTimer();

	void Tick(float fLockFPS = 0.0f);
	//Ÿ�̸��� �ð��� ����
	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0);
	//������ ����Ʈ�� ��ȯ
	float GetTimeElapsed();
	//�������� ��� ��� �ð��� ��ȯ

private:
	bool m_bHardwareHasPerformanceCounter;
	//��ǻ�Ͱ� performance counter�� ������ �ִ°�.
	float m_fTimeScale;
	//scale counter�� ��
	float m_fTimeElapsed;
	//������ ������ ���� ������ �ð�
	__int64 m_nCurrentTime;
	//������ �ð�
	__int64 m_nLastTime;
	//������ �������� �ð�
	__int64 m_PerformanceFrequency;
	//��ǻ���� performance frequency

	float m_fFrameTime[MAX_SAMPLE_COUNT];
	//�����ӽð��� �����ϱ� ���� �迭
	ULONG m_nSampleCount;
	//������ ������ ȹ��

	unsigned long m_nCurrentFrameRate;
	//������ ������ ����Ʈ
	unsigned long m_FramePerSecond;
	//�ʴ� ������ ��
	float m_fFPSTimeElapsed;
	//������ ����Ʈ ��� �ҿ� �ð�
};


