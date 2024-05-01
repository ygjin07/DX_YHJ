#include "PreCompile.h"
#include "PlayGameMode.h"
#include "ContentsValue.h"
#include "Monster.h"
#include <EngineCore/SpriteRenderer.h>
#include <EngineCore/Camera.h>
#include <EngineCore/EngineDebugMsgWindow.h>
#include <EngineBase/EngineRandom.h>

std::shared_ptr<APlayer> APlayGameMode::MainPlayer = nullptr;

APlayGameMode::APlayGameMode()
{
}

APlayGameMode::~APlayGameMode()
{
}

void APlayGameMode::BeginPlay()
{
	Super::BeginPlay();

	float4 MapScale = UEngineTexture::FindRes("Holo_Stage_1_Back_Ground.png")->GetScale();

	CurIndex = { 0, 0 };
	float4 PlayerStartPos = IndexToCenterPos(CurIndex);

	// ī�޶� ����
	std::shared_ptr<UCamera> Camera = GetWorld()->GetMainCamera();
	float4 CameraPos = PlayerStartPos;
	CameraPos.Z = -500.0f;
	Camera->SetActorLocation(CameraPos);
	
	// �÷��̾� ����
	Player = GetWorld()->SpawnActor<APlayer>("Player");
	Player->SetName("Kiara");
	Player->SetActorLocation(PlayerStartPos);
	MainPlayer = Player;
	Player->State.ChangeState("Idle");

	// Ŀ�� ����
	Cursor = GetWorld()->SpawnActor<AHoloCursor>("Cursor");
	AHoloCursor::MouseAimOn = false;
	AHoloCursor::CursorPos = GEngine->EngineWindow.GetScreenMousePos();
	Cursor->SetActorLocation(AHoloCursor::CursorPos);

	// ���� ����
	for (int y = -1; y < 2; y++)
	{
		for (int x = -1; x < 2; x++)
		{
			std::shared_ptr<APlayBackGround> BackGround = GetWorld()->SpawnActor<APlayBackGround>("PlayBackGround");

			BackGround->SetActorScale3D(ContentsValue::GroundTileSize);

			FIntPoint Point;
			Point.X = x;
			Point.Y = y;

			float4 Pos;
			Pos.X = ContentsValue::GroundTileSize.X * x;
			Pos.Y = ContentsValue::GroundTileSize.Y * y;

			Pos.X += ContentsValue::GroundTileSize.hX();
			Pos.Y += ContentsValue::GroundTileSize.hY();
			BackGround->SetActorLocation(Pos);

			BackGroundVector.push_back(BackGround);
		}
	}
}

void APlayGameMode::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	AHoloCursor::CursorPos = GEngine->EngineWindow.GetScreenMousePos();
	ContentsValue::PlayLevelMousePos = FVector{ APlayer::PlayerColPos.X + AHoloCursor::CursorPos.X - (ContentsValue::WindowSize.X / 2.0f), APlayer::PlayerColPos.Y - AHoloCursor::CursorPos.Y + (ContentsValue::WindowSize.Y / 2.0f) };
	Cursor->SetActorLocation(ContentsValue::PlayLevelMousePos);

	InfinityGroundCheck();

	SpawnMonsterTimeSet(_DeltaTime, 0.0f, 20.0f, 5.0f, 
		"Shrimp", 2.0f, 8.0f, 2.0f, 0.35f, 6.0f, EMonsterMoveType::Follow, 
		false, 10);
	SpawnMonsterTimeSet(_DeltaTime, 0.0f, 20.0f, 10.0f,
		"Shrimp", 1.0f, 8.0f, 2.0f, 0.35f, 6.0f, EMonsterMoveType::Follow,
		true, 10);
	SpawnMonsterTimeSet(_DeltaTime, 20.0f, 40.0f, 5.0f, 
		"Deadbeat", 1.0f, 40.0f, 4.0f, 0.4f, 7.0f, EMonsterMoveType::Follow,
		false, 5);
	SpawnMonsterTimeSet(_DeltaTime, 40.0f, 60.0f, 5.0f, 
		"Takodachi", 1.0f, 80.0f, 4.0f, 0.4f, 8.0f, EMonsterMoveType::Follow);
	SpawnMonsterTimeSet(_DeltaTime, 60.0f, 80.0f, 5.0f, 
		"KFP", 1.0f, 20.0f, 2.0f, 1.0f, 3.0f, EMonsterMoveType::StraightToPlayer,
		true, 10);

	PlayTime += _DeltaTime;

	PlayDebugText();
}

void APlayGameMode::LevelEnd(ULevel* _NextLevel)
{
	Super::LevelEnd(_NextLevel);
}

