#include "Sandbox.h"
#include "ErrorHandling.h"
#include <cmath>

#include <iostream>

Sandbox::Sandbox()
{
    width = SCREEN_WIDTH / TILE_SIZE;
    height = SCREEN_HEIGHT / TILE_SIZE;

    numCellsPerChunk = 64 / TILE_SIZE;
    chunkSize = numCellsPerChunk * TILE_SIZE;
    chunk_width = (int)ceil(SCREEN_WIDTH / chunkSize);
    chunk_height = (int)ceil(SCREEN_HEIGHT / chunkSize);

    CreateVertices(width, height);
    CreateIndices(width, height);
    CreateColors(width, height);

    CreateCells(width, height);
    //CreateChunks();

    ssbo = new ShaderStorageBuffer(colors, (width) * (height) * 4 * sizeof(float));

    currentType = STONE;

    //InitFunctionMap();
}

Sandbox::~Sandbox() {

    delete[] vertices;
    delete[] indices;
    delete[] colors;
    delete[] m_cells;
    delete[] chunks;
}

int Sandbox::CreateVertices(int& width, int& height)
{
    vertices = new int[(width + 1) * (height + 1) * 2];

    if (vertices == NULL) {

        std::cout << "Failed to initialize vertices!" << std::endl;

        return -1;
    }

    for (int y = 0, i = 0; y <= height; y++) {

        for (int x = 0; x <= width; x++, i += 2) {

            vertices[i] = x * TILE_SIZE; //x
            vertices[i + 1] = y * TILE_SIZE; //y
        }
    }

    return 0;
}

int Sandbox::CreateIndices(int& width, int& height)
{
    indices = new unsigned int[width * height * 6];

    if (indices == NULL) {

        std::cout << "Failed to initialize indices!" << std::endl;

        return -1;
    }

    for (int ti = 0, vi = 0, y = 0; y < height; y++, vi++) {
        for (int x = 0; x < width; x++, ti += 6, vi++) {

            indices[ti] = vi;
            indices[ti + 1] = vi + 1;
            indices[ti + 2] = vi + width + 2;
            indices[ti + 3] = indices[ti + 2];
            indices[ti + 4] = vi + width + 1;
            indices[ti + 5] = indices[ti];
        }
    }

    return 0;
}

int Sandbox::CreateColors(int& width, int& height)
{
    colors = new float[width * height * 4];

    if (colors == NULL) {

        std::cout << "Failed to initialize colors!" << std::endl;

        return -1;
    }

    for (int y = 0, i = 0; y < height; y++) {

        for (int x = 0; x < width; x++, i += 4) {

            colors[i] = empty_col.r / 255.f; //r
            colors[i + 1] = empty_col.g / 255.f; //g
            colors[i + 2] = empty_col.b / 255.f; //b
            colors[i + 3] = 1.f; //a
        }
    }

    return 0;
}

void Sandbox::CreateCells(int& width, int& height) {

    m_cells = new cell_t[width * height];

    if (!m_cells) {

        std::cout << "Failed to initialize cells!" << std::endl;

        throw std::bad_alloc();
    }

    for (int y = 0, i = 0; y < height; y++) {

        for (int x = 0; x < width; x++) {

            m_cells[width * y + x] = cell_empty();
        }
    }
}

void Sandbox::InitFunctionMap() {

    // Initialize the map with update functions
    updateFunctions[SAND] = [this](int& x, int& y) { UpdateSand(x, y); };
    updateFunctions[WATER] = [this](int& x, int& y) { UpdateWater(x, y, 5); };
    updateFunctions[WOOD] = [this](int& x, int& y) { UpdateWood(x, y); };
    updateFunctions[LAVA] = [this](int& x, int& y) { UpdateLava(x, y, 2); };
    updateFunctions[FIRE] = [this](int& x, int& y) { UpdateFire(x, y); };
}

