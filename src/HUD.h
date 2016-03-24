#ifndef _HUD_H
#define _HUD_H

#include "Qor/Window.h"
#include "Qor/Canvas.h"
#include "Qor/Input.h"
#include "Qor/Interpreter.h"

class Player;

class HUD:
    public Node
{
    public:
        HUD(
            Player* player,
            Window* window,
            Input* input,
            Cache<Resource,std::string>* cache
        );
        virtual ~HUD() {}

        virtual void logic_self(Freq::Time) override;

        bool input() const {
            return m_bInput;
        }

    private:
        
        void redraw();
        
        Window* m_pWindow = nullptr;
        Input* m_pInput = nullptr;
        std::shared_ptr<Canvas> m_pCanvas;
        Cache<Resource, std::string>* m_pCache;
        Pango::FontDescription m_FontDesc;
        float m_Border = 24.0f;
        Player* m_pPlayer;
        
        bool m_bInput = false;
        bool m_bDirty = true;
};

#endif
