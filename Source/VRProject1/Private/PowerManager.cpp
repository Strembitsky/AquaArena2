// Fill out your copyright notice in the Description page of Project Settings.

#include "PowerManager.h"
#include "ScoreManager.h"
#include "VRPawnMechanics.h"
#include "MyPlayerController.h"
#include "Components/AudioComponent.h"
#include "EnvironmentQuery/Generators/EnvQueryGenerator_ActorsOfClass.h"
#include "Iris/Serialization/EnumNetSerializers.h"

// Sets default values
APowerManager::APowerManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APowerManager::BeginPlay()
{
    Super::BeginPlay();
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVRPawnMechanics::StaticClass(), FoundActors);
    BeginTransition = false;
    MusicPlayed = false;
    PlayMusic = false;

    if (FoundActors.Num() > 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("FOUND VRPAWN IN POWERMANAGER"));
        AVRPawnMechanics* FoundVRPawn = Cast<AVRPawnMechanics>(FoundActors[0]);

        if (FoundVRPawn != nullptr)
        {
            // Now you can use FoundVRPawn as a reference to your VRPawn class.
            VRPawn = FoundVRPawn;
            PawnRoot = VRPawn->GetRootComponent();
        }
    }

    for (AStaticMeshActor* Actor : SecondPhaseHallwayAdditions)
    {
        Actor->SetActorHiddenInGame(true);
    }

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMyPlayerController::StaticClass(), FoundActors);

    if (FoundActors.Num() > 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("FOUND VRController IN POWERMANAGER"));
        AMyPlayerController* FoundVRController = Cast<AMyPlayerController>(FoundActors[0]);

        if (FoundVRController != nullptr)
        {
            // Now you can use FoundVRPawn as a reference to your VRPawn class.
            VRPlayerController = FoundVRController;
            VRRoot = VRPlayerController->GetRootComponent();
        }
    }
    
    PowerIsOn = true;
    GoalsSwapped = false;
    InitBGoalPos = BlueGoalRim->GetActorTransform();
    InitOGoalPos = OrangeGoalRim->GetActorTransform();
    InitOLightPos = OrangeLight->GetActorTransform();
    InitBLightPos = BlueLight->GetActorTransform();
    InitBallPos = Ball->GetActorTransform();
    InitGenPos = GeneratorActor->GetActorTransform();

    for (AStaticMeshActor* Button : ButtonArray)
    {
        InitialButtonPositions.Add(Button, Button->GetActorLocation());
        OldInitialButtonPositions.Add(Button, Button->GetActorLocation());
        Button->OnActorBeginOverlap.AddDynamic(this, &APowerManager::OnButtonOverlapBegin);
        
    }
    for (int32 i = 0; i < ButtonArray.Num(); ++i)
    {
        ButtonsReachedMax.Add(false);
        overlappedButtons.Add(false);
    }
}

