#include "../../include/Widgets/TextInputWidget.hpp"
#include "../../include/Utility.hpp"
#include <iostream>

gsf::TextInputWidget::Ptr gsf::TextInputWidget::create(sf::Font &font)
{
    Ptr widget{ std::make_unique<TextInputWidget>(font) };
    return widget;
}

gsf::TextInputWidget::Ptr gsf::TextInputWidget::create(float width, float height, 
        sf::Font &font)
{
    Ptr widget{ std::make_unique<TextInputWidget>(width, height, font) };
    return widget;
}

gsf::TextInputWidget::TextInputWidget(sf::Font &font)
: Widget{ }
//, m_text{ "", font, 12, sf::Color::Black }
, m_text{ nullptr }
, m_font{ font }
, m_charSize{ 12 }
, m_isEditable{ true }
, m_cursor{ "|", font, m_charSize }
, m_cursorColor{ sf::Color::Black }
, m_scrollable{ nullptr }
, m_acceptNewLines{ true }
, m_isFocused{ false }
, m_cursorPos{ 0 }
, m_lBreaksBefCur{ 0 }
, m_isCursorShown{ true }
, m_blinkFreq{ 0.6f }
, m_lastBlinkTime{ 0.f }
, m_whiteListChars{ L"" }
, m_blackListChars{ L"" }
, m_minBreakCharCnt{ 0 }
{
    init();
}
gsf::TextInputWidget::TextInputWidget(float width, float height, 
        sf::Font &font)
: Widget{ width, height }
//, m_text{ "", font, 12, sf::Color::Black }
, m_text{ nullptr }
, m_font{ font }
, m_charSize{ 12 }
, m_isEditable{ true }
, m_cursor{ "|", font, m_charSize }
, m_cursorColor{ sf::Color::Black }
, m_scrollable{ nullptr }
, m_acceptNewLines{ true }
, m_isFocused{ false }
, m_cursorPos{ 0 }
, m_lBreaksBefCur{ 0 }
, m_isCursorShown{ true }
, m_blinkFreq{ 0.6f }
, m_lastBlinkTime{ 0.f }
, m_whiteListChars{ L"" }
, m_blackListChars{ L"" }
, m_minBreakCharCnt{ 0 }
{
    init();
}

void gsf::TextInputWidget::init()
{
    std::unique_ptr<TextWidget> text{ 
        std::make_unique<TextWidget>("", m_font, m_charSize, sf::Color::Black) };
    std::unique_ptr<ScrollableWidget> scrollable{ 
        std::make_unique<ScrollableWidget>(
                getWidth(), 
                getHeight()) };
    m_scrollable = scrollable.get();
    m_text = text.get();
    scrollable->setBackgroundColor(sf::Color::Transparent);
    scrollable->attachChild(std::move(text));
    // Change content area so that the scrollbar fits in the TextInputWidget when
    // necassary.
    scrollable->setOnVerticalScrollNeededChangedListener(
            [](Widget* widget, bool isNeeded)
            {
                // Vertical Scrollbar is needed so we reduce the width of the content
                // area, so that the scrollbar fits in the TextInputWidget
                if (isNeeded)
                {
                    widget->setWidth(widget->getWidth() 
                            - ScrollableWidget::SCROLLBAR_THICKNESS);
                }
                // Vertical Scrollbar is no longer so we can make the 
                // reductoion undone
                else
                {                    
                    widget->setWidth(widget->getWidth() 
                            + ScrollableWidget::SCROLLBAR_THICKNESS);
                }
            });
    attachChild(std::move(scrollable));
    m_cursor.setFillColor(m_cursorColor);
    setOutlineThickness(4.f);
}

void gsf::TextInputWidget::loadAttributes(tinyxml2::XMLDocument &document, 
                std::map<std::string, std::string> &attributes)
{
    Widget::loadAttributes(document, attributes);
    Widget::loadAttributes(document, attributes, "TextInputWidget");
}

