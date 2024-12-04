//
// Created by cpasjuste on 03/12/24.
//

#include "app.h"
#include "filer.h"

using namespace c2d;
using namespace retropico;

Filer::Filer() : RectangleShape(Vector2f()) {
    Filer::setSize({App::Instance()->getSize().x - 2, App::Instance()->getSize().y - 2});
    Filer::setPosition({1, 1});
    Filer::setFillColor(Color::Transparent);
    Filer::setOutlineColor(Color::Red);
    Filer::setOutlineThickness(1);

    m_line_height = FONT_SIZE + 2; // font height + margin
    m_max_lines = static_cast<int>(Filer::getSize().y / static_cast<float>(m_line_height));
    if (static_cast<float>(m_max_lines * m_line_height) < Filer::getSize().y) {
        m_line_height = static_cast<int>(Filer::getSize().y / static_cast<float>(m_max_lines));
    }

    // add highlight
    p_highlight = new RectangleShape({
        1, 1,
        Filer::getSize().x - 2, static_cast<float>(m_line_height + 2)
    });
    p_highlight->setFillColor(Color::Red);
    p_highlight->setOutlineColor(Color::Yellow);
    p_highlight->setOutlineThickness(1);
    Filer::add(p_highlight);

    // add lines
    for (int i = 0; i < m_max_lines; i++) {
        auto line = new Line({
            0, static_cast<float>(m_line_height * i),
            Filer::getSize().x, static_cast<float>(m_line_height)
        });
        p_lines.push_back(line);
        Filer::add(line);
    }
}

void Filer::load() {
    const std::vector<Config::Core> cores = App::Instance()->getConfig()->getCores();
    m_files.resize(cores.size());
    for (size_t i = 0; i < cores.size(); i++) {
        m_files.at(i) = App::Instance()->getIo()->getDirList(cores.at(i).roms_path, true, false);
        printf("%s: %lu files found\n", cores.at(i).roms_path.c_str(), m_files.at(i).size());
        /*
        for (int j = 0; j < 14; j++) {
            m_files.at(i).push_back({"Super Test Rom " + std::to_string(j), "blahblah"});
        }
        */
    }

    setSelection(0);
    redraw();
}

bool Filer::onInput(Input::Player *players) {
    if (!isVisible()) return false;

    const auto buttons = players->buttons;

    if (buttons & Input::Button::Up) {
        const int index = m_file_index + m_highlight_index;
        const int middle = m_max_lines / 2;
        const int size = static_cast<int>(m_files[m_current_core].size());
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
        const int size = static_cast<int>(m_files[m_current_core].size());
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
        redraw();
        return true;
    }

    if (buttons & Input::Button::Right) {
        const int size = static_cast<int>(m_files[m_current_core].size());
        int index = m_file_index + m_highlight_index + m_max_lines;
        if (index > size - 1) index = size - 1;
        setSelection(index);
        redraw();
        return true;
    }

    if (buttons & Input::Button::A) {
        // TODO
        const auto corePath = App::Instance()->getConfig()->getCurrentCore()->path;
        const auto file = m_files[m_current_core].at(m_file_index + m_highlight_index);
        if (!App::Instance()->getRetroWidget()->loadCore(corePath)) {
            printf("Filer::onInput: failed to load core '%s'\n", corePath.c_str());
            return true;
        }

        if (!App::Instance()->getRetroWidget()->loadRom(file.path)) {
            printf("Filer::onInput: failed to load rom '%s'\n", file.path.c_str());
            return true;
        }

        setVisibility(Visibility::Hidden);
        App::Instance()->getRetroWidget()->setVisibility(Visibility::Visible);
        //App::Instance()->getRetroWidget()->unloadRom();
        //App::Instance()->getRetroWidget()->unloadCore();
    }

    return RectangleShape::onInput(players);
}

void Filer::setSelection(const int index) {
    const int size = static_cast<int>(m_files[m_current_core].size());

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
}

void Filer::redraw() const {
    // update "lines"
    const int size = static_cast<int>(m_files[m_current_core].size());

    for (int i = 0; i < m_max_lines; i++) {
        if (m_file_index + i >= size) {
            p_lines[i]->setVisibility(Visibility::Hidden);
        } else {
            p_lines[i]->setVisibility(Visibility::Visible);
            p_lines[i]->setText(m_files[m_current_core].at(i + m_file_index).name);
            if (i == m_highlight_index) {
                p_lines[i]->setFillColor(Color::Orange);
                //p_lines[i]->setScroll(true);
                p_highlight->setPosition(p_highlight->getPosition().x, p_lines[i]->getPosition().y + 1);
            } else {
                p_lines[i]->setFillColor(Color::Yellow);
                //p_lines[i]->setScroll(false);
            }
        }
    }

    // set config
    //Ui::getInstance()->getConfig()->setFilerCurrentCoreIndex(m_file_index + m_highlight_index);
}