void Sandbox::CreateChunks() {

    chunks = new Chunk[chunk_width * chunk_height];

    for (int y = 0; y < chunk_height; y++) {
        for (int x = 0; x < chunk_width; x++) {

            int index = y * chunk_width + x;

            vector_t topLeft = { x * chunkSize, y * chunkSize + chunkSize - 1 };
            vector_t topRight = { x * chunkSize + chunkSize - 1, y * chunkSize + chunkSize - 1 };
            vector_t bottomLeft = { x * chunkSize, y * chunkSize };
            vector_t bottomRight = { x* chunkSize + chunkSize - 1, y * chunkSize };

            chunks[index].topLeft = topLeft;
            chunks[index].topRight = topRight;
            chunks[index].setBottomLeft(bottomLeft);
            chunks[index].setBottomRight(bottomRight);
        }
    }
}

Chunk* Sandbox::GetChunkAtCellCoords(int x, int y) {

    if (InBounds(x, y)) {

        //Divide by numCellsPerChunk to get an index of the chunk - divide by numCellsPerChunk because thats how many cells the chunk has
        return &chunks[chunk_width * (y / numCellsPerChunk) + (x / numCellsPerChunk)];
    }

    return NULL;
}

void Sandbox::ReportToChunk(int x, int y) {

    //Convert coordinates to chunk coordinates
    //getChunkAtCellCoords(x, y)->topLeft.x / TILE_SIZE -- get the top left position of a chunk and convert it to cell coords
    if (InBounds(x, y)) {

        if (x == GetChunkAtCellCoords(x, y)->bottomLeft.x / TILE_SIZE) {

            Chunk* chunk = GetChunkAtCellCoords(x - 1, y);

            if (chunk != NULL)
                chunk->shouldUpdateNextFrame = true;
        }
        if (x == GetChunkAtCellCoords(x, y)->bottomRight.x / TILE_SIZE) {

            Chunk* chunk = GetChunkAtCellCoords(x + 1, y);

            if (chunk != NULL)
                chunk->shouldUpdateNextFrame = true;
        }
        if (y == GetChunkAtCellCoords(x, y)->bottomLeft.y / TILE_SIZE) {

            Chunk* chunk = GetChunkAtCellCoords(x, y - 1);

            if (chunk != NULL)
                chunk->shouldUpdateNextFrame = true;
        }
        if (y == GetChunkAtCellCoords(x, y)->topLeft.y / TILE_SIZE) {

            Chunk* chunk = GetChunkAtCellCoords(x, y + 1);

            if (chunk != NULL)
                chunk->shouldUpdateNextFrame = true;
        }
    }

    Chunk* chunk = GetChunkAtCellCoords(x, y);

    if (chunk != NULL)
        chunk->shouldUpdateNextFrame = true;
}

void Sandbox::UpdateChunks() { //Glitch - when you put moveable solid on the edges, the chunks will remain active - FIXED

    for (int y = 0; y < chunk_height; y++) {
        for (int x = 0; x < chunk_width; x++) {

            if (chunks[chunk_width * y + x].shouldUpdate) {

                UpdateCellsInChunk(&chunks[chunk_width * y + x]);
            }
        }
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            m_cells[width * y + x].moved_last_frame = false;

        }
    }

    //Disable chunks at the end of the frame
    for (int y = 0; y < chunk_height; y++) {
        for (int x = 0; x < chunk_width; x++) {

            chunks[chunk_width * y + x].shiftUpdatesAndReset();
        }
    }
}

bool Sandbox::IsEmpty(int x, int y) {

    if (!InBounds(x, y)) return 0;

    if (m_cells[width * y + x].type == EMPTY) return 1;

    return 0;
}

bool Sandbox::InBounds(int x, int y) {

    if (x >= 0 && x < width && y >= 0 && y < height) return 1;

    return 0;
}