void gsf::TextInputWidget::applyAttribute(const std::string &name, 
        const std::string &value)
{
    Widget::applyAttribute(name, value);
    if (name == "characterSize")
    {
        int charSize{ std::stoi(value) };
        setCharacterSize(charSize);
    }
    else if (name == "cursorColor")
    {
        sf::Color color{ Utility::stringToColor(value) };
        setCursorColor(color);
    }
    else if (name == "isEditable")
    {
        bool val{ Utility::stringToBool(value) };
        setIsEditable(val);
    }
    else if(name == "textColor")
    {
        sf::Color color { Utility::stringToColor(value) };
        setTextColor(color);
    }
    else if (name == "isNewLineAccepted")
    {
        bool val { Utility::stringToBool(value) };
        setIsNewLineAccepted(val);
    }
    else if (name == "isNewLineAccepted")
    {
        bool val { Utility::stringToBool(value) };
        setIsNewLineAccepted(val);
    }
    else if (name == "isVerticalScrollEnabled")
    {
        bool val { Utility::stringToBool(value) };
        setIsVerticalScrollEnabled(val);
    }
    else if (name == "isHorizontalScrollEnabled")
    {
        bool val { Utility::stringToBool(value) };
        setIsHorizontalScrollEnabled(val);
    }
    else if (name == "isVerticalScrollbarDrawn")
    {
        bool val { Utility::stringToBool(value) };
        setIsVerticalScrollbarDrawn(val);
    }
    else if (name == "isHorizontalScrollbarDrawn")
    {
        bool val { Utility::stringToBool(value) };
        setIsHorizontalScrollbarDrawn(val);
    }
    else if (name == "blackListChars")
    {
        setBlackListChars(value);
    }
    else if (name == "whiteListChars")
    {
        setWhiteListChars(value);
    }
    else if (name == "text")
    {
        setText(value);
    }
}

void gsf::TextInputWidget::setCursorColor(sf::Color color)
{
    m_cursorColor = color;
    m_cursor.setFillColor(color);
}

sf::Color gsf::TextInputWidget::getCursorColor() const
{
    return m_cursorColor;
}

void gsf::TextInputWidget::setIsEditable(bool isEditable)
{
    m_isEditable = isEditable;
}

bool gsf::TextInputWidget::isEditable() const
{
    return m_isEditable;
}

void gsf::TextInputWidget::setText(const sf::String &text)
{
    m_currentText = text;
    m_text->setText(m_currentText);
    // Move cursor to end of text
    m_cursorPos = m_currentText.getSize();
    // Adjust text so that it fits the scrollbar when horizontal scrolling is disabled
    adjustShownText();
    m_scrollable->recalculateScroll();
    m_scrollable->scrollToBottom();
    m_scrollable->scrollToLeft();
}

sf::String gsf::TextInputWidget::getText() const
{
    return m_currentText;
}

void gsf::TextInputWidget::setCharacterSize(const unsigned int size)
{
    m_text->setCharacterSize(size);
    m_charSize = size;
    m_cursor.setCharacterSize(size);
}

unsigned int gsf::TextInputWidget::getCharacterSize() const
{
    return m_charSize;
}

void gsf::TextInputWidget::setTextColor(const sf::Color color)
{
    m_text->setTextColor(color);
}

sf::Color gsf::TextInputWidget::getTextColor() const
{
    return m_text->getTextColor();
}

void gsf::TextInputWidget::setIsNewLineAccepted(bool isAccepted)
{
    m_acceptNewLines = isAccepted;
}

bool gsf::TextInputWidget::getIsNewLineAccepted() const
{
    return m_acceptNewLines;
}

bool gsf::TextInputWidget::isFocused() const
{
    return m_isFocused;
}

void gsf::TextInputWidget::setIsVerticalScrollEnabled(bool isEnabled)
{
    m_scrollable->setIsVerticalScrollEnabled(isEnabled);
}

bool gsf::TextInputWidget::isVerticalScrollEnabled() const
{
    return m_scrollable->isVerticalScrollEnabled();
}

void gsf::TextInputWidget::setIsHorizontalScrollEnabled(bool isEnabled)
{
    m_scrollable->setIsHorizontalScrollEnabled(isEnabled);
}

bool gsf::TextInputWidget::isHorizontalScrollEnabled() const
{
    return m_scrollable->isHorizontalScrollEnabled();
}

void gsf::TextInputWidget::setIsVerticalScrollbarDrawn(bool isDrawn)
{
    m_scrollable->setIsVerticalScrollbarDrawn(isDrawn);
}

void gsf::TextInputWidget::setIsHorizontalScrollbarDrawn(bool isDrawn)
{
    m_scrollable->setIsHorizontalScrollbarDrawn(isDrawn);
}

sf::String gsf::TextInputWidget::getBlackListChars() const
{
    return m_blackListChars;
}

void gsf::TextInputWidget::setBlackListChars(sf::String chars)
{
    m_blackListChars = chars;
}

sf::String gsf::TextInputWidget::getWhiteListChars() const
{
    return m_whiteListChars;
}

