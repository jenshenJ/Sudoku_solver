#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <stack>
#include <exception>


#ifdef _WIN32
#include <windows.h>

void sleep(unsigned milliseconds)
{
	Sleep(milliseconds);
}
#else
#include <unistd.h>

void sleep(unsigned milliseconds)
{
	usleep(milliseconds * 1000); // takes microseconds
}
#endif

using namespace std;

void choose_mod();


bool operator== (pair<int, int> lhs, pair<int, int> rhs) {
	return lhs.first == rhs.first && lhs.second == rhs.second;
}
int geti(istream& in, string error_code) {
	int x;
	in >> x;
	while (in.fail() || in.peek() != '\n' && in.peek() != ' ') {
		in.clear();
		in.ignore(32767, '\n');
		cout << error_code << '\n';
		in >> x;
	}
	return x;
}

struct Move {
	size_t fild_size;
	size_t x;
	size_t y;
	size_t old_value;
	size_t new_value;
};


bool operator>> (istream& in, Move& move) {
	in >> move.x;
	move.x--;
	in >> move.y;
	move.y--;
	in >> move.new_value;

	string error;
	bool is_error = false;
	if (move.x < 0 || move.x >= move.fild_size) {
		is_error = true;
		error += "Incorrect x. ";
	}

	if (move.y < 0 || move.y >= move.fild_size) {
		is_error = true;
		error += "Incorrect y. ";
	}

	if (move.new_value < 0 || move.new_value > move.fild_size) {
		is_error = true;
		error += "Incorrect value. ";
	}

	if (is_error) {
		cout << error << " Please, make a vaild move: \n";
		return false;
	}

	return true;
}

ostream& operator<< (ostream& out, Move& move) {
	out << "(" << move.x  + 1 << ", " << move.y + 1 << ") " << move.new_value;
	return out;
}

bool operator== (const Move& lhs, const Move& rhs) {
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

class PartSqr {
public:
	PartSqr() {}
	PartSqr(size_t s) {
		sqr.assign(s, 0);
		size = s;
	}
	void SetSize(int s) {
		sqr.assign(s, 0);
		size = s;
	}
	void SetValue(size_t pos, int val) {
		sqr[pos] = val;
	}

	bool is_vaild() {
		for (int c : sqr) {
			if (c != 0 && count(begin(sqr), end(sqr), c) > 1) {
				return false;
			}
		}
		return true;
	}
	
private:
	vector<int> sqr;
	size_t size;
};


class Fild {
public:
	Fild() {}
	void SetSize(size_t s) {
		fild.assign(s, vector<int>(s));
		sqrs.assign(s, s);
		for (auto sq : sqrs) {
			sq.SetSize(size);
		}
		variants.assign(s, vector<vector<int>>(s));

		size = s;

	}

	Fild(size_t s) {
		fild.assign(s, vector<int>(s));
		size = s;
		width = GetWidth();
		sqrs.assign(s, s);
		variants.assign(s, vector<vector<int>>(s));
		for (auto sq : sqrs) {
			sq.SetSize(size);
		}
		
	}
	size_t GetSize() {
		return size;
	}
	
	void SetValue(size_t y, size_t x, size_t value) {
		fild[y][x] = value;
	}

	bool is_valid() {
		for (auto s : sqrs) {
			if (!s.is_vaild())
				return false;
		}

		for (auto row : fild) {
			for (auto c : row) {
				if (c != 0 && count(begin(row), end(row), c) > 1)
					return false;
			}
		}	

		for (int j = 0; j < size; j++)
			for (int i = 0; i < size; i++) {
				if (fild[i][j] == 0)
					continue;
				for (int k = i + 1; k < size; k++)
					if (fild[i][j] == fild[k][j])
						return false;
			}
		
		return true;
	}

	int GetPartSize() {
		return (int)sqrt(size);
	}

	bool is_solved() {
		int counter = 0;
		for (const auto& line : fild) {
			counter += count(begin(line), end(line), 0);
		}
		return(!counter && is_valid());
	}
	
	void WriteFile(ostream& out) {
		out << size << '\n';
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				out << fild[i][j];
				if (j != size - 1)
					out << ' ';
			}
			out << '\n';
		}
	}
	
	int GetValue(size_t y, size_t x) {
		return fild[y][x];
	}

	size_t GetWidth() {
		size_t s = size;
		size_t count = 1;
		while (s /= 10)
			count++;
		return ++count;
	}

	string GetDivider() {
		stringstream stream;
		for (int i = 0; i < size + GetPartSize() - 1; i++) {
			stream.width(GetWidth());
			stream << "*";
		}
		string s;
		getline(stream, s);
		return s;
	}

	int CountVariants(int i, int j) {
		int old_value = fild[i][j];
		variants[i][j].clear();
		for (int k = 1; k <= size; k++) {
			fild[i][j] = k;
			Update();
			if (is_valid()) {
				variants[i][j].push_back(k);
			}
		}
		fild[i][j] = old_value;
		Update();
		return variants[i][j].size();
	}

	bool is_full() {
		int c = 0;
		for (const auto& row : fild) {
			c += count(begin(row), end(row), 0);
		}
	}
	void Solve() {
		Fild Copy;
		Copy = *this;
		int step = 1;
		int count = 1;
		while (step <= size + 1 && !is_solved()) {
			if (count == 0) {
				step++;
			}
			else {
				count = 0;
				step = 1;
			}
			for (int i = 0; i < size; i++) {
				for (int j = 0; j < size; j++) {
					if (fild[i][j] == 0) {
						if (CountVariants(i, j) == 0) {
							*this = Copy;
							return;
						}
						if (variants[i][j].size() <= step) {
							count++;
							CountVariants(i, j);
							for (const auto& number : variants[i][j]) {
								int old_value = fild[i][j];
								fild[i][j] = number;
								Update();
								if (!is_valid()) {
									fild[i][j] = old_value;
									system("cls");
									
									Update();
								}
								if (variants[i][j].size() > 1) {
									Solve();
								}
								if (is_solved()) {
									return;
								}
							}
							
						}
					}
				}
			}
		}
			
	}


	void Update() {
		for (int sqr_count = 0; sqr_count < size; sqr_count++) {
			for (int i = 0; i < GetPartSize(); i++) {
				for (int j = 0; j < GetPartSize(); j++) {
					sqrs[sqr_count].SetValue(i * GetPartSize() + j, fild[sqr_count / GetPartSize() * GetPartSize() + i][sqr_count % GetPartSize() * GetPartSize() + j]);
				}
			}
		}
	}