void Sandbox::iterateAndApplyMethodBetweenTwoPoints(vector_t pos1, vector_t pos2) {
    // If the two points are the same no need to iterate. Just run the provided function
    if (pos1.x == pos2.x && pos1.y == pos2.y) {

        return;
    }

    int matrixX1 = pos1.x;
    int matrixY1 = pos1.y;
    int matrixX2 = pos2.x;
    int matrixY2 = pos2.y;

    int xDiff = matrixX1 - matrixX2;
    int yDiff = matrixY1 - matrixY2;
    bool xDiffIsLarger = std::abs(xDiff) > std::abs(yDiff);

    int xModifier = xDiff < 0 ? 1 : -1;
    int yModifier = yDiff < 0 ? 1 : -1;

    int longerSideLength = std::max(std::abs(xDiff), std::abs(yDiff));
    int shorterSideLength = std::min(std::abs(xDiff), std::abs(yDiff));
    float slope = (shorterSideLength == 0 || longerSideLength == 0) ? 0 : ((float)(shorterSideLength) / (longerSideLength));

    int shorterSideIncrease;
    for (int i = 1; i <= longerSideLength; i++) {
        shorterSideIncrease = std::round(i * slope);
        int yIncrease, xIncrease;
        if (xDiffIsLarger) {
            xIncrease = i;
            yIncrease = shorterSideIncrease;
        }
        else {
            yIncrease = i;
            xIncrease = shorterSideIncrease;
        }
        int currentY = matrixY1 + (yIncrease * yModifier);
        int currentX = matrixX1 + (xIncrease * xModifier);
        if (IsEmpty(currentX, currentY)) {
            Swap(pos1.x, pos1.y, currentX, currentY);
        }
    }
}


void Sandbox::AddCell(int x, int y) {

    if (currentType == EMPTY) {

        if (!InBounds(x, y)) return;

        m_cells[width * y + x] = cell_current(currentType);
        ChangeQuadColor(width * y + x, colors, m_cells[width * y + x].color);
    }

    //ReportToChunk(x, y);

    if (!InBounds(x, y) || !IsEmpty(x, y)) return;

    m_cells[width * y + x] = cell_current(currentType);
    ChangeQuadColor(width * y + x, colors, m_cells[width * y + x].color);


    if (rand() % 2)
        m_cells[width * y + x].velocity.x = 1.f;
    else
        m_cells[width * y + x].velocity.x = -1.f;
}

void Sandbox::ChangeQuadColor(int index, float* colors, color_t& color) {

    colors[index * 4] = color.r / 255.f; //r
    colors[index * 4 + 1] = color.g / 255.f; //g
    colors[index * 4 + 2] = color.b / 255.f; //b
    colors[index * 4 + 3] = color.a; //a

    ssbo->UpdateColors(index, 4 * sizeof(float), colors);
}

color_t Sandbox::ColorLerp(color_t& from, color_t to, float rate) {

    color_t new_col;

    new_col.r = std::lerp(from.r, to.r, rate * dt);
    new_col.g = std::lerp(from.g, to.g, rate * dt);
    new_col.b = std::lerp(from.b, to.b, rate * dt);
    
    return new_col;
}


color_t Sandbox::ColorLerp3(color_t& from, color_t to1, color_t to2, float rate1, float rate2) {

    color_t intermediate;

    // Step 1: Interpolate between the first two colors
    intermediate.r = std::lerp(from.r, to1.r, rate1 * dt);
    intermediate.g = std::lerp(from.g, to1.g, rate1 * dt);
    intermediate.b = std::lerp(from.b, to1.b, rate1 * dt);

    // Step 2: Interpolate between the intermediate color and the third color
    color_t final_col;
    final_col.r = std::lerp(intermediate.r, to2.r, rate2 * dt);
    final_col.g = std::lerp(intermediate.g, to2.g, rate2 * dt);
    final_col.b = std::lerp(intermediate.b, to2.b, rate2 * dt);

    return final_col;
}

void Sandbox::DrawCircle(int x, int y, int radius) {

    int rSquared = radius * radius;

    for (int u = x - radius; u < x + radius + 1; u++) {
        for (int v = y - radius; v < y + radius + 1; v++) {
            if ((x - u) * (x - u) + (y - v) * (y - v) < rSquared)
            {
                if (Type_Map[currentType] == GAS) {

                    if(RandomFloat(0.f, 1.f) >= 0.8f)
                        AddCell(u, v);
                }
                else
                    AddCell(u, v);
            }
        }
    }
}

void Sandbox::FillScreen() {

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            m_cells[width * y + x] = cell_current(currentType);
            ChangeQuadColor(width * y + x, colors, m_cells[width * y + x].color);
        }
    }
}

