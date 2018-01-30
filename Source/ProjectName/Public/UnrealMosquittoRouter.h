// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node_Switch.h"
#include "UnrealMosquittoRouter.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class PROJECTNAME_API UUnrealMosquittoRouter : public UK2Node_Switch
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = RouteOptions)
	TArray<FString> Routes;

	// UObject interface
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	// UEdGraphNode interface
	virtual bool ShouldShowNodeProperties() const override { return true; }

	// UK2Node interface
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;

	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;

	// UK2Node_Switch Interface
	/*BLUEPRINTGRAPH_API*/ virtual void AddPinToSwitchNode() override;
	virtual FEdGraphPinType GetPinType() const override;

	virtual FString GetPinNameGivenIndex(int32 Index) override;
	virtual void CreateFunctionPin();

protected:
	virtual void CreateSelectionPin() override;
	virtual void CreateCasePins() override;
	virtual void RemovePin(UEdGraphPin* TargetPin) override;
	
	
	
};
