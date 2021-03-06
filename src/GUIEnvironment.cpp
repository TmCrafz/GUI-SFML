#include "../include/Widgets/Widget.hpp"
#include "../include/GUIEnvironment.hpp"
#include "../include/Orientation.hpp"
#include "../include/Widgets/ButtonWidget.hpp"
#include "../include/Widgets/CheckBoxWidget.hpp"
#include "../include/Widgets/ComboBoxWidget.hpp"
#include "../include/Widgets/ConsoleWidget.hpp"
#include "../include/Widgets/ListBoxWidget.hpp"
#include "../include/Widgets/ProgressWidget.hpp"
#include "../include/Widgets/ScrollableWidget.hpp"
#include "../include/Widgets/TextButtonWidget.hpp"
#include "../include/Widgets/TextInputWidget.hpp"
#include "../include/Widgets/TextWidget.hpp"
#include "../include/Widgets/VerticalLayoutWidget.hpp"
#include "../include/Widgets/HorizontalLayoutWidget.hpp"
#include "../include/Widgets/Widget.hpp"
#include "../include/Widgets/WindowWidget.hpp"
#include <iostream>
#include <cassert>

gsf::GUIEnvironment::GUIEnvironment(const sf::RenderWindow &window)
: m_window{ window } 
, m_isMouseInWindow{ true }
, m_isWindowFocused{ true }
, m_lastUpdateTime{ CLOCK::now() }
//, m_isWindowRoughOutEnabled{ false }
{

}

gsf::GUIEnvironment::~GUIEnvironment()
{
}

void gsf::GUIEnvironment::addWidget(Widget::Ptr widget, bool applyGlobalTheme)
{
    widget->setContext(this);
    if (applyGlobalTheme && m_globalThemePath != "")
    {
        widget->applyTheme("assets/themes/BlackWhite.xml");
    }
    placeWidget(widget.get());
    m_widgets.push_back(std::move(widget));
}

/*
void gsf::GUIEnvironment::setIsWindowRoughOutEnabled(bool isEnabled)
{
    m_isWindowRoughOutEnabled = isEnabled;
}

bool gsf::GUIEnvironment::isWindowRoughOutEnabled() const
{
    return m_isWindowRoughOutEnabled;
}
*/

gsf::Widget::Ptr gsf::GUIEnvironment::removeWidget(const Widget& widget)
{
    auto found = std::find_if(m_widgets.begin(), m_widgets.end(), 
            [&] (Widget::Ptr &p) { return p.get() == &widget; });
    // There is an error when we try to detach a widget which does not exists, 
    // so stop execution in debug mode
    assert(found != m_widgets.end());
    Widget::Ptr result = std::move(*found);
    result->removeContext();
    m_widgets.erase(found);
    return result;
}

void gsf::GUIEnvironment::createScene(const std::string &path)
{
    tinyxml2::XMLDocument document;
    if (document.LoadFile(path.c_str()) != tinyxml2::XML_SUCCESS)
    {
        std::cout << "Error by loading scene. Scene file path: " << path << std::endl;
        return;
    }
    tinyxml2::XMLElement *sceneEl{ document.FirstChildElement("Scene") };
    if (!sceneEl)
    {
        std::cout << "Error by loading scene: No valid scene file. Scene file path: "
           << path << std::endl;
        return;
    }
    if (!loadResources(sceneEl))
    {
        std::cout << "Error by loading scene resources. Scene file path: " << path 
            << std::endl;
        return;
    }
    if (!loadWidgets(sceneEl))
    {
        std::cout << "Error by loading widgets. Scene file path: " << path 
            << std::endl;
        return;
    }
}

bool gsf::GUIEnvironment::loadResources(tinyxml2::XMLElement *sceneEl)
{
    tinyxml2::XMLElement *resEl{ sceneEl->FirstChildElement("Resources") };
    if (!resEl)
    {
        return false;
    }
    // Load fonts
    m_fonts.clear();
    for (const tinyxml2::XMLElement *a{ resEl->FirstChildElement("Font") }; a; 
            a = a->NextSiblingElement("Font"))
    {
        // Load id and font path when both is there
        if (a->Attribute("id") && a->Attribute("src"))
        {
            std::string id = a->Attribute("id");
            std::string src = a->Attribute("src");
            m_fonts.load(id, src);
        }
    }
    return true;
}