void Sandbox::Swap(int x1, int y1, int x2, int y2) {

    cell_t temp = m_cells[width * y1 + x1];
    m_cells[width * y1 + x1] = m_cells[width * y2 + x2];
    m_cells[width * y2 + x2] = temp;

    m_cells[width * y1 + x1].moved_last_frame = true;
    m_cells[width * y2 + x2].moved_last_frame = true;

    ChangeQuadColor(width * y1 + x1, colors, m_cells[width * y1 + x1].color);
    ChangeQuadColor(width * y2 + x2, colors, m_cells[width * y2 + x2].color);

    //ReportToChunk(x1, y1);
    //ReportToChunk(x2, y2);
}

void Sandbox::Replace(int x, int y, Element type) {

    if (!InBounds(x, y)) return;

    m_cells[width * y + x] = cell_current(type);
    ChangeQuadColor(width * y + x, colors, m_cells[width * y + x].color);
}

void Sandbox::Draw() {

    GLCall(glDrawElements(GL_TRIANGLES, (width * height) * 6, GL_UNSIGNED_INT, nullptr));

    for (int y = 0; y < height; y++) {

        for (int x = 0; x < width; x++) {

            m_cells[width * y + x].moved_last_frame = false;
        }
    }
}

void Sandbox::CheckCell(cell_t* cell, int &x, int& y) {

    if (cell->moved_last_frame) return;

    switch (cell->type) {

        case EMPTY:
            return;

        case BORDER:
            return;

        case SAND:

            UpdateSand(x, y);
            break;
        case WATER:

            UpdateWater(x, y, 5);
            break;
        case WOOD:

            UpdateWood(x, y);
            break;
        case LAVA:

            UpdateLava(x, y, 2);
            break;
        case FIRE:

            UpdateFire(x, y);
        case SMOKE:

            UpdateSmoke(x, y);
            break;
    }
}

void Sandbox::UpdateCellsInChunk(Chunk* chunk) { ///WORKS but I have to reconstruct applying hit to not receive but give

    for (int y = chunk->bottomLeft.y / TILE_SIZE; y <= (chunk->topLeft.y) / TILE_SIZE; y++) {

        //Fix rendering bias
        const bool leftToRight = rand() % 2 > 0;

        if (leftToRight) {

            for (int x = chunk->bottomLeft.x / TILE_SIZE; x <= (chunk->bottomRight.x) / TILE_SIZE; x++) {

                if (!InBounds(x, y)) continue;

                CheckCell(&m_cells[width * y + x], x, y);
            }
        }
        else {
            for (int x = (chunk->bottomRight.x / TILE_SIZE); x >= chunk->bottomLeft.x / TILE_SIZE; x--) {

                if (!InBounds(x, y)) continue;

                CheckCell(&m_cells[width * y + x], x, y);
            }
        }
    }
}

void Sandbox::Update() {

    for (int y = 0; y < height; y++) {

        //Fix rendering bias
        const bool leftToRight = rand() % 2 > 0;
        

        for (int x = 0; x < width; x++) {

            int columnOffset = leftToRight ? x : -x - 1 + width;
            CheckCell(&m_cells[width * y + columnOffset], columnOffset, y);
        }
    }
}

void Sandbox::SetSurroundingFalling(int x, int y, float& inertialResistance) {

    if (InBounds(x, y - 1) && randomFloat() > inertialResistance) {
        m_cells[width * (y - 1) + x].isFalling = true;
    }
    if (InBounds(x - 1, y) && randomFloat() > inertialResistance) {
        m_cells[width * y + x - 1].isFalling = true;
    }
    if (InBounds(x + 1, y) && randomFloat() > inertialResistance) {
        m_cells[width * y + x + 1].isFalling = true;
    }

    //reportToChunk(x, y);
}

float Sandbox::GetTemperature(int& x, int& y) {

    if (InBounds(x, y)) {

        return m_cells[width * y + x].temperature;
    }
    return 0.0f;
}

