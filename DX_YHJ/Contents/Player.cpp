#include "PreCompile.h"
#include "Player.h"
#include <EngineCore/Renderer.h>
#include <EngineCore/SpriteRenderer.h>
#include <EngineBase/EngineMath.h>
#include "HoloCursor.h"
#include "Weapon.h"
#include "KiaraWeapon.h"
#include "Asacoco.h"

float4 APlayer::PlayerColPos = float4::Zero;
float4 APlayer::PlayerPos = float4::Zero;

APlayer::APlayer()
{
	UDefaultSceneComponent* Root = CreateDefaultSubObject<UDefaultSceneComponent>("Renderer");

	Renderer = CreateDefaultSubObject<USpriteRenderer>("Renderer");
	Renderer->SetupAttachment(Root);
	Renderer->SetPivot(EPivot::BOT);

	ShadowRenderer = CreateDefaultSubObject<USpriteRenderer>("ShadowRenderer");
	ShadowRenderer->SetupAttachment(Root);
	ShadowRenderer->SetAutoSize(ContentsValue::MultipleSize, true);
	ShadowRenderer->SetPivot(EPivot::BOT);

	Collision = CreateDefaultSubObject<UCollision>("Collision");
	Collision->SetupAttachment(Root);
	Collision->SetScale({ 16.0f * ContentsValue::MultipleSize, 16.0f * ContentsValue::MultipleSize });
	Collision->SetCollisionGroup(ECollisionOrder::Player);
	Collision->SetCollisionType(ECollisionType::Rect);

	DetactingMonsterCollision = CreateDefaultSubObject<UCollision>("Collision");
	DetactingMonsterCollision->SetupAttachment(Root);
	DetactingMonsterCollision->SetScale({ 100.0f * ContentsValue::MultipleSize, 100.0f * ContentsValue::MultipleSize });
	DetactingMonsterCollision->SetCollisionGroup(ECollisionOrder::DetactingMonster);
	DetactingMonsterCollision->SetCollisionType(ECollisionType::CirCle);

	AtkDir = CreateDefaultSubObject<USpriteRenderer>("Renderer");
	AtkDir->SetupAttachment(Root);
	AtkDir->SetPivot(EPivot::MAX);

	SetRoot(Root);

	InputOn();
}

APlayer::~APlayer()
{
}

void APlayer::BeginPlay()
{
	Super::BeginPlay();

	CreatePlayerAnimation("Ame");
	CreatePlayerAnimation("Aqua");
	CreatePlayerAnimation("Ayame");
	CreatePlayerAnimation("AZKiPortrait");
	CreatePlayerAnimation("Kiara");
	CreatePlayerAnimation("Kronii"); 

	Renderer->SetAutoSize(ContentsValue::MultipleSize, true);
	Renderer->SetOrder(ERenderOrder::Player);

	ShadowRenderer->SetSprite("Shadow_0.png");
	ShadowRenderer->SetOrder(ERenderOrder::Shadow);
	                                                                        
	Collision->SetPosition({ GetActorLocation().X, GetActorLocation().Y + (10.0f * ContentsValue::MultipleSize) });

	AtkDir->SetOrder(ERenderOrder::Cursor);
	AtkDir->SetPosition(FVector{ PlayerPos.X, PlayerPos.Y + (20.0f * ContentsValue::MultipleSize) });

	AddWeapon<AKiaraWeapon>("KiaraWeapon");

	StateInit();
}

void APlayer::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	State.Update(_DeltaTime);

	PlayerColPos = GetActorLocation();
	PlayerPos = float4{ PlayerColPos.X, PlayerColPos.Y + (20.0f * ContentsValue::MultipleSize) };

	CheckMouseAimMode();
	ChangeMoveAimAtkDir();
	ChangeMouseAimAtkDir();

	CheckNearMonster();

	CheckHit(_DeltaTime);

	{
		for (MPlayerWeaponsIter = MPlayerWeapons.begin(); MPlayerWeaponsIter != MPlayerWeapons.end(); ++MPlayerWeaponsIter)
		{
			*MPlayerWeaponsIter;
		}
	}
}


void APlayer::CreatePlayerAnimation(std::string _Name)
{
	Renderer->CreateAnimation(_Name + "_Idle", _Name, 0.1f, true, 0, 3);
	Renderer->CreateAnimation(_Name + "_Run", _Name, 0.1f, true, 4, 9);
}