// Called every frame
void APowerManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (PlayMusic)
    {
        TurnPowerBackOn2();
        PlayMusic = false;
        PowerIsOn = true;
    }
    
    if (BeginTransition)
    {
        Music2->SetActorTransform(Music->GetActorTransform());
        FVector CurrentDifference = VRPawn->GetActorLocation() - GeneratorActor->GetActorLocation();

        TArray<AActor*> OverlappingActors;
        WhatToTeleportTrigger->GetOverlappingActors(OverlappingActors);
        FVector GeneratorPosition = GeneratorActor->GetActorLocation();
    
        for (AActor* ActorToTeleport : OverlappingActors)
        {
            if (ActorToTeleport->ActorHasTag("ToTeleport"))
            {
                ActorToTeleport->SetActorLocation(InitGenPos.GetLocation() + (ActorToTeleport->GetActorLocation() - GeneratorPosition), false, nullptr, ETeleportType::TeleportPhysics);
            }
        }
        
        // Update the generator actor position first
        GeneratorActor->SetActorTransform(InitGenPos);
        FVector UpdatedGenPosition = GeneratorActor->GetActorLocation();
        // Then compute the new location for the VRPawn
        FVector NewLocation = UpdatedGenPosition + CurrentDifference;
        VRPawn->SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
        //VRPlayerController->VRPawnMovement->Velocity = FVector(0.f);
        // Hide the second phase hallway addition only after moving the actors
        for (AStaticMeshActor* Actor : SecondPhaseHallwayAdditions)
        {
            Actor->SetActorHiddenInGame(true);
        }
        InitialButtonPositions.Empty();
        for (int32 i = 0; i < ButtonArray.Num(); ++i)
        {
            AStaticMeshActor* Button = ButtonArray[i];
            GenHumSoundArray[i]->Stop();
            GenLightArray[i]->SetLightColor(FColor::Red);

            // Disable physics simulation and detach constraints
            Button->GetStaticMeshComponent()->SetSimulatePhysics(false);

            // Teleport button and constraint
            Button->SetActorLocation(OldInitialButtonPositions[Button], false, nullptr, ETeleportType::TeleportPhysics);
    
            // Reset velocity
            Button->GetStaticMeshComponent()->SetPhysicsLinearVelocity(FVector::ZeroVector);
            Button->GetStaticMeshComponent()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
            Button->GetStaticMeshComponent()->SetSimulatePhysics(true);
    
            // Add button to initial positions list
            InitialButtonPositions.Add(Button, Button->GetActorLocation());
        }
        ScoreManager->OpenDoor3();
        BeginTransition = false;
    }
    
    if (!PowerIsOn)
    {
        for (int32 Index = 0; Index < ButtonArray.Num(); ++Index)
        {
            // Only process this button if it hasn't yet reached MaxX
            if (!ButtonsReachedMax[Index])
            {
                AStaticMeshActor* Button = ButtonArray[Index];

                if (overlappedButtons[Index])
                {
                    Button->GetStaticMeshComponent()->AddForce(FVector(500.0f, 0.f, 0.f));
                }
                // The maximum X position this object should be able to reach is its initial position plus the constraint limit.
                FVector InitialPosition = InitialButtonPositions[Button];
                float MaxX = InitialPosition.X + 15.f; // This should match your constraint limit

                if (Button->GetActorLocation().X >= MaxX)
                {
                    Button->GetStaticMeshComponent()->SetSimulatePhysics(false);
                    Button->GetStaticMeshComponent()->ComponentTags.Remove("Button");
                    // Pass the index to the new function
                    PowerGenerator(Index);
                }
            }
        }
        if (Algo::AllOf(ButtonsReachedMax, [](bool Value){ return Value; }))
        {
            if (!GoalsSwapped)
            {
                TurnPowerBackOn1();
            }
            else
            {
                //TurnPowerBackOn2();
            }
            ButtonsReachedMax.Empty();
            overlappedButtons.Empty();

            for (int32 i = 0; i < ButtonArray.Num(); ++i)
            {
                // AStaticMeshActor* Button = ButtonArray[i];
                // Button->GetStaticMeshComponent()->SetSimulatePhysics(true);
                // Button->GetStaticMeshComponent()->ComponentTags.Add("Button");
                ButtonsReachedMax.Add(false);
                overlappedButtons.Add(false);
            }
        }
    }

}

void APowerManager::PowerGenerator(int32 Index)
{
    ButtonClickSoundArray[Index]->Play();
    GenHumSoundArray[Index]->Play();
    GenLightArray[Index]->SetLightColor(FColor::Green);
    ButtonsReachedMax[Index] = true;
}

void APowerManager::TurnPowerOff1()
{
    LightCage->GetStaticMeshComponent()->SetVisibility(true);
    LightCageSphere->GetStaticMeshComponent()->SetVisibility(true);
    LightCageLight->SetEnabled(true);
    LightCageLightSound->Play();
    OrangeGoalRim->GetStaticMeshComponent()->SetSimulatePhysics(true);
    OrangeGoalRim->GetStaticMeshComponent()->SetEnableGravity(true);
    BlueGoalRim->GetStaticMeshComponent()->SetSimulatePhysics(true);
    BlueGoalRim->GetStaticMeshComponent()->SetEnableGravity(true);
    BlueGoalRim->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BlueGoal->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    OrangeGoalRim->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    OrangeGoal->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    ScoreManager->OScoringAllowed = false;
    ScoreManager->BScoringAllowed = false;

    for (APointLight* Light : ArenaLightArray)
    {
        Light->SetEnabled(false);
    }

    ArenaEmissions->SetEnabled(false);
    ArenaEmissions2->SetEnabled(false);
    PowerDown->Play();
    Music->Stop();
    PowerIsOn = false;
}

