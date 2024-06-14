#include "PreCompile.h"
#include "PlayGameMode.h"
#include "ContentsValue.h"
#include <EngineCore/SpriteRenderer.h>
#include <EngineCore/Camera.h>
#include <EngineCore/EngineDebugMsgWindow.h>
#include <EngineBase/EngineRandom.h>
#include "PlayGameModeGUI.h"
#include "Fubuzilla.h"

std::shared_ptr<APlayer> APlayGameMode::MainPlayer = nullptr;
bool APlayGameMode::IsPlayStart = true;
bool APlayGameMode::IsPause = false;

APlayGameMode::APlayGameMode()
{
	InputOn();
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

	// 카메라 세팅
	std::shared_ptr<UCamera> Camera = GetWorld()->GetMainCamera();
	float4 CameraPos = PlayerStartPos;
	CameraPos.Z = -500.0f;
	Camera->SetActorLocation(CameraPos);

	// 플레이어 생성
	Player = GetWorld()->SpawnActor<APlayer>("Player");
	Player->SetName("Kiara");
	Player->SetActorLocation(PlayerStartPos);
	MainPlayer = Player;
	//Player->State.ChangeState("Idle");

	// 커서 생성
	Cursor = GetWorld()->SpawnActor<AHoloCursor>("Cursor");

	// 지면 생성
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

	PlayLevelUI = GetWorld()->SpawnActor<APlayLevelUI>("PlayLevelUI");
	PauseUI = GetWorld()->SpawnActor<APauseUI>("PauseUI");
}

void APlayGameMode::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	ContentsValue::PlayLevelMousePos = FVector{ APlayer::PlayerColPos.X + AHoloCursor::CursorPos.X - (ContentsValue::WindowSize.X / 2.0f), APlayer::PlayerColPos.Y - AHoloCursor::CursorPos.Y + (ContentsValue::WindowSize.Y / 2.0f) };

	InfinityGroundCheck();

	if (true == IsPlayStart)
	{
		
		IsPlayStart = false;
	}
	SpawnNomalMonsterTimeSet(PlayTime, 0.5f, 20.0f, 5.0f, SpawnTerm1,
		"Shrimp", 1.0f, 8, 2.0f, 0.35f, 6.0f, EMonsterMoveType::Follow,
		false, 10);
	//SpawnNomalMonsterTimeSet(PlayTime, 0.5f, 20.0f, 10.0f, SpawnTerm2,
	//	"Shrimp", 1.0f, 8, 2.0f, 0.35f, 6.0f, EMonsterMoveType::Follow,
	//	true, 10);
	//SpawnNomalMonsterTimeSet(PlayTime, 20.0f, 40.0f, 5.0f, SpawnTerm1,
	//	"Deadbeat", 1.0f, 40, 4.0f, 0.4f, 7.0f, EMonsterMoveType::Follow,
	//	false, 5);
	//SpawnNomalMonsterTimeSet(PlayTime, 40.0f, 60.0f, 5.0f, SpawnTerm1,
	//	"Takodachi", 1.0f, 80, 4.0f, 0.4f, 8.0f);
	//SpawnNomalMonsterTimeSet(PlayTime, 60.0f, 80.0f, 5.0f, SpawnTerm1,
	//	"KFP", 1.0f, 20, 2.0f, 1.0f, 3.0f, EMonsterMoveType::StraightToPlayer,
	//	true, 20.0f, true, 10);
	//SpawnBossMonsterTimeSet(5.0f, "Fubuzilla");

	PlayLevelUI->SetPlayTime(PlayTime);

	Pause(_DeltaTime);
	CheckPauseButtonSelect();

	PlayDebugText(_DeltaTime);
}

void APlayGameMode::LevelEnd(ULevel* _NextLevel)
{
	Super::LevelEnd(_NextLevel);

	if (true == IsPause)
	{
		IsPause = false;
		AHoloCursor::MouseAimOn = IsPrevMouseAim;
		GEngine->SetOrderTimeScale(0, 1.f);
		PauseUI->AllActiveOff();
	}
}

