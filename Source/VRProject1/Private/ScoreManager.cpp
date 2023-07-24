#include "ScoreManager.h"

#include <string>

#include "Components/PrimitiveComponent.h"
#include "Components/TextRenderComponent.h"

AScoreManager::AScoreManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AScoreManager::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStaticMeshActor::StaticClass(), FoundActors);
    
	if (BlueGoal && Ball)
	{
		UE_LOG(LogTemp, Warning, TEXT("FOUND BALL AND BLUEGOAL. GENERATING OVERLAP...."))
		BlueGoal->OnActorBeginOverlap.AddDynamic(this, &AScoreManager::OnOverlapBegin);
	}
	if (OrangeGoal && Ball)
	{
		UE_LOG(LogTemp, Warning, TEXT("FOUND BALL AND ORANGEGOAL. GENERATING OVERLAP...."))

		OrangeGoal->OnActorBeginOverlap.AddDynamic(this, &AScoreManager::OnOverlapBegin);
	}
}

void AScoreManager::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor)
{
	// If the actor that overlapped is Ball, increment the score
	if (OtherActor && (OtherActor != this))
	{
		if (OtherActor->IsA(AStaticMeshActor::StaticClass()) && OverlappedActor == BlueGoal)
		{
			orangeScore++;
			if (orangeScore < 3)
			{
				OScore1->GetTextRender()->SetText(FText::AsNumber(orangeScore));
				OScore2->GetTextRender()->SetText(FText::AsNumber(orangeScore));
			}
			else
			{
				OScore1 = 0;
				OScore2 = 0;
				BScore1 = 0;
				BScore2 = 0;
				OScore1->GetTextRender()->SetText(FText::AsNumber(orangeScore));
				OScore2->GetTextRender()->SetText(FText::AsNumber(orangeScore));
				BScore1->GetTextRender()->SetText(FText::AsNumber(blueScore));
				BScore2->GetTextRender()->SetText(FText::AsNumber(blueScore));

				// configure feature to open up doors here
			}
			
			UE_LOG(LogTemp, Warning, TEXT("Orange Score: %d"), orangeScore);
		}
		else if (OtherActor->IsA(AStaticMeshActor::StaticClass()) && OverlappedActor == OrangeGoal)
		{
			blueScore++;
			if (blueScore < 3)
			{
				BScore1->GetTextRender()->SetText(FText::AsNumber(blueScore));
				BScore2->GetTextRender()->SetText(FText::AsNumber(blueScore));
			}
			UE_LOG(LogTemp, Warning, TEXT("Blue Score: %d"), blueScore);
		}
		
	}
}

void AScoreManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}