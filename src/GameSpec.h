#ifndef _GAMESPEC_H
#define _GAMESPEC_H

#include <memory>
#include <string>
#include "Qor/kit/meta/meta.h"
#include "Qor/kit/cache/cache.h"
#include "Qor/Resource.h"
#include "Qor/BasicPartitioner.h"
#include "Weapon.h"
class Node;
class Player;

class GameSpec
{
    public:
        GameSpec(std::string fn, Cache<Resource, std::string>* cache,
            Node* root, BasicPartitioner* part);
        WeaponSpec* weapons() { return &m_WeaponSpec; }

        void setup();
        void register_player(Player* p);
        void deregister_player(Player* p);
        std::shared_ptr<Meta> config() { return m_pConfig; };
        
    private:
        void register_pickup_with_player(std::shared_ptr<Mesh> item, Player* player);
        
        std::shared_ptr<Meta> m_pConfig;
        Cache<Resource, std::string>* m_pCache;
        WeaponSpec m_WeaponSpec;
        Node* m_pRoot;
        BasicPartitioner* m_pPartitioner;

        std::vector<Player*> m_Players;
        std::vector<std::shared_ptr<Mesh>> m_WeaponPickups;
        std::vector<std::shared_ptr<Mesh>> m_ItemPickups;
};

#endif

