#ifndef TEXTINPUTWIDGET_HPP
#define TEXTINPUTWIDGET_HPP
#include <SFML/Graphics.hpp>
#include "Widget.hpp"
#include <string>

namespace gsf
{
    class TextInputWidget: public gsf::Widget
    {
        protected:

        private:
            sf::Text m_text;
            bool m_isFocused;
        public:
            TextInputWidget(float width, float height, sf::Font &font);

            void setText(const std::string &text);
            std::string getText() const;

            void setCharacterSize(const unsigned int size);
            unsigned int getCharacterSize() const;

            void setTextColor(const sf::Color color);
            sf::Color getTextColor() const;
            
            bool isFocused() const;

            virtual void drawWidget(sf::RenderTarget &target, 
                    sf::RenderStates states) const override;
            virtual void update(float dt) override;

        protected:

            virtual bool handleEvent(sf::Event &event) override;

        private:
            void init(std::string text, sf::Font &font, int characterSize, sf::Color color);

    };

}

#endif // TEXTINPUTWIDGET_HPP
