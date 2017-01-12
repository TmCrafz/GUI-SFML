#include "Widget.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>

gsf::Widget::Widget()
: m_width{ 0.f }
, m_height{ 0.f }
, m_bgColor{ sf::Color::Transparent }
, m_parent{ nullptr }
, m_moveToForeground{ false }
, m_isRemoveable{ false }
, m_isVisible{ true }
{

}


gsf::Widget::Widget(float width, float height)
: m_width{ width }
, m_height{ height }
, m_bgColor{ sf::Color::Transparent }
, m_parent{ nullptr }
, m_moveToForeground{ false }
, m_isRemoveable{ false }
, m_isVisible{ true }
{

}

gsf::Widget::~Widget()
{

}

void gsf::Widget::setParent(Widget *parent)
{
    m_parent = parent;
}

gsf::Widget* gsf::Widget::getParent() const
{
    return m_parent;
}

void gsf::Widget::setMoveToForground(bool moveToForeground)
{
    m_moveToForeground = moveToForeground;
}

bool gsf::Widget::isMarkedForMoveToForeground() const
{
    return m_moveToForeground;
}

void gsf::Widget::setIsRemoveable(bool isRemoveable)
{
    m_isRemoveable = isRemoveable;
}

bool gsf::Widget::isRemoveable() const
{
    return m_isRemoveable;
}

void gsf::Widget::setIsVisible(bool isVisible)
{
    m_isVisible = isVisible;
}

bool gsf::Widget::isVisible() const
{
    return m_isVisible;
}

void gsf::Widget::setOnLeftClickListener(std::function<void(Widget *widget, sf::Vector2f)> onLeftClickListener)
{
    m_onLeftClickListener = onLeftClickListener;
}

void gsf::Widget::setWidth(const float width)
{
    m_width = width;
}

float gsf::Widget::getWidth() const
{
    return m_width;
}

void gsf::Widget::setHeight(const float height)
{
    m_height = height;
}

float gsf::Widget::getHeight() const
{
    return m_height;
}

float gsf::Widget::getLeft() const
{
    return getPosition().x - getOrigin().x;
}

float gsf::Widget::getRight() const
{
    return getPosition().x - getOrigin().x + getWidth();
}

float gsf::Widget::getTop() const
{
    return getPosition().y - getOrigin().y;
}

float gsf::Widget::getBottom() const
{
    return getPosition().y - getOrigin().y + getHeight();
}

float gsf::Widget::getWorldLeft() const
{
    return getWorldPosition().x - getOrigin().x;
}

float gsf::Widget::getWorldRight() const
{
    return getWorldPosition().x - getOrigin().x + getWidth();
}

float gsf::Widget::getWorldTop() const
{
    return getWorldPosition().y - getOrigin().y;
}

float gsf::Widget::getWorldBottom() const
{
    return getWorldPosition().y - getOrigin().y + getHeight();
}

void gsf::Widget::setBackgroundColor(const sf::Color color)
{
    m_bgColor = color;
}

sf::Color gsf::Widget::getBackgroundColor() const
{
    return m_bgColor;
}

void gsf::Widget::centerOrigin()
{
    setOrigin(getWidth() / 2.f, getHeight() / 2.f);
}

void gsf::Widget::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    if (m_isVisible)
    {
        drawWidget(target, states);
    }
}

void gsf::Widget::drawWidget(sf::RenderTarget &target, sf::RenderStates states) const
{
    // Do nothing by default
}

bool gsf::Widget::handleEventWidget(sf::Event &event)
{
    if(!isVisible())
    {
        return false;
    }
    return handleEvent(event);
}

bool gsf::Widget::handleEvent(sf::Event &event)
{
    if (event.type == sf::Event::MouseButtonPressed)
    {
        if (event.mouseButton.button == sf::Mouse::Left && isIntersecting(sf::Vector2f(event.mouseButton.x , event.mouseButton.y)))
        {
            if (m_onLeftClickListener)
            {
                m_onLeftClickListener(this, sf::Vector2f(event.mouseButton.x , event.mouseButton.y));
            }
            return true;
        }
    }
    return false;
}

void gsf::Widget::update(float dt)
{
    // Do nothing by default
}

sf::Transform gsf::Widget::getWorldTransform() const
{
    sf::Transform trform = { sf::Transform::Identity };
    for (const Widget *node = this; node != nullptr; node = node->m_parent)
    {
        trform = node->getTransform() * trform;
    }
    return trform;
}

sf::Vector2f gsf::Widget::getWorldPosition() const
{
    // Get world position ignores the real position when the origin is set ( the position is every time the one in the upper left corner)
    // so we add the origin here.
    return ( getWorldTransform() * sf::Vector2f() ) + getOrigin();
}

