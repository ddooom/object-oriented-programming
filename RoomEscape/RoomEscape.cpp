#include <bangtal.h>
using namespace bangtal;

int main()
{
    //장면1 생성
    auto scene1 = Scene::create("룸1", "image/배경-2.png");
    auto scene2 = Scene::create("룸2", "image/배경-1.png");

    //룸1
    //문1 생성
    auto open1 = false;
    auto door1 = Object::create("image/문-오른쪽-닫힘.png", scene1, 910, 270);

    //스위치1 생성
    auto light = true;
    auto button1 = Object::create("image/스위치.png", scene1, 830, 500);

    //상자 생성
    auto box_open = false;
    auto box = Object::create("image/closed_box.png", scene1, 50, 70);
    box->setScale(0.7f);

    //열쇠 생성
    auto key = Object::create("image/열쇠.png", scene1, 275, 170, false);
    key->setScale(0.3f);

    //키패드 생성
    auto keypad = Object::create("image/키패드.png", scene1, 360, 240);

    //뜰채 생성
    auto net = Object::create("image/net.png", scene1, 80, 340);
    net->setScale(0.8f);

    //액자 생성
    auto picture_moved = false;
    auto picture = Object::create("image/picture.png", scene1, 250, 430);
    picture->setScale(0.7f);

    //문제 생성
    auto question = Object::create("image/question.png", scene1, 300, 50, false);

    //화분 생성
    auto flowerpot1 = Object::create("image/화분.png", scene1, 1100, 220);

    //타이머 생성
    auto timer = Timer::create(120.f);


    key->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
        key->pick();
        showMessage("제한시간 2분 안에 방을 탈출하세요.");
        showTimer(timer);
        timer->start();

        return true;
        });

    timer->setOnTimerCallback([&](TimerPtr)->bool {
        endGame();

        return true;
        });

    net->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
        net->pick();

        return true;
        });

    box->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
        showMessage("우측의 비밀번호를 풀어야 합니다.");

        return true;
        });

    box->setOnKeypadCallback([&](ObjectPtr object) -> bool {
        
        box->setImage("image/open_box.png");
        key->show();
        showMessage("상자가 열렸다.");

        return true;
        });

    door1->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
        if (open1) {
            scene2->enter();
        }
        else if (key->isHanded()) {
            door1->setImage("image/문-오른쪽-열림.png");
            open1 = true;
        }
        else {
            showMessage("열쇠가 필요합니다.");
        }

        return true;
        });

    button1->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
        if (light) {
            scene1->setLight(0.3f);
            question->show();
            light = false;
        }
        else {
            scene1->setLight(1.0f);
            question->hide();
            light = true;
        }

        return true;
        });

    keypad->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
        showKeypad("159265", box);

        return true;
        });

    picture->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
        if ((picture_moved == false) && (action == MouseAction::MOUSE_DRAG_DOWN)) { 
            picture->locate(scene1, 340, 260);
            picture_moved = true;
        }
        else if ((picture_moved == true) && (action == MouseAction::MOUSE_DRAG_UP)) {
            picture->locate(scene1, 250, 430);
            picture_moved = false;
        }

        return true;
        });
    

    //룸2
    //문2 생성
    auto door2 = Object::create("image/문-왼쪽-열림.png", scene2, 200, 280);

    //문3 생성
    auto open3 = false;
    auto door3 = Object::create("image/문-오른쪽-닫힘.png", scene2, 955, 260);
    door3->setScale(0.2f);
    auto be_small = false;

    //힌트표시 생성
    auto hint = Object::create("image/hint.png", scene2, 358, 480);
    hint->setScale(.3f);

    //스위치2 생성
    auto button2 = Object::create("image/스위치.png", scene2, 450, 500);

    //화분 생성
    auto flowerpot2 = Object::create("image/화분.png", scene2, 1100, 220);

    //가짜 상자 생성
    auto fake_box_open = false;
    auto fake_box = Object::create("image/closed_box.png", scene2, -50, 30);
    fake_box->setScale(0.7f);

    //고양이 생성
    auto cat_moved = false;
    auto cat_touched = 0;
    auto cat = Object::create("image/cat.png", scene2, 890, 230);
    cat->setScale(0.3f);

    //알약 생성
    auto pill = Object::create("image/pill.png", scene2, 700, 220, false);
    pill->setScale(0.3f);

    //어항 생성
    auto fishbowl = Object::create("image/fishbowl.png", scene2, 350, 50);
    fishbowl->setScale(0.4f);

    //생선 생성
    auto fish = Object::create("image/fish.png", scene2, 350, 50);
    fish->setScale(0.4f);


    door2->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
        scene1->enter();

        return true;
        });

    door3->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
        if (open3) {
            endGame();
        }
        else if (be_small) {
            door3->setImage("image/문-오른쪽-열림.png");
            open3 = true;
        }
        else {
            showMessage("문이 너무 작아 탈출할 수 없습니다.");
        }

        return true;
        });
;   
    fake_box->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
        if (fake_box_open == false) {
            fake_box->setImage("image/open_box.png");
            fake_box_open = true;
        }
        else {
            fake_box->setImage("image/closed_box.png");
            fake_box_open = false;
        }
        return true;
        });

    fish->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
        if (net->isHanded()) {
            fish->pick();
        }
        else {
            showMessage("고양이가 좋아할 것 같은 물고기이다.");
        }
        return true;
        });

    button2->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
        showMessage("몸을 작아지게 하는 약은 물고기 사료이다.");

        return true;
        });

    cat->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
        if ((fish->isHanded()) && (cat_moved==false)) {
            cat->locate(scene2, 610, 230);
            showMessage("맛있다옹~");
            cat_moved = true;
        }
        if (cat_moved) {
            if (cat_touched == 5) {
                showMessage("뿌직!");
                pill->show();
            }
            cat_touched += 1;
        }
        else {
            showMessage("야옹~");
        }

        return true;
        });
    
    pill ->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action) -> bool {
        pill->hide();
        showMessage("몸이 작아졌다.");
        be_small = true;

        return true;
        });
  

    //게임 시작
    startGame(scene1);

    return 0;
}