void APowerManager::TurnPowerOff2()
{
    LightCage->GetStaticMeshComponent()->SetVisibility(true);
    LightCageSphere->GetStaticMeshComponent()->SetVisibility(true);
    LightCageLight->SetEnabled(true);
    LightCageLightSound->Play();
    
    ArenaEmissions->SetEnabled(false);
    ArenaEmissions2->SetEnabled(false);
    
    PowerDown->Play();
    
    TArray<AActor*> OverlappingActors;
    WhatToTeleportTrigger->GetOverlappingActors(OverlappingActors);
    FVector GeneratorPosition = GeneratorActor->GetActorLocation();
    FVector NewGenPos = NewGeneratorActor->GetActorLocation();
    FVector currentDifference;
    for (AActor* ActorToTeleport : OverlappingActors)
    {
        if (ActorToTeleport->ActorHasTag("ToTeleport"))
        {
            currentDifference = ActorToTeleport->GetActorLocation() - GeneratorPosition;
            ActorToTeleport->SetActorLocation(NewGenPos + currentDifference, false, nullptr, ETeleportType::TeleportPhysics);
        }
    }
    
    GeneratorActor->SetActorTransform(NewGeneratorActor->GetActorTransform(), false, nullptr, ETeleportType::TeleportPhysics);
    InitialButtonPositions.Empty();
    
    for (int32 i = 0; i < ButtonArray.Num(); ++i)
    {
        AStaticMeshActor* Button = ButtonArray[i];
        GenHumSoundArray[i]->Stop();
        GenLightArray[i]->SetLightColor(FColor::Red);

        // Disable physics simulation and detach constraints
        Button->GetStaticMeshComponent()->SetSimulatePhysics(false);

        // Teleport button and constraint
        Button->SetActorLocation(NewButtonPositions[i]->GetActorLocation(), false, nullptr, ETeleportType::TeleportPhysics);
    
        // Reset velocity
        Button->GetStaticMeshComponent()->SetPhysicsLinearVelocity(FVector::ZeroVector);
        Button->GetStaticMeshComponent()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
        Button->GetStaticMeshComponent()->SetSimulatePhysics(true);
    
        // Add button to initial positions list
        InitialButtonPositions.Add(Button, Button->GetActorLocation());
    }

    for (AStaticMeshActor* Element : ElementsToMakeInvisible)
    {
        Element->SetActorHiddenInGame(true);
        Element->GetStaticMeshComponent()->SetSimulatePhysics(false);
        Element->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    HallwayShrinkTrigger->OnActorBeginOverlap.AddDynamic(this, &APowerManager::OnOverlapBeginHallShrink);
    MusicTrigger->OnActorBeginOverlap.AddDynamic(this, &APowerManager::OnOverlapBeginMusic);
    
    for (AStaticMeshActor* Actor : SecondPhaseHallwayAdditions)
    {
        Actor->SetActorHiddenInGame(false);
    }

    PowerIsOn = false;
    
}

void APowerManager::TurnOffPowerSection1()
{
    PowerSectionOff->Play();
    for (int32 i = 0; i < ArenaLightArray.Num(); ++i)
    {
        if (i <= (ArenaLightArray.Num()/4))
        {
            APointLight* Light = ArenaLightArray[i];
            Light->SetEnabled(false);
        }
    }
}

void APowerManager::TurnOffPowerSection2()
{
    PowerSectionOff->Play();
    OrangeGoalRim->GetStaticMeshComponent()->SetSimulatePhysics(true);
    OrangeGoalRim->GetStaticMeshComponent()->SetEnableGravity(true);
    OrangeGoalRim->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    OrangeGoal->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    ScoreManager->OScoringAllowed = false;
    for (int32 i = 0; i < ArenaLightArray.Num(); ++i)
    {
        if ((i > (ArenaLightArray.Num()/4)) &&  (i <= (ArenaLightArray.Num()/4)*2))
        {
            APointLight* Light = ArenaLightArray[i];
            Light->SetEnabled(false);
        }
    }
}

void APowerManager::TurnOffPowerSection3()
{
    PowerSectionOff->Play();
    BlueGoalRim->GetStaticMeshComponent()->SetSimulatePhysics(true);
    BlueGoalRim->GetStaticMeshComponent()->SetEnableGravity(true);
    BlueGoalRim->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BlueGoal->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    ScoreManager->BScoringAllowed = false;
    Music->Stop();
    ScaryLaugh->Play();
    
    for (int32 i = 0; i < ArenaLightArray.Num(); ++i)
    {
        if ((i > (ArenaLightArray.Num()/4)*2) &&  (i <= (ArenaLightArray.Num()/4)*3))
        {
            APointLight* Light = ArenaLightArray[i];
            Light->SetEnabled(false);
        }
    }
}

void APowerManager::TurnOffPowerSection4()
{
    PowerSectionOff->Play();
    for (int32 i = 0; i < ArenaLightArray.Num(); ++i)
    {
        if ((i > (ArenaLightArray.Num()/4)*3) &&  (i <= (ArenaLightArray.Num()/4)*4))
        {
            APointLight* Light = ArenaLightArray[i];
            Light->SetEnabled(false);
        }
    }

    TurnPowerOff2();
    
}

void APowerManager::TurnPowerBackOn1()
{
    Solution->Play();
    LightCage->GetStaticMeshComponent()->SetVisibility(false);
    LightCageSphere->GetStaticMeshComponent()->SetVisibility(false);
    LightCageLight->SetEnabled(false);
    LightCageLightSound->Stop();
    OrangeGoalRim->GetStaticMeshComponent()->SetSimulatePhysics(false);
    OrangeGoalRim->GetStaticMeshComponent()->SetEnableGravity(false);
    BlueGoalRim->GetStaticMeshComponent()->SetSimulatePhysics(false);
    BlueGoalRim->GetStaticMeshComponent()->SetEnableGravity(false);
    BlueGoalRim->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BlueGoal->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    OrangeGoalRim->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    OrangeGoal->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    ScoreManager->BScoringAllowed = true;
    ScoreManager->OScoringAllowed = true;

    for (APointLight* Light : ArenaLightArray)
    {
        Light->SetEnabled(true);
    }
    ArenaEmissions->SetEnabled(true);
    ArenaEmissions2->SetEnabled(true);
    
    Music->Play();

    BlueGoalRim->SetActorTransform(InitBGoalPos);
    BlueLight->SetActorTransform(InitBLightPos);
    OrangeGoalRim->SetActorTransform(InitOGoalPos);
    OrangeLight->SetActorTransform(InitOLightPos);
    
    GoalSwapTrigger->OnActorBeginOverlap.AddDynamic(this, &APowerManager::OnOverlapBegin);

    ScoreManager->ResetScore();
    
    PowerIsOn = true;
}

void APowerManager::TurnPowerBackOn2()
{
    LightCage->GetStaticMeshComponent()->SetVisibility(false);
    LightCageSphere->GetStaticMeshComponent()->SetVisibility(false);
    LightCageLight->SetEnabled(false);
    LightCageLightSound->Stop();
    OrangeGoalRim->GetStaticMeshComponent()->SetSimulatePhysics(false);
    OrangeGoalRim->GetStaticMeshComponent()->SetEnableGravity(false);
    BlueGoalRim->GetStaticMeshComponent()->SetSimulatePhysics(false);
    BlueGoalRim->GetStaticMeshComponent()->SetEnableGravity(false);
    BlueGoalRim->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BlueGoal->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    OrangeGoalRim->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    OrangeGoal->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    ScoreManager->OScoringAllowed = true;
    ScoreManager->BScoringAllowed = true;

    for (APointLight* Light : ArenaLightArray)
    {
        Light->SetEnabled(true);
    }
    ArenaEmissions->SetEnabled(true);
    ArenaEmissions2->SetEnabled(true);
    Music2->Play(15.f);
    UE_LOG(LogTemp, Warning, TEXT("SHOULD START MUSIC HERE"));
    
    BlueGoalRim->SetActorTransform(InitOGoalPos);
    BlueLight->SetActorTransform(InitOLightPos);
    OrangeGoalRim->SetActorTransform(InitBGoalPos);
    OrangeLight->SetActorTransform(InitBLightPos);

    ScoreManager->ResetScore();
    
    PowerIsOn = true;
}

void APowerManager::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
    if (!GoalsSwapped)
    {
        BlueGoalRim->SetActorTransform(InitOGoalPos);
        BlueLight->SetActorTransform(InitOLightPos);
        OrangeGoalRim->SetActorTransform(InitBGoalPos);
        OrangeLight->SetActorTransform(InitBLightPos);
        Ball->GetStaticMeshComponent()->SetPhysicsLinearVelocity(FVector(0.f,0.f,0.f));
        Ball->SetActorTransform(InitBallPos);
        InitialButtonPositions.Empty();
        GoalsSwapped = true;
    }
}

