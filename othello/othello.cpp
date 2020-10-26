#include <bangtal>
#include <iostream>
using namespace bangtal;
using namespace std;

ObjectPtr board[8][8];

int black_score = 0;
int white_score = 0;

ObjectPtr black_score_image[2];
ObjectPtr white_score_image[2];
ObjectPtr turn_image;

enum class State {
	BLANK,
	POSSIBLE,
	BLACK,
	WHITE
};
State board_state[8][8];

enum class Turn {
	BLACK,
	WHITE
};
Turn turn = Turn::BLACK;

void setState(int x, int y, State state) {
	switch (state) {
	case State::BLANK: board[y][x]->setImage("Images/blank.png"); break;
	case State::POSSIBLE: board[y][x]->setImage(turn == Turn::BLACK ? "Images/black possible.png" : "Images/white possible.png"); break;
	case State::BLACK: board[y][x]->setImage("Images/black.png"); black_score++; break;
	case State::WHITE: board[y][x]->setImage("Images/white.png"); white_score++; break;
	}	
	board_state[y][x] = state;
}

bool checkPossible(int x, int y, int dx, int dy) {
	State self = turn == Turn::BLACK ? State::BLACK : State::WHITE;
	State other = turn == Turn::BLACK ? State::WHITE : State::BLACK;

	bool possible = false;
	for (x += dx, y += dy; x >= 0 && x < 8 && y >= 0 && y < 8; x += dx, y += dy) {
		if (board_state[y][x] == other) {
			possible = true;
		}
		else if (board_state[y][x] == self) {
			return possible;
		}
		else {
			return false;
		}
	}

	return false;
}

bool checkPossible(int x, int y) {
	if (board_state[y][x] == State::BLACK) return false;
	if (board_state[y][x] == State::WHITE) return false;
	setState(x, y, State::BLANK);

	int delta[8][2] = {
		{ 0, 1 },
		{ 1, 1 },
		{ 1, 0 },
		{ 1, -1 },
		{ 0, -1 },
		{ -1, -1 },
		{ -1, 0 },
		{ -1, 1 },
	};

	bool possible = false;
	for (auto d : delta) {
		if (checkPossible(x, y, d[0], d[1])) possible = true;
	}

	return possible;
}

bool setPossible() {
	bool possible = false;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (checkPossible(x, y)) {
				setState(x, y, State::POSSIBLE);
				possible = true;
			}
		}
	}

	return possible;
}

void reverse(int x, int y, int dx, int dy) {
	State self = turn == Turn::BLACK ? State::BLACK : State::WHITE;
	State other = turn == Turn::BLACK ? State::WHITE : State::BLACK;

	bool possible = false;
	for (x += dx, y += dy; x >= 0 && x < 8 && y >= 0 && y < 8; x += dx, y += dy) {
		if (board_state[y][x] == other) {
			possible = true;
		}
		else if (board_state[y][x] == self) {
			if (possible) {
				for (x -= dx, y -= dy; x >= 0 && x < 8 && y >= 0 && y < 8; x -= dx, y -= dy) {
					if (board_state[y][x] == other) {
						setState(x, y, self);
						other == State::BLACK ? black_score-- : white_score--;
					}
					else {
						return;
					}
				}
			}
		}
		else {
			return;
		}
	}
}

void reverse(int x, int y) {
	int delta[8][2] = {
			{ 0, 1 },
			{ 1, 1 },
			{ 1, 0 },
			{ 1, -1 },
			{ 0, -1 },
			{ -1, -1 },
			{ -1, 0 },
			{ -1, 1 },
	};

	for (auto d : delta) {
		reverse(x, y, d[0], d[1]);
	}
}

// 점수를 표시
void setScore() {
	black_score_image[0]->setImage("Images/L" + to_string(black_score / 10) + ".png");
	black_score_image[1]->setImage("Images/L" + to_string(black_score % 10) + ".png");
	white_score_image[0]->setImage("Images/L" + to_string(white_score / 10) + ".png");
	white_score_image[1]->setImage("Images/L" + to_string(white_score % 10) + ".png");
}

// 방향에 따라 뒤집어지는 돌의 수를 계산하여 return (num_reverse)
int num_reverse(int x, int y, int dx, int dy) {
	State self = turn == Turn::BLACK ? State::BLACK : State::WHITE;
	State other = turn == Turn::BLACK ? State::WHITE : State::BLACK;

	int num_reverse = 0;

	bool possible = false;
	for (x += dx, y += dy; x >= 0 && x < 8 && y >= 0 && y < 8; x += dx, y += dy) {
		if (board_state[y][x] == other) {
			possible = true;
		}
		else if (board_state[y][x] == self) {
			if (possible) {
				for (x -= dx, y -= dy; x >= 0 && x < 8 && y >= 0 && y < 8; x -= dx, y -= dy) {
					if (board_state[y][x] == other) {
						num_reverse++;
					}
					else {
						return num_reverse;
					}
				}
			}
		}
		else {
			return num_reverse;
		}
	}
	return num_reverse;
}

