#include<SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
#include<SFML/Window.hpp>
#include<bits/stdc++.h>
#include<functional>
#include<string.h>
#include<sstream>
#include <chrono>
using namespace std;
using namespace sf;
using pqNode = tuple<int, int, int>; // {cost, x, y}
Font font("arial.ttf");
Text timeText(font);

int num = 60, rows = 60, cols = 60;
Vector2i startPos(15, 15);  
Vector2i endPos(30, 30); 
int grid[60][60];       //map with obstacle
const int cellSize = 800 / 60;
vector<Vector2i> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

void showPath(RenderWindow& window) {
    window.clear(Color::Black);
            
        for (int i = 0; i < 60; ++i) {
            for (int j = 0; j < 60; ++j) {
                RectangleShape cell({static_cast<float>(cellSize - 1), static_cast<float>(cellSize - 1)});
                cell.setPosition({j * static_cast<float>(cellSize), i * static_cast<float>(cellSize)});
                    
                    
                if(grid[i][j] == 1) {
                    cell.setFillColor(Color::Black); // a wall               
                }
                else if(startPos.x == i and startPos.y == j) {
                    cell.setFillColor(Color::Green); // Start pt
                }
                else if(endPos.x == i and endPos.y == j) {
                    cell.setFillColor(Color::Red); // End pt
                }
                else if(grid[i][j] == 3) {
                    cell.setFillColor(Color::Blue); // Visited Cells
                }
                else if(grid[i][j] == 2) {
                    cell.setFillColor(Color::Yellow); // Shortest path
                }
                else if(grid[i][j] == 4) {
                    cell.setFillColor(Color(255, 165, 0)); // Slow cell
                }
                else {
                    cell.setFillColor(Color::White); // empty cell
                }
                
                window.draw(cell);
            }
        }
        window.draw(timeText);
        window.display();
}

void BFS(RenderWindow& window) {
    queue<Vector2i> q;
    vector<vector<bool>> visited(rows, vector<bool> (cols, false)); 
    vector<vector<Vector2i>> parent(rows, vector<Vector2i> (cols, {-1, -1}));
    
    q.push(startPos);
    visited[startPos.x][startPos.y] = true;
    
    
    bool found = false;
    while (!q.empty() && !found) {
        Vector2i current = q.front();
        q.pop();
        
        for(const auto& dir : directions) {
           int newX = current.x + dir.x;
           int newY = current.y + dir.y; 
           
            if (newX >= 0 && newX < rows && newY >= 0 && newY < cols && !visited[newX][newY] && grid[newX][newY] != 1) {
                    visited[newX][newY] = true;
                    parent[newX][newY] = current;
                    q.push({newX, newY});
            
                    // visited cell mark blue
                    if(grid[newX][newY] != 4) grid[newX][newY] = 3;
                    showPath(window);
                    // Break out if we reach the endpoint
                    if (Vector2i(newX, newY) == endPos) {
                        found = true;
                        break;
                    }
            }
        }
    }
    
    // Trace back the shortest path
    if (found) {
        Vector2i current = endPos;
        while (current != startPos) {
            grid[current.x][current.y] = 2;
            current = parent[current.x][current.y];
        }
        grid[startPos.x][startPos.y] = 2;
    }
    
    showPath(window);
}

bool DFS(int x, int y, vector<vector<bool>>& visited, vector<vector<sf::Vector2i>>& parent, RenderWindow& window) {
    
    // Base case: reached the endpoint
    if (x == endPos.x && y == endPos.y) {
        return true;
    }

    // Mark the current cell as visited
    visited[x][y] = true;
    if(grid[x][y] != 4) grid[x][y] = 3; // Mark as visited (blue)
    showPath(window); // Visualize the grid
    
    // Explore all four directions
    for (auto [dx, dy] : directions) {
        int nx = x + dx, ny = y + dy;

        if (nx >= 0 && nx < rows && ny >= 0 && ny < cols && grid[nx][ny] != 1 && !visited[nx][ny]) {
            parent[nx][ny] = {x, y}; // Track the path
            if (DFS(nx, ny, visited, parent, window)) {
                return true; // Stopping recursion if endpoint is reached
            }
        }
    }

    return false; // No path found
}