void Sandbox::AbsorbTemperature(int x, int y, float maxTemp, float minTemp, float tempChangeRate) {

    cell_t& cell = m_cells[width * y + x];

    float temp = 0;
    int cellNumber = 0;

    for (int i = y - 1; i <= y + 1; i++) {
        for (int j = x - 1; j <= x + 1; j++) {

            if (InBounds(j,i) && m_cells[width * i + j].type != EMPTY) {

                temp += m_cells[width * i + j].temperature;

                cellNumber++;
            }
        }
    }

    temp /= cellNumber;

    cell.temperature += (temp - cell.temperature) * tempChangeRate;
}

void Sandbox::AbsorbHeat(int& x, int& y, float tempIncreaseRate, float minTemp) {

    if (!m_cells[width * y + x].isBurning) {

        cell_t* cell = &m_cells[width * y + x];

        if (InBounds(x, y + 1) && m_cells[width * (y + 1) + x].temperature > minTemp && RandomFloat(0.f, 1.f) < 0.15f) {
            cell->temperature += tempIncreaseRate * dt * m_cells[width * (y + 1) + x].temperature;
        }
        if (InBounds(x, y - 1) && m_cells[width * (y - 1) + x].temperature > minTemp && RandomFloat(0.f, 1.f) < 0.15f) {
            cell->temperature += tempIncreaseRate * dt * m_cells[width * (y - 1) + x].temperature;
        }
        if (InBounds(x - 1, y) && m_cells[width * y + x - 1].temperature > minTemp && RandomFloat(0.f, 1.f) < 0.15f) {
            cell->temperature += tempIncreaseRate * dt * m_cells[width * y + x - 1].temperature;
        }
        if (InBounds(x + 1, y) && m_cells[width * y + x + 1].temperature > minTemp && RandomFloat(0.f, 1.f) < 0.15f) {
            cell->temperature += tempIncreaseRate * dt * m_cells[width * y + x + 1].temperature;
        }
    }
}

void Sandbox::Ignite(int& x, int& y) {

    cell_t* cell = &m_cells[width * y + x];

    if (!cell->isBurning) {

        cell->color = RandomizeColor(burn_col);
        ChangeQuadColor(width * y + x, colors, cell->color);
        cell->temperature += 300.f;

        cell->isBurning = true;
    }
}

void Sandbox::Burn(int& x, int& y) {

    if (m_cells[width * y + x].isBurning) {

        cell_t* cell = &m_cells[width * y + x];

        cell->temperature -= (int)(cell->temperature / 4.5f) % 4 + 4.5f;

        cell->color = ColorLerp(cell->color, color_t{ 148, 0, 0 }, 1.5f);
        ChangeQuadColor(width * y + x, colors, cell->color);

        if (IsEmpty(x, y + 1)) {

            if (RandomFloat(0.f, 1.f) >= 0.9f)
                Replace(x, y + 1, FIRE);
        }

        if (cell->temperature <= 300.f) {

            /*If next to water, extinguish
            if (cells[x][y - 1].type == WATER || cells[x][y + 1].type == WATER || cells[x - 1][y].type == WATER || cells[x + 1][y].type == WATER) {
                replace(x, y, CHAR);
            }
            else {
                if (randomChance(50, 1) < 2)
                    replace(x, y, ASH);
                else if (randomChance(50, 1) >= 3 && randomChance(50, 1) < 5) {
                    replace(x, y, SMOKE);
                }
            }*/

            if (RandomFloat(0.f, 1.f) >= 0.8f)
                Replace(x, y, SMOKE);
            else
                Replace(x, y, EMPTY);


            cell->isBurning = false;
        }
    }
}

void Sandbox::UpdateDeltaTime(double dt) {

    this->dt = dt;
}

float Sandbox::UpdateVelocity(int x, int y) {

    //reportToChunk(x, y);

    //GRAVITY - FINALLY WORKING
    m_cells[width * y + x].velocity.y = std::clamp(m_cells[width * y + x].velocity.y + (gravity * (float)this->dt), -10.f, 50.f);

    m_cells[width * y + x].velocity.y += gravity * (float)this->dt;
    if (InBounds(x, y - 1) && m_cells[width * (y - 1) + x].type != EMPTY && m_cells[width * (y - 1) + x].type != SMOKE && m_cells[width * (y - 1) + x].type != STEAM)
        m_cells[width * y + x].velocity.y /= 1.25f;

    return m_cells[width * y + x].velocity.y;
}