void gsf::GUIEnvironment::loadWidgetsRecur(tinyxml2::XMLElement *widgetsEl, 
        gsf::Widget* parentWidget)
{
    for (tinyxml2::XMLElement *el{ widgetsEl->FirstChildElement() }; el; 
            el = el->NextSiblingElement())
    {
        // Load widgets attributes
        std::map<std::string, std::string> attributes;
        for (const tinyxml2::XMLAttribute *a{ el->FirstAttribute() }; a; 
            a = a->Next())
        {
            std::string name{ a->Name() };
            std::string value{ a->Value() };
            attributes[name] = value;
        }
        // Load id, when available
        std::string widgetId = "";
        auto foundId = attributes.find("id");
        bool isIdDefined{ foundId != attributes.end() };
        if (isIdDefined)
        {
            widgetId = attributes["id"];
        }
        // Check if there is a font specified in attributes and if the specified
        // font is loaded
        auto foundFont = attributes.find("font");
        bool isFontDefined{ foundFont != attributes.end() };
        sf::Font *font{ nullptr };
        std::string fontId = "";
        if (isFontDefined)
        {
            fontId = attributes["font"];
            if (m_fonts.exists(fontId))
            {
                font = &m_fonts.get(fontId);
            }
        }
        // When a specified font was not loaded or no font specified, for widgets
        // which need a font, the var get true and the user get notified later
        bool tryToLoadWidgetWithoutFont{ false };
        std::string widgetName{ el->Name() };
        Widget::Ptr widget{ nullptr };
        if (widgetName == "Widget")
        {
            widget = Widget::create();           
        }
        else if (widgetName == "ButtonWidget")
        {
            widget = ButtonWidget::create();
        }
        else if (widgetName == "CheckBoxWidget")
        {
            widget = CheckBoxWidget::create();
        }
        else if (widgetName == "ComboBoxWidget")
        {
            if (font != nullptr)
            {
                widget = ComboBoxWidget::create(*font);
            } 
            else
            {
                tryToLoadWidgetWithoutFont = true;
            }
        }
        else if (widgetName == "ConsoleWidget")
        {
            if (font != nullptr)
            {
                widget = ConsoleWidget::create(*font);
            }
            else
            {
                tryToLoadWidgetWithoutFont = true;
            }
        }
        else if (widgetName == "ListBoxWidget")
        {
            if (font != nullptr)
            {
                widget = ListBoxWidget::create(*font);
            }
            else
            {
                tryToLoadWidgetWithoutFont = true;
            }
        }
        else if (widgetName == "ProgressWidget")
        {
            widget = ProgressWidget::create();
        }
        else if (widgetName == "ScrollableWidget")
        {
            widget = ScrollableWidget::create();
        }
        else if (widgetName == "TextButtonWidget")
        {
            if (font != nullptr)
            {
                widget = TextButtonWidget::create(*font);
            }
            else
            {
                tryToLoadWidgetWithoutFont = true;
            }
        }
        else if (widgetName == "TextInputWidget")
        {
            if (font != nullptr)
            {
                widget = TextInputWidget::create(*font);
            }
            else
            {
                tryToLoadWidgetWithoutFont = true;
            }
        }
        else if (widgetName == "TextWidget")
        {
            if (font != nullptr)
            {
                widget = TextWidget::create(*font);
            }
            else
            {
                tryToLoadWidgetWithoutFont = true;
            }
        }
        else if (widgetName == "VerticalLayoutWidget")
        {
            widget = VerticalLayoutWidget::create();
        }
        else if (widgetName == "HorizontalLayoutWidget")
        {
            widget = HorizontalLayoutWidget::create();
        }
        else if (widgetName == "WindowWidget")
        {
            if (font != nullptr)
            {
                widget = WindowWidget::create(*font);
            }
            else
            {
                tryToLoadWidgetWithoutFont = true;
            }
        }
        if (tryToLoadWidgetWithoutFont)
        {
            std::cout <<  widgetName << " with id: \""<< widgetId << "\" was not "
                << "created. The widget need a font, but there was no font specified,"
                << " or specified font cant get loaded.\n";
        }
        else
        {
            widget->setID(widgetId);
            Widget *widgetPtr{ widget.get() };
            // Load child widgets
            loadWidgetsRecur(el, widgetPtr);
            widgetPtr->applyAttributes(attributes);
            if (!parentWidget)
            {
                addWidget(std::move(widget));
            }
            else
            {
                parentWidget->attachChild(std::move(widget));
            }
            if (!parentWidget)
            {
                placeWidget(widgetPtr);
            }
        }
    }
}

bool gsf::GUIEnvironment::loadWidgets(tinyxml2::XMLElement *sceneEl)
{
    tinyxml2::XMLElement *widgetsEl{ sceneEl->FirstChildElement("Widgets") };
    if (!widgetsEl)
    {
        return false;
    }
    // Load Widgets
    m_widgets.clear();
    loadWidgetsRecur(widgetsEl, nullptr);
    return true;
}

sf::View gsf::GUIEnvironment::getCurrentView() const
{
    return m_window.getView();
}