void gsf::TextInputWidget::setWhiteListChars(sf::String chars)
{
    m_whiteListChars = chars;
}

void gsf::TextInputWidget::adjustShownText()
{
    if (!m_scrollable->isHorizontalScrollEnabled() && m_currentText.getSize() > 0)
    {
        m_lBreaksBefCur = 0;
        m_lBreakIndexes.clear();
        std::wstring shownString{ L"" };
        // The chars which are in the actual line
        unsigned int charCntLine{ 0 };
        // The total width of all chars in the current line
        float lineWidth{ 0.f };
        for (unsigned int i{ 0 }; i < m_currentText.getSize(); i++)
        {
            sf::Uint32 c{ m_currentText[i] };
            // If we have a new line as char we can set the lineWidth and charCntLine
            // to 0 because there is no need to handle the chars in this line
            // (It is already handled by user width the new line char)
            if (c == '\n')
            {
                lineWidth = 0.f;
                charCntLine = 0;
                shownString += c;
                continue;
            }
            // Width of the current char
            float cWidth{ m_text->getWidthAndHeightOfChar(c).x }; 
            lineWidth += cWidth;
            // When Text is out of scrollable widget, we have to add a new line 
            if (lineWidth > m_scrollable->getWidth())
            {
                if (i < m_cursorPos)
                {
                    // We have to increase the "line breaks befor cursor" counter
                    // so we add the cursor later on the right position
                    m_lBreaksBefCur++;
                }
                //shownString += m_currentText.substr(i - charCntLine, charCntLine);
                // Add new line
                shownString += L"\n";
                // Store the position (of the shown text) 
                // where the new line was added
                m_lBreakIndexes.push_back(i + 1);
                // add the char with which the line was to wide in the new line
                shownString += c;
                // We have added the char c in the new line, 
                // so we have now 1 char in the current line
                charCntLine = 1;
                lineWidth = cWidth;
            }
            else
            {
                charCntLine++;
                shownString += c;
            }
        }
        m_shownText = shownString;
        m_text->setText(m_shownText);
    }
}

void gsf::TextInputWidget::resetCursorStatus()
{
    m_lastBlinkTime = 0.f;
    m_isCursorShown = true;
}

unsigned int gsf::TextInputWidget::getAddedLineBreaksUpToIndex
    (unsigned int index) const
{
    unsigned int cnt{ 0 };

    for (unsigned int lBreakIndex : m_lBreakIndexes)
    {    
        if (lBreakIndex < index)
        {
            cnt++;
        }
        else
        {
            return cnt;
        }
    }
    return cnt;
}

void gsf::TextInputWidget::boundsChanged()
{
    m_scrollable->setWidth(getWidth());
    m_scrollable->setHeight(getHeight());
    adjustShownText();
    //m_scrollable->recalculateScroll();
    m_scrollable->scrollToTop();
    m_scrollable->scrollToLeft();
}

