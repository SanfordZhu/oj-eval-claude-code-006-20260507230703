#ifndef SERVER_H
#define SERVER_H

#include <cstdlib>
#include <iostream>

/*
 * You may need to define some global variables for the information of the game map here.
 * Although we don't encourage to use global variables in real cpp projects, you may have to use them because the use of
 * class is not taught yet. However, if you are member of A-class or have learnt the use of cpp class, member functions,
 * etc., you're free to modify this structure.
 */
int rows;         // The count of rows of the game map. You MUST NOT modify its name.
int columns;      // The count of columns of the game map. You MUST NOT modify its name.
int total_mines;  // The count of mines of the game map. You MUST NOT modify its name. You should initialize this
                  // variable in function InitMap. It will be used in the advanced task.
int game_state;  // The state of the game, 0 for continuing, 1 for winning, -1 for losing. You MUST NOT modify its name.

// Game state arrays
char** actual_map;      // The actual map with mines ('X' for mines, '.' for safe)
int** visited;          // Whether each cell has been visited (1) or not (0)
int** marked;           // Whether each cell is marked as mine (1) or not (0)
int** mine_count;       // Number of adjacent mines for each cell
int visited_count;      // Count of visited non-mine cells
int marked_count;       // Count of marked cells

/**
 * @brief The definition of function InitMap()
 *
 * @details This function is designed to read the initial map from stdin. For example, if there is a 3 * 3 map in which
 * mines are located at (0, 1) and (1, 2) (0-based), the stdin would be
 *     3 3
 *     .X.
 *     ...
 *     ..X
 * where X stands for a mine block and . stands for a normal block. After executing this function, your game map
 * would be initialized, with all the blocks unvisited.
 */
void InitMap() {
  std::cin >> rows >> columns;

  // Allocate memory for game state arrays
  actual_map = new char*[rows];
  visited = new int*[rows];
  marked = new int*[rows];
  mine_count = new int*[rows];

  for (int i = 0; i < rows; i++) {
    actual_map[i] = new char[columns];
    visited[i] = new int[columns]();
    marked[i] = new int[columns]();
    mine_count[i] = new int[columns]();
  }

  // Read the actual map and count mines
  total_mines = 0;
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      std::cin >> actual_map[i][j];
      if (actual_map[i][j] == 'X') {
        total_mines++;
      }
    }
  }

  // Calculate mine counts for each cell
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (actual_map[i][j] == '.') {
        int count = 0;
        // Check all 8 adjacent cells
        for (int di = -1; di <= 1; di++) {
          for (int dj = -1; dj <= 1; dj++) {
            if (di == 0 && dj == 0) continue;
            int ni = i + di, nj = j + dj;
            if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
              if (actual_map[ni][nj] == 'X') {
                count++;
              }
            }
          }
        }
        mine_count[i][j] = count;
      }
    }
  }

  // Initialize game state
  game_state = 0;
  visited_count = 0;
  marked_count = 0;
}

/**
 * @brief The definition of function VisitBlock(int, int)
 *
 * @details This function is designed to visit a block in the game map. We take the 3 * 3 game map above as an example.
 * At the beginning, if you call VisitBlock(0, 0), the return value would be 0 (game continues), and the game map would
 * be
 *     1??
 *     ???
 *     ???
 * If you call VisitBlock(0, 1) after that, the return value would be -1 (game ends and the players loses) , and the
 * game map would be
 *     1X?
 *     ???
 *     ???
 * If you call VisitBlock(0, 2), VisitBlock(2, 0), VisitBlock(1, 2) instead, the return value of the last operation
 * would be 1 (game ends and the player wins), and the game map would be
 *     1@1
 *     122
 *     01@
 *
 * @param r The row coordinate (0-based) of the block to be visited.
 * @param c The column coordinate (0-based) of the block to be visited.
 *
 * @note You should edit the value of game_state in this function. Precisely, edit it to
 *    0  if the game continues after visit that block, or that block has already been visited before.
 *    1  if the game ends and the player wins.
 *    -1 if the game ends and the player loses.
 *
 * @note For invalid operation, you should not do anything.
 */
void VisitBlock(int r, int c) {
  // Check for invalid coordinates
  if (r < 0 || r >= rows || c < 0 || c >= columns) {
    return;
  }

  // Check if already visited or marked
  if (visited[r][c] == 1 || marked[r][c] == 1) {
    return;
  }

  // Check if it's a mine
  if (actual_map[r][c] == 'X') {
    visited[r][c] = 1;
    game_state = -1;  // Game over
    return;
  }

  // Visit the cell
  visited[r][c] = 1;
  visited_count++;

  // If mine count is 0, auto-visit adjacent cells
  if (mine_count[r][c] == 0) {
    for (int di = -1; di <= 1; di++) {
      for (int dj = -1; dj <= 1; dj++) {
        if (di == 0 && dj == 0) continue;
        int ni = r + di, nj = c + dj;
        if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
          if (!visited[ni][nj] && !marked[ni][nj]) {
            VisitBlock(ni, nj);
          }
        }
      }
    }
  }

  // Check for victory
  if (visited_count == rows * columns - total_mines) {
    game_state = 1;  // Victory
  }
}