gsf::Widget* gsf::GUIEnvironment::getWidgetByID(const std::string &id) const
{
    for (const auto &widget : m_widgets)
    {
        if (widget->getID() == id)
        {
            return widget.get();
        }
        Widget* childTmp{ widget->getChildWidgetByID(id) };
        if (childTmp != nullptr)
        {
            return childTmp;
        }
    }
    return nullptr;
}

void gsf::GUIEnvironment::loadFont(const std::string &id, const std::string &path)
{
    m_fonts.load(id, path);

}

sf::Font& gsf::GUIEnvironment::getFont(const std::string &id) const
{
    return m_fonts.get(id);
}

void gsf::GUIEnvironment::placeWidget(Widget *widget)
{
    int orientation { widget->getOrientation() };
    if (orientation & Orientation::Left)
    {
        widget->setLeftPosition(0.f);
    }
    if (orientation & Orientation::Right)
    {
        widget->setRightPosition(getCurrentView().getSize().x);
    }
    if (orientation & Orientation::Top)
    {
        widget->setTopPosition(0.f);
    }
    if (orientation & Orientation::Bottom)
    {
        widget->setBottomPosition(getCurrentView().getSize().y);
    }
    if (orientation & Orientation::Center)
    {
        sf::Vector2f size{ getCurrentView().getSize() };
        widget->setCenterPosition(size.x / 2.f, size.y / 2.f);
    }
    if (orientation & Orientation::CenterHorizontal)
    {
        sf::Vector2f size{ getCurrentView().getSize() };
        widget->setHorizontalCenterPosition(size.x / 2.f);
    }
    if (orientation & Orientation::CenterVertical)
    {
        sf::Vector2f size{ getCurrentView().getSize() };
        widget->setVerticalCenterPosition(size.y /2.f);
    }
    widget->placeChildWidgets();
}

void gsf::GUIEnvironment::replaceWidgets()
{
    for (const Widget::Ptr &widget : m_widgets)
    {
        placeWidget(widget.get());
    }
}

bool gsf::GUIEnvironment::handleEvent(sf::Event &event)
{
    /*
    if (event.type == sf::Event::MouseLeft)
    {
        m_isMouseInWindow = false;
    }
    else if(event.type == sf::Event::MouseEntered)
    {
        m_isMouseInWindow = true;
    }
    if (event.type == sf::Event::LostFocus)
    {
       m_isWindowFocused = false;
    }
    if (event.type == sf::Event::GainedFocus)
    {
        m_isWindowFocused = true;
    }
    */

    // Dont handle mouse move events when mouse is not inside window or 
    // window is not focussed
    if (event.type == sf::Event::MouseMoved && 
            (!m_isMouseInWindow || !m_isWindowFocused))
    {
        return true;
    }

    // Iterate backwards, because the widgets which are shown at the front of 
    // the window have a higher priority forevent handling
    // (Widgets drawn at the front are the ones which are at the end of the vector)
    for (auto it = m_widgets.rbegin(); it != m_widgets.rend(); it++)
    {
        if ((*it)->handleEvent(event, m_window))
        {
            // Event handled so we have nothing to handly anymore.
            // So we can ensure that a event is handled by the window in the front
            // and not affect a window behind it.
                   
            return true;
        }
    }
    return false;
}

void gsf::GUIEnvironment::update(float dt)
{
    for (const Widget::Ptr &widget : m_widgets)
    {
        widget->update(dt);
    }
    // Move widget to foreground which are marked for this
    // (Forground at the window, which is the last item in our vector)
    bool moved{ false };
    do
    {
        moved = false;
        for (auto it = m_widgets.begin(); it != m_widgets.end(); it++)
        {
            if ((*it)->isMarkedForMoveToForeground()) {
                (*it)->setMoveToForground(false);
                // Move actual object to end of vector
                std::rotate(it, it + 1 , m_widgets.end());
                // Leave actual loop and start again from beginning, 
                // because vector has changed
                moved = true;
                break;
            }
        }
    } while (moved);
    // Remove widgets which signals that there are removeable
    m_widgets.erase(std::remove_if(m_widgets.begin(), m_widgets.end(),
                    [] (Widget::Ptr &widget) { return widget->isRemoveable(); }),
                    m_widgets.end());
}

void gsf::GUIEnvironment::update()
{
    std::chrono::duration<float> timeSpan{ CLOCK::now() - m_lastUpdateTime };
    m_lastUpdateTime = CLOCK::now();
    // Get deltaTime as float in seconds
    float dt{ std::chrono::duration_cast<std::chrono::duration
        <float, std::ratio<1>>>(timeSpan).count() };
    update(dt);
}

void gsf::GUIEnvironment::draw(sf::RenderTarget &target, 
        sf::RenderStates states) const
{
    for (const Widget::Ptr &widget : m_widgets)
    {
        widget->draw(target, states, target.getView());
    }
}
