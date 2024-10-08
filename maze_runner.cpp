#include <stdio.h>
#include <stack>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <queue>
#include <utility>
#include <thread>
#include <chrono>
#include <cstdlib>

// Representação do labirinto
using Maze = std::vector<std::vector<char>>;

// Estrutura para representar uma posição no labirinto
struct Position {
    int row;
    int col;
};

// Variáveis globais
Maze maze;
int num_rows;
int num_cols;
std::stack<Position> valid_positions;

// Função para carregar o labirinto de um arquivo
Position load_maze(const std::string& file_name) {
    Position initial_pos = {-1, -1};  // Inicializando com valores inválidos
    std::ifstream arquivo(file_name);
    num_rows = 0;
    num_cols = 0;

    if (arquivo.is_open()) {
        std::string primeiraLinha;
        if (std::getline(arquivo, primeiraLinha)) {
            std::istringstream iss(primeiraLinha);
            if (iss >> num_rows >> num_cols) {
                maze.resize(num_rows);
            }
        }

        std::string linha;
        int row = 0;
        while (std::getline(arquivo, linha) && row < num_rows) {
            std::vector<char> linhaLabirinto;
            for (char c : linha) {
                linhaLabirinto.push_back(c);
            }
            maze[row] = linhaLabirinto;
            row++;
        }

        // Encontrar a posição inicial (marcada por 'e')
        for (int i = 0; i < num_rows; i++) {
            for (int j = 0; j < num_cols; j++) {
                if (maze[i][j] == 'e') {
                    initial_pos.row = i;
                    initial_pos.col = j;
                }
            }
        }
    }

    return initial_pos;
}

// Função para imprimir o labirinto
void print_maze() {
    for (const auto& row : maze) {
        for (char cell : row) {
            std::cout << cell;
        }
        std::cout << '\n'; // Quebra de linha após cada linha do labirinto
    }
}

// Função para verificar se uma posição é válida
bool is_valid_position(int row, int col) {
    return (row >= 0 && row < num_rows && col >= 0 && col < num_cols && (maze[row][col] == 'x' || maze[row][col] == 's'));
}

// Função principal para navegar pelo labirinto
bool walk(Position pos) {
    // Verificar se a posição atual é a saída
    if (maze[pos.row][pos.col] == 's') {
        return true;
    }

    // Marcar posição atual como visitada
    maze[pos.row][pos.col] = '.';
    print_maze();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Verificar posições adjacentes e adicionar à pilha se forem válidas
    std::vector<Position> directions = {
        {pos.row - 1, pos.col},  // Cima
        {pos.row + 1, pos.col},  // Baixo
        {pos.row, pos.col - 1},  // Esquerda
        {pos.row, pos.col + 1}   // Direita
    };

    for (const Position& next : directions) {
        if (is_valid_position(next.row, next.col)) {
            valid_positions.push(next);
        }
    }

    // Processar posições válidas
    while (!valid_positions.empty()) {
        Position next_pos = valid_positions.top();
        valid_positions.pop();
        if (walk(next_pos)) {
            return true;
        }
    }

    return false; // Não encontrou a saída
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_labirinto>" << std::endl;
        return 1;
    }

    Position initial_pos = load_maze(argv[1]);
    if (initial_pos.row == -1 || initial_pos.col == -1) {
        std::cerr << "Posição inicial não encontrada no labirinto." << std::endl;
        return 1;
    }

    bool exit_found = walk(initial_pos);

    if (exit_found) {
        std::cout << "Saída encontrada!" << std::endl;
    } else {
        std::cout << "Não foi possível encontrar a saída." << std::endl;
    }

    return 0;
}
