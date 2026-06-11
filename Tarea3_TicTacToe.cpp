//Compilar con: g++ Tarea3_TicTacToe.cpp -o TicTacToe

// tic-tac-toe 
#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>
#include <array>
#include <cstdlib>   //usar rand, srand
#include <ctime>
#include <algorithm>

using namespace std;

// thrown when set() encounters an illegal input
struct InputException { };

//Estructura para representar movimientos
struct Move
{
    int x;
    int y;
};

//Representa el Estado del Juego
class State
{
    public:

        enum Players { P1 = -1, P2 = 1 }; //Jugadores (P1 = X ; P2 = O)
        static const array<char, 3> DISP; //Simbolos del tablero ([0] = x; [1] = - ; [2] = O)

        // initialize empty board
        // P1 to move
        State(int rows = 3, int cols = 3, int K = 3)
        {
            this->rows = rows;
            this->cols = cols;
            this->K = K;

            sq = vector<vector<signed char>>(rows, vector<signed char>(cols, 0));
            to_move = P1;   //Comienza el P1
            filled = 0;
        }

        // return true if board is full
        bool full() const
        {
            return filled >= rows * cols;
        }
  
        // initialize state from string (P1 to move)
        // throw InputExceptiuon when you encounter an error in s
        void set(const string & s)
        {
            // create input stream from string, now you can use >>, etc.
            // like for cin
            istringstream is(s);
            char c;

            to_move = P1;
            filled = 0;

            //Recorrer Tablero
            for (int y = 0; y < rows; ++y) 
            {
                for (int x = 0; x < cols; ++x) 
                {
                    is >> c;

                    if (!is) { throw InputException(); }    //Error si faltan caracteres

                    if (c == DISP[1 + P1]) { sq[y][x] = P1; ++filled; }   //si es 'o'
                    else if (c == DISP[1 + P2]) { sq[y][x] = P2; ++filled; }  //si es `x`
                    else { sq[y][x] = 0; }  //si est vacio
                }
            }

            //Verificar caracteres extras
            is >> c;
            if (is) { throw InputException(); }
        }
  
        //Imprimir Tablero
        // print state to cout
        // format:
        //
        //  xox
        //  o-x
        //  xxo
        //  x (8)
        //
        // last line: player to move, number of filled squares
        // followed by new-line
        void print() const
        {
            cout << endl;

            //Imprimir coordenadas del tablero
            cout << "  ";

            for(int x = 0; x < cols; x++)
            {
                cout << x << " ";
            }

            cout << endl;


            for (int y = 0; y < rows; ++y) 
            {
                cout << y << " ";

                for (int x = 0; x < cols; ++x) 
                {
                    cout << DISP[sq[y][x] + 1] << " ";
                }

                cout << endl;
            }
            cout << endl;

            // print player to move and #filled squares
            //cout << DISP[to_move + 1] << " (" << filled << ")" << endl;

            //Mostrar jugador actual
            cout << "Turno: " << DISP[to_move + 1] << endl;

            //Mostrar casillas ocupadas
            cout << "Casillas ocupadas: " << filled << endl;
        }
  

        // make move (x, y) for player to_move
        // and return true iff move is legal
        // pre-condition: x, y within range
        bool make_move(int x, int y)
        {
            //Verificar rango
            assert(x >= 0 && x < cols && y >= 0 && y < rows);

            auto &c = sq[y][x];     //Referencia a casilla

            if (c) 
            {
                return false; // casilla ya ocupada
            }
            
            c = to_move;            //Colocar pieza
            to_move = - to_move;    //Cambiar de jugador
            ++filled;

            return true;
        }

        // return player to move (retorna jugador actual)
        int get_to_move() const
        {
            return to_move;
        }

        //Getter del tablero
        const vector<vector<signed char>>& get_board() const
        {
            return sq;
        }

        int get_rows() const
        {
            return rows;
        }

        int get_cols() const
        {
            return cols;
        }

        int get_K() const
        {
            return K;
        }
  
    private:
        int to_move;    //Jugador actual

        // squares (2d array)
        // stores P1,0,P2 values
        // bounds are checked in debug mode
        // and sq[y][x] = 0 works
        vector<vector<signed char>> sq;

        int rows;   //Filas del tablero (M)
        int cols;   //Columnas del tablero (N)
        int K;      //Nº de Figuras a alinear (K)

        int filled;        //Numero de casillas ocupadas    
};

// how pieces are displayed ...
// P1, empty, P2
const array<char, 3> State::DISP = {{ 'x', '-', 'o' }};