void APlayGameMode::LevelStart(ULevel* _PrevLevel)
{
	Super::LevelStart(_PrevLevel);

	ContentsValue::ChangeBGM("StageOneBGM.mp3");

	IsPause = false;

	PlayLevelUI->StartSetting();
	Player->State.ChangeState("Idle");
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

std::shared_ptr<ANomalMonster> APlayGameMode::SpawnNomalMonster(std::string _Name, float _Size, int _Hp, float _Atk, float _Speed, float _Exp, EMonsterMoveType _MoveType, bool _WillTimeOutDestroy, float _TimeOutDestoryDelay)
{
	std::shared_ptr<ANomalMonster> Monster;

	Monster = GetWorld()->SpawnActor<ANomalMonster>(_Name);
	Monster->GetRenderer()->SetAutoSize(_Size * ContentsValue::MultipleSize, true);
	Monster->GetRenderer()->ChangeAnimation(_Name);
	Monster->SetMonsterStatus(_Hp, _Atk, _Speed, _Exp, _MoveType, _WillTimeOutDestroy, _TimeOutDestoryDelay);
	Monster->GetCollosion()->SetScale({ _Size * 16.0f * ContentsValue::MultipleSize, _Size * 16.0f * ContentsValue::MultipleSize });
	Monster->GetCollosion()->SetPosition({ Monster->GetActorLocation().X, Monster->GetActorLocation().Y + (_Size * 10.0f * ContentsValue::MultipleSize) });
	Monster->GetSavedRenderer()->SetPosition({ Monster->GetActorLocation().X, Monster->GetActorLocation().Y + (50.0f * ContentsValue::MultipleSize) });
	Monster->GetOverCheckCollision()->SetScale({ _Size * 16.0f * ContentsValue::MultipleSize, _Size * 32.0f * ContentsValue::MultipleSize });
	Monster->GetOverCheckCollision()->SetPosition({ Monster->GetActorLocation().X, Monster->GetActorLocation().Y + (_Size * 18.0f * ContentsValue::MultipleSize)});

	return Monster;
}

float4 APlayGameMode::RandomLocation(bool _Group)
{
	float4 MonsterPos;
	// 뭉쳐서 나오지 않을 때
	if (false == _Group)
	{
		MonsterPos = APlayer::PlayerPos;

		while (MonsterPos.X > (APlayer::PlayerPos.X - 350.0f) && MonsterPos.X < (APlayer::PlayerPos.X + 350.0f))
		{
			MonsterPos.X += UEngineRandom::MainRandom.RandomFloat(-450.0f, 450.0f);
		}
		while (MonsterPos.Y > (APlayer::PlayerPos.Y - 300.0f) && MonsterPos.Y < (APlayer::PlayerPos.Y + 300.0f))
		{
			MonsterPos.Y += UEngineRandom::MainRandom.RandomFloat(-450.0f, 450.0f);
		}
	}
	else
	{
		//뭉쳐서 나올 때 
		if (false == GroupSpawn)
		{
			GroupMonsterPos = APlayer::PlayerPos;

			while (GroupMonsterPos.X > (APlayer::PlayerPos.X - 350.0f) && GroupMonsterPos.X < (APlayer::PlayerPos.X + 350.0f))
			{
				GroupMonsterPos.X += UEngineRandom::MainRandom.RandomFloat(-450.0f, 450.0f);
			}
			while (GroupMonsterPos.Y > (APlayer::PlayerPos.Y - 300.0f) && GroupMonsterPos.Y < (APlayer::PlayerPos.Y + 300.0f))
			{
				GroupMonsterPos.Y += UEngineRandom::MainRandom.RandomFloat(-450.0f, 450.0f);
			}
		}

		MonsterPos = GroupMonsterPos;

		MonsterPos.X += UEngineRandom::MainRandom.RandomFloat(-50.0f, 50.0f);
		MonsterPos.Y += UEngineRandom::MainRandom.RandomFloat(-50.0f, 50.0f);
	}

	return MonsterPos;
}

void APlayGameMode::RandomSpawnNomalMonster(std::string _Name, float _Size, int _Hp, float _Atk, float _Speed, float _Exp, EMonsterMoveType _MoveType, 
	bool _WillTimeOutDestroy, float _TimeOutDestoryDelay, bool _Group, int _Quantity)
{
	if (0 >= _Quantity)
	{
		MsgBoxAssert("스폰하려는 몬스터의 수가 0 이하 입니다.");
		return;
	}

	FVector GroupToPlayerDir;

	for (int i = 0; i < _Quantity; i++)
	{
		std::shared_ptr<ANomalMonster> Monster = SpawnNomalMonster(_Name, _Size, _Hp, _Atk, _Speed, _Exp, _MoveType, _WillTimeOutDestroy, _TimeOutDestoryDelay);

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

void APlayGameMode::SpawnNomalMonsterTimeSet(float _DeltaTime, float _SpawnBegin, float _SpawnEnd, float _Term, float& _SpawnTerm,
	std::string _Name, float _Size, int _Hp, float _Atk, float _Speed, float _Exp, EMonsterMoveType _MoveType,
	bool _WillTimeOutDestroy, float _TimeOutDestoryDelay, bool _Group, int _Quantity)
{
	if (PlayTime >= _SpawnBegin && PlayTime < _SpawnEnd)
	{
		if (0.0f >= _SpawnTerm)
		{
			RandomSpawnNomalMonster(_Name, _Size, _Hp, _Atk, _Speed, _Exp, _MoveType, _Group, _Quantity, _WillTimeOutDestroy, _TimeOutDestoryDelay);
			_SpawnTerm = _Term;
		}
		else
		{
			_SpawnTerm -= PlayDeltaTime;
		}
	}
}

void APlayGameMode::SpawnBossMonsterTimeSet(float _SpawnTime, std::string _Name)
{
	if (PlayTime >= _SpawnTime && PlayTime < (_SpawnTime + 1.0f))
	{
		if (0.0f >= BossSpawn)
		{
			if (UEngineString::ToUpper("Fubuzilla") == UEngineString::ToUpper(_Name))
			{
				std::shared_ptr<AFubuzilla> Boss = GetWorld()->SpawnActor<AFubuzilla>("Fubuzilla");
				Boss->SetActorLocation(RandomLocation());

				BossSpawn = 1.5f;
			}
		}
		else if (PlayTime >= (_SpawnTime + 1.0f) && PlayTime < (_SpawnTime + 2.0f))
		{
			BossSpawn = 0.0f;
		}
	}
}

void APlayGameMode::Pause(float _DeltaTime)
{
	if (true == IsDown(VK_ESCAPE))
	{
		if (true == IsPause)
		{
			IsPause = false;
			AHoloCursor::MouseAimOn = IsPrevMouseAim;
			GEngine->SetOrderTimeScale(0, 1.f);
			PauseUI->AllActiveOff();
		}
		else
		{
			IsPause = true;
			IsPrevMouseAim = AHoloCursor::MouseAimOn;
			GEngine->SetOrderTimeScale(0, 0.f);
			PauseUI->AllActiveOn();
		}
	}

	if (false == IsPause)
	{
		PlayDeltaTime = _DeltaTime;
	}
	else
	{
		PlayDeltaTime = 0.0f;
	}
	PlayTime += PlayDeltaTime;
	_DeltaTime = 0.0f;
}

void APlayGameMode::CheckPauseButtonSelect()
{
	if (true == IsPause)
	{
		if (true == IsDown('W'))
		{
			--PauseButtonSelect;
			if (0 > PauseButtonSelect)
			{
				PauseButtonSelect = PauseUI->GetVPauseButtonSize() - 1;
			}
		}
		else if (true == IsDown('S'))
		{
			++PauseButtonSelect;
			if (PauseUI->GetVPauseButtonSize() <= PauseButtonSelect)
			{
				PauseButtonSelect = 0;
			}
		}

		for (int i = 0; i < 2; ++i)
		{
			if (true == PauseUI->GetVPauseButtonIsCursorOn(i))
			{
				PauseButtonSelect = i;
			}
			PauseUI->SetVPauseButtonIsSelect(i, false);
		}

		switch (PauseButtonSelect)
		{
		case 0:
			PauseUI->SetVPauseButtonIsSelect(PauseButtonSelect, true);
			if (true == IsDown(VK_RETURN))
			{
				IsPause = false;
				AHoloCursor::MouseAimOn = IsPrevMouseAim;
				GEngine->SetOrderTimeScale(0, 1.f);
				PauseUI->AllActiveOff();
			}
			else if (true == PauseUI->GetVPauseButtonIsCursorOn(PauseButtonSelect))
			{
				if (true == IsDown(VK_LBUTTON))
				{
					IsPause = false;
					AHoloCursor::MouseAimOn = IsPrevMouseAim;
					GEngine->SetOrderTimeScale(0, 1.f);
					PauseUI->AllActiveOff();
				}
			}
			break;
		case 1:
			PauseUI->SetVPauseButtonIsSelect(PauseButtonSelect, true);
			if (true == IsDown(VK_RETURN))
			{
				GEngine->ChangeLevel("LobbyLevel");
			}
			else if (true == PauseUI->GetVPauseButtonIsCursorOn(PauseButtonSelect))
			{
				if (true == IsDown(VK_LBUTTON))
				{
					GEngine->ChangeLevel("LobbyLevel");
				}
			}
			break;
		default:
			break;
		}
	}
}

void APlayGameMode::PlayDebugText(float _DeltaTime)
{
	AGameMode* ThisLevel = dynamic_cast<AGameMode*>(this);

	FIntPoint Index = PosToIndex(APlayer::PlayerPos);
	CurIndex = Index;

	//프레임 체크
	UEngineDebugMsgWindow::PushMsg(std::format("Frame : {}\n", 1.0f / _DeltaTime));
	//플레이어 위치
	UEngineDebugMsgWindow::PushMsg(std::format("PlayerPos : X : {}, Y : {}", APlayer::PlayerPos.X, APlayer::PlayerPos.Y));
	//플레이어가 있는 BackGround
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
	//마지막으로 움직인 방향
	UEngineDebugMsgWindow::PushMsg(std::format("PlayerDir : {}", PlayerDir));
	//마우스모드 각도
	UEngineDebugMsgWindow::PushMsg(std::format("Angle : {}", Player->GetAngle()));
}