void runDFS(RenderWindow& window) {
    vector<vector<bool>> visited(rows, vector<bool> (cols, false)); 
    vector<vector<Vector2i>> parent(rows, vector<Vector2i> (cols, {-1, -1}));
    
    if(DFS(startPos.x, startPos.y, visited, parent, window)) {
        Vector2i current = endPos;
        while(current != startPos) {
            grid[current.x][current.y] = 2;
            current = parent[current.x][current.y];
        }
        showPath(window);
    }
    
}

void Dijkstra(RenderWindow& window) {
    vector<vector<int>> dist(rows, vector<int>(cols, INT_MAX));
    vector<vector<Vector2i>> parent(rows, vector<Vector2i>(cols, {-1, -1}));

    dist[startPos.x][startPos.y] = 0;
    
    // Min-heap (priority queue) to store the cells, ordered by distance
    priority_queue<pqNode, vector<pqNode>, greater<pqNode>> pq;

    pq.push({0, startPos.x, startPos.y});

    while (!pq.empty()) {
        auto [currentDist, x, y] = pq.top();
        pq.pop();

        // If we have reached the end, we will stop the search
        if (x == endPos.x && y == endPos.y) {
            break;
        }

        // Explore neighbors
        for (auto [dx, dy] : directions) {
            int newX = x + dx;
            int newY = y + dy;

            if (newX >= 0 && newY >= 0 && newX < rows && newY < cols && grid[newX][newY] != 1) { 
                int newDist = currentDist + (grid[newX][newY] == 4 ? 3 : 1); // slow cell takes more time (+3)

                if (newDist < dist[newX][newY]) {
                    dist[newX][newY] = newDist;
                    parent[newX][newY] = {x, y};
                    if(grid[newX][newY] != 4) grid[newX][newY] = 3;
                    pq.push({newDist, newX, newY});
                }
            }
        }
        // visualzing the path here :)
        showPath(window); 
    }

    // Showing the correct path retracing it using parent
    Vector2i current = endPos;
    while (parent[current.x][current.y] != Vector2i(-1, -1)) {
        grid[current.x][current.y] = 2; // Mark shortest path (green or any other color)
        current = parent[current.x][current.y];
    }

    showPath(window); // Final visualization
}

