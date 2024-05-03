#include "PreCompile.h"
#include "TitleGameMode.h"
#include "TitleLogo.h"
#include "TitleBackGround.h"
#include <EngineCore/Camera.h>

ATitleGameMode::ATitleGameMode()
{
	InputOn();
}

ATitleGameMode::~ATitleGameMode()
{
}

void ATitleGameMode::BeginPlay()
{
	Super::BeginPlay();

	std::shared_ptr<UCamera> Camera = GetWorld()->GetMainCamera();
	Camera->SetActorLocation(FVector(0.0f, 0.0f, -100.0f));

	GetWorld()->SpawnActor<ATitleBackGround>("TitleBackGround");

	Cursor = GetWorld()->SpawnActor<AHoloCursor>("Cursor");
}

void ATitleGameMode::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);
}

void ATitleGameMode::LevelEnd(ULevel* _NextLevel)
{
	Super::LevelEnd(_NextLevel);
}

void ATitleGameMode::LevelStart(ULevel* _PrevLevel)
{
	Super::LevelStart(_PrevLevel);

}