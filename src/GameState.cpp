#include "GameState.h"
#include "Qor/Input.h"
#include "Qor/Qor.h"
#include "Qor/TileMap.h"
#include "Qor/ScreenFader.h"
#include "Qor/Sound.h"
#include "Qor/Sprite.h"
#include <glm/glm.hpp>
#include <cstdlib>
#include <chrono>
#include <thread>
#include "Qor/PlayerInterface3D.h"
#include "Qor/Light.h"
#include "Qor/Material.h"
#include "Qor/kit/log/log.h"
//#include <OALWrapper/OAL_Funcs.h>
using namespace std;
using namespace glm;

GameState :: GameState(
    Qor* engine
    //std::string fn
):
    m_pQor(engine),
    m_pInput(engine->input()),
    m_pRoot(make_shared<Node>()),
    m_pOrthoRoot(make_shared<Node>()),
    //m_pInterpreter(engine->interpreter()),
    //m_pScript(make_shared<Interpreter::Context>(engine->interpreter())),
    m_pPipeline(engine->pipeline())
{
    m_Shader = m_pPipeline->load_shaders({"lit"});
}

void GameState :: preload()
{
    m_pPlayerMesh = make_shared<Mesh>();
    m_pPlayerMesh->set_box(Box(
        vec3(-0.2f, -0.6f, -0.2f),
        vec3(0.2f, 0.6f, 0.2f)
    ));
    m_pPlayerMesh->disable_physics();
    m_pPlayerMesh->set_physics(Node::Physics::DYNAMIC);
    m_pPlayerMesh->set_physics_shape(Node::CAPSULE);
    m_pPlayerMesh->friction(1.0f);
    m_pPlayerMesh->mass(10.0f);
    m_pPlayerMesh->inertia(false);
    m_pCamera = make_shared<Camera>(m_pQor->resources(), m_pQor->window());
    //m_pRoot->add(m_pCamera);
    m_pPlayerMesh->add(m_pCamera);
    m_pCamera->position(vec3(0.0f, 0.6f, 0.0f));
    m_pRoot->add(m_pPlayerMesh);

    //m_pRoot->add(m_pCamera);
    
    m_pOrthoCamera = make_shared<Camera>(m_pQor->resources(), m_pQor->window());
    m_pOrthoCamera->ortho();
    m_pOrthoRoot->add(m_pOrthoCamera);
    
    auto win = m_pQor->window();

    auto tex = m_pQor->resources()->cache_cast<Texture>("crosshair2.png");
    auto crosshair = make_shared<Mesh>(
        make_shared<MeshGeometry>(
            Prefab::quad(
                -vec2((float)tex->center().x, (float)tex->center().y) / 2.0f,
                vec2((float)tex->center().x, (float)tex->center().y) / 2.0f
            )
        )
    );
    crosshair->add_modifier(make_shared<Wrap>(Prefab::quad_wrap(
        vec2(1.0f, -1.0f)
    )));
    crosshair->material(make_shared<MeshMaterial>(tex));
    crosshair->position(glm::vec3(win->center().x, win->center().y, 0.0f));
    m_pOrthoRoot->add(crosshair);

    m_pDecal = m_pQor->resources()->cache_cast<ITexture>(
        "decal_bullethole1.png"
    );
    
    auto l = make_shared<Light>();
    l->diffuse(Color(1.0f, 0.2f, 0.2f, 1.0f));
    l->specular(Color(1.0f, 0.2f, 0.2f, 1.0f));
    //l->diffuse(Color(1.0f, 1.0f, 1.0f));
    //l->specular(Color(1.0f, 1.0f, 1.0f));
    l->atten(glm::vec3(0.0f, 0.1f, 0.01f));
    m_pRoot->add(l);

    l = make_shared<Light>();
    l->position(glm::vec3(10.0f, 0.0f, 0.0f));
    //l->diffuse(Color(1.0f, 1.0f, 1.0f));
    //l->specular(Color(1.0f, 1.0f, 1.0f));
    l->diffuse(Color(0.2f, 0.2f, 1.0f, 1.0f));
    l->specular(Color(0.2f, 0.2f, 1.0f, 1.0f));
    l->atten(glm::vec3(0.0f, 0.1f, 0.01f));
    m_pRoot->add(l);

    l = make_shared<Light>();
    l->position(glm::vec3(20.0f, 0.0f, 0.0f));
    //l->diffuse(Color(1.0f, 1.0f, 1.0f));
    //l->specular(Color(1.0f, 1.0f, 1.0f));
    l->diffuse(Color(0.2f, 1.0f, 0.2f, 1.0f));
    l->specular(Color(0.2f, 1.0f, 0.2f, 1.0f));
    l->atten(glm::vec3(0.0f, 0.1f, 0.01f));
    m_pRoot->add(l);

    //m_pPipeline = make_shared<Pipeline>(
    //    m_pQor->window(),
    //    m_pQor->resources(),
    //    m_pRoot,
    //    m_pCamera
    //);
    
    m_pRoot->add(m_pQor->make<Mesh>("theDunes.obj"));
    m_pController = m_pQor->session()->profile(0)->controller();
    m_pPlayer = kit::init_shared<PlayerInterface3D>(
        m_pController,
        m_pCamera,
        m_pPlayerMesh,
        m_pQor->session()->profile(0)->config()
    );
    m_pPlayer->speed(12.0f);
    //m_pPlayer->fly();
    
    const bool ads = false;
    auto gun = m_pQor->make<Mesh>("gun_tacticalsupershotgun.obj");
    //auto gun = m_pQor->make<Mesh>("glock.obj");
    m_pViewModel = make_shared<ViewModel>(m_pCamera, gun);
    gun->disable_physics();
    m_pRoot->add(m_pViewModel);
    
    //gun = m_pQor->make<Mesh>("glock.obj");
    //gun->set_physics(Node::DYNAMIC);
    //gun->set_physics_shape(Node::BOX);
    //gun->mass(1.0f);
    //gun->inertia(true);
    //gun->position(glm::vec3(-2.0f, 1.0f, 0.0f));
    
    
    //m_pViewModel->node()->rotate(0.5f, Axis::Z);
    //m_pViewModel->node()->position(glm::vec3(
    //    ads ? 0.0f : 0.05f,
    //    ads ? -0.04f : -0.06f,
    //    ads ? -0.05f : -0.15f
    //));
    
    m_pViewModel->model_pos(glm::vec3(
        //0.111f, -0.168f, -0.223f //glock
        0.0f, -0.228f, -0.385f //shotgun
        //0.0f, -0.12f, -0.18f //bullpup
    ));
    m_pViewModel->zoomed_model_pos(glm::vec3(
        //0.0f, -0.039f, -0.282f // glock
        0.0f, -0.15f, -0.472f // shotgun
        //0.0f, -0.12f, -0.18f // bullpup
    ));
    m_pViewModel->reset_zoom();
    
    // TODO: ensure filename contains only valid filename chars
    //m_pScript->execute_file("mods/"+ m_Filename +"/__init__.py");
    //m_pScript->execute_string("preload()");
    
    m_pPlayerMesh->position(vec3(-2.0f, 2.0f, 0.0f));
    m_pPhysics = make_shared<Physics>(m_pRoot.get(), this);
    m_pPhysics->generate(m_pRoot.get(), (unsigned)Physics::GenerateFlag::RECURSIVE);

    btRigidBody* pmesh_body = (btRigidBody*)m_pPlayerMesh->body()->body();
    pmesh_body->setActivationState(DISABLE_DEACTIVATION);
    pmesh_body->setAngularFactor(btVector3(0,0,0));
    ////pmesh_body->setRestitution(0.0f);
    m_pPhysics->world()->setGravity(btVector3(0.0, -9.8, 0.0));
}

