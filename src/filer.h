//
// Created by cpasjuste on 03/12/24.
//

#ifndef FILER_H
#define FILER_H

#include "cross2d/skeleton/io.h"
#include "config.h"

#define FONT_SIZE 22

namespace retropico {
    class Filer final : public c2d::RectangleShape {
    public:
        Filer();

        void load();

        bool onInput(c2d::Input::Player *players) override;

    private:
        class Line final : public c2d::Rectangle {
        public:
            explicit Line(const c2d::FloatRect &rect) : Rectangle(rect) {
                p_text = new c2d::Text("", FONT_SIZE);
                p_text->setOrigin(c2d::Origin::Left);
                p_text->setPosition(3, (Line::getSize().y / 2) + 2);
                p_text->setFillColor(c2d::Color::Yellow);
                Line::add(p_text);
            }

            void setText(const std::string &text) const {
                p_text->setString(text);
            }

            void setTextColor(const c2d::Color &color) const {
                p_text->setFillColor(color);
            }

        private:
            c2d::Text *p_text = nullptr;
        };

        std::vector<std::vector<c2d::Io::File> > m_files;
        int m_current_core = 0;
        int m_max_lines = 0;
        int m_line_height = 0;
        int m_file_index = 0;
        int m_highlight_index = 0;
        c2d::RectangleShape *p_highlight;
        std::vector<Line *> p_lines;

        void setSelection(int index);

        void redraw() const;
    };
}

#endif //FILER_H
