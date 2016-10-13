#include "SinglePlayState.h"

#include "game/AppContext.h"
#include "game/WellEvent.h"
#include "game/components/Piece.h"
#include "system/Localize.h"

#include <cmath>


SinglePlayState::SinglePlayState(AppContext& app)
    : paused(false)
    , gameover(false)
    , tex_background(app.gcx().loadTexture("data/gamebg.png"))
    , texts_need_update(false)
    , ui_well(app)
    , next_pieces(4)
    , lineclears_per_level(10)
    , lineclears_left(lineclears_per_level)
    , current_level(1)
    , current_score(0)
    , gametime(Duration::zero())
    , gametime_text("00:00")
    , score_table({
            {LINE_CLEAR_SINGLE, 100},
            {LINE_CLEAR_DOUBLE, 200},
            {LINE_CLEAR_TRIPLE, 500},
            {LINE_CLEAR_PERFECT, 800},
            {SOFTDROP, 1},
            {HARDDROP, 2}
        })
    , previous_lineclear_type(LINE_CLEAR_SINGLE)
{
    registerObservers();

    // TODO: consider alternative algorithm
    for (int i = 14; i >= 0; i--) {
        float multiplier = std::pow(0.8 - (i * 0.007), i);
        gravity_levels.push(std::chrono::duration_cast<Duration>(multiplier * std::chrono::seconds(1)));
    }

    addNextPiece();
    ui_well.well().setGravity(gravity_levels.top());


    // UI

    font_boxtitle = app.gcx().loadFont("data/fonts/OpenSans-CondLight.ttf", 28);
    font_boxcontent = app.gcx().loadFont("data/fonts/OpenSans-CondBold.ttf", 30);
    font_big = app.gcx().loadFont("data/fonts/OpenSans-CondBold.ttf", 60);


    tex_hold = app.gcx().renderText(tr("HOLD"), font_boxtitle, 0xEEEEEE_rgb);
    tex_next = app.gcx().renderText(tr("NEXT"), font_boxtitle, 0xEEEEEE_rgb);
    tex_goal = app.gcx().renderText(tr("GOAL"), font_boxtitle, 0xEEEEEE_rgb);
    tex_level = app.gcx().renderText(tr("LEVEL"), font_boxtitle, 0xEEEEEE_rgb);
    tex_goal_counter = app.gcx().renderText(std::to_string(lineclears_left), font_boxcontent, 0xEEEEEE_rgb);
    tex_level_counter = app.gcx().renderText(std::to_string(current_level), font_boxcontent, 0xEEEEEE_rgb);
    tex_score = app.gcx().renderText(tr("SCORE"), font_boxtitle, 0xEEEEEE_rgb);
    tex_score_counter = app.gcx().renderText(std::to_string(current_score), font_boxcontent, 0xEEEEEE_rgb);
    tex_time_counter = app.gcx().renderText(gametime_text, font_boxcontent, 0xEEEEEE_rgb);


    ui.sidebars.left.inner.w = 5 * Mino::texture_size_px;
    ui.sidebars.left.inner.h = ui_well.height();
    ui.sidebars.left.padding = {0, 10, 0, 0};
    ui.sidebars.left.outer.w = ui.sidebars.left.inner.w
                             + ui.sidebars.left.padding.left + ui.sidebars.left.padding.right;
    ui.sidebars.left.outer.h = ui.sidebars.left.inner.h
                             + ui.sidebars.left.padding.top + ui.sidebars.left.padding.bottom;

    ui.sidebars.right.inner.w = 5 * Mino::texture_size_px;
    ui.sidebars.right.inner.h = ui_well.height();
    ui.sidebars.right.padding = {0, 0, 0, 10};
    ui.sidebars.right.outer.w = ui.sidebars.right.inner.w
                              + ui.sidebars.right.padding.left + ui.sidebars.left.padding.right;
    ui.sidebars.right.outer.h = ui.sidebars.right.inner.h
                              + ui.sidebars.right.padding.top + ui.sidebars.left.padding.bottom;
}

SinglePlayState::~SinglePlayState() = default;

