#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GRID_SIZE 10
#define NUM_SHIPS 5

// Ship Structure, size being the number of grid spaces the ship occupies
// and symbol being the character used to represent the ship on the grid
typedef struct {
    int size;
    char symbol;
} Ship;

// Ship names with their respective unique letters and sizes
Ship ships[NUM_SHIPS] = {
    {5, 'A'}, // Aircraft Carrier
    {4, 'B'}, // Battleship
    {3, 'S'}, // Submarine
    {3, 'C'}, // Cruiser
    {2, 'D'}  // Destroyer
};

typedef struct {
    int row, col;
    int active;  // 1 if AI is in "Targeting Mode", 0 otherwise, the ai will remember its last hit and enters targeting mode
} AI_Target;

AI_Target lastHit = {-1, -1, 0};

// Function to Initialize the Grid
void initializeGrid(char grid[GRID_SIZE][GRID_SIZE]) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = '~'; // this is representing water as a symbol in the grid
        }
    }
}
/* this is the function that displayes the game board/grid
this helps place the ships randomly on the grid and makes sure that they are only placed vertically or horizontally
it ensures that the ships are placed within the bounds of the grid */
void displayGrid(char grid[GRID_SIZE][GRID_SIZE], int revealShips) {
    printf("   ");
    for (int i = 0; i < GRID_SIZE; i++) {
        printf(" %d ", i);
    }
    printf("\n");

    for (int i = 0; i < GRID_SIZE; i++) {
        printf(" %d ", i);
        for (int j = 0; j < GRID_SIZE; j++) {
            if (revealShips || grid[i][j] == 'X' || grid[i][j] == 'O') {
                printf(" %c ", grid[i][j]);
            } else {
                printf(" ~ ");
            }
        }
        printf("\n");
    }
}
int placeShips(char grid[GRID_SIZE][GRID_SIZE]) {
    for (int i = 0; i < NUM_SHIPS; i++) {
        int placed = 0;
        int attempts = 0;

        while (!placed && attempts < 100) {
            int row = rand() % GRID_SIZE;
            int col = rand() % GRID_SIZE;
            int direction = rand() % 2; // 0 being horizontal and 1 being vertical
            int valid = 1;

            // Check if ship fits and doesn't overlap with the other ships 
            for (int j = 0; j < ships[i].size; j++) {
                int newRow = row + (direction ? j : 0);
                int newCol = col + (direction ? 0 : j);

                if (newRow >= GRID_SIZE || newCol >= GRID_SIZE || grid[newRow][newCol] != '~') {
                    valid = 0;
                    break;
                }
            }
            if (valid) {
                for (int j = 0; j < ships[i].size; j++) {
                    grid[row + (direction ? j : 0)][col + (direction ? 0 : j)] = ships[i].symbol;
                }
                placed = 1;
            }
            attempts++;
        }

        if (!placed) {
            return 0;
        }
    }
    return 1;
}

/* makes sure that the attack being made is inside the grid and/or hasn't already been attacked  
Hunting Mode: Randomly picks a spot. 
Targeting Mode: If the AI has already hit a ship, it tries adjacent positions.
Marking Hits and Misses 'O' for misses and 'X' for hits.
If the AI hits a ship, it remembers the location (lastHit) to attack nearby.
NOTE THAT THE AI CODE IS NOT MY OWN WORK, i took inspiration from a couple different websites */
int isValidMove(char grid[GRID_SIZE][GRID_SIZE], int row, int col) {
    return row >= 0 && row < GRID_SIZE && col >= 0 && col < GRID_SIZE &&
           grid[row][col] != 'X' && grid[row][col] != 'O';
}

// the targeting mode for AI Attack and its logic
void aiAttack(char playerGrid[GRID_SIZE][GRID_SIZE]) {
    int row, col;
    // targeting Mode
    if (lastHit.active) {
        int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}}; // Up, Down, Left, Right
        for (int i = 0; i < 4; i++) {
            int newRow = lastHit.row + directions[i][0];
            int newCol = lastHit.col + directions[i][1];

            if (isValidMove(playerGrid, newRow, newCol)) {
                row = newRow;
                col = newCol;
                goto makeMove;
            }
        }
        lastHit.active = 0;
    }
    // hunting Mode
    do {
        row = rand() % GRID_SIZE;
        col = rand() % GRID_SIZE;
    } while (!isValidMove(playerGrid, row, col));

makeMove:
    if (playerGrid[row][col] == '~') {
        playerGrid[row][col] = 'O';
        printf("AI missed at %d, %d\n", row, col);
    } else {
        playerGrid[row][col] = 'X';
        printf("AI hit a ship at %d, %d!\n", row, col);
        lastHit.row = row;
        lastHit.col = col;
        lastHit.active = 1;
    }
}

/* this handles all the players inputs coordinates basically everything the ai could do but player controlled */
void playerMove(char aiGrid[GRID_SIZE][GRID_SIZE]) {
    int row, col;
    printf("Enter your attack coordinates (row and column): ");
    scanf("%d %d", &row, &col);

    if (isValidMove(aiGrid, row, col)) {
        if (aiGrid[row][col] == '~') {
            aiGrid[row][col] = 'O';
            printf("You missed!\n");
        } else {
            aiGrid[row][col] = 'X';
            printf("Hit!\n");
        }
    } else {
        printf("Invalid move. Try again.\n");
        playerMove(aiGrid);  // this is if the input is invalid it lets the player try again and input another set of coordinates
    }
}

// this checks win condition
int checkWin(char grid[GRID_SIZE][GRID_SIZE]) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] != '~' && grid[i][j] != 'X' && grid[i][j] != 'O') {
                return 0;
            }
        }
    }
    return 1;
}

/* main game loop, Initialize grids for both the player and AI.
	Randomly places ships.
	Loop until a player wins:
	1.	Display grids.
	2.	Player makes a move.
	3.	AI makes a move.
	4.	Check for a winner. */
int main() {
    char playerGrid[GRID_SIZE][GRID_SIZE];
    char aiGrid[GRID_SIZE][GRID_SIZE];

    initializeGrid(playerGrid);
    initializeGrid(aiGrid);

    srand(time(0));
    placeShips(playerGrid);
    printf("\n[DEBUG] Player Ship Placement:\n");
    displayGrid(playerGrid, 1);

    srand(time(0) + 1000);  // ensure a different seed for AI so that the placements are random for both player and AI
    placeShips(aiGrid);
    printf("\n[DEBUG] AI Ship Placement:\n");
    displayGrid(aiGrid, 1);

    printf("Welcome to Battleship!\n");

    while (1) {
        printf("\nYour Grid:\n");
        displayGrid(playerGrid, 1);
        printf("\nEnemy Grid:\n");
        displayGrid(aiGrid, 0);

        playerMove(aiGrid);
        if (checkWin(aiGrid)) {
            printf("Congratulations! You sank all enemy ships!\n");
            break;
        }

        aiAttack(playerGrid);
        if (checkWin(playerGrid)) {
            printf("Game Over! The AI sank all your ships!\n");
            break;
        }
    }

    return 0;
}