GameState :: ~GameState()
{
    //m_pPipeline->partitioner()->clear();
}

void GameState :: enter()
{
    m_pPipeline->shader(1)->use();
    m_pPipeline->override_shader(PassType::NORMAL, m_Shader);
     
    m_pCamera->perspective();
    m_pInput->relative_mouse(true);

    on_tick.connect(std::move(screen_fader(
        [this](Freq::Time, float fade) {
            m_pPipeline->shader(1)->use();
            int fadev = m_pPipeline->shader(1)->uniform("Brightness");
            if(fadev != -1)
                m_pPipeline->shader(1)->uniform(
                    fadev,
                    glm::vec3(fade,fade,fade)
                );
        },
        [this](Freq::Time){
            if(m_pInput->key(SDLK_ESCAPE))
                return true;
            return false;
        },
        [this](Freq::Time){
            m_pPipeline->shader(1)->use();
            int u = m_pPipeline->shader(1)->uniform("Brightness");
            if(u >= 0)
                m_pPipeline->shader(1)->uniform(u, Color::white().vec3());
            m_pPipeline->blend(false);
            m_pQor->pop_state();
        }
    )));
}

void GameState :: logic(Freq::Time t)
{
    Actuation::logic(t);
    m_pPhysics->logic(t);
    
    //LOGf("object: %s", Matrix::to_string(*m_pCamera->matrix()));
    //LOGf("world: %s", Matrix::to_string(*m_pCamera->matrix(Space::WORLD)));
    
    if(m_pInput->key(SDLK_ESCAPE))
        m_pQor->quit();

    if(m_pController->button("zoom").pressed_now())
        m_pViewModel->zoom(not m_pViewModel->zoomed());
    
    if(m_pController->button("fire").pressed_now()) {
        //Sound::play(m_pCamera.get(), "shotgun.wav", m_pQor->resources());
        auto s = m_pQor->make<Sound>("shotgun3.wav");
        m_pCamera->add(s);
        s->play();
        s->detach_on_done();
        m_pViewModel->recoil(Freq::Time(50), Freq::Time(500));

        for(int i=0; i<8; ++i)
        {
            auto mag_var = (rand() % 1000) * 0.001f * 0.1f;
            auto ang_var = (rand() % 1000) * 0.001f;
            vec3 dir = glm::vec3(
                cos(K_TAU * ang_var) * mag_var,
                sin(K_TAU * ang_var) * mag_var,
                -1.0f
            );
            dir = glm::normalize(dir);
            
            auto hit = m_pPhysics->first_hit(
                m_pCamera->position(Space::WORLD),
                m_pCamera->position(Space::WORLD) +
                    m_pCamera->orient_to_world(dir) * 100.0f
            );
            if(std::get<0>(hit))
            {
                decal(
                    std::get<1>(hit),
                    std::get<2>(hit),
                    *m_pCamera->matrix(Space::WORLD)
                );
            }
        }
    }

    //LOGf("children: %s", m_pRoot->num_subnodes());

    m_pViewModel->sway(m_pPlayer->move() != glm::vec3(0.0f));
    m_pViewModel->sprint(
        m_pPlayer->move() != glm::vec3(0.0f) && m_pPlayer->sprint()
    );
    
    m_pOrthoRoot->logic(t);
    m_pRoot->logic(t);
    
    //m_pViewModel->position(m_pCamera->position(Space::WORLD));

    if(m_pInput->key(SDLK_DOWN))
        m_pViewModel->zoomed_model_move(glm::vec3(0.0f, -t.s(), 0.0f));
    else if(m_pInput->key(SDLK_UP))
        m_pViewModel->zoomed_model_move(glm::vec3(0.0f, t.s(), 0.0f));
    else if(m_pInput->key(SDLK_LEFT))
        m_pViewModel->zoomed_model_move(glm::vec3(-t.s(), 0.0f, 0.0f));
    else if(m_pInput->key(SDLK_RIGHT))
        m_pViewModel->zoomed_model_move(glm::vec3(t.s(), 0.0f, 0.0f));
    else if(m_pInput->key(SDLK_w))
        m_pViewModel->zoomed_model_move(glm::vec3(0.0f, 0.0f, t.s()));
    else if(m_pInput->key(SDLK_r))
        m_pViewModel->zoomed_model_move(glm::vec3(0.0f, 0.0f, -t.s()));

    LOGf("model pos %s", Vector::to_string(m_pViewModel->model_pos()));
    LOGf("zoomed model pos %s", Vector::to_string(m_pViewModel->zoomed_model_pos()));
}

