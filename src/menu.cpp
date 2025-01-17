//
// Created by cpasjuste on 04/12/24.
//

#include "app.h"

using namespace c2d;
using namespace retropico;

Menu::Line::Line(const FloatRect &rect) : RectangleShape(rect) {
    // outline
    Line::setFillColor(Color::Transparent);
    Line::setOutlineColor(Color::Transparent);
    Line::setOutlineThickness(1);

    // add icon
    p_sprite = new Sprite();
    p_sprite->setPosition(4, Line::getSize().y / 2 + 4);
    p_sprite->setOrigin(Origin::Left);
    Line::add(p_sprite);

    // add text
    p_text = new Text("dummy", static_cast<int>(Line::getSize().y - 16));
    p_text->setPosition(p_sprite->getSize().x + 6, (Line::getSize().y / 2) - 2); // minus outline
    p_text->setOrigin(Origin::Left);
    p_text->setFillColor(Color::Yellow);
    p_text->setOutlineColor(Color::Black);
    p_text->setOutlineThickness(1);
    Line::add(p_text);
}

void Menu::Line::setIcon(Texture *texture) {
    p_sprite->setTexture(texture);
    const float scaling = std::min(
        getSize().x / texture->getSize().x,
        getSize().y / texture->getSize().y);
    p_sprite->setScale(scaling, scaling);
    p_text->setPosition(p_sprite->getSize().x * p_sprite->getScale().x + 16,
                        (Line::getSize().y / 2) - 2); // minus outline
}

Menu::Menu(const FloatRect &rect) : RectangleShape(rect) {
    Menu::setFillColor(Color::GrayDark);
    Menu::setOutlineColor(Color::Red);
    Menu::setOutlineThickness(2);
    Menu::setAlpha(240);

    // load icons
    const auto cores = App::Instance()->getConfig()->getCores();
    for (const auto &core: cores) {
        const std::string iconPath = App::Instance()->getIo()->getDataPath()
                                     + "res/icons/" + Utility::toLower(core.name) + ".png";
        p_textures.emplace_back(new C2DTexture(iconPath));
    }

    // max lines / height
    m_line_height = 34 + 2; // font height + margin
    m_max_lines = static_cast<int>(Menu::getSize().y / static_cast<float>(m_line_height));
    if (static_cast<float>(m_max_lines * m_line_height) < Menu::getSize().y) {
        m_line_height = static_cast<int>(Menu::getSize().y / static_cast<float>(m_max_lines));
    }

    // add highlight
    p_highlight = new RectangleShape({
        1, 1, Menu::getSize().x - 2, static_cast<float>(m_line_height + 2)
    });
    p_highlight->setAlpha(100);
    p_highlight->setFillColor(Color::Red);
    p_highlight->setOutlineColor(Color::Yellow);
    p_highlight->setOutlineThickness(1);
    Menu::add(p_highlight);

    // add lines
    for (int i = 0; i < m_max_lines; i++) {
        auto line = new Line({
            0, static_cast<float>(m_line_height * i),
            Menu::getSize().x, static_cast<float>(m_line_height)
        });
        p_lines.push_back(line);
        Menu::add(line);
    }

    redraw();
}

Menu::~Menu() {
    for (const auto tex: p_textures) {
        delete tex;
    }
}

void Menu::setSelection(const int index) {
    const auto cores = App::Instance()->getConfig()->getCores();
    const int size = static_cast<int>(cores.size());

    if (index < m_max_lines / 2) {
        m_file_index = 0;
        m_highlight_index = 0;
    } else if (index > size - m_max_lines / 2) {
        m_highlight_index = m_max_lines - 1;
        m_file_index = size - 1 - m_highlight_index;
        if (m_highlight_index >= size) {
            m_highlight_index = size - 1;
            m_file_index = 0;
        }
    } else {
        m_highlight_index = m_max_lines / 2;
        m_file_index = index - m_highlight_index;
    }

    redraw();
}

bool Menu::onInput(Input::Player *players) {
    if (!Menu::isVisible()) return false;

    const auto cores = App::Instance()->getConfig()->getCores();
    const auto buttons = players->buttons;

    if (buttons & Input::Button::Up) {
        const int index = m_file_index + m_highlight_index;
        const int middle = m_max_lines / 2;
        const int size = static_cast<int>(cores.size());
        if (m_highlight_index <= middle && index - middle > 0) {
            m_file_index--;
        } else {
            m_highlight_index--;
        }
        if (m_highlight_index < 0) {
            m_highlight_index = size < m_max_lines ? size - 1 : m_max_lines - 1;
            m_file_index = size - 1 - m_highlight_index;
        }

        redraw();
        return true;
    }

    if (buttons & Input::Button::Down) {
        const int index = m_file_index + m_highlight_index;
        const int middle = m_max_lines / 2;
        const int size = static_cast<int>(cores.size());
        if (m_highlight_index > middle && index + middle < size) {
            m_file_index++;
        } else {
            m_highlight_index++;
        }
        if (m_highlight_index >= m_max_lines || m_file_index + m_highlight_index >= size) {
            m_file_index = 0;
            m_highlight_index = 0;
        }

        redraw();
        return true;
    }

    if (buttons & Input::Button::Left) {
        int index = m_file_index + m_highlight_index - m_max_lines;
        if (index < 0) index = 0;
        setSelection(index);
        return true;
    }

    if (buttons & Input::Button::Right) {
        const int size = static_cast<int>(cores.size());
        int index = m_file_index + m_highlight_index + m_max_lines;
        if (index > size - 1) index = size - 1;
        setSelection(index);
        return true;
    }

    if (buttons & Input::Button::Menu1 ||
        buttons & Input::Button::A || buttons & Input::Button::B ||
        buttons & Input::Button::Start || buttons & Input::Button::Select) {
        App::Instance()->getInput()->clear();
        setVisibility(Visibility::Hidden);
        return true;
    }

    return RectangleShape::onInput(players);
}

void Menu::redraw() const {
    // update "lines"
    const auto cores = App::Instance()->getConfig()->getCores();
    const int size = static_cast<int>(cores.size());

    for (int i = 0; i < m_max_lines; i++) {
        if (m_file_index + i >= size) {
            p_lines[i]->setVisibility(Visibility::Hidden);
        } else {
            p_lines[i]->setVisibility(Visibility::Visible);
            p_lines[i]->setText(cores.at(i + m_file_index).name);
            p_lines[i]->setIcon(p_textures.at(i + m_file_index));
            if (i == m_highlight_index) {
                p_highlight->setPosition(p_highlight->getPosition().x, p_lines[i]->getPosition().y + 1);
            } else {
                p_lines[i]->setFillColor(Color::Transparent);
            }
        }
    }

    App::Instance()->getFiler()->setCore(m_file_index + m_highlight_index);
}
