#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <mutex>  // Adicionado para usar mutex

using namespace std;

int num_rows;
int num_cols;

void clearScreen() {
#ifdef _WIN32
    // Para Windows
    system("cls");
#else
    // Para Unix/Linux
    system("clear");
#endif
}

struct pos_t {
    int i;
    int j;
    std::vector<std::vector<char>> labirinto;
};

pos_t load_maze(const char* file_name) {
    pos_t initial_pos;
    std::ifstream arquivo(file_name);
    num_rows = 0;
    num_cols = 0;

    if (arquivo.is_open()) {
        std::string primeiraLinha;
        if (std::getline(arquivo, primeiraLinha)) {
            std::istringstream iss(primeiraLinha);
            if (iss >> num_rows >> num_cols) {
                //std::cout << num_rows << " " << num_cols << std::endl;
            }
        }

        std::string linha;
        while (std::getline(arquivo, linha)) {

            std::vector<char> linhaLabirinto;
            for (char c : linha) {
                linhaLabirinto.push_back(c);
            }
            initial_pos.labirinto.push_back(linhaLabirinto);
        }
    }

    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            if (initial_pos.labirinto[i][j] == 'e') {
                initial_pos.i = i;
                initial_pos.j = j;
            }
        }
    }

    return initial_pos;
}

// Mutex para controlar a impressão
std::mutex printMutex;

void print(std::vector<std::vector<char>> maze) {
    printMutex.lock();  // Bloqueia o mutex antes de imprimir
    clearScreen();
    for (int i = 0; i < num_rows; ++i) {
        for (int j = 0; j < num_cols; ++j) {
            cout << maze[i][j];
        }
        cout << endl;
    }
    printMutex.unlock();  // Desbloqueia o mutex após a impressão
    this_thread::sleep_for(chrono::milliseconds(50));
}

bool findPath(vector<vector<char>>& maze, int row, int col) {
    int numRows = maze.size();
    int numCols = maze[0].size();

    // Verificar se estamos fora dos limites da matriz.
    if (row < 0 || row >= numRows || col < 0 || col >= numCols) {
        return false;
    }
    // Verificar se atingimos o ponto de chegada.
    if (maze[row][col] == 's') {
        return true;
    }

    // Verificar se a célula atual é uma parede, já foi visitada ou não faz parte do caminho correto.
    if (maze[row][col] == '#' || maze[row][col] == '-' || maze[row][col] == ' ') {
        return false;
    }

    // Marcar a célula como parte do caminho temporariamente.
    char originalCell = maze[row][col];
    maze[row][col] = '-';
    print(maze);

    // Vetor para armazenar as threads.
    std::vector<std::thread> threads;

    // Função anônima para explorar uma direção em uma nova thread.
    auto exploreDirection = [&maze, row, col](int dr, int dc) {
        if (findPath(maze, row + dr, col + dc)) {
            return true;
        }
        return false;
    };

    // Criar threads para explorar as quatro direções possíveis.
    threads.emplace_back(exploreDirection, 1, 0);
    threads.emplace_back(exploreDirection, -1, 0);
    threads.emplace_back(exploreDirection, 0, 1);
    threads.emplace_back(exploreDirection, 0, -1);

    // Aguardar que todas as threads terminem.
    for (auto& thread : threads) {
        thread.join();
    }

    // Restaurar a célula original, pois não faz parte do caminho correto.
    maze[row][col] = originalCell;
    return false;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Uso: " << argv[0] << " <arquivo de labirinto>" << endl;
        return 1;
    }

    pos_t maze = load_maze(argv[1]);
    int startRow = maze.i;
    int startCol = maze.j;
    cout << startRow << "  " << startCol << endl;
    cout << num_rows << "  " << num_cols << endl;

    // Encontrar o caminho correto.
    findPath(maze.labirinto, startRow, startCol);

    return 0;
}
