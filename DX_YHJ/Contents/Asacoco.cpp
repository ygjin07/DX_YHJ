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

	if (0 < Delay) // 공격 쿨이 돌기 전
	{
		
	}
	else // 공격 시작
	{
		CreateBullet<AAsacocoBullet>();
		Delay = APlayGameMode::MainPlayer->GetAtkTime();
	}
}