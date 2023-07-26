#include "ScoreManager.h"

#include <string>

#include "Components/PrimitiveComponent.h"
#include "Components/SpotLightComponent.h"
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

	BlueGoal->AttachToActor(BlueGoalRim, FAttachmentTransformRules::KeepRelativeTransform);
	OrangeGoal->AttachToActor(OrangeGoalRim, FAttachmentTransformRules::KeepRelativeTransform);
    
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

	if (Flashlight)
	{
		Flashlight->SetActorHiddenInGame(true);
		Flashlight->GetStaticMeshComponent()->SetSimulatePhysics(false);
		Flashlight->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		for (UActorComponent* Component : Flashlight->GetComponents())
		{
			if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(Component))
			{
				StaticMeshComponent->SetVisibility(false, true);
			}
			else if (ULightComponent* LightComponent = Cast<USpotLightComponent>(Component))
			{
				LightComponent->SetVisibility(false, true);
			}
		}
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
				orangeScore = 0;
				blueScore = 0;
				OScore1->GetTextRender()->SetText(FText::FromString("ERROR"));
				OScore2->GetTextRender()->SetText(FText::FromString("ERROR"));
				BScore1->GetTextRender()->SetText(FText::FromString("ERROR"));
				BScore2->GetTextRender()->SetText(FText::FromString("ERROR"));
				OpenDoors();
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
			else
			{
				orangeScore = 0;
				blueScore = 0;
				OScore1->GetTextRender()->SetText(FText::FromString("NO"));
				OScore2->GetTextRender()->SetText(FText::FromString("NO"));
				BScore1->GetTextRender()->SetText(FText::FromString("HOPE"));
				BScore2->GetTextRender()->SetText(FText::FromString("HOPE"));
				OpenDoors();
				// configure feature to open up doors here
			}
			UE_LOG(LogTemp, Warning, TEXT("Blue Score: %d"), blueScore);
		}
		
	}
}

void AScoreManager::OpenDoors()
{
	OpeningDoors = true;
	Flashlight->GetStaticMeshComponent()->SetSimulatePhysics(true);
	Flashlight->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Flashlight->SetActorHiddenInGame(false);
	OrangeGoalRim->GetStaticMeshComponent()->SetSimulatePhysics(true);
	OrangeGoalRim->GetStaticMeshComponent()->SetEnableGravity(true);
	BlueGoalRim->GetStaticMeshComponent()->SetSimulatePhysics(true);
	BlueGoalRim->GetStaticMeshComponent()->SetEnableGravity(true);
	Light1->SetEnabled(false);
	Light2->SetEnabled(false);
	Light3->SetEnabled(false);
	Light4->SetEnabled(false);
	Light5->SetEnabled(false);
	Light6->SetEnabled(false);
	Light7->SetEnabled(false);
	Light8->SetEnabled(false);
	
	
	for (UActorComponent* Component : Flashlight->GetComponents())
	{
		if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(Component))
		{
			StaticMeshComponent->SetVisibility(true, true);
		}
		else if (ULightComponent* LightComponent = Cast<USpotLightComponent>(Component))
		{
			LightComponent->SetVisibility(true, true);
		}
	}
	Flashlight->GetStaticMeshComponent()->SetPhysicsAngularVelocityInDegrees(FVector(0.f,0.f,60.f));
}


void AScoreManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (OpeningDoors && 
		(FVector::Dist(Door1->GetActorLocation(), Door1Open->GetActorLocation()) > KINDA_SMALL_NUMBER) && 
		(FVector::Dist(Door2->GetActorLocation(), Door2Open->GetActorLocation()) > KINDA_SMALL_NUMBER))
	{
		FVector NewLocation1 = FMath::VInterpTo(Door1->GetActorLocation(), Door1Open->GetActorLocation(), DeltaTime, 0.1f);
		Door1->SetActorLocation(NewLocation1);
		
		FVector NewLocation2 = FMath::VInterpTo(Door2->GetActorLocation(), Door2Open->GetActorLocation(), DeltaTime, 0.1f);
		Door2->SetActorLocation(NewLocation2);
	}

	else if (OpeningDoors)
	{
		OpeningDoors = false;
	}
}
