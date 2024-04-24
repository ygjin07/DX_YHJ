#include "PreCompile.h"
#include "Melee.h"

AMelee::AMelee()
{
}

AMelee::~AMelee()
{
}

void AMelee::BeginPlay()
{
	Super::BeginPlay();
}

void AMelee::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	//������ ���϶�
	if (true == Renderer->IsActive())
	{
		//���� �ִϸ��̼��� ����� ����
		if (true == Renderer->IsCurAnimationEnd())
		{
			Renderer->SetActive(false);
		}
		else
		{
			int a = 0;
		}
	}
	else // ������ ������� ���� ��
	{
		Angle = PlayerAngle;
		if (0 < Delay) // ���� ���� ���� ��
		{
			Renderer->SetRotationDeg(FVector{ 0.0f, 0.0f, Angle });
			Delay -= _DeltaTime;
		}
		else // ���� ����
		{
			Delay = AtkTime;
			Renderer->SetActive(true);
		}
	}
}

void AMelee::SetKnifeTypeMeleeLocation(float _DistanceFromPlayer)
{
	Dir = float4::DegToDir(Angle);
	Dir.Z = 0.0f;

	SetActorLocation(FVector{ APlayer::PlayerPos.X, APlayer::PlayerPos.Y + (20.0f * ContentsValue::MultipleSize) });
	AddActorLocation(Dir * _DistanceFromPlayer * ContentsValue::MultipleSize);
}