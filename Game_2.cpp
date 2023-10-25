#include<iostream>
#include<fstream>
#include<thread>
#include<chrono>

using namespace std;
//Массив для поля
int field[100][100]; // -1 это мина, от 0 до 8 - открытая клетка с цифрами. 
// Поле игрока
int field_p[100][100]; // -1 это флаг, -2 это закрытая клетка и 0-8 это открытая с цифрами.
// Длина, ширина и количество мин.
const int N = 10, M = 10, K = 10; 

bool is_bot = false;
//Функция для вывода поля в консоль.
void print_field() {
	system("cls");
	cout << "  ";
	for (int i = 0; i < M; ++i) {
		cout << " " << i + 1;
		if (i + 1 < 10) cout << ' ';
	}
	cout << endl;
	for (int i = 0; i < N; i++) {
		cout << i + 1 << ' ';
		if (i + 1 < 10) cout << ' ';
		for (int j = 0; j < M; j++) {
			if (field_p[i][j] == -2) cout << "_  ";
			else if (field_p[i][j] == -1) cout << "f  ";
			else if (field_p[i][j] == -3) cout << "*  ";
			else cout << field_p[i][j] << "  ";
		}
		cout << endl;
	}
	cout << endl;
}
// Открытие нулевых ячеек. 
void dfs(int x, int y) {
	if (x < 0 || x >= N || y < 0 || y >= M || field_p[x][y] > - 2) return;
	field_p[x][y] = field[x][y];
	if (field[x][y] > 0) return;
	for (int i2 = x - 1; i2 <= x + 1; ++i2) {
		for (int j2 = y - 1; j2 <= y + 1; ++j2) {
			if(i2 != x, j2 != y) dfs(i2, j2);
		}
	}
}

//Функция для открытия клетки. true - не взорвался, false - взорвался.
bool open_cell(int x, int y) {
	if (field[x][y] == -1) return false;
	if (field[x][y] > 0) {
		field_p[x][y] = field[x][y];
		return true;
	}
	dfs(x, y);
}

bool is_win() {
	int opened = 0;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			if (field_p[i][j] >= 0) opened++;
		}
	}
	return (N * M - K == opened);
}

void end_game(bool is_win = false) {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			if (field_p[i][j] == -1) field[i][j] = -2;
			if (field[i][j] == -1) field_p[i][j] = -3;
		}
	}
	print_field();
	cout << "Vi " <<(is_win ? "viigraly" : "proigraly") << ". \n Chtoby nachaty novuyu igru, vvedite lubuyu stroku.";
	string s;
	cin >> s;
	is_bot = false;
}



void wait(int millesec) {
	this_thread::sleep_for(chrono::milliseconds(millesec));
}

void save_field() {
	ofstream fout("field.txt", ios_base::trunc);
	if (!fout.is_open()) {
		cout << "No file field.txt";
		exit(0);
	}
	fout << N << ' ' << M << ' ' << K << '\n';
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			fout << field_p[i][j] << ' ';
		}
		fout << '\n';
	}
	fout.close();
}

int main() {
	while (true) {
		//Заполнили поле нулями.
		srand(time(0));
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < M; j++) {
				field[i][j] = 0;
				field_p[i][j] = -2;
			}
		}
		//Сгенерировали мины на поле.
		for (int i = 0; i < K; ++i) {
			while (true) {
				int x = rand() % N;
				int y = rand() % M;
				if (field[x][y] != -1) {
					field[x][y] = -1;
					break;
				}
			}
		}
		//Количество мин около активированной клетки.
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < M; j++) {
				if (field[i][j] != -1) {
					field[i][j] = 0;
					// Координаты, соседствующие с i и j.
					for (int i2 = i - 1; i2 <= i + 1; ++i2) {
						for (int j2 = j - 1; j2 <= j + 1; ++j2) {
							//Если в соседней клетке есть мина, то значения i, j увеличивается.
							if (i2 >= 0 && i2 < N && j2 >= 0 && j2 < M &&
								field[i2][j2] == -1) ++field[i][j];
						}
					}
				}
			}
		}
		while (true) {
			print_field();
			cout << "Enter command: "; // Виды команд: \o х и у - открыть клетку, \f х у - это команда для того, чтобы поставить/убрать флаг.
			// \n - новая игра, \q - завершение программы.
			string comand;
			ifstream fin;
			if (is_bot) {
				save_field();
				system("Bot.exe");
				fin.open("comand.txt");
				if (!fin.is_open()) {
					cout << "No file comand.txt";
					return 0;
				}
				fin >> comand;
				cout << comand;
			} 
			else {
				cin >> comand;
			}
			if (comand == "\\o") { // Ввод координат для открытия поля.
				int x, y;
				if (is_bot) {
					fin >> x >> y;
					cout << x << ' ' << y;
					wait(3000);
				}
				else {
					cin >> x >> y;
				}
				--x; --y;
				if (x < 0 || x >= N || y < 0 || y >= M || field_p[x][y] >= 0) continue;
				if (!open_cell(x, y)) {
					end_game();
					break;
				}
				if (is_win()) {
					end_game(true);
					break;
				}
			}
			else if (comand == "\\f") { // Операции с флагом
				int x, y;
				if (is_bot) {
					fin >> x >> y;
					cout << x << ' ' << y;
					wait(3000);
				}
				else {
					cin >> x >> y;
				}
				--x; --y;
				if (x < 0 || x >= N || y < 0 || y >= M || field_p[x][y] >= 0) continue;
				if (field_p[x][y] == -1) field_p[x][y] = -2;
				else field_p[x][y] = -1;
			}
			// Новая игра.
			else if (comand == "\\n") {
				break;
			}
			else if (comand == "\\b") {
				is_bot = true;
				break;
			}
			// Прервать игру.
			else if (comand == "\\q") {
				return 0;
			}
		}
	}
}

/* Условия появления мин
for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			if (field[i][j] == -1) cout << '*' << ' ';
			else cout << field[i][j] << ' ';
		}
		cout << endl;
	}
*/