#include "Widget.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>

gsf::Widget::Widget()
: m_width{ 0.f }
, m_height{ 0.f }
, m_bgColor{ sf::Color::Transparent }
, m_parent{ nullptr }
{

}


gsf::Widget::Widget(float width, float height)
: m_width{ width }
, m_height{ height }
, m_bgColor{ sf::Color::Transparent }
, m_parent{ nullptr }
{

}

gsf::Widget::~Widget()
{

}

void gsf::Widget::attachChild(Ptr child)
{
    child->m_parent = this;
    m_children.push_back(std::move(child));

    arrangeChildren();
    calculateSize();
}

gsf::Widget::Ptr gsf::Widget::detachChild(const Widget& node)
{
    auto found = std::find_if(m_children.begin(), m_children.end(), [&] (Ptr &p) -> bool { return p.get() == &node; });
    // There is an error when we try to detach a child which does not exists,so stop execution in debug mode
    assert(found != m_children.end());

    Ptr result = std::move(*found);
    result->m_parent = nullptr;
    m_children.erase(found);

    arrangeChildren();
    calculateSize();

    return result;
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
    return getWorldPosition().y - getOrigin().y + getHeight();
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
        states.transform *= getTransform();
        drawCurrent(target, states);
        drawChildren(target, states);
}

void gsf::Widget::drawCurrent(sf::RenderTarget &target, sf::RenderStates states) const
{
    //Do nothing by default
}

void gsf::Widget::drawChildren(sf::RenderTarget &target, sf::RenderStates states) const
{
    for (const Ptr &child : m_children)
    {
        child->draw(target, states);
    }
}

bool gsf::Widget::handleEvent(sf::Event &event)
{
    // If the widget return true, it has handled the event and
    // children should not handle it.
    if (!handleEventCurrent(event))
    {
        return handleEventChildren(event);
    }
    return true;
}

bool gsf::Widget::handleEventChildren(sf::Event &event)
{
    for (auto itr = m_children.rbegin(); itr != m_children.rend(); itr++)
    {
        if ((*itr)->handleEvent(event))
        {
            return true;
        }
    }
    return false;
}

bool gsf::Widget::handleEventCurrent(sf::Event &event)
{
    return false;
}

void gsf::Widget::update(float dt)
{
    updateCurrent(dt);
    updateChildren(dt);
}

void gsf::Widget::updateCurrent(float dt)
{
    // Do nothing by default
}

void gsf::Widget::updateChildren(float dt)
{
    for (const Ptr &child : m_children)
    {
        child->update(dt);
    }
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

void gsf::Widget::calculateSize()
{
    // Do nothing by default
}

void gsf::Widget::arrangeChildren()
{
    // Do nothing by default
}
