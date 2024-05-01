#include "PreCompile.h"
#include "Asacoco.h"
#include "AsacocoBullet.h"

AAsacoco::AAsacoco()
{
}

AAsacoco::~AAsacoco()
{
}

void AAsacoco::BeginPlay()
{
	Super::BeginPlay();

	Name = "Asacoco";
}

void AAsacoco::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	if (0 < Delay) // ���� ���� ���� ��
	{
		
	}
	else // ���� ����
	{
		CreateBullet<AAsacocoBullet>();
		Delay = APlayGameMode::MainPlayer->GetAtkTime();
	}
}