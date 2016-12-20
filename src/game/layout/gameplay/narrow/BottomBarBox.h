#pragma once

#include "game/layout/Box.h"
#include "system/Color.h"

#include <memory>

class AppContext;
class Font;
class GraphicsContext;
class Texture;


namespace Layout {
class BottomBarBox : public Layout::Box {
public:
    BottomBarBox(AppContext&, int width);

    void setPosition(int x, int y) override;

    void updateLevelCounter(unsigned);
    void updateScore(unsigned num);

    void drawActive(GraphicsContext&) const;
    void drawPassive(GraphicsContext&) const;

private:
    const RGBAColor box_color = 0x0A0AFF80_rgba;

    std::shared_ptr<Font> font_content;
    std::shared_ptr<Font> font_content_highlight;

    ::Rectangle rect_level;
    std::unique_ptr<Texture> tex_level;
    std::unique_ptr<Texture> tex_level_counter;

    ::Rectangle rect_score;
    std::unique_ptr<Texture> tex_score;
    std::unique_ptr<Texture> tex_score_counter;
};
} // namespace Layout
