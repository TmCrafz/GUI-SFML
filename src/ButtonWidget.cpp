#include "ButtonWidget.hpp"
#include <iostream>

gsf::ButtonWidget::ButtonWidget()
: Widget{  }
, m_isHovering{ false }
, m_hoverFillColor{ sf::Color::Black }
{
    init();
}

gsf::ButtonWidget::ButtonWidget(float width, float height)
: Widget(width, height)
, m_isHovering{ false }
, m_hoverFillColor{ sf::Color::Black }
{
    init();
}

void gsf::ButtonWidget::init()
{
    m_outlineThickness = 2.f;
    m_outlineColor = sf::Color::Black;
}

sf::Color gsf::ButtonWidget::getHoverFillColor() const
{
    return m_hoverFillColor;
}

void gsf::ButtonWidget::setHoverFillColor(sf::Color color)
{
    m_hoverFillColor = color;
}

void gsf::ButtonWidget::drawWidget(sf::RenderTarget &target, sf::RenderStates states) const
{
    // Draw hover color as background color if button is in hover mode
    // To do (maybe): Let it handle from widget class. Instead of drawing the hover
    // background over the normal background, we can set the normal background to the
    // hover fill color. But it is necessary to store the normal color inside this
    // class so it can get restored after hovering ends
    if (m_isHovering)
    {
        sf::RectangleShape shape{ { getWidth(), getHeight() } };
        shape.setFillColor(m_hoverFillColor);
        target.draw(shape, states);
    }
}

void gsf::ButtonWidget::update(float dt)
{
    // Do nothing by default
}

bool gsf::ButtonWidget::handleEvent(sf::Event &event)
{
    bool handled = Widget::handleEvent(event);
    if (event.type == sf::Event::MouseMoved)
    {
        sf::Vector2f mousePos{ (float) event.mouseMove.x, (float) event.mouseMove.y };
        bool intersects{ isIntersecting(mousePos) };
        bool isInShownArea{ getShownArea().contains(mousePos) };
        if (intersects && isInShownArea)
        {
            m_isHovering = true;
        }
        else if (m_isHovering)
        {
            m_isHovering = false;
        }
    }
    return handled;
}
