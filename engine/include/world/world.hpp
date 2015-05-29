#ifndef _WORLD_HPP_
#define _WORLD_HPP_

#include "world/grid.hpp"
#include "geometry/vec2.hpp"
#include "character/character.hpp"
#include <vector>
#include <memory>

template <typename C, typename W> class world {
    public:
    
    std::vector<grid<W>> maps;
    std::vector<std::unique_ptr<C>> characters;

    const int width;
    const int height;
    world(const int width, const int height) : 
        width(width),
        height(height)
    {
        maps.push_back(grid<W>(width, height));
    }

    W& get_random_empty_cell(int index) {
        for (;;) {
            int x = rand() % width;
            int y = rand() % height;
            W& ret = maps[index].get_cell(x, y);
            if (!ret.is_solid()) {
                return ret;
            }
        }
        return maps[index][0][0];
    }

    void move_character(C &c, vec2<int> coord) {
        if (!maps[c.level_index].get_cell(coord).is_solid()) {
            c.coord = coord;
        }
    }
};

#endif