//Revisar ganador de la partida
int check_winner(const State& state)
{
    auto board = state.get_board();
    int rows = state.get_rows();
    int cols = state.get_cols();
    int K = state.get_K();

    // Direcciones: horizontal, vertical, diagonal principal, diagonal secundaria
    int dx[] = {1, 0, 1, 1};
    int dy[] = {0, 1, 1, -1};

    for(int y = 0; y < rows; y++)
    {
        for(int x = 0; x < cols; x++)
        {
            int player = board[y][x];

            if(player == 0){ continue; }

            for(int dir = 0; dir < 4; dir++)
            {
                int count = 1;

                for(int step = 1; step < K; step++)
                {
                    int nx = x + dx[dir] * step;
                    int ny = y + dy[dir] * step;

                    //Salir si se sale del tablero
                    if(nx < 0 || nx >= cols ||
                       ny < 0 || ny >= rows)
                    {
                        break;
                    }

                    if(board[ny][nx] == player) { count++; }
                    else{ break; }

                    if(count >= K) { return player; }

                }
            }
        }

    }

    return 0;
}

//Comprobar heuristica
int heuristic(const State& state)
{
    auto board = state.get_board();

    int rows = state.get_rows();
    int cols = state.get_cols();

    int bestX = 0;
    int bestO = 0;

    // Direcciones: horizontal, vertical, diagonal principal, diagonal secundaria
    int dx[] = {1, 0, 1, 1};
    int dy[] = {0, 1, 1, -1};

    for(int y = 0; y < rows; y++)
    {
        for(int x = 0; x < cols; x++)
        {
            int player = board[y][x];

            if(player == 0){ continue; }

            for(int dir = 0; dir < 4; dir++)
            {
                int count = 1;

                int nx = x + dx[dir];
                int ny = y + dy[dir];

                while (nx >= 0 && nx < cols &&
                       ny >= 0 && ny < rows &&
                       board[ny][nx] == player)
                {
                    count++;

                    nx += dx[dir];
                    ny += dy[dir];
                }
                
                if(player == State::P1)
                {
                    bestX = max(bestX, count);
                }
                else
                {
                    bestO = max(bestO, count);
                }
            }
        }
    }

    //Retorna un valor negativo si favorece a X y uno positivo si favorece a O
    return bestO - bestX;
}



//Obtener movimientos validos
vector<Move> get_valid_moves(const State& state)
{
    vector<Move> valid_moves;
    auto board = state.get_board();

    //Buscar espacios vacios
    for(int y = 0; y < state.get_rows(); y++)
    {
        for(int x = 0; x < state.get_cols(); x++)
        {
            if(board[y][x] == 0)
            {
                valid_moves.push_back({x, y});
            }
        }
    }

    return valid_moves;
}

//Crear nuevo estado aplicando movimiento
State apply_move(State state, Move move)
{
    state.make_move(move.x, move.y);
    return state;
}

////////////////////////////////// MiniMax ////////////////////////////////////
int MiniMax(State state, int depth)
{
    int winner = check_winner(state);

    //Comprobar ganador
    if(winner == State::P1){ return -10000; }    //Gana X
    if(winner == State::P2){ return 10000; }    //Gana O
    if(state.full()){ return 0; }    //Empate

    //comprobar profundidad
    if(depth == 0){ return heuristic(state); }

    vector<Move> valid_moves = get_valid_moves(state);

    //Turno de X (Minimizador)
    if(state.get_to_move() == State::P1)
    {
        int best = 100000;

        for(auto move : valid_moves)
        {
            State next = apply_move(state, move);
            int score = MiniMax(next, depth - 1);

            best = min(best, score);
        }

        return best;
    }
    else    //Turno de O (maximizador)
    {
        int best = -100000;

        for(auto move : valid_moves)
        {
            State next = apply_move(state, move);
            int score = MiniMax(next, depth - 1);

            best = max(best, score);
        }

        return best;
    }

}

////////////////////////////////// NegaMax ////////////////////////////////////
int NegaMax(State state, int depth, int color)
{
    int winner = check_winner(state);

    //Estados terminales
    if(winner == State::P1){ return color * (-10000); }    //Si le toca jugar a O, entocnes gana X acaba de ganar
    if(winner == State::P2){ return color * (10000); }    //Si le toca jugar a X, entocnes gana O acaba de ganar
    if(state.full()){ return 0; }    //Empate

    //comprobar profundidad
    if(depth == 0){ return color * heuristic(state); }

    int best = -100000;

    vector<Move> valid_moves = get_valid_moves(state);

    for(auto move : valid_moves)
    {
        State next = apply_move(state, move);
        int score = -NegaMax(next, depth - 1, -color);

        best = max(best, score);
    }

    return best;
}


