#include"TextInputWidget.hpp"
#include<iostream>

gsf::TextInputWidget::TextInputWidget(float width, float height, sf::Font &font)
:Widget(width, height)
{
    m_text.setFont(font);
    m_text.setCharacterSize(12);
    m_text.setFillColor(sf::Color::Black);
}

void gsf::TextInputWidget::setText(const std::string &text)
{
   m_text.setString(text);
}

std::string gsf::TextInputWidget::getText() const
{
   return m_text.getString().toAnsiString();
}

void gsf::TextInputWidget::setCharacterSize(const unsigned int size)
{
   m_text.setCharacterSize(size);
}

unsigned int gsf::TextInputWidget::getCharacterSize() const
{
   return m_text.getCharacterSize();
}

void gsf::TextInputWidget::setTextColor(const sf::Color color)
{
   m_text.setFillColor(color);
}

sf::Color gsf::TextInputWidget::getTextColor() const
{
   return m_text.getFillColor();
}

bool gsf::TextInputWidget::isFocused() const
{
    return m_isFocused;
}

void gsf::TextInputWidget::drawWidget(sf::RenderTarget &target, 
        sf::RenderStates states) const
{
   // Draw text
   target.draw(m_text, states);
}

void gsf::TextInputWidget::update(float dt)
{
   // Do nothing by default
}

bool gsf::TextInputWidget::handleEvent(sf::Event &event)
{
    bool handled{ Widget::handleEvent(event) };
    // Check if actual Widget is focused
    if (event.type == sf::Event::MouseButtonPressed)
    {        
        sf::Vector2f mousePos{ (float) event.mouseButton.x, 
        (float) event.mouseButton.y };
        bool isMouseInShownArea{ getShownArea().contains(mousePos) };
        bool intersecting{ isIntersecting(mousePos) };
        if (isMouseInShownArea && intersecting)
        {
            m_isFocused = true;
        }
        else
        {
            m_isFocused = false;
        }
    }
    // If Widget is focused and Text entered, handle entered text
    if (m_isFocused && event.type == sf::Event::TextEntered)
    {
        std::string text{ getText() };
        char entered{ static_cast<char>(event.text.unicode) };
        std::cout << "Entered Text: " << entered
             << std::endl;
        text += entered;
        setText(text);
        return true;
    }
    return handled;
}
