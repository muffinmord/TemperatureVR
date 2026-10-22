#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"

#include "TemperatureNode.generated.h"


UCLASS()
class TEMPERATURE_GRID_API ATemperatureNode : public AActor
{
    GENERATED_BODY()

public:

    ATemperatureNode();

protected:

    virtual void BeginPlay() override;

public:


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Temperature|Grid",
        meta = (ClampMin = "1"))
    int32 GridWidth = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Temperature|Grid",
        meta = (ClampMin = "1"))
    int32 GridHeight = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Temperature|Grid",
        meta = (ClampMin = "1.0"))
    float CellSize = 50.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Temperature|Grid")
    TObjectPtr<UInstancedStaticMeshComponent> GridMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Temperature|Grid")
    TObjectPtr<UStaticMesh> CellMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Temperature|Data",
        meta = (FilePathFilter = "GRIB2 Files (*.grib2)|*.grib2|All Files (*.*)|*.*"))
    FString GR2FilePath = TEXT("ThirdParty/grib2-dataset/2MT.2D.201804_subset.nc");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Temperature|Display")
    float MinTemperature = -20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Temperature|Display")
    float MaxTemperature = 40.0f;

    UFUNCTION(BlueprintCallable, Category = "Temperature|Grid")
    void BuildGrid();

    UFUNCTION(BlueprintCallable, Category = "Temperature|Data")
    void LoadGRIBData();

    UFUNCTION(BlueprintCallable, Category = "Temperature|Display")
    void UpdateTemperature(const TArray<float>& NewTemperatures);
};