private:
	size_t size, width;
	size_t part_size;
	vector<PartSqr> sqrs;
	vector<vector<int>> fild;
	vector<vector<vector<int>>> variants;
};

istream& operator>> (istream& in, Fild& fild) {
	try {
		for (int i = 0; i < fild.GetSize(); i++) {
			for (int j = 0; j < fild.GetSize(); j++) {
				int val;
				in >> val;
				if (val < 0 || val > fild.GetSize())
					throw runtime_error("Numbers are out of range, ");
				fild.SetValue(i, j, val);
			}
		}
		fild.Update();
		if (!fild.is_valid())
			throw runtime_error("Fild is invalid, ");
		return in;
	}
	catch (runtime_error ex) {
		system("cls");
		cout << ex.what() << "enter the correct fild: \n";
		in.clear();
		in >> fild;
	}
}

ostream& operator<< (ostream& out, Fild fild) {
		
	for (int i = 0; i < fild.GetSize(); i++) {
		
		for (int j = 0; j < fild.GetSize(); j++) {
			out.width(fild.GetWidth());
			if (fild.GetValue(i, j) == 0)
				out << ' ';
			else
				out << fild.GetValue(i, j);
			if (j != 0 && j + 1 != fild.GetSize() && (j + 1) * (j + 1) % fild.GetSize() == 0) {
				out.width(fild.GetWidth());
				out << " *";
			}
		}
		
		out << "\n";
		if (i != 0 && i + 1 != fild.GetSize() && (i + 1) * (i + 1) % fild.GetSize() == 0)
			out << fild.GetDivider() << '\n';
	}
	return out;
}


class Game {
public:
	Game() {
	
	}

	bool CheckFixed(Move move) {
		if (count(begin(FixedCeils), end(FixedCeils), move))
			return true;
	}

	void SaveInFile(string path) {
		ofstream out(path, ios_base::trunc);
		GameFild.WriteFile(out);

		out << FixedCeils.size() << '\n';
		for (int i = 0; i < FixedCeils.size(); i++) {
			out << FixedCeils[i].x + 1<< ' ' << FixedCeils[i].y + 1 << ' ' << FixedCeils[i].new_value << '\n';
		}

		out << Moves.size() << '\n';
		for(const auto& move : Moves){
			out << move.x + 1 << ' ' << move.y + 1<< ' ' << move.new_value << ' ' << move.old_value << '\n';
		}
	}

	void LoadFromFile(string path) {
		ifstream in(path);
		size_t size;
		in >> size;
		GameFild.SetSize(size);
		GameFild.Update();
		in >> GameFild;
		in >> size;
		FixedCeils.resize(size);
		for (int i = 0; i < size; i++) {
			FixedCeils[i].fild_size = GameFild.GetSize();
			
			in >> FixedCeils[i];
		}
		in >> size;
		Moves.resize(size);
		for (int i = 0; i < size; i++) {
			Moves[i].fild_size = GameFild.GetSize();
			in >> Moves[i];
			in >> Moves[i].old_value;
		}
	}

