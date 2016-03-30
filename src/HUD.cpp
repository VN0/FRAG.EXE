#include "HUD.h"
#include "Player.h"
using namespace std;
using namespace glm;

HUD :: HUD(Player* player, Window* window, Input* input, Cache<Resource,std::string>* cache):
    m_pPlayer(player),
    m_pWindow(window),
    m_pInput(input),
    m_pCache(cache)
{
    auto sw = m_pWindow->size().x;
    auto sh = m_pWindow->size().y;

    m_pCanvas = make_shared<Canvas>(sw, sh);
    add(m_pCanvas);
    //m_pCanvas->position(vec3(0.0f, float(sh) - 72.0f, 0.0f));
    //m_pCanvas->position(vec3(0.0f, float(sh) - 72.0f, 0.0f));
    //m_FontDesc = Pango::FontDescription("Audiowide Normal 56");
    //m_pCanvas->layout()->set_font_description(m_FontDesc);
}

void HUD :: redraw()
{
    auto sw = m_pWindow->size().x;
    auto sh = m_pWindow->size().y;
    auto cx = m_pWindow->center().x;
    auto cy = m_pWindow->center().y;
    auto win = m_pWindow;
    
    // clear black
    //auto cairo = m_pCanvas->context();
    //cairo->set_source_rgb(0.0f, 0.0f, 0.0f);
    //cairo->paint();
    m_pCanvas->clear(Color(0.0f, 0.0f, 0.0f, 0.0f));
    
    //m_pCanvas->clear(Color(1.0f, 1.0f, 1.0f, 0.0f));
    m_pCanvas->font("Audiowide",80);
    //m_pCanvas->font("Digital Dream",80);
    
    string hps = to_string(m_HP);
    float fade = 1.0f * m_HP / 100.0f;

    string ammo;
    if(m_Ammo == -1)
    {
        if(m_AmmoMax != -1)
            ammo = to_string(m_AmmoMax);
    }
    else
        ammo = to_string(m_Ammo) + " / " + to_string(m_AmmoMax);
    
    // Draw backgrounds
    Cairo::TextExtents extents;
    auto cairo = m_pCanvas->context();

    m_pCanvas->color(m_Fade);
    m_pCanvas->rectangle(
       0.0f, 0.0f, sw, sh
    ); cairo->fill();
    
    cairo->get_text_extents("+ "+hps+"%", extents);
    cairo->set_source_rgba(1.0f-fade, fade, 0.0f, 0.5f);
    m_pCanvas->rectangle(
        m_Border  - m_Border/2.0f, sh - extents.height - m_Border  - m_Border/2.0f,
        extents.width  + m_Border, extents.height  + m_Border,
        16.0f
    ); m_pCanvas->context()->fill();
    cairo->get_text_extents(ammo, extents);
    cairo->set_source_rgba(1.0f, 1.0f, 0.0f, 0.5f);
    m_pCanvas->rectangle(
        sw - extents.width - m_Border - m_Border/2.0f, sh - extents.height - m_Border - m_Border/2.0f,
        extents.width + m_Border, extents.height + m_Border,
        16.0f
    ); m_pCanvas->context()->fill();
    
    m_pCanvas->text("+", Color::black(), vec2(4.0f + m_Border, sh + 4.0f - m_Border), Canvas::LEFT);
    m_pCanvas->text("+", Color(1.0f,0.0f,0.0f), vec2(m_Border, sh - m_Border), Canvas::LEFT);
    m_pCanvas->text("  "+hps+"%", Color::black(), vec2(4.0f + m_Border, sh + 4.0f - m_Border), Canvas::LEFT);
    m_pCanvas->text("  "+hps+"%", Color::white(), vec2(m_Border,sh - m_Border), Canvas::LEFT);
    
    m_pCanvas->text(ammo, Color::black(), vec2(4.0f + sw - m_Border, sh + 4.0f - m_Border), Canvas::RIGHT);
    m_pCanvas->text(ammo, Color::white(), vec2(sw - m_Border,sh - m_Border), Canvas::RIGHT);

    m_pCanvas->text(m_Msg, Color::black(), vec2(cx,cy / 4.0f) + 4.0f, Canvas::CENTER);
    m_pCanvas->text(m_Msg, m_MsgColor, vec2(cx,cy / 4.0f), Canvas::CENTER);

    m_pCanvas->font("Audiowide",40);
    cairo->get_text_extents("1000", extents);
    cairo->set_source_rgba(1.0f, 0.0f, 0.0f, 0.5f);
    m_pCanvas->rectangle(
        win->center().x - extents.width, 0.0f,
        extents.width, extents.height
    ); m_pCanvas->context()->fill();
    cairo->set_source_rgba(0.0f, 0.0f, 1.0f, 0.5f);
    m_pCanvas->rectangle(
        win->center().x, 0.0f,
        extents.width, extents.height
    ); m_pCanvas->context()->fill();

    m_pCanvas->text("0", Color::white(), vec2(win->center().x - extents.width/2.0f, extents.height), Canvas::CENTER);
    m_pCanvas->text("0", Color::white(), vec2(win->center().x + extents.width/2.0f, extents.height), Canvas::CENTER);
    
    //layout->set_text("100%");
    //layout->show_in_cairo_context(ctext);
    //m_pCanvas->dirty(false);
}

void HUD :: logic_self(Freq::Time t)
{
    if(not m_Msg.empty())
    {
        m_MsgTime = Freq::Time(std::max<int>(0, int(m_MsgTime.value) - t.ms()));
        m_MsgColor.a() = kit::saturate(m_MsgTime.value/500.0);
        if(not m_MsgTime)
        {
            m_Msg = string();
            m_bDirty = true;
        }
    }

    if(m_bDirty) {
        redraw();
        m_bDirty = false;
    }
}

void HUD :: hp(int value)
{
    m_HP = value;
    m_bDirty = true;
}

void HUD :: ammo(int value, int max)
{
    m_Ammo = value;
    m_AmmoMax = max;
    m_bDirty = true;
}

void HUD :: message(string msg, Color c)
{
    m_Msg = msg;
    m_MsgColor = c;
    m_MsgTime = Freq::Time::seconds(4);
    m_bDirty = true;
}

