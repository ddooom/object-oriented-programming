#include <bangtal>
#include <iostream>
#include <Windows.h>
using namespace bangtal;
using namespace std;

int main()
{
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);

//배경 설정
	auto main_scene = Scene::create("Main", "images/main.png");
	auto how_scene = Scene::create("How to Play", "images/howtoplay_main.png");
	auto game_scene = Scene::create("inGame", "images/game_background.png");

//버튼 설정
	auto start_button = Object::create("images/start.png", main_scene, 560, 480);
	start_button->setScale(0.4f);
	auto how_button = Object::create("images/howtoplay.png", main_scene, 560, 375);
	how_button->setScale(0.4f);
	auto record_button = Object::create("images/bestrecord.png", main_scene, 560, 270);
	record_button->setScale(0.4f);
	auto end_button = Object::create("images/end.png", main_scene, 560, 165);
	end_button->setScale(0.4f);

//게임 방법 클릭 시 : 게임 방법 화면 설정
	how_button->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
		how_scene->enter();
		return true;
	});
	//게임 방법 화면 내의 뒤로 가기
	auto how_back = Object::create("images/back.png", how_scene, 365, 675);
	how_back->setScale(0.15f);
	how_back->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
		main_scene->enter();
		return true;
	});

//최고 기록 클릭 시
	int best_record = 0;
	record_button->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
		string record = "최고 기록 : " + to_string(best_record);
		bangtal::showMessage(record.c_str());
		return true;
		});

//종료 클릭 시
	end_button->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
		endGame();
		return true;
		});

