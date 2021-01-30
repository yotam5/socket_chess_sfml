#include "./source/Game.cpp"

//TODO: need to detect chess and check mate
//castling

int main(int argc, char *argv[])
{

    if (argc > 1)
    {
        std::cout << "param" << *argv[1] << std::endl;
        Game game(*argv[1]);
        game.run();
    }
    else
    {
        std::cout << "no param" << std::endl;
        Game game('w');
        game.run();
    }
}