void Sandbox::MovingSolid(int& x, int& y, cell_t* cell, float inertialResistance) {


    //If is landing transfer some of y velocity to x velocity and reduce y velocity
    if ((!IsEmpty(x, y - 1)) && cell->isFalling) {
        cell->velocity.x = RandomFloat(-1.f, 1.f);
        cell->velocity.y /= 2.f;
    }

    if (cell->velocity.x < -2.f) {
        cell->velocity.x = -2.f;
    }
    if (cell->velocity.x > 2.f) {
        cell->velocity.x = 2.f;
    }

    if (!cell->isFalling) {

        cell->velocity.y = 0;

        if (std::abs(cell->velocity.x) < 0.1f)
            cell->velocity.x = 0;


        //cell->velocity.y += (0 - cell->velocity.y) * 0.25f;

        //Only check downwards
        if (IsEmpty(x, y - 1)) {

            cell->isFalling = true;
        }


        //Move the cell on x axis depending on its velocity - a little buggy rn
        else if (std::abs(cell->velocity.x) > 0.1f) {

            if ((cell->velocity.x < 0 && (InBounds(x - 1, y) && !IsEmpty(x - 1, y)))
                || (cell->velocity.x > 0 && InBounds(x + 1, y) && !IsEmpty(x + 1, y))) {
                cell->velocity.x *= -1;
                cell->velocity.x -= cell->velocity.x > 0 ? 0.2f : -0.2f;
            }

            //Fixes the chunks on borders
            else if ((m_cells[width * y + x - 1].type == BORDER) || (m_cells[width * y + x + 1].type == BORDER)) {

                cell->velocity.x *= -1;
                cell->velocity.x -= cell->velocity.x > 0 ? 0.1f : -0.1f;
            }

            int lastGood = 0;
            for (int i = 1; i <= std::ceil(std::abs(cell->velocity.x)); i++) {

                ///Experimental - if something breaks, remove
                if (cell->velocity.x > 0 ? IsEmpty(x + i, y - 1) : IsEmpty(x - i, y - 1)) {

                    cell->isFalling = true;
                    break;
                }

                else if (cell->velocity.x > 0 ? IsEmpty(x + i, y) : IsEmpty(x - i, y)) {
                    lastGood = cell->velocity.x < 0 ? -i : i;
                }
                else
                    break;

                cell->velocity.x += (0 - cell->velocity.x) * 0.26f;
            }

            Swap(x, y, x + lastGood, y);
        }
    }

    if (cell->isFalling) {

        float v_y = UpdateVelocity(x, y);

        if (std::abs(cell->velocity.x) >= 0.2f) {

            // Calculate the target position based on velocity
            int targetX = x + static_cast<int>(std::round(cell->velocity.x));
            int targetY = y - static_cast<int>(std::round(cell->velocity.y));

            // Ensure targetX and targetY are within bounds
            targetX = std::max(0, std::min(targetX, width - 1));
            targetY = std::max(0, std::min(targetY, height - 1));

            int lastGoodX = x;
            int lastGoodY = y;

            // Check if the cell can move to the target position
            if (IsEmpty(targetX, targetY)) {
                lastGoodX = targetX;
                lastGoodY = targetY;
            }
            else {
                // Check intermediate positions along the path
                for (int i = 1; i <= std::max(std::abs(targetX - x), std::abs(targetY - y)); i++) {

                    int intermediateX = x + static_cast<int>(std::round(i * (cell->velocity.x / std::max(std::abs(targetX - x), std::abs(targetY - y)))));
                    int intermediateY = y - static_cast<int>(std::round(i * (cell->velocity.y / std::max(std::abs(targetX - x), std::abs(targetY - y)))));

                    // Ensure intermediate positions are within bounds
                    if (!InBounds(intermediateX, intermediateY))
                        break;

                    if (IsEmpty(intermediateX, intermediateY)) {
                        lastGoodX = intermediateX;
                        lastGoodY = intermediateY;
                    }
                    else {
                        break;
                    }
                }
            }

            // Move the cell to the last good position found
            if (lastGoodX != x || lastGoodY != y) {
                Swap(x, y, lastGoodX, lastGoodY);
            }

            // Gradually lerp velocity.x to 0 while falling
            cell->velocity.x += (0 - cell->velocity.x) * 0.34f;

            // Set the surrounding cells to falling if necessary - sometimes breaks
            //SetSurroundingFalling(lastGoodX, lastGoodY, inertialResistance);
        }

        if (IsEmpty(x, y - 1)) {

            int lastGood = 1;

            for (int i = 1; i <= cell->velocity.y; i++) {

                if (IsEmpty(x, y - i))
                    lastGood = i;
                else
                    break;
                SetSurroundingFalling(x, y - lastGood, inertialResistance);
            }


            Swap(x, y, x, y - lastGood);

        }

        else if (IsEmpty(x - 1, y - 1) || IsEmpty(x + 1, y - 1)) {
            int random = rand() % 2;

            if (random > 0) {
                if (IsEmpty(x - 1, y - 1)) {

                    SetSurroundingFalling(x - 1, y - 1, inertialResistance);
                    Swap(x, y, x - 1, y - 1);
                    //setSurroundingFalling(x, y, inertialResistance);

                }
            }
            else {
                if (IsEmpty(x + 1, y - 1)) {

                    SetSurroundingFalling(x + 1, y - 1, inertialResistance);
                    Swap(x, y, x + 1, y - 1);
                    //setSurroundingFalling(x, y, inertialResistance);
                }
            }

            // Gradually lerp velocity.x to 0 while falling
            //cell->velocity.x += (0.5f - cell->velocity.x) * 0.26f;
        }
    }


    //At the end of the frame set isFalling to false
    cell->isFalling = false;

}

