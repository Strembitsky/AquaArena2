// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/SpotLight.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Sound/AmbientSound.h"
#include "Algo/AllOf.h"
#include "Engine/PointLight.h"
#include "Engine/TriggerBox.h"
#include "PowerManager.generated.h"

class AScoreManager;

UCLASS()
class VRPROJECT1_API APowerManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APowerManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	TArray<AStaticMeshActor*> ButtonArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	TArray<AAmbientSound*> ButtonClickSoundArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	TArray<AAmbientSound*> GenHumSoundArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	TArray<ASpotLight*> GenLightArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AScoreManager* ScoreManager;

	UPROPERTY()
	TArray<UPhysicsConstraintComponent*> ConstraintComponents;

	UPROPERTY()
	TMap<AStaticMeshActor*, FVector> InitialButtonPositions;

	UPROPERTY()
	TArray<bool> ButtonsReachedMax;

	UPROPERTY()
	bool PowerIsOn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	TArray<APointLight*> ArenaLightArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	APointLight* LightCageLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	ASpotLight* RoomSpotLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	APointLight* OrangeLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	APointLight* BlueLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AStaticMeshActor* Ball;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AStaticMeshActor* SecondPhaseHallwayAddition;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	AStaticMeshActor* BlueGoal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	AActor* GeneratorActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	AActor* NewGeneratorActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	AStaticMeshActor* BlueGoalRim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	AStaticMeshActor* OrangeGoal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	AStaticMeshActor* OrangeGoalRim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	ATriggerBox* GoalSwapTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	AAmbientSound* PowerDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AAmbientSound* LightCageLightSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	AStaticMeshActor* LightCage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	AStaticMeshActor* LightCageSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AAmbientSound* Music;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AAmbientSound* Solution;

	UPROPERTY()
	FTransform InitBallPos;
	UPROPERTY()
	FTransform InitOGoalPos;
	UPROPERTY()
	FTransform InitBGoalPos;
	UPROPERTY()
	FTransform InitOLightPos;
	UPROPERTY()
	FTransform InitBLightPos;
	UPROPERTY()
	bool GoalsSwapped;

	UPROPERTY()
	TArray<bool> overlappedButtons;
	
	

	UFUNCTION()
	void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

	UFUNCTION()
	void OnButtonOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

	UFUNCTION()
	void PowerGenerator(int32 Index);

	UFUNCTION()
	void TurnPowerOff1();

	UFUNCTION()
	void TurnPowerOff2();

	UFUNCTION()
	void TurnPowerBackOn1();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