// 방향에 따라 뒤집어지는 돌의 수의 합
int num_reverse(int x, int y) {
	int delta[8][2] = {
			{ 0, 1 },
			{ 1, 1 },
			{ 1, 0 },
			{ 1, -1 },
			{ 0, -1 },
			{ -1, -1 },
			{ -1, 0 },
			{ -1, 1 },
	};

	int total_num_reverse = 0;

	for (auto d : delta) {
		total_num_reverse += num_reverse(x, y, d[0], d[1]);
	}

	return total_num_reverse;
}

// POSSIBLE한 위치에 돌을 놨을 때 뒤집어지는 상대 돌의 수(best_reverse)가 가장 많은 곳(best_x, best_y)에 돌을 놓는 함수 
void computer() {
	int best_x = -1;
	int best_y = -1;
	int best_reverse = -1;
	
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (board_state[y][x] == State::POSSIBLE) {
				int n_reverse = num_reverse(x, y);
				// best_reverse가 같은 위치가 많을 경우에는 rand() % 2 == 0 로 무작위성을 주어 best_x와 best_y를 매번 다르게 했다.
				if ((best_reverse == n_reverse && rand() % 2 == 0) || (best_reverse < n_reverse)) { 
					best_x = x;
					best_y = y;
					best_reverse = n_reverse;
				}
			}
		}
	}

	if (best_reverse != -1) {
		//best_x, best_y에 돌을 놓고 상대 돌을 뒤집는다.
		setState(best_x, best_y, turn == Turn::BLACK ? State::BLACK : State::WHITE);
		reverse(best_x, best_y);
		turn = turn == Turn::BLACK ? Turn::WHITE : Turn::BLACK;
		turn_image->setImage(turn == Turn::BLACK ? "Images/black turn.png" : "Images/white turn.png");
	}
}

int main(){
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);

	auto scene = Scene::create("Othello", "Images/background.png");
	turn_image = Object::create("Images/black turn.png", scene, 750, 160);

	auto computer_button = Object::create("Images/computers turn.png", scene, 750, 120);
	computer_button->setScale(0.25f);

	// 컴퓨터 버튼을 만들어 버튼을 클릭시 컴퓨터가 돌을 놓게 하였다.
	computer_button->setOnMouseCallback([&](ObjectPtr object, int, int, MouseAction) -> bool {
		computer();
		
		if (!setPossible()) {
			turn = turn == Turn::BLACK ? Turn::WHITE : Turn::BLACK;

			if (!setPossible()) {
				black_score > white_score ? showMessage("Black Win!") : showMessage("White Win!");
			}
		}

		setScore();
		return true;
	});

	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			board[y][x] = Object::create("Images/blank.png", scene, 40 + x * 80, 40 + y * 80);
			board[y][x]->setOnMouseCallback([&, x, y](ObjectPtr object, int, int, MouseAction) -> bool {
				if (board_state[y][x] == State::POSSIBLE) {
					setState(x, y, turn == Turn::BLACK ? State::BLACK : State::WHITE);
					reverse(x, y);
					turn = turn == Turn::BLACK ? Turn::WHITE : Turn::BLACK;
					turn_image->setImage(turn == Turn::BLACK ? "Images/black turn.png" : "Images/white turn.png");
					
					if (!setPossible()) {
						turn = turn == Turn::BLACK ? Turn::WHITE : Turn::BLACK;

						if (!setPossible()) {
							black_score > white_score ? showMessage("Black Win!") : showMessage("White Win!");
						}
					}
				}
				setScore();
				return true;
			});
			board_state[y][x] = State::BLANK;
		}
	}

	for (int i = 0; i < 2; i++) {
		black_score_image[i] = Object::create("Images/L0.png", scene, 753 + i * 74, 220);
		white_score_image[i] = Object::create("Images/L0.png", scene, 1073 + i * 74, 220);
	}

	setState(3, 3, State::BLACK);
	setState(4, 4, State::BLACK);
	setState(3, 4, State::WHITE);
	setState(4, 3, State::WHITE);

	setPossible();
	setScore();

	bangtal::startGame(scene);
}