void Sandbox::MovingGas(int& x, int& y, cell_t* cell) {


    const bool leftToRight = rand() % 2 > 0;
    const int offset = leftToRight ? 1 : -1;

    if (IsEmpty(x, y + 1)) {

        if(rand() % 2)
            Swap(x, y, x, y + 1);
    }
    else if (IsEmpty(x + offset, y)) {

        if (rand() % 2)
            Swap(x, y, x + offset, y);
    }
    else if (IsEmpty(x - offset, y)) {

        if (rand() % 2)
            Swap(x, y, x - offset, y);
    }
    else if (IsEmpty(x + offset, y + 1)) {

        if (rand() % 2)
            Swap(x, y, x + offset, y + 1);
    }
    else if (IsEmpty(x - offset, y + 1)) {

        if (rand() % 2)
            Swap(x, y, x - offset, y + 1);
    }
}

void Sandbox::MovingTest(int& x, int& y, cell_t* cell, float inertialResistance) {

    // If landing, transfer some of y velocity to x velocity and reduce y velocity
    if ((!IsEmpty(x, y - 1)) && cell->isFalling) {
        cell->velocity.x = rand() % 2 > 0 ? cell->velocity.y / (4.f / 1 * inertialResistance) : -cell->velocity.y / (4.f / 1 * inertialResistance);
        cell->velocity.y /= 2.f;
    }

    if (std::abs(cell->velocity.x) < 0.1f)
        cell->velocity.x = 0.f;

    if (!cell->isFalling) {
        cell->velocity.y = 0;

        if (std::abs(cell->velocity.x) < 0.1f)
            cell->velocity.x = 0;

        if (IsEmpty(x, y - 1)) {
            cell->isFalling = true;
        }

    }

    if (cell->isFalling) {
        float v_y = UpdateVelocity(x, y);

        // Calculate the target position based on velocity
        int targetX = x + static_cast<int>(std::round(cell->velocity.x));
        int targetY = y - static_cast<int>(std::round(cell->velocity.y));

        // Ensure targetX and targetY are within bounds
        targetX = std::max(0, std::min(targetX, width - 1));
        targetY = std::max(0, std::min(targetY, height - 1));

        int lastGoodX = x;
        int lastGoodY = y;

        // Check if the cell can move to the target position
        if (IsEmpty(targetX, targetY)) {
            lastGoodX = targetX;
            lastGoodY = targetY;
        }
        else {
            // Check intermediate positions along the path
            for (int i = 1; i <= std::max(std::abs(targetX - x), std::abs(targetY - y)); i++) {
                int intermediateX = x + static_cast<int>(std::round(i * (cell->velocity.x / std::max(std::abs(targetX - x), std::abs(targetY - y)))));
                int intermediateY = y - static_cast<int>(std::round(i * (cell->velocity.y / std::max(std::abs(targetX - x), std::abs(targetY - y)))));

                // Ensure intermediate positions are within bounds
                if (!InBounds(intermediateX, intermediateY))
                    break;

                if (IsEmpty(intermediateX, intermediateY)) {
                    lastGoodX = intermediateX;
                    lastGoodY = intermediateY;
                }
                else {
                    break;
                }
                cell->velocity.x += (0 - cell->velocity.x) * 0.26f;
            }
        }

        // Move the cell to the last good position found
        if (lastGoodX != x || lastGoodY != y) {
            Swap(x, y, lastGoodX, lastGoodY);
        }

        // Gradually lerp velocity.x to 0 while falling
        cell->velocity.x += (0 - cell->velocity.x) * 0.26f;

        // Set the surrounding cells to falling if necessary
        SetSurroundingFalling(lastGoodX, lastGoodY, inertialResistance);
    }

    // At the end of the frame set isFalling to false if there's no empty space below
        cell->isFalling = false;
}

