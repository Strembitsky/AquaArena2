#include "ScoreManager.h"

#include <string>

#include "Components/PrimitiveComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/TextRenderComponent.h"
#include "VRPawnMechanics.h"

AScoreManager::AScoreManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AScoreManager::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVRPawnMechanics::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("FOUND VRPAWN IN POWERMANAGER"));
		AVRPawnMechanics* FoundVRPawn = Cast<AVRPawnMechanics>(FoundActors[0]);

		if (FoundVRPawn != nullptr)
		{
			// Now you can use FoundVRPawn as a reference to your VRPawn class.
			VRPawn = FoundVRPawn;
		}
	}
	
	PowerResetOnce = false;
	turnPowerBackOn1 = false;
	InitDoor1Pos = Door1->GetActorLocation();
	SecondPhaseBegun = false;
	TimeSecondPhaseBegun = 0.f;
	TurnOffPowerSection1 = false;
	TurnOffPowerSection2 = false;
	TurnOffPowerSection3 = false;
	TurnOffPowerSection4 = false;
	SecondPhaseComplete = false;
	PowerOffOnce = false;
	OScoringAllowed = true;
	BScoringAllowed = true;

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
		if (BScoringAllowed)
		{
			if (OtherActor->ActorHasTag("Score") && OverlappedActor == BlueGoal)
			{
				UE_LOG(LogTemp, Warning, TEXT("DISTANCE TO BLUE GOAL %f"), (VRPawn->ballReleasePosition - BlueGoalRim->GetActorLocation()).Size())
				if ((VRPawn->ballReleasePosition - BlueGoalRim->GetActorLocation()).Size() >= 1125.f)
				{
					orangeScore += 3;
				}
				else
				{
					orangeScore += 2;
				}
				GoalScore->Play();
				OScore1->GetTextRender()->SetText(FText::AsNumber(orangeScore));
				OScore2->GetTextRender()->SetText(FText::AsNumber(orangeScore));
				UE_LOG(LogTemp, Warning, TEXT("Orange Score: %d"), orangeScore);
				if (blueScore < 6 && orangeScore < 6)
				{
					if (PowerResetOnce && !PowerResetTwice && !SecondPhaseBegun)
					{
						BeginSecondPhase();
					}
				}
				else
				{
					if (!PowerOffOnce)
					{
						breakScore1();
						OpenDoor1();
						PowerOffOnce = true;
					}
				}
			}
		}
		if (OScoringAllowed)
			{
			if (OtherActor->ActorHasTag("Score") && OverlappedActor == OrangeGoal)
			{
				UE_LOG(LogTemp, Warning, TEXT("DISTANCE TO ORANGE GOAL %f"), (VRPawn->ballReleasePosition - OrangeGoalRim->GetActorLocation()).Size())
				if ((VRPawn->ballReleasePosition - OrangeGoalRim->GetActorLocation()).Size() >= 1125.f)
				{
					blueScore += 3;
				}
				else
				{
					blueScore += 2;
				}
				UE_LOG(LogTemp, Warning, TEXT("Blue Score: %d"), blueScore);
				GoalScore->Play();
				BScore1->GetTextRender()->SetText(FText::AsNumber(blueScore));
				BScore2->GetTextRender()->SetText(FText::AsNumber(blueScore));
				if (blueScore < 6 && orangeScore < 6)
				{
					if (PowerResetOnce && !PowerResetTwice && !SecondPhaseBegun)
					{
						BeginSecondPhase();
					}
				}
				else
				{
					if (!PowerOffOnce)
					{
						breakScore1();
						OpenDoor1();
						PowerOffOnce = true;
					}
				}
			}
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
	Flashlight->GetStaticMeshComponent()->SetSimulatePhysics(true);
	Flashlight->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Flashlight->SetActorHiddenInGame(false);
	FlashlightLight->SetEnabled(true);
	FlashlightLight2->SetEnabled(true);
	FlashlightBuzz->Play();
	BallSound->Stop();
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

void AScoreManager::OpenDoor3()
{
	Door2->SetActorLocation(Door2Open->GetActorLocation());
}

void AScoreManager::BeginSecondPhase()
{
	Door1->SetActorLocation(InitDoor1Pos);
	TimeSecondPhaseBegun = GetWorld()->GetTimeSeconds();
	SecondPhaseBegun = true;
	OpeningDoor1 = false;
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

void AScoreManager::breakScoreSection1()
{
	BScore1->GetTextRender()->SetText(FText::FromString("?"));
	BScore2->GetTextRender()->SetText(FText::FromString("?"));
	Hyphen1->GetTextRender()->SetText(FText::FromString("v"));
	Hyphen2->GetTextRender()->SetText(FText::FromString("v"));
}

void AScoreManager::breakScoreSection2()
{
	OScore1->GetTextRender()->SetText(FText::FromString("?"));
	OScore2->GetTextRender()->SetText(FText::FromString("?"));
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

	if (SecondPhaseBegun && !SecondPhaseComplete)
	{
		float currentTime = GetWorld()->GetTimeSeconds();
		if ((currentTime - TimeSecondPhaseBegun > 10.f) && !TurnOffPowerSection1)
		{
			powerRef->TurnOffPowerSection1();
			TurnOffPowerSection1 = true;
		}
		else if ((currentTime - TimeSecondPhaseBegun > 30.f) && !TurnOffPowerSection2)
		{
			powerRef->TurnOffPowerSection2();
			breakScoreSection1();
			TurnOffPowerSection2 = true;
		}
		else if ((currentTime - TimeSecondPhaseBegun > 45.f) && !TurnOffPowerSection3)
		{
			powerRef->TurnOffPowerSection3();
			breakScoreSection2();
			TurnOffPowerSection3 = true;
		}
		else if ((currentTime - TimeSecondPhaseBegun > 50.f) && !TurnOffPowerSection4)
		{
			powerRef->TurnOffPowerSection4();
			TurnOffPowerSection4 = true;
			OpenDoor2();
			breakScore2();
		}
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
