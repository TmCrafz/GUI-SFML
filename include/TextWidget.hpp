#ifndef TEXTWIDGET_HPP
#define TEXTWIDGET_HPP
#include <SFML/Graphics.hpp>
#include "Widget.hpp"
#include <string>

namespace gsf
{
    class TextWidget: public gsf::Widget
    {
        protected:

        private:
            sf::Text m_text;
        public:
            TextWidget();
            TextWidget(std::string text, sf::Font &font);
            TextWidget(std::string text, sf::Font &font, int characterSize);
            TextWidget(std::string text, sf::Font &font, int characterSize, 
                    sf::Color color);
            TextWidget(std::wstring text, sf::Font &font);
            TextWidget(std::wstring text, sf::Font &font, int characterSize);
            TextWidget(std::wstring text, sf::Font &font, int characterSize, 
                    sf::Color color);

            virtual ~TextWidget();
            
            //sf::Text& getText();
            void setText(const sf::String &text);
            void setText(const std::wstring text);
            sf::String getText() const;
            // get text as wide string
            std::wstring getWideText() const;
           
            void setFont(const sf::Font &font);
            const sf::Font& getFont() const;

            void setCharacterSize(const unsigned int size);
            unsigned int getCharacterSize() const;

            void setTextColor(const sf::Color color);
            sf::Color getTextColor() const;

            sf::Vector2f findCharacterPos(std::size_t index) const;
            
            // Returns the width and height of the given char which it have
            // with the used character size and font
            sf::Vector2f getWidthAndHeightOfChar(wchar_t c) const;
            // Returns the local bounds of the char of the given index
            sf::FloatRect getLocalBoundsOfChar(std::size_t i) const;

            virtual void drawWidget(sf::RenderTarget &target, 
                    sf::RenderStates states) const override;
            virtual void update(float dt) override;

        protected:
            virtual bool handleEvent(sf::Event &event) override;

        private:
            void init(std::string text, sf::Font &font, int characterSize, 
                    sf::Color color);

            // Calculate the size of the current widget
            virtual void calculateSize() override;
    };

}

#endif // TEXTWIDGET_HPP