bool gsf::TextInputWidget::handleEventCurrentAfterChildren(sf::Event &event, 
        const sf::RenderTarget &target)
{

    bool handled{ Widget::handleEventCurrentAfterChildren(event, target) };
    if (!m_isEditable)
    {
        // Nothing to do
        return handled;
    }
    
    //bool handled{ ChildWidget::handleEvent(event) };/*|| 
      //  m_scrollable->handleEventWidget(event) };*/
    // Check if actual Widget is focused
    if (event.type == sf::Event::MouseButtonPressed && 
            event.mouseButton.button == sf::Mouse::Left)
    {        
        sf::Vector2f mousePos{ target.mapPixelToCoords({ event.mouseButton.x, 
            event.mouseButton.y }) };
        sf::Vector2f localPos{ convertToLocalPoint(mousePos) };

        bool isMouseInShownArea{ getShownArea().contains(mousePos) };
        bool intersecting{ isIntersecting(mousePos) };
        if (isMouseInShownArea && intersecting)
        {
            m_isFocused = true;
            handled = true;
            // Put cursor to clicked postion           
            // Get the index of the char where the mouse has clicked
            int clickedCharIndex{ m_text->findIndexOfCharOnPos(localPos) };
            // Clicked on a char?
            if (clickedCharIndex > -1)
            {
                // The index is the index of m_shownText, but we need the index of
                // the char in m_currentText, so we have to remove the automatic
                // added line breaks.
                unsigned int autoAddedLineBreaks{ 
                    getAddedLineBreaksUpToIndex(clickedCharIndex) };
                m_cursorPos = clickedCharIndex - autoAddedLineBreaks; 
                m_lBreaksBefCur = autoAddedLineBreaks;
            }
        }
        else
        {
            m_isFocused = false;
        }
    }
    if (event.type == sf::Event::KeyPressed && m_isFocused)
    {
        switch (event.key.code)
        {
        case sf::Keyboard::Left:
            if (m_cursorPos > 0)
            {
                m_cursorPos--;
            }
            // when cursor is moved it should be drawn so we reset its status
            resetCursorStatus();
            adjustShownText();
            //m_cursor.setPosition(
            //    m_text->findCharacterPos(m_cursorPos + m_lBreaksBefCur));
            return true;
        case sf::Keyboard::Right: 
            if (m_cursorPos < m_currentText.getSize())
            {
                m_cursorPos++;
            }
            resetCursorStatus();
            adjustShownText();
            //m_cursor.setPosition
            //    (m_text->findCharacterPos(m_cursorPos + m_lBreaksBefCur));
            return true;
        default: break;
        }
    }
    // If Widget is focused and Text entered, handle entered text
    if (m_isFocused && event.type == sf::Event::TextEntered)
    {
        // To handle umlauts and other 'exotic' chars we use widestring
        // and wide char
        //std::wstring actualTxt{ m_text.getString().toWideString() };
        //wchar_t c{ static_cast<wchar_t>(event.text.unicode) };
        sf::Uint32 c{ event.text.unicode };
        //std::cout << "Entered: " << c << std::endl;
        switch (c)
        {
        // Backspace
        case 8: 
            if (m_currentText.getSize() > 0) 
            {
                // Remove chars right of cursor when there are chars
                if (m_cursorPos > 0 && m_cursorPos < m_currentText.getSize())
                {
                    m_currentText.erase(m_cursorPos - 1, 1);
                    m_cursorPos--;
                }
                // When cursos is at the end of the text, p
                // place cursor behind char which we want to delete,
                else if (m_cursorPos == m_currentText.getSize())
                {
                    // Delete last char
                    std::wstring curTxt{ m_currentText.toWideString() };
                    curTxt.pop_back();
                    m_currentText = curTxt;
                    m_cursorPos--;
                }
            }
            break;
        // Delete Pressed
        case 127: 
            if (m_currentText.getSize() > 0 && 
                    m_cursorPos < m_currentText.getSize())
            {
                m_currentText.erase(m_cursorPos, 1);
            }
            break;
        // Enter key
        case 13: 
            // Dont add new line, when new lines are not accepted
            if (!m_acceptNewLines)
            {
                return false;
            }
            m_currentText.insert(m_cursorPos, L"\n"); m_cursorPos++;
            break;
        // Add char to text
        default:
            // If there are white listed chars specified, check if char is in it.
            // If not return
            if (m_whiteListChars.getSize() > 0 && 
                    m_whiteListChars.find(c) == std::wstring::npos)
            {
                return false;
            }
            // Check if char is black listed, if so return
            if(m_blackListChars.find(c) != std::wstring::npos)
            {
                return false;
            }
            m_currentText.insert(m_cursorPos, c);
                 m_cursorPos++;
        }
        resetCursorStatus();
        m_shownText = m_currentText;
        m_text->setText(m_shownText);
        adjustShownText();
        //m_cursor.setPosition
            //(m_text->findCharacterPos(m_cursorPos + m_lBreaksBefCur));
        m_scrollable->recalculateScroll();
        m_scrollable->scrollToRight();
        m_scrollable->scrollToBottom();
        return true;
    }
    return handled;
}

void gsf::TextInputWidget::updateCurrentAfterChildren(float dt)
{
    if (!m_isEditable)
    {
        // Nothing to do
        return;
    }
    
    // Update cursor stuff
    m_lastBlinkTime += dt;
    if (m_lastBlinkTime >= m_blinkFreq)
    {
        m_isCursorShown = !m_isCursorShown;
        m_lastBlinkTime = 0.f;
    }

    m_cursor.setPosition(
            m_text->findLocalCharacterPos(m_cursorPos + m_lBreaksBefCur));
    //std::wstring text{ m_currentText };
    //std::wstring text{ m_shownText };
    //m_text->setText(text); 
}

void gsf::TextInputWidget::drawCurrentAfterChildren(sf::RenderTarget &target, 
                    sf::RenderStates states, sf::View defaultView) const
{   
    // Draw cursor after children, so that children are not drawn over cursor
    if (m_isFocused && m_isCursorShown && m_isEditable)
    {
        target.draw(m_cursor, states);
    }
}
