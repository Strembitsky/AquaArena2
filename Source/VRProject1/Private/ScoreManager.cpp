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
	PowerResetOnce = false;
	turnPowerBackOn1 = false;
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStaticMeshActor::StaticClass(), FoundActors);

	OTextColor = OScore1->GetTextRender()->TextRenderColor;
	BTextColor = BScore1->GetTextRender()->TextRenderColor;
	
	Ball->GetStaticMeshComponent()->OnComponentHit.AddDynamic(this, &AScoreManager::OnHit);
	
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
		FlashlightLight->SetEnabled(false);
		FlashlightLight2->SetEnabled(false);
		InitFlashlightPos = Flashlight->GetActorTransform();
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

void AScoreManager::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor)
	{
		FString hitActorName = OtherActor->GetName();
		if (hitActorName == "StaticMeshActor_40" || hitActorName == "StaticMeshActor_39")
		{
			BallCollideHard->Play();
		}
		else
		{
			BallCollideSoft->Play();	
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
				GoalScore->Play();
				OScore1->GetTextRender()->SetText(FText::AsNumber(orangeScore));
				OScore2->GetTextRender()->SetText(FText::AsNumber(orangeScore));
			}
			else if (orangeScore == 3)
			{
				if (!PowerResetOnce)
				{
					breakScore1();
					OpenDoor1();
				}
				else if (!PowerResetTwice)
				{
					breakScore2();
					OpenDoor2();
				}
			}
			
			UE_LOG(LogTemp, Warning, TEXT("Orange Score: %d"), orangeScore);
		}
		else if (OtherActor->IsA(AStaticMeshActor::StaticClass()) && OverlappedActor == OrangeGoal)
		{
			blueScore++;
			if (blueScore < 3)
			{
				GoalScore->Play();
				BScore1->GetTextRender()->SetText(FText::AsNumber(blueScore));
				BScore2->GetTextRender()->SetText(FText::AsNumber(blueScore));
			}
			else if (blueScore == 3)
			{
				if (!PowerResetOnce)
				{
					breakScore1();
					OpenDoor1();
				}
				else if (!PowerResetTwice)
				{
					breakScore2();
					OpenDoor2();
				}
			}
			UE_LOG(LogTemp, Warning, TEXT("Blue Score: %d"), blueScore);
		}
		
	}
}