sf::View gsf::Widget::getShownAreaView(sf::RenderTarget &target) const
{
    sf::View view;
    // The view should have the same size as the widget, so the shown area of the widget is never bigger than the size of the widget,
    // although when containing widgets of the widget are bigger.
    view.setSize(getWidth(), getHeight());

    view.setCenter(getWorldLeft() + (getWidth() / 2.f), getWorldTop() + (getHeight() / 2.f) );

    float viewLeft = ( getWorldLeft() ) / target.getSize().x;
    float viewTop = ( getWorldTop() ) / target.getSize().y;
    float viewWidth = getWidth() / target.getSize().x;
    float viewHeight = getHeight() / target.getSize().y;

    // The viewport is the area where the widget is on screen
    view.setViewport(sf::FloatRect(viewLeft , viewTop , viewWidth, viewHeight));

    // View based. This has the advantage in opposite to position based, that not only the parents area
    // is respected, but also the parents parent etc...
    if (m_parent != nullptr)
    {
         // We have to get the parents viewport, so we dont ignore its viewport.
        // (The views of this view have to be in the viewport of the actuals widgets parent, too
        sf::View parentView = { m_parent->getShownAreaView(target) };
        sf::FloatRect parentViewPort = { parentView.getViewport() };

        // this Widget is widgetA parent widget is widgetB
        float leftA = { viewLeft };
        float rightA = { viewLeft + viewWidth};
        float topA = { viewTop };
        float bottomA = { viewTop + viewHeight };

        float leftB = { parentViewPort.left };
        float rightB = { parentViewPort.left + parentViewPort.width };
        float topB = { parentViewPort.top };
        float bottomB = { parentViewPort.top + parentViewPort.height };
        // Calculate where the viewport starts. (The startpoint should be right of the left side of its parent and
        // under the top side of its parent. Is the startpoint right of the right side of its parent or
        // under the bottom side of its parent the overlapping area is zero, so nothing is drawn
        float viewportLeftNew = { std::max(leftA, leftB) };
        float viewportTopNew = { std::max(topA, topB) };
        // The start of the view in pixels
        float pixelLeft = { viewportLeftNew * target.getSize().x };
        float pixelTop = { viewportTopNew * target.getSize().y };


        // The shown size should only have the size of the area of the widget which is on the parent widget
        // For this we need the intersecting area of the this widget with its parent
        float overlapX = { std::max(0.f, std::min(rightA, rightB) - std::max(leftA, leftB)) };
        float overlapY = { std::max(0.f, std::min(bottomA, bottomB) - std::max(topA, topB)) };
        // We need the pixels instead of the size in viewport range, so we convert them back
        float pixelOverlapX = { overlapX * target.getSize().x };
        float pixelOverlapY = { overlapY * target.getSize().y };


        view.setSize(pixelOverlapX , pixelOverlapY);
        view.setCenter(pixelLeft + (pixelOverlapX / 2.f), pixelTop + (pixelOverlapY / 2.f) );
        view.setViewport(sf::FloatRect(viewportLeftNew , viewportTopNew , overlapX, overlapY));
    }
    /*
    // position based
    if (m_parent != nullptr)
    {
        // Do to: use parentView values instead of parents world values to calculate the view right
        // We have to get the parents viewport, so we dont ignore its viewport.
        // (The views of this view have to be in the viewport of the actuals widgets parent, too
        sf::View parentView = { m_parent->getShownAreaView(target) };

        // this Widget is widgetA parent widget is widgetB
        float leftA = { getWorldLeft() };
        float rightA = { getWorldRight() };
        float topA = { getWorldTop() };
        float bottomA = { getWorldBottom() };

        float leftB = { m_parent->getWorldLeft() };
        float rightB = { m_parent->getWorldRight() };
        float topB = { m_parent->getWorldTop() };
        float bottomB = { m_parent->getWorldBottom() };
        // Calculate where the viewport starts. (The startpoint should be right of the left side of its parent and
        // under the top side of its parent. Is the startpoint right of the right side of its parent or
        // under the bottom side of its parent the overlapping area is zero, so nothing is drawn
        float startX = { std::max(leftA, leftB) };
        float startY = { std::max(topA, topB) };
        float viewportStartX = { startX / target.getSize().x };
        float viewportStartY = { startY / target.getSize().y };

        // The shown size should only have the size of the area of the widget which is on the parent widget
        // For this we need the intersecting area of the this widget with its parent
        float overlapX = { std::max(0.f, std::min(rightA, rightB) - std::max(leftA, leftB)) };
        float overlapY = { std::max(0.f, std::min(bottomA, bottomB) - std::max(topA, topB)) };

        float viewportWidth = { overlapX / target.getSize().x };
        float viewportHeight = { overlapY / target.getSize().y };
        view.setSize(overlapX, overlapY);
        view.setCenter(startX + (overlapX / 2.f), startY + (overlapY / 2.f) );
        view.setViewport(sf::FloatRect(viewportStartX , viewportStartY , viewportWidth, viewportHeight));
    }
    */
    return view;
}

bool gsf::Widget::isIntersecting(sf::Vector2f pos) const
{
    return pos.x >= getWorldLeft() && pos.x <= getWorldRight() && pos.y >= getWorldTop() && pos.y <= getWorldBottom();
}

void gsf::Widget::calculateSize()
{
    // Do nothing by default
}