/**
 * @brief The definition of function MarkMine(int, int)
 *
 * @details This function is designed to mark a mine in the game map.
 * If the block being marked is a mine, show it as "@".
 * If the block being marked isn't a mine, END THE GAME immediately. (NOTE: This is not the same rule as the real
 * game) And you don't need to
 *
 * For example, if we use the same map as before, and the current state is:
 *     1?1
 *     ???
 *     ???
 * If you call MarkMine(0, 1), you marked the right mine. Then the resulting game map is:
 *     1@1
 *     ???
 *     ???
 * If you call MarkMine(1, 0), you marked the wrong mine(There's no mine in grid (1, 0)).
 * The game_state would be -1 and game ends immediately. The game map would be:
 *     1?1
 *     X??
 *     ???
 * This is different from the Minesweeper you've played. You should beware of that.
 *
 * @param r The row coordinate (0-based) of the block to be marked.
 * @param c The column coordinate (0-based) of the block to be marked.
 *
 * @note You should edit the value of game_state in this function. Precisely, edit it to
 *    0  if the game continues after visit that block, or that block has already been visited before.
 *    1  if the game ends and the player wins.
 *    -1 if the game ends and the player loses.
 *
 * @note For invalid operation, you should not do anything.
 */
void MarkMine(int r, int c) {
  // Check for invalid coordinates
  if (r < 0 || r >= rows || c < 0 || c >= columns) {
    return;
  }

  // Check if already visited or marked
  if (visited[r][c] == 1 || marked[r][c] == 1) {
    return;
  }

  // Check if it's actually a mine
  if (actual_map[r][c] != 'X') {
    // Marking a non-mine causes immediate game failure
    marked[r][c] = 1;
    game_state = -1;
    return;
  }

  // Mark the mine
  marked[r][c] = 1;
  marked_count++;
}

/**
 * @brief The definition of function AutoExplore(int, int)
 *
 * @details This function is designed to auto-visit adjacent blocks of a certain block.
 * See README.md for more information
 *
 * For example, if we use the same map as before, and the current map is:
 *     ?@?
 *     ?2?
 *     ??@
 * Then auto explore is available only for block (1, 1). If you call AutoExplore(1, 1), the resulting map will be:
 *     1@1
 *     122
 *     01@
 * And the game ends (and player wins).
 */
void AutoExplore(int r, int c) {
  // Check for invalid coordinates
  if (r < 0 || r >= rows || c < 0 || c >= columns) {
    return;
  }

  // Check if the cell is visited and is a non-mine
  if (visited[r][c] != 1 || actual_map[r][c] == 'X') {
    return;
  }

  // Count marked mines around this cell
  int marked_around = 0;
  for (int di = -1; di <= 1; di++) {
    for (int dj = -1; dj <= 1; dj++) {
      if (di == 0 && dj == 0) continue;
      int ni = r + di, nj = c + dj;
      if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
        if (marked[ni][nj] == 1) {
          marked_around++;
        }
      }
    }
  }

  // If marked mines equal the mine count, visit all unvisited non-mine neighbors
  if (marked_around == mine_count[r][c]) {
    for (int di = -1; di <= 1; di++) {
      for (int dj = -1; dj <= 1; dj++) {
        if (di == 0 && dj == 0) continue;
        int ni = r + di, nj = c + dj;
        if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
          if (!visited[ni][nj] && !marked[ni][nj]) {
            VisitBlock(ni, nj);
          }
        }
      }
    }
  }
}

/**
 * @brief The definition of function ExitGame()
 *
 * @details This function is designed to exit the game.
 * It outputs a line according to the result, and a line of two integers, visit_count and marked_mine_count,
 * representing the number of blocks visited and the number of marked mines taken respectively.
 *
 * @note If the player wins, we consider that ALL mines are correctly marked.
 */
void ExitGame() {
  // Output game result
  if (game_state == 1) {
    std::cout << "YOU WIN!" << std::endl;
  } else {
    std::cout << "GAME OVER!" << std::endl;
  }

  // Output visit count and marked mine count
  if (game_state == 1) {
    // If victory, all mines are considered marked
    std::cout << visited_count << " " << total_mines << std::endl;
  } else {
    std::cout << visited_count << " " << marked_count << std::endl;
  }

  // Clean up memory
  for (int i = 0; i < rows; i++) {
    delete[] actual_map[i];
    delete[] visited[i];
    delete[] marked[i];
    delete[] mine_count[i];
  }
  delete[] actual_map;
  delete[] visited;
  delete[] marked;
  delete[] mine_count;

  exit(0);  // Exit the game immediately
}

/**
 * @brief The definition of function PrintMap()
 *
 * @details This function is designed to print the game map to stdout. We take the 3 * 3 game map above as an example.
 * At the beginning, if you call PrintMap(), the stdout would be
 *    ???
 *    ???
 *    ???
 * If you call VisitBlock(2, 0) and PrintMap() after that, the stdout would be
 *    ???
 *    12?
 *    01?
 * If you call VisitBlock(0, 1) and PrintMap() after that, the stdout would be
 *    ?X?
 *    12?
 *    01?
 * If the player visits all blocks without mine and call PrintMap() after that, the stdout would be
 *    1@1
 *    122
 *    01@
 * (You may find the global variable game_state useful when implementing this function.)
 *
 * @note Use std::cout to print the game map, especially when you want to try the advanced task!!!
 */
void PrintMap() {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (game_state == 1 && actual_map[i][j] == 'X') {
        // Victory state: show all mines as @
        std::cout << '@';
      } else if (marked[i][j] == 1) {
        // Marked cell
        if (actual_map[i][j] == 'X') {
          std::cout << '@';
        } else {
          // This shouldn't happen in normal gameplay, but show X if marked non-mine
          std::cout << 'X';
        }
      } else if (visited[i][j] == 1) {
        // Visited cell
        if (actual_map[i][j] == 'X') {
          std::cout << 'X';
        } else {
          std::cout << mine_count[i][j];
        }
      } else {
        // Unvisited cell
        std::cout << '?';
      }
    }
    std::cout << std::endl;
  }
}

#endif
