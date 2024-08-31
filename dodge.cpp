#include <chrono>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>

class Player {
private:
  int row;

public:
  Player() { row = 1; }

  void setRow(int r) { row = r; }

  int getRow() { return row; }

  int getPos() { return 4 + row * 60; }
};

class Obstacle {
private:
  int row;
  int col;

public:
  Obstacle() {
    row = rand() % 3;
    col = 59;
  }

  void setCol(int c) { col = c; }

  int getRow() { return row; }

  int getCol() { return col; }

  int getPos() { return col + row * 60; }
};

std::deque<Obstacle> moveObstacles(std::deque<Obstacle> obstacles) {
  for (int i = 0; i < obstacles.size(); i++) {
    obstacles[i].setCol(obstacles[i].getCol() - 1);
  }
  if (obstacles[0].getCol() == -1)
    obstacles.pop_front();
  return obstacles;
}

void drawTerminal(Player player, int objectsPassed,
                  std::deque<Obstacle> obstacles) {
  int pos = 0;
  bool b = false;

  std::cout << "J - Down  K - Up\tObjects Avoided: " << objectsPassed << '\r'
            << '\n';
  std::cout << "____________________________________________________________"
            << '\r' << '\n';

  for (int r = 0; r < 3; r++) {
    for (int c = 0; c < 60; c++) {
      if (c == 4 && player.getRow() == r &&
          (obstacles[0].getCol() != 4 ||
           obstacles[0].getRow() != player.getRow())) {
        std::cout << '>';
        b = true;
      } else {
        for (int i = 0; i < obstacles.size(); i++) {
          if (obstacles[i].getPos() == pos) {
            std::cout << '|';
            b = true;
            break;
          }
        }
      }
      if (!b)
        std::cout << ' ';
      b = false;
      pos++;
    }
    std::cout << '\r' << '\n';
  }
  std::cout << "____________________________________________________________"
            << '\r' << '\n';
}

bool haveCollided(Player player, Obstacle o) {
  return player.getPos() == o.getPos();
}

void movePlayer(Player &player) {
  char input;

  while (true) {
    input = getchar();
    std::cout << "\b \b";

    if (input == 'j' && player.getRow() != 2)
      player.setRow(player.getRow() + 1);
    else if (input == 'k' && player.getRow() != 0)
      player.setRow(player.getRow() - 1);
  }
}

int main() {

  using clock = std::chrono::steady_clock;
  using frames = std::chrono::duration<int, std::ratio<1, 60>>;

  srand((unsigned)time(NULL));

  std::deque<Obstacle> obstacles;
  obstacles.push_back(Obstacle());
  Player player;
  int count = 0;
  int objectsPassed = 0;
  int objectsPassedCounter = 55;
  int difficulty = 1;
  int countComp;

  system("clear");
  std::cout << "Please enter your difficulty 1 - 3 (Default is 1): ";
  std::cin >> difficulty;

  std::cout << "\e[?25l";

  switch (difficulty) {
  case 2:
    countComp = 20;
    break;
  case 3:
    countComp = 10;
    break;
  default:
    countComp = 30;
  }

  std::thread input_thread(movePlayer, std::ref(player));
  input_thread.detach();

  system("stty raw");

  while (true) {
    auto start = clock::now();

    system("clear");
    count++;
    if (count == countComp) {
      obstacles.push_back(Obstacle());
      count = 0;
    }

    drawTerminal(player, objectsPassed, obstacles);
    if (haveCollided(player, obstacles[0]))
      break;
    obstacles = moveObstacles(obstacles);

    objectsPassedCounter--;
    if (objectsPassedCounter == 0) {
      objectsPassedCounter = countComp;
      objectsPassed++;
    }

    auto finish = clock::now();
    if (finish - start < frames(1))
      std::this_thread::sleep_until(start + frames(1));
  }
  system("stty cooked");
  std::cout << "\e[?25h";
  std::cout << "You passed " << objectsPassed << " objects." << '\n';
  return 0;
}
