//
// Created by cpasjuste on 04/12/24.
//

#ifndef RETROPICO_MENU_H
#define RETROPICO_MENU_H

#include "cross2d/c2d.h"

namespace retropico {
    class Menu final : public c2d::RectangleShape {
    public:
        explicit Menu(const c2d::FloatRect &rect);

        ~Menu() override;

        void setSelection(int index);

    private:
        class Line final : public RectangleShape {
        public:
            explicit Line(const c2d::FloatRect &rect);

            void setText(const std::string &text) const {
                p_text->setString(text);
            }

            void setIcon(c2d::Texture *texture);

        private:
            c2d::Text *p_text;
            c2d::Sprite *p_sprite;
        };

        int m_max_lines = 0;
        int m_line_height = 0;
        int m_file_index = 0;
        int m_highlight_index = 0;
        RectangleShape *p_highlight;
        std::vector<Line *> p_lines;
        std::vector<c2d::Texture *> p_textures;

        void redraw() const;

        bool onInput(c2d::Input::Player *players) override;
    };
}

#endif //RETROPICO_MENU_H
