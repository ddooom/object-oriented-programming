#include <bangtal.h>
#include <string>
#include <iostream>
#include <ctime>

using namespace bangtal;
using namespace std;

int main()
{
    setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
    setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);

    auto main_scene = Scene::create("Main", "images/main.png");
    
    string path;
    string selection_string_num, puzzle_string_num;

    //퍼즐 게임의 수와 수에 따른 버튼들
    const int num_of_selection = 2;
    ScenePtr scene[num_of_selection];
    ObjectPtr record_button[num_of_selection], enter_puzzle[num_of_selection];

    //기본 변수들과 게임 내의 퍼즐을 저장하는 배열과 퍼즐의 위치를 저장하는 배열
    const int num_of_puzzles = 16;
    const int start_x = 300, start_y = 530, puzzle_size = 170;
    int location_seq_puzzle[num_of_selection][num_of_puzzles];
    ObjectPtr location_puzzle[num_of_selection][num_of_puzzles];
    
    int blank[num_of_selection];
    int start_time, end_time, best_time[num_of_puzzles];

    for (int i = 0; i < num_of_selection; i++) {
        //퍼즐의 배경, 게임 버튼, 기록 버튼 생성
        selection_string_num = to_string(i + 1);
        path = "images/background" + selection_string_num + ".png";
        scene[i] = Scene::create("Puzzle " + selection_string_num, path);
        
        path = "images/puzzle" + selection_string_num + ".png";
        enter_puzzle[i] = Object::create(path, main_scene, 120, 350 - i * 220);
        enter_puzzle[i]->setScale(0.2f);

        record_button[i] = Object::create("images/record.png", main_scene, 420, 390 - i * 220);
        record_button[i]->setScale(0.6f);

        blank[i] = 15;
        best_time[i] = 10000000;

        for (int j = 0; j < num_of_puzzles; j++) {
            //게임 내의 퍼즐 생성
            puzzle_string_num = to_string(j + 1);
            path = "images/puzzle" + selection_string_num + "-" + puzzle_string_num + ".png";    
            location_puzzle[i][j] = Object::create(path, scene[i], start_x + puzzle_size * (j % 4), start_y - puzzle_size * (j / 4));
            location_seq_puzzle[i][j] = j;

            //퍼즐 이동
            location_puzzle[i][j]->setOnMouseCallback([=, &location_puzzle, &location_seq_puzzle, &blank, &start_time, &end_time, &best_time](ObjectPtr object, int x, int y, MouseAction action) -> bool {
                int location, temp;
                for (location = 0; location < num_of_puzzles; location++) {
                    if (location_seq_puzzle[i][location] == j) break;
                }

                if (((location % 4 > 0) && (location - 1 == blank[i])) ||
                    ((location % 4 < 3) && (location + 1 == blank[i])) ||
                    ((location / 4 > 0) && (location - 4 == blank[i])) ||
                    ((location / 4 < 3) && (location + 4 == blank[i])))
                {
                    location_puzzle[i][j]->locate(scene[i], start_x + (blank[i] % 4) * puzzle_size, start_y - (blank[i] / 4) * puzzle_size);
                    
                    temp = location_seq_puzzle[i][location];
                    location_seq_puzzle[i][location] = location_seq_puzzle[i][blank[i]];
                    location_seq_puzzle[i][blank[i]] = temp;
                    blank[i] = location;
                }

                //퍼즐의 완성 여부를 수시로 체크하고 시간 기록
                int correct = 0;
                for (int k = 0; k < num_of_puzzles; k++) {
                    if (location_seq_puzzle[i][k] == k) correct += 1;
                }
                if (correct == 16) {
                    main_scene->enter();
                    end_time = time(NULL);
                    string record = "기록 : " + to_string(difftime(end_time, start_time));
                    showMessage(record.c_str());

                    if (difftime(end_time, start_time) < best_time[i]) best_time[i] = difftime(end_time, start_time);
                }

                return true;
            });
        }

        //메인 화면에서 퍼즐 그림 클릭 시 게임 시작, 퍼즐 섞기
        enter_puzzle[i]->setOnMouseCallback([=, &location_puzzle, &location_seq_puzzle, &blank, &start_time](ObjectPtr object, int x, int y, MouseAction action)->bool {
            scene[i]->enter();
            start_time = time(NULL);

            int location, temp;
            int iter=0;

            while (iter < 100) {
                location = rand() % 16;

                if (((location % 4 > 0) && (location - 1 == blank[i])) ||
                    ((location % 4 < 3) && (location + 1 == blank[i])) ||
                    ((location / 4 > 0) && (location - 4 == blank[i])) ||
                    ((location / 4 < 3) && (location + 4 == blank[i])))
                {
                    location_puzzle[i][location_seq_puzzle[i][location]]->locate(scene[i], start_x + (blank[i] % 4) * puzzle_size, start_y - (blank[i] / 4) * puzzle_size);

                    temp = location_seq_puzzle[i][location];
                    location_seq_puzzle[i][location] = location_seq_puzzle[i][blank[i]];
                    location_seq_puzzle[i][blank[i]] = temp;
                    blank[i] = location;

                    iter += 1;
                }
            }
            return true;
        });

        //기록 버튼 클릭 시 최단 기록을 보여줌
        record_button[i]->setOnMouseCallback([=, &best_time](ObjectPtr object, int x, int y, MouseAction action)->bool {
            if (best_time[i] == 10000000) showMessage("기록이 없습니다.");
            else {
                string best_record = "최단 기록 : " + to_string(best_time[i]);
                showMessage(best_record.c_str());
            }
            return true;
        });
    }

    bangtal::startGame(main_scene);
}

