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
		std::shared_ptr<AAsacocoBullet> Bullet;
		Bullet = GetWorld()->SpawnActor<AAsacocoBullet>("Bullet");

		Delay = APlayGameMode::MainPlayer->GetAtkTime();
	}
}