#include "PreCompile.h"
#include "TitleLogo.h"

ATitleLogo::ATitleLogo()
{
	Renderer = CreateDefaultSubObject<USpriteRenderer>("Renderer");
	SetRoot(Renderer);
}

ATitleLogo::~ATitleLogo()
{
}


void ATitleLogo::BeginPlay()
{
	Super::BeginPlay();

	SetActorScale3D(FVector(700.0f, 300.0f, 100.0f));

	Renderer->SetSprite("holocure.png");
}

void ATitleLogo::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);
}
