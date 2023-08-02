// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/SpotLight.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Sound/AmbientSound.h"
#include "Algo/AllOf.h"
#include "Engine/DecalActor.h"
#include "Engine/PointLight.h"
#include "Engine/TriggerBox.h"
#include "PowerManager.generated.h"

class AScoreManager;
class AVRPawnMechanics;
class AMyPlayerController;

UCLASS()
class VRPROJECT1_API APowerManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APowerManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AVRPawnMechanics* VRPawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AMyPlayerController* VRPlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	USceneComponent* PawnRoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	USceneComponent* VRRoot;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	TArray<AStaticMeshActor*> ButtonArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	TArray<AAmbientSound*> ButtonClickSoundArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	TArray<AAmbientSound*> GenHumSoundArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	TArray<ASpotLight*> GenLightArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	TArray<AActor*> NewButtonPositions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	TArray<AStaticMeshActor*> ElementsToMakeInvisible;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AScoreManager* ScoreManager;

	UPROPERTY()
	TArray<UPhysicsConstraintComponent*> ConstraintComponents;

	UPROPERTY()
	TMap<AStaticMeshActor*, FVector> InitialButtonPositions;

	UPROPERTY()
	TMap<AStaticMeshActor*, FVector> OldInitialButtonPositions;

	UPROPERTY()
	TArray<bool> ButtonsReachedMax;

	UPROPERTY()
	bool PowerIsOn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	TArray<APointLight*> ArenaLightArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	APointLight* LightCageLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	APointLight* RoomPointLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	ASpotLight* ArenaEmissions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	ASpotLight* ArenaEmissions2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	APointLight* OrangeLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	APointLight* BlueLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AStaticMeshActor* Ball;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AStaticMeshActor* Flashlight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	ADecalActor* BloodSplatter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	ADecalActor* BloodSplatter2;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AStaticMeshActor* OrangeBubble;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AStaticMeshActor* OrangeBubbleInterior;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AStaticMeshActor* BlueBubble;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AStaticMeshActor* BlueBubbleInterior;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	TArray<AStaticMeshActor*> SecondPhaseHallwayAdditions;
	
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
	ATriggerBox* HallwayShrinkTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	ATriggerBox* WhatToTeleportTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	ATriggerBox* MusicTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	ATriggerBox* CloseArenaOpenElevatorTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	ATriggerBox* EnableGravityTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	ATriggerBox* SplatTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	ATriggerBox* SplatTeleportTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	AAmbientSound* PowerDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	AAmbientSound* ScaryLaugh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AAmbientSound* LightCageLightSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	AStaticMeshActor* LightCage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	AStaticMeshActor* LightCageSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	AStaticMeshActor* ArenaCloseWall;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	AStaticMeshActor* LivingRoomSealWall;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	AStaticMeshActor* ElevatorOpenWall;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AAmbientSound* Music;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AAmbientSound* Music2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AAmbientSound* Solution;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AAmbientSound* PowerSectionOff;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	UAudioComponent* WindFall;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	UAudioComponent* Splat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	UMaterialInterface* BloodDragDecal;

	UPROPERTY()
	FTransform InitBallPos;
	UPROPERTY()
	FTransform InitGenPos;
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
	bool HallShrunk;
	UPROPERTY()
	bool BeginTransition;

	UPROPERTY()
	bool SplatMoved;

	UPROPERTY()
	bool paintedDecal;

	UPROPERTY()
	FVector playerPosition;

	UPROPERTY()
	bool SlowDownMusic2;

	UPROPERTY()
	bool SlowDownMusic;

	UPROPERTY()
	bool SpeedMusicUp1;

	UPROPERTY()
	bool GravityEnabled;

	UPROPERTY()
	bool GoEnableGravity;

	UPROPERTY()
	bool MusicPlayed;

	UPROPERTY()
	bool PlayMusic;

	UPROPERTY()
	bool ElevatorOpened;

	UPROPERTY()
	bool Splatted;

	UPROPERTY()
	TArray<bool> overlappedButtons;
	
	

	UFUNCTION()
	void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

	UFUNCTION()
	void OnOverlapBeginHallShrink(class AActor* OverlappedActor, class AActor* OtherActor);

	UFUNCTION()
	void OnOverlapBeginCloseArenaOpenElevator(class AActor* OverlappedActor, class AActor* OtherActor);

	UFUNCTION()
	void OnOverlapBeginMusic(class AActor* OverlappedActor, class AActor* OtherActor);

	UFUNCTION()
	void OnOverlapBeginSplat(class AActor* OverlappedActor, class AActor* OtherActor);

	UFUNCTION()
	void OnOverlapEnableGravity(class AActor* OverlappedActor, class AActor* OtherActor);

	UFUNCTION()
	void OnButtonOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

	UFUNCTION()
	void PowerGenerator(int32 Index);

	UFUNCTION()
	void TurnPowerOff1();

	UFUNCTION()
	void TurnPowerOff2();
	
	UFUNCTION()
	void TurnOffPowerSection1();

	UFUNCTION()
	void TurnOffPowerSection2();

	UFUNCTION()
	void TurnOffPowerSection3();

	UFUNCTION()
	void TurnOffPowerSection4();

	UFUNCTION()
	void TurnPowerBackOn1();

	UFUNCTION()
	void TurnPowerBackOn2();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