void APlayGameMode::LevelStart(ULevel* _PrevLevel)
{
	Super::LevelStart(_PrevLevel);
}

float4 APlayGameMode::IndexToCenterPos(FIntPoint _Index)
{
	float4 Pos;
	Pos.X = ContentsValue::GroundTileSize.X * _Index.X;
	Pos.Y = ContentsValue::GroundTileSize.Y * _Index.Y;

	Pos.X += ContentsValue::GroundTileSize.hX();
	Pos.Y += ContentsValue::GroundTileSize.hY();

	return Pos;
}

FIntPoint APlayGameMode::PosToIndex(float4 _Pos)
{
	FIntPoint Index;

	float4 Location = _Pos;

	float4 Pos;
	Pos.X = Location.X / ContentsValue::GroundTileSize.X;
	Pos.Y = Location.Y / ContentsValue::GroundTileSize.Y;

	if (0 >= Pos.X)
	{
		Pos.X += -1;
	}
	if (0 >= Pos.Y)
	{
		Pos.Y += -1;
	}

	Index.X = Pos.X;
	Index.Y = Pos.Y;
	return Index;
}

void APlayGameMode::InfinityGroundCheck()
{
	FIntPoint Index = PosToIndex(APlayer::PlayerPos);

	if (Index.X != CurIndex.X || Index.Y != CurIndex.Y)
	{
		int GroundCount = 0;

		float4 MovePos;
		MovePos.X = Index.X * ContentsValue::GroundTileSize.X;
		MovePos.Y = Index.Y * ContentsValue::GroundTileSize.Y;

		for (int y = -1; y < 2; y++)
		{
			for (int x = -1; x < 2; x++)
			{
				std::shared_ptr<APlayBackGround> BackGround = BackGroundVector[GroundCount];

				BackGround->SetActorScale3D(ContentsValue::GroundTileSize);

				FIntPoint Point;
				Point.X = x;
				Point.Y = y;

				float4 Pos;
				Pos.X = ContentsValue::GroundTileSize.X * x;
				Pos.Y = ContentsValue::GroundTileSize.Y * y;

				Pos.X += ContentsValue::GroundTileSize.hX();
				Pos.Y += ContentsValue::GroundTileSize.hY();
				BackGround->SetActorLocation(Pos + MovePos);

				++GroundCount;
			}
		}

		CurIndex = Index;
	}
}

template <typename Monster>
std::shared_ptr<Monster> APlayGameMode::SpawnMonster(std::string _Name, float _Size, float _Hp, float _Atk, float _Speed, float _Exp, EMonsterMoveType _MoveType)
{
	std::shared_ptr<AMonster> Monster;

	Monster = GetWorld()->SpawnActor<AMonster>(_Name);
	Monster->GetRenderer()->SetAutoSize(_Size * ContentsValue::MultipleSize, true);
	Monster->GetRenderer()->ChangeAnimation(_Name);
	Monster->SetMonsterStatus(_Hp, _Atk, _Speed, _Exp, _MoveType);
	Monster->GetCollosion()->SetScale({ _Size * 16.0f * ContentsValue::MultipleSize, _Size * 16.0f * ContentsValue::MultipleSize });
	Monster->GetCollosion()->SetPosition({ Monster->GetActorLocation().X, Monster->GetActorLocation().Y + (_Size * 10.0f * ContentsValue::MultipleSize) });
	Monster->GetSavedRenderer()->SetPosition({ Monster->GetActorLocation().X, Monster->GetActorLocation().Y + (50.0f * ContentsValue::MultipleSize)});

	return Monster;
}

void APlayGameMode::RandomSpawnMonster(std::string _Name, float _Size, float _Hp, float _Atk, float _Speed, float _Exp, EMonsterMoveType _MoveType, 
	bool _Group, int _Quantity)
{
	if (0 >= _Quantity)
	{
		MsgBoxAssert("�����Ϸ��� ������ ���� 0 ���� �Դϴ�.");
		return;
	}

	FVector GroupToPlayerDir;
	
	for (int i = 0; i < _Quantity; i++)
	{
		std::shared_ptr<AMonster> Monster = SpawnMonster<AMonster>(_Name, _Size, _Hp, _Atk, _Speed, _Exp, _MoveType);

		FVector GroupPos = RandomLocation(_Group);
		Monster->SetActorLocation(GroupPos);
		if (true == _Group)
		{
			if (false == GroupSpawn)
			{
				GroupToPlayerDir = Monster->CreateGroupToPlayerDir();
				Monster->SetToPlayerDir(GroupToPlayerDir);
				GroupSpawn = true;
			}
			else
			{
				Monster->SetToPlayerDir(GroupToPlayerDir);
			}
		}
		else
		{
			FVector Dir = APlayer::PlayerPos - Monster->GetActorLocation();
			Dir = Dir.Normalize2DReturn();
			Monster->SetToPlayerDir(Dir);
		}
	}
	GroupSpawn = false;
}

