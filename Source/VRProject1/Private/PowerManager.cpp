// Fill out your copyright notice in the Description page of Project Settings.

#include "ScoreManager.h"
#include "PowerManager.h"

#include "EnvironmentQuery/Generators/EnvQueryGenerator_ActorsOfClass.h"

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

    PowerIsOn = true;
    GoalsSwapped = false;
    InitBGoalPos = BlueGoalRim->GetActorTransform();
    InitOGoalPos = OrangeGoalRim->GetActorTransform();
    InitOLightPos = OrangeLight->GetActorTransform();
    InitBLightPos = BlueLight->GetActorTransform();
    InitBallPos = Ball->GetActorTransform();

    for (AStaticMeshActor* Button : ButtonArray)
    {
        Button->GetStaticMeshComponent()->SetSimulatePhysics(false);
        Button->AttachToActor(GeneratorActor, FAttachmentTransformRules::KeepRelativeTransform);
        Button->GetStaticMeshComponent()->SetSimulatePhysics(true);
        InitialButtonPositions.Add(Button, Button->GetActorLocation());
        
        UPhysicsConstraintComponent* ConstraintComponent = NewObject<UPhysicsConstraintComponent>(this);

        // Register this component
        ConstraintComponent->RegisterComponent();

        // Attach the constraint to the button component
        ConstraintComponent->AttachToComponent(Button->GetStaticMeshComponent(), FAttachmentTransformRules::KeepRelativeTransform);

        // Set the constrained component
        ConstraintComponent->SetConstrainedComponents(Button->GetStaticMeshComponent(), NAME_None, nullptr, NAME_None);

        // Only allow movement along the X axis
        ConstraintComponent->SetLinearXLimit(ELinearConstraintMotion::LCM_Limited, 15.f);
        
        ConstraintComponents.Add(ConstraintComponent);

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
            TurnPowerBackOn1();
            for (int32 i = 0; i < ButtonArray.Num(); ++i)
            {
                AStaticMeshActor* Button = ButtonArray[i];
                Button->GetStaticMeshComponent()->SetSimulatePhysics(true);
                Button->GetStaticMeshComponent()->ComponentTags.Add("Button");
                ButtonsReachedMax.Add(false);
                overlappedButtons.Add(false);
            }
        }
    }

}

void APowerManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    // Iterate through each of your constraint components
    for (auto& ConstraintComponent : ConstraintComponents)
    {
        // Make sure it's valid before trying to manipulate it
        if (ConstraintComponent && ConstraintComponent->IsValidLowLevel())
        {
            ConstraintComponent->BreakConstraint(); // Breaks the constraint
            ConstraintComponent->DestroyComponent(); // Destroy the component
        }
    }

    // Clear the array
    ConstraintComponents.Empty();
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

    for (APointLight* Light : ArenaLightArray)
    {
        Light->SetEnabled(false);
    }
    
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
    OrangeGoalRim->GetStaticMeshComponent()->SetSimulatePhysics(true);
    OrangeGoalRim->GetStaticMeshComponent()->SetEnableGravity(true);
    BlueGoalRim->GetStaticMeshComponent()->SetSimulatePhysics(true);
    BlueGoalRim->GetStaticMeshComponent()->SetEnableGravity(true);
    BlueGoalRim->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BlueGoal->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    OrangeGoalRim->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    OrangeGoal->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    for (APointLight* Light : ArenaLightArray)
    {
        Light->SetEnabled(false);
    }
    PowerDown->Play();
    Music->Stop();
    GeneratorActor->SetActorTransform(NewGeneratorActor->GetActorTransform());
    
    for (int32 i = 0; i < ButtonArray.Num(); ++i)
    {
        GenHumSoundArray[i]->Stop();
        GenLightArray[i]->SetLightColor(FColor::Red);
    }

    for (AStaticMeshActor* Button : ButtonArray)
    {
        Button->GetStaticMeshComponent()->SetConstraintMode(EDOFMode::Default);
        InitialButtonPositions.Add(Button, Button->GetActorLocation());
        
        UPhysicsConstraintComponent* ConstraintComponent = NewObject<UPhysicsConstraintComponent>(this);

        // Register this component
        ConstraintComponent->RegisterComponent();

        // Attach the constraint to the button component
        ConstraintComponent->AttachToComponent(Button->GetStaticMeshComponent(), FAttachmentTransformRules::KeepRelativeTransform);

        // Set the constrained component
        ConstraintComponent->SetConstrainedComponents(Button->GetStaticMeshComponent(), NAME_None, nullptr, NAME_None);

        // Only allow movement along the X axis
        ConstraintComponent->SetLinearXLimit(ELinearConstraintMotion::LCM_Limited, 15.f);
        
        ConstraintComponents.Add(ConstraintComponent);
    }
    SecondPhaseHallwayAddition->SetActorHiddenInGame(false);

    PowerIsOn = false;
    
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

    for (APointLight* Light : ArenaLightArray)
    {
        Light->SetEnabled(true);
    }
    
    Music->Play();

    BlueGoalRim->SetActorTransform(InitBGoalPos);
    BlueLight->SetActorTransform(InitBLightPos);
    OrangeGoalRim->SetActorTransform(InitOGoalPos);
    OrangeLight->SetActorTransform(InitOLightPos);
    
    GoalSwapTrigger->OnActorBeginOverlap.AddDynamic(this, &APowerManager::OnOverlapBegin);

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
        // Iterate through each of your constraint components
        for (auto& ConstraintComponent : ConstraintComponents)
        {
            // Make sure it's valid before trying to manipulate it
            if (ConstraintComponent && ConstraintComponent->IsValidLowLevel())
            {
                ConstraintComponent->BreakConstraint(); // Breaks the constraint
                ConstraintComponent->DestroyComponent(); // Destroy the component
            }
        }
        // Clear the array
        ConstraintComponents.Empty();

        for (int32 i = 0; i < ButtonArray.Num(); ++i)
        {
            AStaticMeshActor* Button = ButtonArray[i];
            Button->SetLockLocation(false);
            Button->GetStaticMeshComponent()->SetConstraintMode(EDOFMode::None);
        }
        
        GoalsSwapped = true;
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

