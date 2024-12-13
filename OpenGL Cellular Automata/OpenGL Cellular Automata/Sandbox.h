#pragma once

#include "Shader.h"
#include "Cells.h"
#include "ShaderStorageBuffer.h"
#include <vector>
#include "Chunk.h"
#include <algorithm>
#include <functional>

#define TILE_SIZE 4
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

class Sandbox {

private:
	cell_t* m_cells;
	cell_t* m_cells_prev;
	Chunk* chunks;
	std::unordered_map<int, std::function<void(int&, int&)>> updateFunctions;

public:

	int* vertices;
	unsigned int* indices;
	float* colors;
	float gravity = 9.81f;
	double dt;

public:

	Sandbox();
	~Sandbox();

	int width;
	int height;

	int numCellsPerChunk;
	int chunkSize;

	int chunk_width;
	int chunk_height;

	Element currentType;

	void ChangeQuadColor(int index, float* colors, color_t& color);
	void DrawCircle(int x, int y, int radius);

	void Draw();

	void CheckCell(cell_t* cell, int& x, int& y);
	void Update();
	void UpdateCellsInChunk(Chunk* chunk);
	void UpdateChunks();
	void UpdateDeltaTime(double dt);

	void FillScreen();

private:

	ShaderStorageBuffer* ssbo;

	int CreateVertices(int& width, int& height);
	int CreateIndices(int& width, int& height);
	int CreateColors(int& width, int& height);
	void CreateCells(int& width, int& height);
	void InitFunctionMap();

	color_t ColorLerp(color_t& from, color_t to, float rate);
	color_t ColorLerp3(color_t& from, color_t to1, color_t to2, float rate1, float rate2);

	float GetTemperature(int& x, int& y);
	void AbsorbTemperature(int x, int y, float maxTemp, float minTemp, float tempChangeRate);
	void AbsorbHeat(int& x, int& y, float tempIncreaseRate, float minTemp);

	void Ignite(int& x, int& y);
	void Burn(int& x, int& y);

	void CreateChunks();
	Chunk* GetChunkAtCellCoords(int x, int y);
	void ReportToChunk(int x, int y);

	void AddCell(int x, int y);
	void Replace(int x, int y, Element type);
	void Swap(int x1, int y1, int x2, int y2);
	bool IsEmpty(int x, int y);
	bool InBounds(int x, int y);

	void iterateAndApplyMethodBetweenTwoPoints(vector_t pos1, vector_t pos2);

	void SetSurroundingFalling(int x, int y, float& inertialResistance);
	float UpdateVelocity(int x, int y);
	void MovingSolid(int& x, int& y, cell_t* cell, float inertialResistance);
	void MovingGas(int& x, int& y, cell_t* cell);
	void MovingTest(int& x, int& y, cell_t* cell, float inertialResistance);
	void UpdateSand(int& x, int& y);
	void UpdateWater(int& x, int& y, int dispersionRate);
	void UpdateWood(int& x, int& y);
	void UpdateLava(int& x, int& y, int dispersionRate);
	void UpdateFire(int& x, int& y);
	void UpdateSmoke(int& x, int& y);
};