void AScoreManager::OpenDoor1()
{
	OpeningDoor1 = true;
	Flashlight->GetStaticMeshComponent()->SetSimulatePhysics(true);
	Flashlight->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Flashlight->SetActorHiddenInGame(false);
	FlashlightLight->SetEnabled(true);
	FlashlightLight2->SetEnabled(true);
	FlashlightBuzz->Play();
	BallSound->Stop();
	powerRef->TurnPowerOff1();
	
	
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

void AScoreManager::OpenDoor2()
{
	OpeningDoor1 = true;
	Flashlight->SetActorTransform(InitFlashlightPos);
	BallSound->Stop();
	powerRef->TurnPowerOff2();
	Flashlight->GetStaticMeshComponent()->SetPhysicsAngularVelocityInDegrees(FVector(0.f,0.f,60.f));
}

void AScoreManager::breakScore1()
{
	orangeScore = 3;
	blueScore = 3;
	FString noHopeString = "";
	for(int i = 0; i < 9; i++) {
		noHopeString += "errorERRORnullErrorERROR\n";
	}

	OScore1->GetTextRender()->SetText(FText::FromString(noHopeString));
	OScore2->GetTextRender()->SetText(FText::FromString(noHopeString));
	BScore1->GetTextRender()->SetText(FText::FromString(noHopeString));
	BScore2->GetTextRender()->SetText(FText::FromString(noHopeString));
	Hyphen1->GetTextRender()->SetText(FText::FromString("nullPTRexceptEXCEPTION\nIS BLANK OR EMPTY\nNULL NULL NULL NULL"));
	Hyphen2->GetTextRender()->SetText(FText::FromString("nullPTRexceptEXCEPTION\nIS BLANK OR EMPTY\nNULL NULL NULL NULL"));
	OScore1->GetTextRender()->SetWorldSize(30);
	OScore2->GetTextRender()->SetWorldSize(30);
	BScore1->GetTextRender()->SetWorldSize(30);
	BScore2->GetTextRender()->SetWorldSize(30);
	Hyphen1->GetTextRender()->SetWorldSize(60);
	Hyphen2->GetTextRender()->SetWorldSize(60);
	OScore1->GetTextRender()->TextRenderColor = FColor::White;
	OScore2->GetTextRender()->TextRenderColor = FColor::White;
	Hyphen1->GetTextRender()->TextRenderColor = FColor::Red;
	Hyphen2->GetTextRender()->TextRenderColor = FColor::Red;
	BScore1->GetTextRender()->TextRenderColor = FColor::White;
	BScore2->GetTextRender()->TextRenderColor = FColor::White;
}

void AScoreManager::breakScore2()
{
	orangeScore = 3;
	blueScore = 3;
	FString noHopeString = "";
	for(int i = 0; i < 9; i++) {
		noHopeString += "he is here do not go he is\n";
	}

	OScore1->GetTextRender()->SetText(FText::FromString(noHopeString));
	OScore2->GetTextRender()->SetText(FText::FromString(noHopeString));
	BScore1->GetTextRender()->SetText(FText::FromString(noHopeString));
	BScore2->GetTextRender()->SetText(FText::FromString(noHopeString));
	Hyphen1->GetTextRender()->SetText(FText::FromString("STAYhereSTAYdontGOdontGO\nDONTgoDONTgoDONTgo\nSTAYstaySTAY"));
	Hyphen2->GetTextRender()->SetText(FText::FromString("STAYhereSTAYdontGOdontGO\nDONTgoDONTgoDONTgo\nSTAYstaySTAY"));
	OScore1->GetTextRender()->SetWorldSize(30);
	OScore2->GetTextRender()->SetWorldSize(30);
	BScore1->GetTextRender()->SetWorldSize(30);
	BScore2->GetTextRender()->SetWorldSize(30);
	Hyphen1->GetTextRender()->SetWorldSize(60);
	Hyphen2->GetTextRender()->SetWorldSize(60);
	OScore1->GetTextRender()->TextRenderColor = FColor::White;
	OScore2->GetTextRender()->TextRenderColor = FColor::White;
	Hyphen1->GetTextRender()->TextRenderColor = FColor::Red;
	Hyphen2->GetTextRender()->TextRenderColor = FColor::Red;
	BScore1->GetTextRender()->TextRenderColor = FColor::White;
	BScore2->GetTextRender()->TextRenderColor = FColor::White;
}

void AScoreManager::breakScore3()
{
	orangeScore = 3;
	blueScore = 3;
	FString noHopeString = "";
	for(int i = 0; i < 9; i++) {
		noHopeString += "NO HOPE NO HOPE NO HOPE N\n";
	}

	OScore1->GetTextRender()->SetText(FText::FromString(noHopeString));
	OScore2->GetTextRender()->SetText(FText::FromString(noHopeString));
	BScore1->GetTextRender()->SetText(FText::FromString(noHopeString));
	BScore2->GetTextRender()->SetText(FText::FromString(noHopeString));
	Hyphen1->GetTextRender()->SetText(FText::FromString("helpMEHELPMEhelpmeHELPME\nHELP HELP HELP\nme me me"));
	Hyphen2->GetTextRender()->SetText(FText::FromString("helpMEHELPMEhelpmeHELPME\nHELP HELP HELP\nme me me"));
	OScore1->GetTextRender()->SetWorldSize(30);
	OScore2->GetTextRender()->SetWorldSize(30);
	BScore1->GetTextRender()->SetWorldSize(30);
	BScore2->GetTextRender()->SetWorldSize(30);
	Hyphen1->GetTextRender()->SetWorldSize(60);
	Hyphen2->GetTextRender()->SetWorldSize(60);
	OScore1->GetTextRender()->TextRenderColor = FColor::White;
	OScore2->GetTextRender()->TextRenderColor = FColor::White;
	Hyphen1->GetTextRender()->TextRenderColor = FColor::Red;
	Hyphen2->GetTextRender()->TextRenderColor = FColor::Red;
	BScore1->GetTextRender()->TextRenderColor = FColor::White;
	BScore2->GetTextRender()->TextRenderColor = FColor::White;
}

void AScoreManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (OpeningDoor1 && 
		(FVector::Dist(Door1->GetActorLocation(), Door1Open->GetActorLocation()) > KINDA_SMALL_NUMBER))
		//
		// && 
		// (FVector::Dist(Door2->GetActorLocation(), Door2Open->GetActorLocation()) > KINDA_SMALL_NUMBER))
	{
		FVector NewLocation1 = FMath::VInterpTo(Door1->GetActorLocation(), Door1Open->GetActorLocation(), DeltaTime, 0.1f);
		Door1->SetActorLocation(NewLocation1);
		
		// FVector NewLocation2 = FMath::VInterpTo(Door2->GetActorLocation(), Door2Open->GetActorLocation(), DeltaTime, 0.1f);
		// Door2->SetActorLocation(NewLocation2);
	}

	else if (OpeningDoor1)
	{
		OpeningDoor1 = false;
	}
}

void AScoreManager::ResetScore()
{
	BallSound->Play();
	orangeScore = 0;
	blueScore = 0;
	OScore1->GetTextRender()->SetText(FText::AsNumber(orangeScore));
	OScore2->GetTextRender()->SetText(FText::AsNumber(orangeScore));
	BScore1->GetTextRender()->SetText(FText::AsNumber(blueScore));
	BScore2->GetTextRender()->SetText(FText::AsNumber(blueScore));
	Hyphen1->GetTextRender()->SetText(FText::FromString("-"));
	Hyphen2->GetTextRender()->SetText(FText::FromString("-"));
	OScore1->GetTextRender()->TextRenderColor = OTextColor;
	OScore2->GetTextRender()->TextRenderColor = OTextColor;
	Hyphen1->GetTextRender()->TextRenderColor = FColor::White;
	Hyphen2->GetTextRender()->TextRenderColor = FColor::White;
	BScore1->GetTextRender()->TextRenderColor = BTextColor;
	BScore2->GetTextRender()->TextRenderColor = BTextColor;
	OScore1->GetTextRender()->SetWorldSize(350);
	OScore2->GetTextRender()->SetWorldSize(350);
	BScore1->GetTextRender()->SetWorldSize(350);
	BScore2->GetTextRender()->SetWorldSize(350);
	Hyphen1->GetTextRender()->SetWorldSize(350);
	Hyphen2->GetTextRender()->SetWorldSize(350);
	PowerResetOnce = true;
}
