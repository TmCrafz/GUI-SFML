#ifndef VERTICALLAYOUT_HPP
#define VERTICALLAYOUT_HPP
#include <SFML/Graphics.hpp>
#include <string>
#include "ScrollableWidget.hpp"

namespace gsf
{
    class VerticalLayout: public gsf::ScrollableWidget
    {
        protected:

        private:

        public:
            VerticalLayout(float width, float height);
            virtual ~VerticalLayout();


        private:
            virtual void drawCurrent(sf::RenderTarget &target, sf::RenderStates states) const override;

            virtual void updateCurrent(float dt) override;

            virtual bool handleEventCurrent(sf::Event &event) override;

            // Calculate the size of the current View
            virtual void calculateSize() override;
            virtual void arrangeChildren() override;
    };

}

#endif // VERTICALLAYOUT_HPP
