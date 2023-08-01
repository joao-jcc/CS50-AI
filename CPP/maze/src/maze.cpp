#include "maze.hpp"

// Constructor of the Maze class that takes the file path as an argument and calls the read_maze function to initialize the maze.
Maze::Maze(std::string path) {
    read_maze(path);
    _frontier = new QueueFrontier();
    _solution = std::vector<Vector2>();
    _explored_nodes = std::vector<Node*>();
    _trash_nodes = std::vector<Node*>();
}

// Destructor of the Maze class. It releases the memory of all the Node objects in the _explored_nodes vector.
Maze::~Maze() {
    for (Node* node : _explored_nodes) {
        delete node;
    }

    for (Node* node : _trash_nodes) {
        delete node;
    }

    delete _frontier;
}

// Function to read the maze from a file specified by the given path and initialize the internal maze representation.
void Maze::read_maze(std::string path) {
    std::ifstream file(path);

    // Check if the file could be opened successfully.
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << path << std::endl;
        exit(2); // Exit the program with an error code 2.
    }

    // Clear the existing maze data.
    _walls.clear();

    float l = 0; // Initialize a line counter.
    float c;
    // Read the file line by line.
    std::string line;
    while (getline(file, line)) {
        std::vector<bool> row; // Create a vector to represent a row in the maze.

        // Process each character in the current line.
        for (c = 0; c < line.size(); ++c) {
            if (line[c] == '#') {
                // '#' character represents a wall.
                row.push_back(true);
                continue;
            } else if (line[c] == 'A') {
                // 'A' character represents the starting position in the maze.
                _start = Vector2{l, c};
            } else if (line[c] == 'B') {
                // 'B' character represents the goal position in the maze.
                _goal = Vector2{l, c};
            }
            row.push_back(false); // Any other character represents an open path.
        }
        _walls.push_back(row); // Add the row to the maze representation.
        ++l; // Increment the line counter.
    }

    _dimension = Vector2{l, c};

    file.close(); // Close the file after reading.
}

// Function to write the solution path into a file specified by the given path.
void Maze::write_solution(std::string path) {
    if (_solution.empty()) {
        std::cerr << "No solution to be written!" << std::endl;
        exit(3); // Exit the program with an error code 3.
    }

    std::ofstream file(path);
    // Check if the file could be opened successfully.
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << path << std::endl;
        exit(2); // Exit the program with an error code 2.
    }

    // Write path cost and number of explored nodes
    file << "Explored Nodes: " << _explored_nodes.size() << std::endl;
    file << "Path Cost: " << _solution.size() - 1 << std::endl;

    // Create a maze representation with characters to be written into the file.
    std::vector< std::vector<char> > maze(_dimension.x, std::vector<char>(_dimension.y, ' '));

    // Convert the maze data into characters for visualization.
    for (int l = 0; l < _dimension.x; ++l) {
        for (int c = 0; c < _dimension.y; ++c) {
            maze[l][c] = _walls[l][c] ? '#' : ' ';
        }
    }

    for (Node* node: _explored_nodes) {
        maze[node->position.x][node->position.y] = '-';
    }

    // Mark the solution path in the maze representation.
    for (Vector2 position : _solution) {
        maze[position.x][position.y] = '*';
    }

    // Mark the start and goal positions in the maze representation.
    maze[_start.x][_start.y] = (char)'A';
    maze[_goal.x][_goal.y] = (char)'B';
    
    // Write the maze representation into the file.
    for (int l = 0; l < _dimension.x; ++l) {
        for (int c = 0; c < _dimension.y; ++c) {
            file << maze[l][c];
        }
        file << std::endl; // Add a new line after each row.
    }


    file.close(); // Close the file after writing.
}

std::vector<Vector2> Maze::actions(Node* node) {
    int l = node->position.x;
    int c = node->position.y;
    std::vector<Vector2> options;

    // Down
    if (l < _dimension.x - 1 && !_walls[l + 1][c]) {
        options.push_back(Vector2{1, 0});
    }
    // Up
    if (l > 0 && !_walls[l - 1][c]) {
        options.push_back(Vector2{-1, 0});
    }
    // Right
    if (c < _dimension.y - 1 && !_walls[l][c + 1]) {
        options.push_back(Vector2{0, 1});
    }
    // Left
    if (c > 0 && !_walls[l][c - 1]) {
        options.push_back(Vector2{0, -1});
    }

    return options;
}

Node* Maze::result(Node* node, Vector2 action) {
    Vector2 new_position = Vector2{node->position.x + action.x, node->position.y + action.y};
    Node* new_node = new Node{new_position, action, node};

    return new_node;
}

bool Maze::explored(Node* target_node) {
    for (Node* node : _explored_nodes) {
        if(equal_node(target_node, node)) {
            return true;
        }
    }

    return false;
}

void Maze::solve() {
    Node* start_node = new Node{_start, Vector2{0, 0}, nullptr};

    _frontier->add(start_node);

    Node* node = new Node{};
    while(true) {
        if (_frontier->empty()) {
            std::cerr << "No Solution!" << std::endl;
            exit(5);
        }

        node = _frontier->remove();
        _explored_nodes.push_back(node);        
        if (equal_v2(node->position, _goal)) {
            break;
        }

        for (Vector2 action : actions(node)) {
            Node* temp_node = result(node, action);
            if (!(_frontier->contains_state(temp_node)) && !explored(temp_node)) {
                _frontier->add(temp_node);
            } else {
                _trash_nodes.push_back(temp_node);
            }
        }
    }

    while(node->parent != nullptr) {
        _solution.insert(_solution.begin(), node->position);
        node = node->parent;
    }
    _solution.insert(_solution.begin(), node->position);
}

// Getters
Vector2 Maze::get_vector2(char key) {
    switch (key) {
        case 's':
            return _start;
            break;
        
        case 'g':
            return _goal;
            break;
        
        case 'd':
            return _dimension;
            break;

        default:
            std::cerr << "Invalid Keyword!" << std::endl;
            exit(4);

    }
}

std::vector< std::vector<bool> > Maze::get_walls() {
    return _walls;
};
