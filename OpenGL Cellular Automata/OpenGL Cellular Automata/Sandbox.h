#pragma once

#include "Shader.h"
#include "Cells.h"
#include "ShaderStorageBuffer.h"
#include <vector>
#include "Chunk.h"
#include <algorithm>

#define TILE_SIZE 4
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

class Sandbox {

private:
	cell_t* m_cells;
	cell_t* m_cells_prev;
	Chunk* chunks;

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

private:

	ShaderStorageBuffer* ssbo;

	int CreateVertices(int& width, int& height);
	int CreateIndices(int& width, int& height);
	int CreateColors(int& width, int& height);
	void CreateCells(int& width, int& height);

	void CreateChunks();
	Chunk* GetChunkAtCellCoords(int x, int y);
	void ReportToChunk(int x, int y);

	void AddCell(int x, int y);
	void Swap(int x1, int y1, int x2, int y2);
	bool IsEmpty(int x, int y);
	bool InBounds(int x, int y);

	void SetSurroundingFalling(int x, int y, float& inertialResistance);
	float UpdateVelocity(int x, int y);
	void MovingSolid(int& x, int& y, cell_t* cell, float inertialResistance);
	void UpdateSand(int& x, int& y);
	void UpdateWater(int& x, int& y, int dispersionRate);

	void UpdateLava(int& x, int& y, int dispersionRate);
};