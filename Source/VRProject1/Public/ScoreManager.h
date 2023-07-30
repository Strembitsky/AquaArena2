// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PowerManager.h"
#include "Engine/PointLight.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/TextRenderActor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Sound/AmbientSound.h"
#include "ScoreManager.generated.h"

UCLASS()
class VRPROJECT1_API AScoreManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AScoreManager();

	UFUNCTION()
	void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AAmbientSound* Music;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AStaticMeshActor* Ball;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AAmbientSound* BallSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AAmbientSound* BallCollideSoft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AAmbientSound* BallCollideHard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AAmbientSound* PowerDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AAmbientSound* FlashlightBuzz;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AAmbientSound* GoalScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AStaticMeshActor* BlueGoal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AStaticMeshActor* BlueGoalRim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AStaticMeshActor* OrangeGoal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AStaticMeshActor* OrangeGoalRim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AStaticMeshActor* Door1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AStaticMeshActor* Door2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AStaticMeshActor* Flashlight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	APointLight* FlashlightLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	APointLight* FlashlightLight2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	APointLight* LightCageLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AStaticMeshActor* LightCage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AStaticMeshActor* LightCageSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AAmbientSound* LightCageLightSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	APointLight* Light1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	APointLight* Light2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	APointLight* Light3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	APointLight* Light4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	APointLight* Light5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	APointLight* Light6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	APointLight* Light7;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	APointLight* Light8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AActor* Door1Open;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AActor* Door2Open;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AActor* Door1Closed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AActor* Door2Closed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	ATextRenderActor* OScore1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	ATextRenderActor* OScore2;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	ATextRenderActor* BScore1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	ATextRenderActor* BScore2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	ATextRenderActor* Hyphen1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	ATextRenderActor* Hyphen2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int blueScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int orangeScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	bool OpeningDoor1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	bool ClosingDoor1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	bool turnPowerBackOn1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	APowerManager* powerRef;

	UPROPERTY()
	FTransform InitFlashlightPos;

	UPROPERTY()
	FVector InitDoor1Pos;

	UPROPERTY()
	bool BScoringAllowed;

	UPROPERTY()
	bool OScoringAllowed;

	UPROPERTY()
	bool PowerResetOnce;

	UPROPERTY()
	bool PowerResetTwice;

	UPROPERTY()
	bool SecondPhaseBegun;

	UPROPERTY()
	bool SecondPhaseComplete;

	UPROPERTY()
	float TimeSecondPhaseBegun;

	UPROPERTY()
	bool TurnOffPowerSection1;

	UPROPERTY()
	bool TurnOffPowerSection2;

	UPROPERTY()
	bool TurnOffPowerSection3;

	UPROPERTY()
	bool TurnOffPowerSection4;
	

	UPROPERTY()
	FColor OTextColor;

	UPROPERTY()
	FColor BTextColor;

	UFUNCTION()
	void OpenDoor1();

	UFUNCTION()
	void OpenDoor2();

	UFUNCTION()
	void OpenDoor3();

	UFUNCTION()
	void ResetScore();

	UFUNCTION()
	void breakScore1();

	UFUNCTION()
	void breakScore2();

	UFUNCTION()
	void breakScore3();

	UFUNCTION()
	void breakScoreSection1();

	UFUNCTION()
	void breakScoreSection2();

	UFUNCTION()
	void BeginSecondPhase();

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