void APowerManager::OnOverlapBeginHallShrink(AActor* OverlappedActor, AActor* OtherActor)
{
    if (!HallShrunk && OtherActor->GetName().Contains("Pawn"))
    {
        BeginTransition = true;
        HallShrunk = true;
    }

}

void APowerManager::OnOverlapBeginMusic(AActor* OverlappedActor, AActor* OtherActor)
{
    if (!MusicPlayed && OtherActor->GetName().Contains("Pawn"))
    {
        PlayMusic = true;
        MusicPlayed = true;
    }
}
void APowerManager::OnButtonOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
    UE_LOG(LogTemp, Warning, TEXT("%s"), *OverlappedActor->GetName());
    UE_LOG(LogTemp, Warning, TEXT("%s"), *OtherActor->GetName());
    if (!PowerIsOn)
    {
        if (OtherActor->GetName().Contains("VRPawn") && OverlappedActor->GetName() == "1M_Cube2_97")
        {
            overlappedButtons[0] = true;
        }
        else if (OtherActor->GetName().Contains("VRPawn") && OverlappedActor->GetName() == "1M_Cube2_105")
        {
            overlappedButtons[1] = true;
        }
        else if (OtherActor->GetName().Contains("VRPawn") && OverlappedActor->GetName() == "1M_Cube2_106")
        {
            overlappedButtons[2] = true;
        }
        else if (OtherActor->GetName().Contains("VRPawn") && OverlappedActor->GetName() == "1M_Cube2_107")
        {
            overlappedButtons[3] = true;
        }
    }
}
