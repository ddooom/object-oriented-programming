#include <bangtal.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <cstring>
using namespace bangtal;

//랜덤하게 퍼즐을 섞기 위한 배열을 섞는 함수
void shuffle(int* arr, int num) 
{
    int temp;
    int random_num;
    for (int i = 0; i < num - 1; i++) {
        random_num = rand() % (num - i) + i;
        temp = arr[i];
        arr[i] = arr[random_num];
        arr[random_num] = temp;
    }
}

//퍼즐이 모두 맞춰졌을 때를 판단하기 위한 함수
bool check_correct(int* puzzle_seq, ScenePtr main_scene) {
    int initial_seq[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 ,13 ,14, 15};
    bool result = false;

    int correct = 0;
    for (int i = 0; i < 16; i++) {
        if (puzzle_seq[i] == initial_seq[i]) {
            correct += 1;
            
        }
    }

    if (correct == 16) {
        main_scene->enter();
        result = true;
    }

    return result;
}

//퍼즐이 움직였을 때 퍼즐의 위치를 저장하는 puzzle_seq의 위치도 바꿔주는 함수
void switch_seq(int* puzzle_seq, int a, int b) {
    int seq_temp;
    seq_temp = puzzle_seq[a];
    puzzle_seq[a] = puzzle_seq[b];
    puzzle_seq[b] = seq_temp;
}

//puzzle_seq을 기반으로 퍼즐이 움직일 수 있는지를 판단하고 옮기는 함수
//puzzle_seq는 길이가 16인 배열로 0~3번째 값 : row1 / 4~7번째 값 : row2 / 8~11번재 값 : row3 / 12~15번째 값 : row4 
//puzzle_seq에서 0~14의 값은 퍼즐의 번호이고 15는 공백인 곳을 나타낸다. 
void move_puzzle(ObjectPtr* location_puzzle, int* puzzle_seq, int seq, int start_x, int start_y, int puzzle_size, ScenePtr scene) {
    int location, row, col;
    int num_of_puzzles = 16;

    for (int i = 0; i < num_of_puzzles; i++) {
        if (puzzle_seq[i] == seq) {
            location = i;
            break;
        }
    }

    row = location / 4;
    col = location % 4;

    if ((row > 0) && (puzzle_seq[4 * (row - 1) + col] == 15)) {
        location_puzzle[seq]->locate(scene, (start_x + puzzle_size * col), (start_y - puzzle_size * (row - 1)));
        switch_seq(puzzle_seq, 4 * row + col, 4 * (row - 1) + col);
    }

    if ((row < 3) && (puzzle_seq[4 * (row + 1) + col] == 15)) {
        location_puzzle[seq]->locate(scene, (start_x + puzzle_size * col), (start_y - puzzle_size * (row + 1)));
        switch_seq(puzzle_seq, 4 * row + col, 4 * (row + 1) + col);
    }

    if ((col > 0) && (puzzle_seq[4 * row + (col - 1)] == 15)) {
        location_puzzle[seq]->locate(scene, (start_x + puzzle_size * (col - 1)), (start_y - puzzle_size * row));
        switch_seq(puzzle_seq, 4 * row + col, 4 * row + (col - 1));
    }

    if ((col < 3) && (puzzle_seq[4 * row + (col + 1)] == 15)) {
        location_puzzle[seq]->locate(scene, (start_x + puzzle_size * (col + 1)), (start_y - puzzle_size * row));
        switch_seq(puzzle_seq, 4 * row + col, 4 * row + (col + 1));
    }
}

//위의 move_puzzle을 이용하여 게임이 시작할 때 퍼즐을 랜덤으로 섞는 함수
void mix_puzzle(ObjectPtr* location_puzzle, int* puzzle_seq, int start_x, int start_y, int puzzle_size, ScenePtr scene) {
    int arr[16];
    for (int i = 0; i < 16; i++) {
        arr[i] = i;
    }
    
    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 16; j++) {
            move_puzzle(location_puzzle, puzzle_seq, arr[j], start_x, start_y, puzzle_size, scene);
        }
    }
}