// Heuristic function: Manhattan distance
int heuristic(const Vector2i& a, const Vector2i& b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

struct Node {
    Vector2i position;
    int g, h, f; // g: cost from start, h: heuristic, f = g + h

    bool operator>(const Node& other) const {
        return f > other.f; // Min-heap based on f value
    }
};


void AStar(RenderWindow& window) {
    // Priority queue for A*
    vector<vector<int>> dist(rows, vector<int>(cols, INT_MAX));
    vector<vector<Vector2i>> parent(rows, vector<Vector2i>(cols, {-1, -1}));

    dist[startPos.x][startPos.y] = 0;
    
    priority_queue<Node, vector<Node>, greater<Node>> openList;

    openList.push({startPos, 0, heuristic(startPos, endPos), heuristic(startPos, endPos)});
    
    while (!openList.empty()) {
        Node currentNode = openList.top();
        openList.pop();

        Vector2i current = currentNode.position;
        if (current == endPos) {
            // Reached the goal, retrace path
            Vector2i path = endPos;
            while (path != startPos) {
                grid[path.x][path.y] = 2; // Mark path
                path = parent[path.x][path.y];
            }
            grid[startPos.x][startPos.y] = 2;
            break;
        }

        // Explore neighbors
        for (auto& dir : directions) {
            Vector2i neighbor = current + dir;
            int newX = neighbor.x;
            int newY = neighbor.y;
            if (newX >= 0 && newX < rows && newY >= 0 && newY < cols && grid[newX][newY] != 1) {
                int gCost = currentNode.g + (grid[newX][newY] == 4 ? 3 : 1);
                int hCost = heuristic(neighbor, endPos);
                int fCost = gCost + hCost;
                
                if(dist[newX][newY] > fCost) {
                    dist[newX][newY] = fCost;
                    openList.push({neighbor, gCost, hCost, fCost});
                    if(grid[newX][newY] != 4) grid[newX][newY] = 3;
                    parent[neighbor.x][neighbor.y] = current;
                }
            }
        }
        // Visualization
        showPath(window);
    }
    
    showPath(window);
}



int main() {
    bool isMousePressed = false;
    bool draggingStart = false;
    bool draggingEnd = false;

    // Create text object for displaying time
    timeText.setFont(font);
    timeText.setCharacterSize(24); // Font size
    timeText.setFillColor(Color::Black);
    timeText.setString("Hello, Let's Start");
    
    for(int i = 0; i < 60; i++) {
        for(int j = 0; j < 60; j++) {
            grid[i][j] = 0;
        }
    }
    
    
    
    RenderWindow window(VideoMode({800, 800}), "Pathfinding Visualizer");
    
    while (window.isOpen()) {
        while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) 
                window.close();
        }
        
        if (!Mouse::isButtonPressed(Mouse::Button::Left)) {
            draggingStart = draggingEnd = false;
        }
        
        if (Mouse::isButtonPressed(Mouse::Button::Left)) {
             Vector2i mousePos = Mouse::getPosition(window);

            // Convert mouse position to grid cell (row, col)
            int row = mousePos.y / cellSize;
            int col = mousePos.x / cellSize;
            // Toggle cell state (if within bounds)
            if(row == startPos.x and col == startPos.y) {
                draggingStart = true;
            }
            else if(row == endPos.x and col == endPos.y) {
                draggingEnd = true;
            }
                
                
            if (row >= 0 && row < rows && col >= 0 && col < cols) {
                if(draggingStart and grid[row][col] == 0) {
                    startPos = {row, col};
                }
                else if(draggingEnd and grid[row][col] == 0) {
                    endPos = {row, col};
                }
                else if(!(row == startPos.x and col == startPos.y) and
                    !(row == endPos.x and col == endPos.y))
                    grid[row][col] = 1; //  1 (wall)
                }        
        }
        
        if (Mouse::isButtonPressed(Mouse::Button::Right)) {
            Vector2i mousePos = Mouse::getPosition(window);
            int x = mousePos.y / cellSize;
            int y = mousePos.x / cellSize;
            Vector2i mousePoint = {x, y};
            if (x >= 0 && x < rows && y >= 0 && y < cols && grid[x][y] == 0) {
                if(mousePoint != startPos && mousePoint != endPos)
                grid[x][y] = 4; // Set slow-down cell
            }
        }
        

        showPath(window);
        
        if (Keyboard::isKeyPressed(Keyboard::Key::B)) {
            auto start = chrono::high_resolution_clock::now();
            BFS(window);
            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double> elapsed = end - start;
            timeText.setString("BFS Time: " + std::to_string(elapsed.count()) + " seconds");
        }
        
        if (Keyboard::isKeyPressed(Keyboard::Key::D)) {
            auto start = chrono::high_resolution_clock::now();
            runDFS(window);
            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double> elapsed = end - start;
            timeText.setString("DFS Time: " + std::to_string(elapsed.count()) + " seconds");
        }
        
        if (Keyboard::isKeyPressed(Keyboard::Key::K)) {
            auto start = chrono::high_resolution_clock::now();
            Dijkstra(window);
            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double> elapsed = end - start;
            timeText.setString("Dijkstra Time: " + std::to_string(elapsed.count()) + " seconds");
        }
        
        if (Keyboard::isKeyPressed(Keyboard::Key::A)) {
            auto start = chrono::high_resolution_clock::now();
            AStar(window);
            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double> elapsed = end - start;
            timeText.setString("AStar Algo Time: " + std::to_string(elapsed.count()) + " seconds");
        }
        
        if (Keyboard::isKeyPressed(Keyboard::Key::R)) {
            // Reset grid to default state
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {
                    grid[i][j] = 0; // Reset all cells to empty
                }
            }
            // Reseting start and end positions
            startPos = {15, 15};
            endPos = {30, 30};

            // Update the grid visually
            showPath(window);
        }
        
        if (Keyboard::isKeyPressed(Keyboard::Key::Y)) {
            
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {
                    if(grid[i][j] == 2 or grid[i][j] == 3) grid[i][j] = 0; 
                }
            }
            // Update the grid visually
            timeText.setString("Hello, Let's Start");
            showPath(window);
        }
    }

    return 0;
}