void Sandbox::UpdateSand(int& x, int& y) {

    MovingSolid(x, y, &m_cells[width * y + x], 0.1f);
}

void Sandbox::UpdateWater(int& x, int& y, int dispersionRate) {

    const bool leftToRight = rand() % 2 > 0;
    const int offset = leftToRight ? 1 : -1;

    if (IsEmpty(x, y - 1)) {

        Swap(x, y, x, y - 1);
    }
    else if (IsEmpty(x + offset, y)) {
        Swap(x, y, x + offset, y);
    }
}

void Sandbox::UpdateWood(int& x, int& y) {

    cell_t* cell = &m_cells[width * y + x];

    AbsorbHeat(x, y, 25.f, 100.f);

    if (!cell->isBurning && cell->temperature >= 300.f && RandomFloat(0.f, 1.f) < 0.3f) {

        Ignite(x, y);
    }

    Burn(x, y);

    if (cell->life <= 0)
        Replace(x, y, EMPTY);
}

void Sandbox::UpdateLava(int& x, int& y, int dispersionRate) {

    const bool leftToRight = rand() % 2 > 0;
    const int offset = leftToRight ? 1 : -1;

    if (IsEmpty(x, y - 1)) {

        Swap(x, y, x, y - 1);
    }
    else if (IsEmpty(x + offset, y)) {
        Swap(x, y, x + offset, y);
    }

    if (IsEmpty(x, y + 1) && RandomFloat(0.f, 1.f) >= 0.98f) {
        Replace(x, y + 1, FIRE);
    }
}

void Sandbox::UpdateFire(int& x, int& y) {

    cell_t* cell = &m_cells[width * y + x];

    cell->color = ColorLerp(cell->color, color_t{ 255,0,0 }, cell->life / (cell->life / 2.f));
    ChangeQuadColor(width * y + x, colors, cell->color);

    if (cell->life <= 0) {

        if (RandomFloat(0.f, 1.f) >= 0.85f)
            Replace(x, y, SMOKE);
        else
            Replace(x, y, EMPTY);

        return;
    }

    cell->life --;


    MovingGas(x, y, &m_cells[width * y + x]);
}

void Sandbox::UpdateSmoke(int& x, int& y) {

    cell_t* cell = &m_cells[width * y + x];

    if (cell->life <= 0) {

        Replace(x, y, EMPTY);
        return;
    }

    cell->life-=0.05f;


    MovingGas(x, y, &m_cells[width * y + x]);
}