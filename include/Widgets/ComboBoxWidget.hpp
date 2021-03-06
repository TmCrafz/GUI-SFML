#pragma once
#ifndef GSF_COMBOBOXWIDGET_HPP
#define GSF_COMBOBOXWIDGET_HPP
#include <SFML/Graphics.hpp>
#include "Widget.hpp"
#include "ListBoxWidget.hpp"

namespace gsf
{
    class ComboBoxWidget: public gsf::Widget
    {
    public:
        typedef std::unique_ptr<ComboBoxWidget> Ptr;
    private:
        ListBoxWidget::Ptr m_listBoxWidgetUnique;
        ListBoxWidget *m_listBoxWidget;
        TextWidget *m_currentText;
        int m_charSize;
        const sf::Font &m_font;
        
        std::function<void(Widget*, int)> m_onSelectionChangedListener;
    public:
        static Ptr create(const sf::Font &font);
        static Ptr create(float width, float height, const sf::Font &font);
        explicit ComboBoxWidget(const sf::Font &font);
        ComboBoxWidget(float width, float height, const sf::Font &font);
        
        virtual ~ComboBoxWidget();

        void addElement(sf::String element);
        sf::String getElement(int index) const;
        sf::String currentText() const;
        int currentIndex() const;
        int count() const;
        // returns true when index was valid and element was successfully selected
        bool selectElement(std::size_t index);
        // Selects the first element with the given text. Returns true when text 
        // was in listbox and element was successfully selected
        bool selectElement(const sf::String &text);
        
        sf::Color getSelectionColor() const;
        void setSelectionColor(sf::Color color);

        void setOnSelectionChangedListener(std::function<void(Widget*, int)> 
                listener);
    protected:
        virtual void loadAttributes(tinyxml2::XMLDocument &document, 
                std::map<std::string, std::string> &attributes) override;
        virtual void applyAttribute(const std::string &name, 
                const std::string &value) override;
        
        virtual void contextSet() override;
        virtual void contextRemoved() override;
        virtual void boundsChanged() override;
        
        void placeListBox();

        virtual bool handleEventCurrentAfterChildren(sf::Event &event, 
                const sf::RenderTarget &target) override;
        virtual void updateCurrentAfterChildren(float dt) override;
        virtual void drawCurrentAfterChildren(sf::RenderTarget &target, 
                sf::RenderStates states, sf::View defaultView) const override;
    private:
        void init(const sf::Font &font);
    };
}

#endif // !GSF_COMBOBOXWIDGET_HPP