	void MakingMoves() {
		
		while (!GameFild.is_solved()) {
			system("cls");
			cout << GameFild << '\n';
			for (auto& m : Moves) {
				cout << m << '\n';
			}
			cout << "make your move (x, y, val) -q to choose mod, -b to decline the last move, -s to save game in file, -ex to exit\n";
			string command;
			getline(cin, command);
			if (command == "-q") {
				choose_mod();
			}
			else if (command == "-ex") {
				exit(0);
			}
			else if (command == "-s") {
				SaveInFile("iofile.txt");
				cout << "Saved\n";
				sleep(500);
			}
			else if (command == "-b") {
				if (!Moves.empty()) {
					Move move;
					move = Moves[Moves.size() - 1];
					Moves.pop_back();
					GameFild.SetValue(move.y, move.x, move.old_value);
					GameFild.Update();
				}
				else {
					cout << "You don't make a move yet\n";
					sleep(500);
				}
			}

			else if(!command.empty()) {
				Move move;
				stringstream in(command);
				move.fild_size = GameFild.GetSize();
				while (!(in >> move)) {
					getline(cin, command);
					in.clear();
					in.str(command);
				}

				if (CheckFixed(move)) {
					cout << "You can't change this value\n";
					sleep(500);
					continue;
				}
				move.old_value = GameFild.GetValue(move.y, move.x);
				GameFild.SetValue(move.y, move.x, move.new_value);
				GameFild.Update();
				if (GameFild.is_valid()) {
					Moves.push_back(move);
				}
				else {
					cout << "This move is invalid\n";
					GameFild.SetValue(move.y, move.x, move.old_value);
					GameFild.Update();
					sleep(500);
				}
			}

		}

		if (GameFild.is_solved())
			cout << "You solve the puzzle \n";
	
	}
	void ChooseMod() {
		system("cls");
		cout << "Choose the mod of game: " << '\n' <<
			"1.Fill the gamefild in console" << '\n' <<
			"2.Load last fild from file" << '\n';

		int chooser;
		do {
			cout << "Choose the option(1 / 2)" << '\n';
			chooser = geti(cin, "Choose the option (1 / 2)");
		} while (chooser != 1 && chooser != 2);

		switch (chooser) {
		case 1:
			system("cls");
			int s;
			do {
				cout << "Enter the size of game (Full square integer): " << '\n';
				s = geti(cin, "Enter the size of game (Full square integer): ");
			} while (s != pow((int)sqrt(s), 2));
			GameFild.SetSize(s);
			system("cls");
			cout << "Enter the fild (0 if ceil is empty): \n";
			cin >> GameFild;
			system("cls");
			for (size_t i = 0; i < GameFild.GetSize(); i++)
				for (size_t j = 0; j < GameFild.GetSize(); j++)
					if (GameFild.GetValue(i, j)) {
						Move move;
						move.x = j;
						move.y = i;
						move.new_value = 0;
						FixedCeils.push_back(move);
					}
			MakingMoves();
			break;
		case 2:
			LoadFromFile("iofile.txt");
			MakingMoves();
			break;
		}
	}

	void ChooseSolveMod() {
		system("cls");
		cout << "Choose the mod of solving: " << '\n' <<
			"1.Fill the tthe fild to solve in console" << '\n' <<
			"2.Load last fild from file" << '\n';

		int chooser;
		do {
			cout << "Choose the option(1 / 2)" << '\n';
			chooser = geti(cin, "Choose the option (1 / 2)");
		} while (chooser != 1 && chooser != 2);

		switch (chooser) {
		case 1:
			system("cls");
			int s;
			do {
				cout << "Enter the size of game (Full square integer): " << '\n';
				s = geti(cin, "Enter the size of game (Full square integer): ");
			} while (s != pow((int)sqrt(s), 2));
			GameFild.SetSize(s);
			system("cls");
			cout << "Enter the fild (0 if ceil is empty): \n";
			cin >> GameFild;
			system("cls");
			GameFild.Solve();
			break;
		case 2:
			LoadFromFile("iofile.txt");
			cout << GameFild << "\n\n";
			GameFild.Solve();
			cout << GameFild << '\n';
			break;
		}
	}

private: 
	Fild GameFild;
	vector<Move> FixedCeils;
	vector<Move> Moves;
	size_t FixedCount = 0;
};


void choose_mod() {
	while (1) {
		cout << "Choose the option: " << '\n' <<
			"1.Play game" << '\n' <<
			"2.Autosolving sudoku" << '\n';

		int chooser = 0;
		do {
			cout << "Choose the option(1 / 2)" << '\n';
			chooser = geti(cin, "Choose the option (1 / 2)");
		} while (chooser != 1 && chooser != 2);

		switch (chooser) {
		case 1:
		{
			Game game;
			game.ChooseMod();
		}

		break;
		case 2:
			Game game;
			game.ChooseSolveMod();
			break;
		}
	}
}

int main() {
	try {
		choose_mod();
	}
	catch (exception ex) {
		system("cls");
		choose_mod();
	}
}