//Bishop class

//Bishop piece

#ifndef Bishop_H
#define Bishop_H
#include "../source/Piece.cpp"
#include <array>

class Bishop : public Piece
{
public:
    Bishop(int posX, int posY, const sf::Texture *texture, Color color, std::string name = "Bishop");
    ~Bishop();
    std::vector<std::pair<int, int>> getPossiblePositions(const std::array<std::array<Piece*,8>,8> &board) const;

private:
    static const std::array<std::pair<int, int>, 4> directions;
};

#endif