////////////////////////////////// AlphaBeta ////////////////////////////////////
int AlphaBeta(State state, int depth, int alpha, int beta)
{
    int winner = check_winner(state);

    //Comprobar ganador
    if(winner == State::P1){ return -10000; }    //Gana X
    if(winner == State::P2){ return 10000; }    //Gana O
    if(state.full()){ return 0; }    //Empate

    //comprobar profundidad
    if(depth == 0){ return heuristic(state); }

    vector<Move> valid_moves = get_valid_moves(state);

    //Turno de X (Minimizador)
    if(state.get_to_move() == State::P1)
    {
        int best = 100000;

        for(auto move : valid_moves)
        {
            State next = apply_move(state, move);
            int score = AlphaBeta(next, depth - 1, alpha, beta);

            best = min(best, score);

            beta = min(beta, best);

            if(beta <= alpha) { break; }
        }

        return best;
    }
    else    //Turno de O (maximizador)
    {
        int best = -100000;

        for(auto move : valid_moves)
        {
            State next = apply_move(state, move);
            int score = AlphaBeta(next, depth - 1, alpha, beta);

            best = max(best, score);

            alpha = max(alpha, best);

            if(beta <= alpha) { break; }
        }

        return best;
    }
}


//////////////////////////////// Tipos de Jugadores ////////////////////////
//Jugador Aleatorio
Move random_Player(const State& state)
{
    auto board = state.get_board();

    vector<Move> valid_moves;   //movimientos validos a realizar

    //Buscar espacios vacios
    for(int y = 0; y < state.get_rows(); y++)
    {
        for(int x = 0; x < state.get_cols(); x++)
        {
            if(board[y][x] == 0)
            {
                valid_moves.push_back({x, y});
            }
        }
    }

    //Escoger un movimieto de los validos
    int randomMov = rand() % valid_moves.size();

    return valid_moves[randomMov];
}


//Jugador MiniMax
Move minimax_Player(const State& state, int depth)
{
    vector<Move> valid_moves = get_valid_moves(state);
    Move bestMove = valid_moves[0];

    //X minimiza
    if(state.get_to_move() == State::P1)
    {
        int bestScore = 100000;

        for(auto move : valid_moves)
        {
            State next = apply_move(state, move);
            int score = MiniMax(next, depth - 1);

            if(score < bestScore) 
            { 
                bestScore = score; 
                bestMove = move;
            }
        }

    }
    else    //O maximiza
    {
        int bestScore = -100000;

        for(auto move : valid_moves)
        {
            State next = apply_move(state, move);
            int score = MiniMax(next, depth - 1);

            if(score > bestScore) 
            { 
                bestScore = score; 
                bestMove = move;
            }        
        }

    }

    return bestMove;
}


//Jugador Negamax
Move negamax_Player(const State& state, int depth)
{
    vector<Move> valid_moves = get_valid_moves(state);
    Move bestMove = valid_moves[0];

    int bestScore = -100000;
    int color;

    if(state.get_to_move() == State::P1) { color = -1; }
    else { color = 1; }

    for(auto move : valid_moves)
    {
        State next = apply_move(state, move);
        int score = -NegaMax(next, depth - 1, -color);

            if(score > bestScore) 
            { 
                bestScore = score; 
                bestMove = move;
            }
    }

    return bestMove;
}


//Jugador AlphaBeta
Move alphabeta_Player(const State& state, int depth)
{
    vector<Move> valid_moves = get_valid_moves(state);
    Move bestMove = valid_moves[0];

    //X minimiza
    if(state.get_to_move() == State::P1)
    {
        int bestScore = 100000;

        for(auto move : valid_moves)
        {
            State next = apply_move(state, move);
            int score = AlphaBeta(next, depth - 1, -100000, 100000);

            if(score < bestScore) 
            { 
                bestScore = score; 
                bestMove = move;
            }
        }

    }
    else    //O maximiza
    {
        int bestScore = -100000;

        for(auto move : valid_moves)
        {
            State next = apply_move(state, move);
            int score = AlphaBeta(next, depth - 1, -100000, 100000);

            if(score > bestScore) 
            { 
                bestScore = score; 
                bestMove = move;
            }        
        }

    }

    return bestMove;
}


