// Fill out your copyright notice in the Description page of Project Settings.


#include "TemperatureNode.h"

#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Misc/MessageDialog.h"
#include <iostream>

#include <F:\studium\grib2dec-master(1)\grib2dec-master\include\grib2dec\grib2dec.hpp>
#include <F:\studium\grib2dec-master(1)\grib2dec-master\include\grib2dec\types.h>

ATemperatureNode::ATemperatureNode()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root =
		CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	RootComponent = Root;

	GridMesh =
		CreateDefaultSubobject<UInstancedStaticMeshComponent>(
			TEXT("GridMesh"));

	GridMesh->SetupAttachment(RootComponent);
	GridMesh->NumCustomDataFloats = 1;
}

void ATemperatureNode::BeginPlay()
{
	Super::BeginPlay();

	BuildGrid();
	LoadGRIBData();
}

void ATemperatureNode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATemperatureNode::BuildGrid()
{
	if (!GridMesh)
	{
		std::cout << "TemperatureNode: No GridMesh component found.";
		return;
	}

	GridMesh->ClearInstances();

	if (!CellMesh)
	{
		std::cout << "TemperatureNode: No CellMesh component found.";
		return;
	}

	if (GridWidth <= 0 || GridHeight <= 0 || CellSize <= 0.0f)
	{
		std::cout << "TemperatureNode: Invalid grid parameters.";
		return;
	}

	GridMesh->SetStaticMesh(CellMesh);

	const float half_width = (GridWidth - 1) * CellSize * 0.5f;
	const float half_height = (GridHeight - 1) * CellSize * 0.5f;

	for (int32 y = 0; y < GridHeight; ++y)
	{
		for (int32 x = 0; x < GridWidth; ++x)
		{
			FVector location(
				x * CellSize - half_width,
				y * CellSize - half_height,
				0.0f);

			FTransform transform(location);

			const int32 index = GridMesh->AddInstance(transform);

			GridMesh->SetCustomDataValue(
				index,
				0,
				0.0f,
				false);
		}
	}

	GridMesh->MarkRenderStateDirty();
	std::cout << "TemperatureNode: Built grid with " << GridWidth * GridHeight << " cells." << std::endl;
}

void ATemperatureNode::UpdateTemperature(const TArray<float>& NewTemperatures)
{
	if (!GridMesh)
	{
		return;
	}

	const int32 expected_count = GridWidth * GridHeight;

	if (NewTemperatures.Num() != expected_count)
	{
		std::cout << "TemperatureNode: Temperature array size mismatch. Expected " << expected_count << ", got " << NewTemperatures.Num() << "." << std::endl;
		return;
	}

	for (int32 i = 0; i < expected_count; ++i)
	{
		const float temp = NewTemperatures[i];

		float NormalizedTemp =
			(temp - MinTemperature) /
			(MaxTemperature - MinTemperature);

		NormalizedTemp = FMath::Clamp(NormalizedTemp, 0.0f, 1.0f);

		GridMesh->SetCustomDataValue(
			i,
			0,
			NormalizedTemp,
			false);
	}

	GridMesh->MarkRenderStateDirty();
}

void ATemperatureNode::LoadGRIBData()
{

	if (GR2FilePath.IsEmpty())
	{
		std::cout << "TemperatureNode: GR2FilePath is empty." << std::endl;
		return;
	}

	if (!FPaths::FileExists(GR2FilePath))
	{
		std::cout << "TemperatureNode: GR2 file not found: " << *GR2FilePath << std::endl;
		return;
	}

	const FTCHARToUTF8 FilePathUtf8(*GR2FilePath);
	grib2dec::Grib2Dec* Decoder =
		grib2dec::Grib2Dec::create(FilePathUtf8.Get());
	
	TArray<float> Temperatures;
	int32 DataWidth = 0;
	int32 DataHeight = 0;
	bool bFoundData = false;

	while (true)
	{
		G2DEC_Message Message{};

		const G2DEC_Status Status =
			Decoder->nextMessage(Message);

		if (Status == G2DEC_STATUS_END)
		{
			break;
		}

		if (Status != G2DEC_STATUS_OK)
		{
			std::cout << "TemperatureNode: GRIB decode error: " << static_cast<int32>(Status) << std::endl;
			break;
		}

		const int32 Width =
			static_cast<int32>(Message.grid.ni);
		const int32 Height =
			static_cast<int32>(Message.grid.nj);
		const int32 Count =
			static_cast<int32>(Message.valuesLength);

		std::cout << "GRIB message: " << Width << " x " << Height << ", values=" << Count << std::endl;

		if (Width <= 0 || Height <= 0 ||
			Count <= 0 || Message.values == nullptr)
		{
			std::cout << "TemperatureNode: Invalid or empty message." << std::endl;
			continue;
		}

		if (static_cast<int64>(Width) * Height != Count)
		{
			std::cout << "TemperatureNode: Grid/value count mismatch." << std::endl;
			continue;
		}

		Temperatures.Reset();
		Temperatures.Reserve(Count);

		for (int32 I = 0; I < Count; ++I)
		{
			Temperatures.Add(
				static_cast<float>(Message.values[I]));
		}

		DataWidth = Width;
		DataHeight = Height;
		bFoundData = true;

		break;
	}

	delete Decoder;

	if (!bFoundData)
	{
		std::cout << "TemperatureNode: No usable GRIB data found." << std::endl;
		return;
	}

	GridWidth = DataWidth;
	GridHeight = DataHeight;

	std::cout << "TemperatureNode: Applying " << GridWidth << " x " << GridHeight << " grid (" << Temperatures.Num() << " values)." << std::endl;

	BuildGrid();
	UpdateTemperature(Temperatures);
}