void GameState :: decal(glm::vec3 contact, glm::vec3 normal, glm::mat4 observer)
{
    const float decal_scale = 0.1f;
    auto m = make_shared<Mesh>(make_shared<MeshGeometry>(Prefab::quad(
        glm::vec2(-decal_scale, -decal_scale),
        glm::vec2(decal_scale, decal_scale)
    )));
    m->add_modifier(make_shared<Wrap>(Prefab::quad_wrap()));
    m->material(make_shared<MeshMaterial>(m_pDecal));
    m->position(contact + glm::vec3(0.0f, 0.0f, 0.001f));
    m->pend();
    m_pRoot->add(m);
    //auto l = make_shared<Light>();
    //l->position(contact);
    //l->diffuse(Color(1.0f, 0.2f, 0.2f, 1.0f));
    //l->specular(Color(1.0f, 0.2f, 0.2f, 1.0f));
    //l->atten(glm::vec3(0.0f, 0.1f, 0.01f));
    //m_pRoot->add(l);
}

void GameState :: render() const
{
    m_pPipeline->override_shader(PassType::NORMAL, m_Shader);
    m_pPipeline->winding(false);
    m_pPipeline->blend(false);
    m_pPipeline->render(
        m_pRoot.get(),
        m_pCamera.get(),
        nullptr,
        Pipeline::LIGHTS
    );
    m_pPipeline->override_shader(PassType::NORMAL, (unsigned)PassType::NONE);
    m_pPipeline->winding(true);
    m_pPipeline->blend(true);
    m_pPipeline->render(
        m_pOrthoRoot.get(),
        m_pOrthoCamera.get(),
        nullptr,
        Pipeline::NO_CLEAR | Pipeline::NO_DEPTH
    );
    m_pPipeline->blend(false);
}