void APlayer::CalStatus()
{
	AtkTime = roundf(1.0f / (1.0f + Haste));

	CalSpeed = ContentsValue::BaseSpeed * Speed;
	LineSpeed = CalSpeed * 0.75f;
}

void APlayer::CheckMouseAimMode()
{
	if (false == AHoloCursor::MouseAimOn)
	{
		AtkDir->SetSprite("spr_arrow_1.png");
		AtkDir->SetAutoSize(ContentsValue::MultipleSize, true);
	}
	else
	{
		AtkDir->SetSprite("spr_arrow_2.png");
		AtkDir->SetAutoSize(ContentsValue::MultipleSize, true);
	}
}

void APlayer::ChangeMoveAimAtkDir()
{
	if (false == AHoloCursor::MouseAimOn)
	{
		switch (PlayerDir)
		{
		case EPlayerDir::N:
			Angle = 90.0f;
			AtkDir->SetRotationDeg(FVector{ 0.0f, 0.0f, Angle });
			break;
		case EPlayerDir::NE:
			Angle = 45.0f;
			AtkDir->SetRotationDeg(FVector{ 0.0f, 0.0f, Angle });
			break;
		case EPlayerDir::E:
			Angle = 0.0f;
			AtkDir->SetRotationDeg(FVector{ 0.0f, 0.0f, Angle });
			break;
		case EPlayerDir::SE:
			Angle = 315.0f;
			AtkDir->SetRotationDeg(FVector{ 0.0f, 0.0f, Angle });
			break;
		case EPlayerDir::S:
			Angle = 270.0f;
			AtkDir->SetRotationDeg(FVector{ 0.0f, 0.0f, Angle });
			break;
		case EPlayerDir::SW:
			Angle = 225.0f;
			AtkDir->SetRotationDeg(FVector{ 0.0f, 0.0f, Angle });
			break;
		case EPlayerDir::W:
			Angle = 180.0f;
			AtkDir->SetRotationDeg(FVector{ 0.0f, 0.0f, Angle });
			break;
		case EPlayerDir::NW:
			Angle = 135.0f;
			AtkDir->SetRotationDeg(FVector{ 0.0f, 0.0f, Angle });
			break;
		default:
			break;
		}
	}
}

void APlayer::ChangeMouseAimAtkDir()
{
	if (true == AHoloCursor::MouseAimOn)
	{
		Angle = atan2f((ContentsValue::PlayLevelMousePos.Y - APlayer::PlayerPos.Y), (ContentsValue::PlayLevelMousePos.X - APlayer::PlayerPos.X)) * 180.0f / UEngineMath::PI;
		AtkDir->SetRotationDeg(FVector{ 0.0f, 0.0f, Angle });

		if (-90.0f <= Angle && 90.0f > Angle)
		{
			Renderer->SetDir(EEngineDir::Right);
		}
		else
		{
			Renderer->SetDir(EEngineDir::Left);
		}
	}
}

void APlayer::CheckNearMonster()
{
	DetactingMonsterCollision->CollisionEnter(ECollisionOrder::Monster, [=](std::shared_ptr<UCollision> _Collison)
		{
			LDetactingMonster.push_back(dynamic_cast<AMonster*>(_Collison->GetActor()));
			int a = 0;
		}
	);

	DetactingMonsterCollision->CollisionExit(ECollisionOrder::Monster, [=](std::shared_ptr<UCollision> _Collison)
		{
			LDetactingMonster.remove(dynamic_cast<AMonster*>(_Collison->GetActor()));
			int a = 0;
		}
	);
}

void APlayer::CheckHit(float _DeltaTime)
{
	HitDelay -= _DeltaTime;
	if (0.0f >= HitDelay)
	{
		HitDelay = 0.1f;
	}

	Collision->CollisionEnter(ECollisionOrder::Monster, [=](std::shared_ptr<UCollision> _Collison)
		{
			AMonster* Monster = dynamic_cast<AMonster*>(_Collison->GetActor());

			float MAtk = Monster->GetAtk();

			Hp -= lround(MAtk);
		}
	);

	Collision->CollisionStay(ECollisionOrder::Monster, [=](std::shared_ptr<UCollision> _Collison)
		{
			if (0.0f >= HitDelay)
			{
				AMonster* Monster = dynamic_cast<AMonster*>(_Collison->GetActor());

				float MAtk = Monster->GetAtk();
				Hp -= lround(MAtk);

				HitDelay = 0.1f;
			}
		}
	);

	if (0 > Hp)
	{
		Hp = 0;
	}
}