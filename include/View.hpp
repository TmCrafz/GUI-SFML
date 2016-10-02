#ifndef VIEW_HPP
#define VIEW_HPP
#include <SFML/Graphics.hpp>

namespace gsf
{
    class View: public sf::Transformable, public sf::Drawable
    {
        public:
            typedef std::unique_ptr<View> Ptr;
            typedef std::pair<View*, View*> Pair;
            /*
            enum class Orientation
            {
                Left,
                Right,
                Top,
                Down,
                Center
            };
            */

        protected:
            float m_width;
            float m_height;

        private:
            std::vector<Ptr> m_children;
            View *m_parent;

        public:
            View();
            virtual ~View();

            void attachChild(Ptr child);
            Ptr detachChild(const View& node);

            void setWidth(const float width);
            float getWidth() const;
            void setHeight(const float height);
            float getHeight() const;

            // dt is the delta time
            void update(float dt);
            //void onCommand(const Command &command, float dt);
            // Get absolute world transform
            sf::Transform getWorldTransform() const;
            // Get absolute position in world
            sf::Vector2f getWorldPosition() const;

            bool isActive() const;
            void setIsActive(bool isActive);

            // draw should not get overridden
            virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const final;

        private:

            virtual void drawCurrent(sf::RenderTarget &target, sf::RenderStates states) const;
            void drawChildren(sf::RenderTarget &target, sf::RenderStates states) const;

            virtual void updateCurrent(float dt);
            void updateChildren(float dt);

            // Calculate the size of the current View
            virtual void calculateSize();
    };

}

#endif // VIEW_HPP