int main()
{
    setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
    setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);

    //배경
    auto main_scene = Scene::create("Main", "images/main.png");
    auto scene1 = Scene::create("Puzzle 1", "images/background1.png");
    auto scene2 = Scene::create("Puzzle 2", "images/background2.png");

    //기록 버튼
    auto record_button1 = Object::create("images/record.png", main_scene, 420, 390);
    record_button1->setScale(0.6f);
    auto record_button2 = Object::create("images/record.png", main_scene, 420, 170);
    record_button2->setScale(0.6f);

    //두가지 선택지
    auto enter_puzzle1 = Object::create("images/puzzle1.png", main_scene, 120, 350);
    enter_puzzle1->setScale(0.2f);
    auto enter_puzzle2 = Object::create("images/puzzle2.png", main_scene, 120, 130);
    enter_puzzle2->setScale(0.2f);

    //시간 기록 변수 선언
    clock_t time_start, time_end;
    double record1, record2;
    double best_record1=1000000, best_record2= 1000000;
    char record1_char[20], record2_char[20], best_record1_char[20], best_record2_char[20]; //message로 남기기 위한 char array

    //퍼즐 수와 퍼즐이 들어갈 시작 좌표 초기화
    const int num_of_puzzles = 16;
    const int start_x = 300, start_y = 530, puzzle_size = 170;

    //좌측 상단부터 들어갈 퍼즐의 순서를 나타낼 배열
    int puzzle_seq1[num_of_puzzles];
    for (int i = 0; i < num_of_puzzles; i++) {
        puzzle_seq1[i] = i;
    }
    
    int puzzle_seq2[num_of_puzzles];
    for (int i = 0; i < num_of_puzzles; i++) {
        puzzle_seq2[i] = i;
    }

    //퍼즐은 create 할 때 좌표를 문자열로 나타내 줄 변수 선언
    std::string path;
    std::string puzzle_num;

    //퍼즐1의 퍼즐들 초기화
    ObjectPtr location_puzzle1[num_of_puzzles];
    for (int i = 0; i < num_of_puzzles; i++) {
        puzzle_num = std::to_string(i+1);
        path = "images/puzzle1-" + puzzle_num + ".png";     //puzle1-16은 없으므로 생성 안됨
        location_puzzle1[i] = Object::create(path, scene1, start_x + puzzle_size * (i%4), start_y - puzzle_size * (i/4));
    }
    
    //퍼즐2의 퍼즐들 초기화
    ObjectPtr location_puzzle2[num_of_puzzles];
    for (int i = 0; i < num_of_puzzles; i++) {
        puzzle_num = std::to_string(i+1);
        path = "images/puzzle2-" + puzzle_num + ".png";     //puzle1-16은 없으므로 생성 x
        location_puzzle2[i] = Object::create(path, scene2, start_x + puzzle_size * (i%4), start_y - puzzle_size * (i/4));
    }

    //main화면에서 퍼즐1 그림을 선택
    enter_puzzle1->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
        scene1->enter();
        
        mix_puzzle(location_puzzle1, puzzle_seq1, start_x, start_y, puzzle_size, scene1);

        time_start = clock();

        return true;
     });


    //퍼즐1을 클릭하면 동작하고 다 맞추면 기록을 남김
    for (int i = 0; i < num_of_puzzles; i++) {
        location_puzzle1[i]->setOnMouseCallback([=, &location_puzzle1, &puzzle_seq1, &time_start, &time_end, &record1, &best_record1, &record1_char, &best_record1_char](ObjectPtr object, int x, int y, MouseAction action) -> bool {
            move_puzzle(location_puzzle1, puzzle_seq1, i, start_x, start_y, puzzle_size, scene1);

            if (check_correct(puzzle_seq1, main_scene)) {
                time_end = clock();
                record1 = (double)(time_end - time_start) / CLOCKS_PER_SEC;
                sprintf_s(record1_char, "%f", record1);

                showMessage((const char*) record1_char);

                if (best_record1 > record1) {
                    best_record1 = record1;
                    sprintf_s(best_record1_char, "%f", best_record1);
                }
            }

            return true;
            });
    }

    //main화면에서 퍼즐2 그림을 선택
    enter_puzzle2->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
        scene2->enter();

        mix_puzzle(location_puzzle2, puzzle_seq2, start_x, start_y, puzzle_size, scene2);

        time_start = clock();

        return true;
     });

    //퍼즐2을 클릭하면 동작하고 다 맞추면 기록을 남김
    for (int i = 0; i < num_of_puzzles; i++) {
        location_puzzle2[i]->setOnMouseCallback([=, &location_puzzle2, &puzzle_seq2, &time_start, &time_end, &record2, &best_record2, &record2_char, &best_record2_char](ObjectPtr object, int x, int y, MouseAction action) -> bool {
            move_puzzle(location_puzzle2, puzzle_seq2, i, start_x, start_y, puzzle_size, scene2);

            if (check_correct(puzzle_seq2, main_scene)) {
                time_end = clock();
                record2 = (double)(time_end - time_start) / CLOCKS_PER_SEC;
                sprintf_s(record2_char, "%f", record2);

                showMessage((const char*)record2_char);

                if (best_record2 > record2) {
                    best_record2 = record2;
                    sprintf_s(best_record2_char, "%f", best_record2);
                }
            }

            return true;
            });
    }

    //record_button을 클릭하면 최고기록을 메세지로 남김
    record_button1->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
        if (best_record1 == 1000000) {
            showMessage("없음");
        }
        else {
            showMessage((const char*)best_record1_char);
        }

        return true;
        });

    record_button2->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
        if (best_record2 == 1000000) {
            showMessage("없음");
        }
        else {
            showMessage((const char*)best_record2_char);
        }

        return true;
        });

    bangtal::startGame(main_scene);
}
