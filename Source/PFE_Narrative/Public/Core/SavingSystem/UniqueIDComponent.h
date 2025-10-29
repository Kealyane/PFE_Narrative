// copyright2025 Allen Broquet Lamouche Tamen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UniqueIDComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PFE_NARRATIVE_API UUniqueIDComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UUniqueIDComponent();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Save")
	FString ActorID;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
