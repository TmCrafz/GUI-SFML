#pragma once
#ifndef GSF_PROGRESSWIDGET_HPP
#define GSF_PROGRESSWIDGET_HPP
#include <SFML/Graphics.hpp>
#include "Widget.hpp"
#include <string>

namespace gsf
{
    class ProgressWidget: public gsf::Widget
    {
    public:
        typedef std::unique_ptr<ProgressWidget> Ptr;
    private:
        sf::Color m_progessColor;
        // Set margin of the progressbar
        float m_progressMargin;

        // The actual progress in percent
        int m_progress;

    public:
        static Ptr create();
        static Ptr create(float width, float height);
        ProgressWidget();
        ProgressWidget(float width, float height);

        sf::Color getProgressColor() const;
        void setProgressColor(sf::Color color);
        float getProgressMargin() const;
        void setProgressMargin(float margin);

        int getProgress() const;
        // Value between 0 and 100 (inclusive) is the value
        // lower then 0 its set ti 0 and higher then 100 the progress
        // is set to 100
        void setProgress(int progress);

    protected:
        virtual void loadAttributes(tinyxml2::XMLDocument &document, 
                std::map<std::string, std::string> &attributes) override;
        virtual void applyAttribute(const std::string &name, const std::string 
                &value) override;
        
        virtual bool handleEventCurrentAfterChildren(sf::Event &event, 
                const sf::RenderTarget &target) override;
        virtual void updateCurrentAfterChildren(float dt) override;
        virtual void drawCurrentAfterChildren(sf::RenderTarget &target, 
                sf::RenderStates states, sf::View defaultView) const override;
    private:
        void init();
    };
}

#endif // !GSF_PROGRESSWIDGET_HPP