void SinglePlayState::registerObservers()
{
    ui_well.well().registerObserver(WellEvent::Type::NEXT_REQUESTED, [this](const WellEvent&){
        this->addNextPiece();
    });

    ui_well.well().registerObserver(WellEvent::Type::HOLD_REQUESTED, [this](const WellEvent&){
        auto& well = this->ui_well.well();

        this->piece_holder.onSwapRequested();
        if (this->piece_holder.swapAllowed()) {
            auto type = well.activePiece()->type();
            well.deletePiece();
            if (this->piece_holder.isEmpty()) {
                this->piece_holder.swapWithEmpty(type);
                this->addNextPiece();
            }
            else
                well.addPiece(this->piece_holder.swapWith(type));
        }
    });

    ui_well.well().registerObserver(WellEvent::Type::LINE_CLEAR, [this](const WellEvent& event){
        assert(event.type == WellEvent::Type::LINE_CLEAR);
        assert(event.count > 0);
        assert(event.count <= 4);

        this->texts_need_update = true;
        this->lineclears_left -= event.count;


        // increase score
        this->current_score += this->score_table.at(static_cast<ScoreTypes>(event.count))
                               * this->current_level;
        if (event.count == 4 && this->previous_lineclear_type == LINE_CLEAR_PERFECT)
            this->current_score += this->score_table.at(LINE_CLEAR_PERFECT) * 0.5f; // back-to-back bonus

        this->previous_lineclear_type = static_cast<ScoreTypes>(event.count);


        // increase gravity level
        if (this->lineclears_left <= 0) {
            if (this->gravity_levels.size() <= 1) {
                this->lineclears_left = 0;
                return;
            }
            this->gravity_levels.pop();
            this->ui_well.well().setGravity(this->gravity_levels.top());
            this->lineclears_left += lineclears_per_level;
            this->current_level++;
        }
    });

    ui_well.well().registerObserver(WellEvent::Type::HARDDROPPED, [this](const WellEvent& event){
        assert(event.count < 22);
        this->texts_need_update = true;
        this->current_score += event.count * this->score_table.at(HARDDROP);
    });

    ui_well.well().registerObserver(WellEvent::Type::SOFTDROPPED, [this](const WellEvent&){
        this->texts_need_update = true;
        this->current_score += this->score_table.at(SOFTDROP);
    });

    ui_well.well().registerObserver(WellEvent::Type::GAME_OVER, [this](const WellEvent&){
        gameover = true;
    });
}

void SinglePlayState::addNextPiece()
{
    ui_well.well().addPiece(next_pieces.next());
    piece_holder.onNextTurn();
}

void SinglePlayState::update(const std::vector<InputEvent>& inputs, AppContext& app)
{
    for (const auto& input : inputs) {
        if (input.type() == InputType::GAME_PAUSE && input.down()) {
            paused = !paused;
            return;
        }
    }


    // UI
    // TODO: only on screen resize event
    ui_well.setPosition((app.gcx().screenWidth() - ui_well.width()) / 2,
                        (app.gcx().screenHeight() - ui_well.height()) / 2);

    ui.sidebars.left.outer.x = ui_well.x() - ui.sidebars.left.outer.w;
    ui.sidebars.left.inner.x = ui.sidebars.left.outer.x + ui.sidebars.left.padding.left;
    ui.sidebars.left.outer.y = ui_well.y();
    ui.sidebars.left.inner.y = ui.sidebars.left.outer.y + ui.sidebars.left.padding.top;
    ui.sidebars.left.items.goal_counter = {
        ui.sidebars.left.inner.x,
        ui.sidebars.left.inner.y + ui.sidebars.left.inner.h
        - ui.sidebars.text_height - ui.sidebars.text_padding * 2,
        ui.sidebars.left.inner.w, ui.sidebars.text_height + ui.sidebars.text_padding * 2};
    ui.sidebars.left.items.level_counter = {
        ui.sidebars.left.inner.x,
        ui.sidebars.left.items.goal_counter.y
        - ui.sidebars.text_padding - ui.sidebars.text_height - ui.sidebars.text_padding
        - ui.sidebars.item_padding
        - ui.sidebars.left.items.goal_counter.h,
        ui.sidebars.left.inner.w, ui.sidebars.text_height + ui.sidebars.text_padding * 2};

    ui.sidebars.right.outer.x = ui_well.x() + ui_well.width();
    ui.sidebars.right.inner.x = ui.sidebars.right.outer.x + ui.sidebars.right.padding.left;
    ui.sidebars.right.outer.y = ui_well.y();
    ui.sidebars.right.inner.y = ui.sidebars.right.outer.y + ui.sidebars.right.padding.top;
    ui.sidebars.right.items.score_counter = {
        ui.sidebars.right.inner.x,
        ui.sidebars.right.inner.y + ui.sidebars.right.inner.h
        - ui.sidebars.text_height - ui.sidebars.text_padding * 2,
        ui.sidebars.right.inner.w, ui.sidebars.text_height + ui.sidebars.text_padding * 2};
    ui.sidebars.right.items.time_counter = {
        ui.sidebars.right.inner.x,
        ui.sidebars.right.items.score_counter.y
        - ui.sidebars.text_padding - ui.sidebars.text_height - ui.sidebars.text_padding
        - ui.sidebars.item_padding
        - ui.sidebars.right.items.score_counter.h,
        ui.sidebars.right.inner.w, ui.sidebars.text_height + ui.sidebars.text_padding * 2};


    if (paused)
        return;

    ui_well.update(inputs, app);
    piece_holder.update();

    if (texts_need_update) {
        app.gcx().renderText(tex_goal_counter, std::to_string(lineclears_left),
                             font_boxcontent, 0xEEEEEE_rgb);
        app.gcx().renderText(tex_level_counter, std::to_string(current_level),
                             font_boxcontent, 0xEEEEEE_rgb);
        app.gcx().renderText(tex_score_counter, std::to_string(current_score),
                             font_boxcontent, 0xEEEEEE_rgb);
        texts_need_update = false;
    }

    if (gameover)
        return;

    updateGametime(app);
}

