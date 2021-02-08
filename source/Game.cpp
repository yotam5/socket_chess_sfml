
#pragma once
#include "../headers/Game.h"

//init window
void Game::initWindow()
{
    this->window = new sf::RenderWindow(sf::VideoMode(WIDTH, HEIGHT), "Chess",
                                        sf::Style::Close | sf::Style::Titlebar);
    this->window->setFramerateLimit(144);
    this->window->setVerticalSyncEnabled(false);
}

//update game all updats
void Game::update()
{
    if (this->netManager.getFullyRecive())
    {
        auto dataStr = this->netManager.getMsgCopy();
        auto pieceData = this->netManager.convertTo<std::array<int, 4>>(dataStr);
        this->EngineBoard->moveFromTo(
            7 - pieceData[2], 7- pieceData[3],
            7 - pieceData[0], 7 - pieceData[1]);
        this->netManager.setFullyRecive(false);
        this->currentPlayer = this->currentPlayer == WHITE ? BLACK : WHITE;
    }
    if (this->EngineBoard->isCheckMate(Color::BLACK) || this->EngineBoard->isCheckMate(Color::WHITE)) //NOTE
    {
        this->chess = true;
        Debug("winner");
        exit(1); //NOTE
    }
    this->updatePollEvents();
}

//constructor
Game::Game(char color)
{
    this->initWindow();
    this->initTexture();
    this->initVariables();
    this->EngineBoard = new Board(color); //create board
    this->thisPlayer = (color == 'w') ? Color::WHITE : Color::BLACK;
}

//init textures
void Game::initTexture() //jpeg error~
{
    this->boardTexture = new sf::Texture;
    if (!this->boardTexture->loadFromFile("./asserts/board1.png"))
    {
        throw std::invalid_argument("ERROR LOADING BOARD\n");
    }
}

//initialize variables
void Game::initVariables()
{
    //the window
    this->board.setTexture((*this->boardTexture));
    this->board.setPosition(0, 0);
    this->rowChoose = 0;
    this->columnChoose = 0;

    //player data
    this->currentPlayer = WHITE;
    this->currentState = WAITING;

    //font
    if (!font.loadFromFile("./fonts/mrsmonster.ttf"))
    {
        throw std::invalid_argument("coudnt load font Game.cpp\n");
    }

    this->text.setFont(font);
    this->text.setString("");
    this->text.setCharacterSize(50);
    this->text.setFillColor(sf::Color::Green);
    this->text.setPosition(WIDTH / 5, HEIGHT / 3);
}

//destructor
Game::~Game()
{
    for (auto &i : this->textures)
    {
        delete i.second;
    }
    delete EngineBoard;
    delete boardTexture;
    delete this->window;
}

//render game
void Game::render()
{
    this->window->clear();
    this->window->draw(board);
    this->EngineBoard->draw(*this->window);
    int sleepTime = 0;
    if (chess && this->text.getString() == "")
    {
        std::string str = "The winner is: ";
        str += this->EngineBoard->isInChess(WHITE) ? "Black" : "White";
        this->text.setString(str);
        sleepTime = 5;
        std::cout << "yap\n";
    }
    this->window->draw(this->text);
    this->window->display();
    sleep(sleepTime); //FIXME WTF!
}

//update events
void Game::updatePollEvents()
{
    sf::Event ev;
    while (this->window->pollEvent(ev))
    {
        if (ev.type == sf::Event::Closed)
        {
            this->window->close();
        }
        else if (ev.type == sf::Event::MouseButtonPressed &&
                 this->thisPlayer == this->currentPlayer) //works as expected?
        {
            this->handleTurns();
        }
    }
}

//handle if press on mouse
void Game::handleTurns()
{
    //handle position stuff
    auto MouseData = sf::Mouse::getPosition(*this->window);
    //std::cout << sf::Mouse::getPosition(*this->window).x << std::endl;
    int column = Board::clickToPlace(MouseData.x);
    int row = Board::clickToPlace(MouseData.y);
    if (!chess)
    {
        if (!EngineBoard->isEmpty(row, column) &&
            EngineBoard->getColor(row, column) == this->currentPlayer) //FIXME
        {
            this->currentState = CHOOSE_PIECE;
            this->rowChoose = row;
            this->columnChoose = column;
        }
        else if (this->currentState == CHOOSE_PIECE)
        {
            this->currentState = CHOOSE_PLACE;
        }
        if (this->currentState == CHOOSE_PLACE)
        {
            //if the move is valid
            if (this->EngineBoard->move(row, column, this->rowChoose, this->columnChoose))
            {
                this->currentState = WAITING;
                this->currentPlayer = (this->currentPlayer == WHITE) ? BLACK : WHITE;
                std::cout << "sending move" << std::endl;
                this->netManager.sendMsg<std::array<int, 4>>(
                    {row, column, rowChoose, columnChoose});
            }
            //this->chess = EngineBoard->isInChess(currentPlayer); //FIXME
        }
    }
}

//run game
void Game::run()
{
    std::cin.get();                        //FIXME
    vector<string> ops = {"Play", "Quit"}; //game options
    Menu menu = Menu(WIDTH, HEIGHT, ops, this->window, 50, "prev.jpg");
    int res = 0;
    menu.run(res);
    if (res == 0)
    {
        while (this->window->isOpen())
        {
            this->update();
            this->render();
            if (this->chess)
            {
                Debug("Winner");
                exit(1);
                break;
            }
        }
    }
    this->window->close();
}