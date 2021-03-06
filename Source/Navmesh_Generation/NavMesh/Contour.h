// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Contour.generated.h"

class AOpenHeightfield;
class UOpenSpan;

USTRUCT()
struct FContourVertexData
{
	GENERATED_USTRUCT_BODY()

	//Coordinates of the vertex considered
	FVector Coordinate = FVector(0.f, 0.f, 0.f);

	//External region ID to which the vertex is adjacent by/connected to (the one of the neighbor span considered in the processing)
	int ExternalRegionID = 0;

	//Internal region ID to which the vertex is adjacent by/connected to (the one of the current span considered in the processing)
	//Stored for debug purposes
	int InternalRegionID = 0;

	//Vertex inside inside the raw contour, useful for looping while creating the simplfied contour
	int RawIndex = 0;
};

UCLASS()
class NAVMESH_GENERATION_API AContour : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AContour();

	void Tick(float DeltaTime) override;

	//Initialize the base data of the countour using the one retrieved from the Height and Open fields
	void Init(const AOpenHeightfield* OpenHeightfield);

	//Generate the countour of the region
	void GenerateContour(const AOpenHeightfield* OpenHeightfield);

	//TODO: Double check the way in which the region spans are checked, there's code you can reuse
	//Check if the axis neighbor of a specific span belong or not to the same region the span considered is in - the span in the NULL_REGIOn are skipped
	void FindNeighborRegionConnection(const AOpenHeightfield* OpenHeightfield, int& NumberOfContourDiscarded);

	//Build the raw countour of a region, by iterating through all the border spans of it
	void BuildRawContours(UOpenSpan* Span, const int StartDir, bool& OnlyNullRegionConnection, TArray<FContourVertexData>& RawVertices);

	//From the raw countour data, retrieved the simplified contour by removing the non-mandatory vertices
	//The non-mandatory vertices are the ones that represent a switch in the region the contour is bordering
	//For island region the top-right and bottom-left vertices are saved instead
	void BuildSimplifiedCountour(const int CurrentRegionID, const bool OnlyNullRegionConnection, TArray<FContourVertexData>& VerticesRaw, TArray<FContourVertexData>& VerticesSimplified);

	//Reinsert into the simplified contour vertices from the raw contour according to the EdgeMaxDeviation value (only for vertices bordering the NULL REGION)
	//Such that none of the original vertices are farther than edgeMaxDeviation distance from the simplified edges
	//The algorithm used to ensure this is the Ramer Douglas Peucker - https://karthaus.nl/rdp/
	void ReinsertNullRegionVertices(TArray<FContourVertexData>& VerticesRaw, TArray<FContourVertexData>& VerticesSimplified);

	//Insert additional vertices to make sure that no edge is longer than the MaxEdgeLength value
	//The edge inserted are located at the midpoint of the dge taken into consideration
	void CheckNullRegionMaxEdge(TArray<FContourVertexData>& VerticesRaw, TArray<FContourVertexData>& VerticesSimplified);

	//Check for possible consecutive vertices duplicates and, if found, remove them
	void RemoveDuplicatesVertices(TArray<FContourVertexData>& VerticesSimplified);

	//Get the highest grid cell value of the corner spans
	int GetCornerHeightIndex(UOpenSpan* Span, const int NeighborDir);

	//Draw the raw contour of the region passed in
	void DrawRegionRawContour(TArray<FContourVertexData>& Vertices);

	//Vertices representing the simplified contour
	TArray<FContourVertexData> SimplifiedVertices;

	//Total number of regions
	int RegionCount;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	//The maximum distance the edge of the contour may deviate from the source geometry - less the distance, more precise and intense the calculation
	float EdgeMaxDeviation = 60.f;

	float MaxEdgeLenght = 60.f;

	//Min coordinates of the heightfield derived from the bounds of the navmesh area
	FVector BoundMin;

	//Max coordinates of the heightfield derived from the bounds of the navmesh area
	FVector BoundMax;
	
	//Size of the single cells (voxels) in which the heightfiels is subdivided, the cells are squared
	float CellSize;
	
	//Height of the single cells (voxels) in which the heightfiels is subdivided
	float CellHeight;

	//Region ID to which the countour refer to
	int RegionID;
};
