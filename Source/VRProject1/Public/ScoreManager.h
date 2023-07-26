// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/PointLight.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/TextRenderActor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
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
	AStaticMeshActor* Ball;

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
	int blueScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int orangeScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	bool OpeningDoors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	bool ClosingDoors;

	UFUNCTION()
	void OpenDoors();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