//시작 클릭 시
	const int num_of_birds = 4;								//새의 종류

	int num_which_side[num_of_birds] = {0, 1, 2, 3};		//게임마다 달라지는 위치에 따른 새의 종류를 나타낸 배열 (0, 2번째 : 좌 / 1, 3번째 : 우)
	ObjectPtr birds_which_side[num_of_birds];				//위 배열에 따른 새를 좌, 우에 위치시키는 배열
	
	int num_game_line[5];									//게임에서 줄을 선 새의 종류를 나타낸 배열
	ObjectPtr birds_game_line[5];							//게임에서 줄을 설 새를 위치시키는 배열

	int first_bird_idx;										//줄을 선 새 배열의 맨 앞 index
	int next_bird_num;										//줄에서 다음으로 생성될 새의 종류
	int score;												//게임의 점수
	int life;												//게임의 생명
	ObjectPtr life_object[3];

	int bird_location[num_of_birds][num_of_birds] = { {445, 65}, {700, 65}, {385, 175}, {770, 175} };

	auto game_timer = Timer::create(60.0f);

	start_button->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
		game_scene->enter();
		std::cout << "* Game Start *" << endl << "> Score" << endl;

		first_bird_idx = 0;	
		score = 0;	
		life = 3;

		showTimer(game_timer);
		game_timer->start();

		//새의 종류를 shuffle
		int temp;
		int random_num;
		for (int i = 0; i < num_of_birds; i++) {
			random_num = rand() % num_of_birds;
			temp = num_which_side[i];
			num_which_side[i] = num_which_side[random_num];
			num_which_side[random_num] = temp;
		}

		//inGame 화면에 좌우, 그리고 줄에 새와 life를 create
		for (int i = 0; i < 5; i++) {
			if (i < 3) {
				life_object[i] = Object::create("images/life.png", game_scene, 860 - 35 * i, 670);
				life_object[i]->setScale(0.2f);
			}
			if (i < 4) {
				birds_which_side[i] = Object::create("images/bird" + to_string(num_which_side[i]) + ".png", game_scene, bird_location[i][0], bird_location[i][1]);
				birds_which_side[i]->setScale(0.4f);
			}
			next_bird_num = rand() % 2;
			num_game_line[i] = num_which_side[next_bird_num];
			birds_game_line[i] = Object::create("images/bird" + to_string(num_which_side[next_bird_num]) + ".png", game_scene, 575, 150 + 115 * i);
			birds_game_line[i]->setScale(0.45f);
		}

		return true;
	});

	//게임 화면 내의 게임 동작
	int level;
	int iteration;
	auto life_timer = Timer::create(0.3f);
	auto bomb_timer = Timer::create(0.1f);

	ObjectPtr moving_birds[5];		//새의 이동 애니메이션 효과를 위한 object배열
	int move_direction[5];			//애니메이션 효과 : 이동할 방향 배열
	int move_num[5];				//애니메이션 효과 : 얼마나 이동되었는지 나타내는 배열

	game_scene->setOnKeyboardCallback([&](ScenePtr scene, int key, bool pressed)->bool {
		if (score < 500) level = 2;
		else if (score < 1000) level = 3;
		else level = 4;

		//새를 알맞은 둥지로 보냈을 때
		if ((((key == 82) && (pressed)) && ((num_game_line[first_bird_idx] == num_which_side[0]) || (num_game_line[first_bird_idx] == num_which_side[2])))
			|| (((key == 83) && (pressed)) && ((num_game_line[first_bird_idx] == num_which_side[1]) || (num_game_line[first_bird_idx] == num_which_side[3]))))
		{
			birds_game_line[first_bird_idx]->hide();
			for (int i = 1; i < 5; i++) {
				birds_game_line[(first_bird_idx + i) % 5]->locate(game_scene, 575, 150 + 115 * (i - 1));
			}

			if (key == 82) move_direction[first_bird_idx] = -20;
			else if (key == 83) move_direction[first_bird_idx] = 25;

			//둥지로 보내는 새의 애니메이션 효과
			move_num[first_bird_idx] = 3;
			moving_birds[first_bird_idx] = Object::create("images/bird" + to_string(num_game_line[first_bird_idx]) + ".png", game_scene, 575 + move_direction[first_bird_idx] * move_num[first_bird_idx], 150);
			moving_birds[first_bird_idx]->setScale(0.45f);
			auto move_timer = Timer::create(0.05f);
			move_timer->start();
			move_timer->setOnTimerCallback([=, &move_num](TimerPtr)->bool {
				moving_birds[first_bird_idx]->locate(game_scene, 575 + (move_direction[first_bird_idx] * move_num[first_bird_idx]), 150);
				moving_birds[first_bird_idx]->setScale(0.45f / move_num[first_bird_idx] * 2);
				move_timer->set(0.05f);
				if (move_num[first_bird_idx] == 8) moving_birds[first_bird_idx]->hide();
				else {
					move_num[first_bird_idx] += 1;
					move_timer->start();
				}
				return true;
			});

			//줄의 끝에 다음 새를 배치
			if (score % 300 == 0) {
				num_game_line[first_bird_idx] = 4;
				birds_game_line[first_bird_idx] = Object::create("images/bomb.png", game_scene, 575, 150 + 115 * 4);
				birds_game_line[first_bird_idx]->setScale(0.45f);
			}
			else {
				next_bird_num = rand() % level;
				num_game_line[first_bird_idx] = num_which_side[next_bird_num];
				birds_game_line[first_bird_idx] = Object::create("images/bird" + to_string(num_which_side[next_bird_num]) + ".png", game_scene, 575, 150 + 115 * 4);
				birds_game_line[first_bird_idx]->setScale(0.45f);
			}
			first_bird_idx = (first_bird_idx + 1) % 5;

			score += 10;
			std::cout << "> " << score << endl;
		}
		
		//폭탄을 폭발시켰을 때
		else if (pressed & (num_game_line[first_bird_idx] == 4)) {
			iteration = 1;
			bomb_timer->start();

			bomb_timer->setOnTimerCallback([&](TimerPtr)->bool {
				//폭탄 터지는 모션 
				moving_birds[first_bird_idx] = Object::create("images/explode.png", game_scene, 575, 150);
				moving_birds[first_bird_idx]->setScale(0.45f);
				auto move_timer = Timer::create(0.1f);
				move_timer->start();
				move_timer->setOnTimerCallback([=, &move_num](TimerPtr)->bool {
					moving_birds[first_bird_idx]->setScale(0.45f / move_num[first_bird_idx]);
					moving_birds[first_bird_idx]->hide();
					return true;
				});

				//줄의 끝에 있는 새를 보내고 다음 새를 배치
				birds_game_line[first_bird_idx]->hide();
				for (int i = 1; i < 5; i++) {
					birds_game_line[(first_bird_idx + i) % 5]->locate(game_scene, 575, 150 + 115 * (i - 1));
				}

				next_bird_num = rand() % level;
				num_game_line[first_bird_idx] = num_which_side[next_bird_num];
				birds_game_line[first_bird_idx] = Object::create("images/bird" + to_string(num_which_side[next_bird_num]) + ".png", game_scene, 575, 150 + 115 * 4);
				birds_game_line[first_bird_idx]->setScale(0.45f);

				first_bird_idx = (first_bird_idx + 1) % 5;

				score += 10; 
				std::cout << "> " << score << endl;

				bomb_timer->set(0.1f);

				//5번 반복한 뒤에는 타이머 종료
				if (iteration < 6) {
					iteration += 1;
					bomb_timer->start();
				}
				return true;
			});
		}

		//새를 다른 둥지로 보냈을 때
		else if ((((key == 82) && (pressed)) && ((num_game_line[first_bird_idx] != num_which_side[0]) && (num_game_line[first_bird_idx] != num_which_side[2])))
				|| (((key == 83) && (pressed)) && ((num_game_line[first_bird_idx] != num_which_side[1]) && (num_game_line[first_bird_idx] != num_which_side[3]))))
		{
			game_scene->setLight(0.3f);
			life_timer->start();
			life_timer->setOnTimerCallback([&](TimerPtr)->bool {
				game_scene->setLight(1.0f);
				life_timer->set(0.3f);
				return true;
			});

			life -= 1;
			life_object[life]->hide();
			if (life == 0) {
				main_scene->enter();
			}
		}
		
		return true;
	});

	//게임 화면 내의 뒤로 가기
	auto ingame_back = Object::create("images/back.png", game_scene, 365, 675);
	ingame_back->setScale(0.15f);
	ingame_back->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
		main_scene->enter();
		return true;
		});

	//게임 시간 60초가 종료되었을 때
	game_timer->setOnTimerCallback([&](TimerPtr)->bool {
		main_scene->enter();
		return true;
	});

	//게임 화면에서 나갈 때 (뒤로 가기를 누르거나 게임이 끝났을 경우)
	game_scene->setOnLeaveCallback([&](ScenePtr scene)->bool {
		game_timer->stop();
		game_timer->set(60.0f);
		hideTimer();
		
		for (int i = 0; i < 5; i++) {
			if (i < 3) life_object[i]->hide();
			if (i < 4) birds_which_side[i]->hide();
			birds_game_line[i]->hide();
		}
		string str_score = "게임 기록 : " + to_string(score);
		bangtal::showMessage(str_score.c_str());

		if (score > best_record) best_record = score;
		
		std::cout << "* Game End" << endl << endl;

		return true;
		});


	bangtal::startGame(main_scene);

    return 0;
}