//Jugador Humano
Move human_Player(const State& state)
{
    auto board = state.get_board();
    int x, y;

    while(true)
    {
        cout << "Ingrese coordenadas (X Y): ";
        cin >> x >> y;

        //Verificar rango
        bool valid_range = 
            x >= 0 && x < state.get_cols() &&
            y >= 0 && y < state.get_rows();

        if(valid_range)
        {
            //Verificar casilla vacia
            bool empty_cell = board[y][x] == 0;

            if(empty_cell)
            {
                return {x, y};
            }
        }

        cout << "Movimiento invalido, ingrese otro." << endl;
    }
}



int main(int argc, char* argv[])
{
    srand(time(0));     //Iniciar random

    //Verificar ingreso de argumentos
    if(argc!= 7)
    {
        cout << "Uso: " << argv[0] << "M N K H Agente1 Agente2" << endl;

        cout << endl;
        cout << "Agentes disponibles:" << endl;
        cout << "0 = Humano" << endl;
        cout << "1 = Aleatorio" << endl;
        cout << "2 = MiniMax" << endl;
        cout << "3 = NegaMax" << endl;
        cout << "4 = AlphaBeta" << endl;

        return 1;
    }

    //Leer parametros de argv[]
    int M = atoi(argv[1]);     // Filas del tablero
    int N = atoi(argv[2]);     // Columnas del tablero
    int K = atoi(argv[3]);     // Figuras consecutivas para ganar
    int H = atoi(argv[4]);     // Profundidad de búsqueda

    int p1_type = atoi(argv[5]);   // Agente jugador X
    int p2_type = atoi(argv[6]);   // Agente jugador O

    //Validar parametros ingresados
    if(M <= 0 || N <= 0 || K <= 0 || H <= 0)
    {
        cout << "Error: M, N, K y H deben ser mayores que 0." << endl;
        return 1;
    }

    // K no puede ser mayor que ambas dimensiones
    if(K > max(M, N))
    {
        cout << "Error: K es demasiado grande para el tablero." << endl;
        return 1;
    }

    // Validar agentes
    if(p1_type < 0 || p1_type > 4 || p2_type < 0 || p2_type > 4)
    {
        cout << "Error: agente invalido." << endl;
        return 1;
    }

    //Configuracion del tablero
    cout << "==================================" << endl;
    cout << "      TIC-TAC-TOE M,N,K" << endl;
    cout << "==================================" << endl;

    cout << "Tablero: " << M << " x " << N << endl;
    cout << "K: " << K << endl;
    cout << "Profundidad H: " << H << endl;

    cout << "Jugador X: " << p1_type << endl;
    cout << "Jugador O: " << p2_type << endl;

    cout << "==================================" << endl;


    //Crear estado inicial del juego
    State state(M,N,K);
    int turn = 1;   //Turnos del juego

    
    while (true)
    {
        //Separador visual
        cout << "\n========== TURNO " << turn << " ==========\n";

        state.print();

        //comprobar ganador
        int winner = check_winner(state);

        //Gana jugador 1
        if(winner == State::P1)
        {
            cout << endl;
            cout << "¡¡¡ Gana X !!!!" << endl;
            break;
        }

        //Gana jugador 2
        if(winner == State::P2)
        {
            cout << endl;
            cout << "¡¡¡ Gana O !!!!" << endl;
            break;
        }

        //Empate
        if(state.full())
        {
            cout << endl;
            cout << "Se ha Empatado" << endl;
            break;
        }

        //Obtener movimiento realizado
        Move move;
        int currentAgent;

        //Turno jugador X
        if(state.get_to_move() == State::P1)
        {
            currentAgent = p1_type;
        }
        else    //Turno jugador O
        {
            currentAgent = p2_type;
        }

        //Ejecutar agente seleccionado
        switch(currentAgent)
        {
            case 0: move = human_Player(state);     //Humano
                    break;

            case 1: move = random_Player(state);     //Aleatorio
                    break;

            case 2: move = minimax_Player(state, H);     //Minimax
                    break;

            case 3: move = negamax_Player(state, H);     //Negamax
                    break;

            case 4: move = alphabeta_Player(state, H);     //Alphabeta
                    break;

            default: cout << "Error: Se ha seleccionado un agente invalido." << endl;
                     break;
        }

        //Aplicar movimiento
        state.make_move(move.x, move.y);    //Realizar movimiento
        turn++;
    }
    
    return 0;

}