float4 APlayGameMode::RandomLocation(bool _Group)
{
	float4 MonsterPos;
	// ���ļ� ������ ���� ��
	if (false == _Group)
	{
		MonsterPos = APlayer::PlayerPos;

		while (MonsterPos.X > (APlayer::PlayerPos.X - 300.0f) && MonsterPos.X < (APlayer::PlayerPos.X + 300.0f))
		{
			MonsterPos.X += UEngineRandom::MainRandom.RandomFloat(-5.0f, 5.0f) * 100.0f;
		}
		while (MonsterPos.Y > (APlayer::PlayerPos.Y - 250.0f) && MonsterPos.Y < (APlayer::PlayerPos.Y + 250.0f))
		{
			MonsterPos.Y += UEngineRandom::MainRandom.RandomFloat(-5.0f, 5.0f) * 100.0f;
		}
	}
	else
	{
		//���ļ� ���� �� 
		if (false == GroupSpawn)
		{
			GroupMonsterPos = APlayer::PlayerPos; 

			while (GroupMonsterPos.X > (APlayer::PlayerPos.X - 300.0f) && GroupMonsterPos.X < (APlayer::PlayerPos.X + 300.0f))
			{
				GroupMonsterPos.X += UEngineRandom::MainRandom.RandomFloat(-5.0f, 5.0f) * 100.0f;
			}
			while (GroupMonsterPos.Y > (APlayer::PlayerPos.Y - 250.0f) && GroupMonsterPos.Y < (APlayer::PlayerPos.Y + 250.0f))
			{
				GroupMonsterPos.Y += UEngineRandom::MainRandom.RandomFloat(-5.0f, 5.0f) * 100.0f;
			}
		}

		MonsterPos = GroupMonsterPos;

		MonsterPos.X += UEngineRandom::MainRandom.RandomFloat(-5.0f, 5.0f) * 10.0f;
		MonsterPos.Y += UEngineRandom::MainRandom.RandomFloat(-5.0f, 5.0f) * 10.0f;
	}
	
	return MonsterPos;
}

void APlayGameMode::SpawnMonsterTimeSet(float _DeltaTime, float _SpawnBegin, float _SpawnEnd, float _Term, 
	std::string _Name, float _Size, float _Hp, float _Atk, float _Speed, float _Exp, EMonsterMoveType _MoveType, 
	bool _Group, int _Quantity)
{
	if (PlayTime >= _SpawnBegin && PlayTime < _SpawnEnd)
	{
		if (SpawnTerm <= 0)
		{
			RandomSpawnMonster(_Name, _Size, _Hp, _Atk, _Speed, _Exp, _MoveType, _Group, _Quantity);
			SpawnTerm = _Term;
		}
		else
		{
			SpawnTerm -= _DeltaTime;
		}
	}
}

void APlayGameMode::PlayDebugText()
{
	AGameMode* ThisLevel = dynamic_cast<AGameMode*>(this);

	FIntPoint Index = PosToIndex(APlayer::PlayerPos);
	CurIndex = Index;
	
	//�÷��̾� ��ġ
	UEngineDebugMsgWindow::PushMsg(std::format("PlayerPos : X : {}, Y : {}", APlayer::PlayerPos.X, APlayer::PlayerPos.Y));
	//�÷��̾ �ִ� BackGround
	UEngineDebugMsgWindow::PushMsg(std::format("BackGroundIndex : {}, {}", Index.X, Index.Y));
	
	std::string PlayerDir = "";
	switch (Player->GetPlayerDir())
	{
	case EPlayerDir::N:
		PlayerDir = "N";
		break;
	case EPlayerDir::NE:
		PlayerDir = "NE";
		break;
	case EPlayerDir::NW:
		PlayerDir = "NW";
		break;
	case EPlayerDir::E:
		PlayerDir = "E";
		break;
	case EPlayerDir::W:
		PlayerDir = "W";
		break;
	case EPlayerDir::S:
		PlayerDir = "S";
		break;
	case EPlayerDir::SE:
		PlayerDir = "SE";
		break;
	case EPlayerDir::SW:
		PlayerDir = "SW";
		break;
	default:
		break;
	}
	//���������� ������ ����
	UEngineDebugMsgWindow::PushMsg(std::format("PlayerDir : {}", PlayerDir));
	//���콺��� ����
	UEngineDebugMsgWindow::PushMsg(std::format("Angle : {}", Player->GetAngle()));
}