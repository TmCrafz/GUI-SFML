#include <SFML/Graphics.hpp>
#include "TextWidget.hpp"

int main()
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "GUI-SFML");

    gsf::TextWidget textWidget("Im a Text", sf::F, 12, sf::Color::White);


    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.display();
    }

    return 0;
}