void SinglePlayState::updateGametime(AppContext& app)
{
    gametime += Timing::frame_duration;

    unsigned minutes = std::chrono::duration_cast<std::chrono::minutes>(gametime).count();
    unsigned seconds = std::chrono::duration_cast<std::chrono::seconds>(gametime).count() % 60;
    std::string newstr;
    if (minutes < 10)
        newstr += "0";
    newstr += std::to_string(minutes);
    newstr += ":";
    if (seconds < 10)
        newstr += "0";
    newstr += std::to_string(seconds);

    // render text
    if (newstr != gametime_text) {
        gametime_text = newstr;
        app.gcx().renderText(tex_time_counter, gametime_text, font_boxcontent, 0xEEEEEE_rgb);
    }
}

void SinglePlayState::drawLeftSidebar(GraphicsContext& gcx)
{
    // hold queue
    gcx.drawTexture(tex_hold, ui.sidebars.left.inner.x, ui.sidebars.left.inner.y);
    piece_holder.draw(gcx, ui.sidebars.left.inner.x,
                      ui.sidebars.left.inner.y + ui.sidebars.text_height + ui.sidebars.text_padding);

    // goal
    gcx.drawFilledRect(ui.sidebars.left.items.goal_counter, ui.sidebars.box_color);
    gcx.drawTexture(tex_goal_counter,
                    ui.sidebars.left.items.goal_counter.x
                    + (ui.sidebars.left.items.goal_counter.w - gcx.textureWidth(tex_goal_counter)) / 2,
                    ui.sidebars.left.items.goal_counter.y + 3);
    gcx.drawTexture(tex_goal, ui.sidebars.left.items.goal_counter.x,
                    ui.sidebars.left.items.goal_counter.y
                    - ui.sidebars.text_padding - ui.sidebars.text_height);

    // level
    gcx.drawFilledRect(ui.sidebars.left.items.level_counter, ui.sidebars.box_color);
    gcx.drawTexture(tex_level_counter,
                    ui.sidebars.left.items.level_counter.x
                    + (ui.sidebars.left.items.level_counter.w - gcx.textureWidth(tex_level_counter)) / 2,
                    ui.sidebars.left.items.level_counter.y + 3);
    gcx.drawTexture(tex_level, ui.sidebars.left.items.level_counter.x,
                    ui.sidebars.left.items.level_counter.y
                    - ui.sidebars.text_padding - ui.sidebars.text_height);
}

void SinglePlayState::drawRightSidebar(GraphicsContext& gcx)
{
    // next queue
    gcx.drawTexture(tex_next,
                    ui.sidebars.right.inner.x + ui.sidebars.right.inner.w - gcx.textureWidth(tex_next),
                    ui.sidebars.right.inner.y);
    next_pieces.draw(gcx, ui.sidebars.right.inner.x,
                     ui.sidebars.right.inner.y + ui.sidebars.text_height + ui.sidebars.text_padding);

    // score
    gcx.drawFilledRect(ui.sidebars.right.items.score_counter, ui.sidebars.box_color);
    gcx.drawTexture(tex_score_counter,
                    ui.sidebars.right.items.score_counter.x
                    + (ui.sidebars.right.items.score_counter.w - gcx.textureWidth(tex_score_counter)) / 2,
                    ui.sidebars.right.items.score_counter.y + 3);
    gcx.drawTexture(tex_score,
                    ui.sidebars.right.items.score_counter.x
                    + ui.sidebars.right.inner.w - gcx.textureWidth(tex_score),
                    ui.sidebars.right.items.score_counter.y
                    - ui.sidebars.text_padding - ui.sidebars.text_height);

    // time
    gcx.drawFilledRect(ui.sidebars.right.items.time_counter, ui.sidebars.box_color);
    gcx.drawTexture(tex_time_counter,
                    ui.sidebars.right.items.time_counter.x
                    + (ui.sidebars.right.items.time_counter.w - gcx.textureWidth(tex_time_counter)) / 2,
                    ui.sidebars.right.items.time_counter.y + 3);
}

void SinglePlayState::draw(GraphicsContext& gcx)
{
    gcx.drawTexture(tex_background, {0, 0, gcx.screenWidth(), gcx.screenHeight()});

    ui_well.draw(gcx, paused);
    drawLeftSidebar(gcx);
    drawRightSidebar(gcx);
}
