#ifndef _GRID_HPP_
#define _GRID_HPP_

#include "geometry/directions.hpp"
#include "util/non_null_iterator.hpp"
#include "util/dereference_iterator.hpp"
#include <vector>
#include <array>
#include <algorithm>

template <typename T> class simple_grid {

    protected:
    const int n_cells_;
    std::vector<T> cells_;
    std::vector<T*> rows_;

    public:
    typedef typename std::vector<T>::iterator iterator;
    const int width;
    const int height;

    simple_grid(const int width, const int height) :
        n_cells_(width * height),
        width(width), height(height) 
    {
        /* Create cells */
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                cells_.push_back(T(j, i));
            }
        }
 
        /* Populate pointer array */
        for (int i = 0; i < height; ++i) {
            rows_.push_back(&cells_[i*width]);
        }
    }


    T * const operator[](const unsigned int idx) const {
        return rows_[idx];
    }

    iterator begin() {
        return cells_.begin();
    }
    iterator end() {
        return cells_.end();
    }

};

template <typename T> class grid : public simple_grid<T> {

    private:
    class cell_internal {
        public:
        T& cell;
        std::array<T*, directions::n_directions> neighbours;

        cell_internal(T &cell) :
            cell(cell) 
        {}


        typedef non_null_iterator<std::array<T*, directions::n_directions>, cell_internal> neighbour_iterator;

        neighbour_iterator neighbour_begin() {
            return neighbour_iterator(neighbours, directions::n_directions);
        }
        neighbour_iterator neighbour_end() {
            return neighbour_iterator(neighbours, directions::n_directions, directions::n_directions);
        }
        neighbour_iterator cardinal_neighbour_begin() {
            return neighbour_iterator(neighbours, directions::n_cardinal_directions);
        }
        neighbour_iterator cardinal_neighbour_end() {
            return neighbour_iterator(neighbours, directions::n_cardinal_directions, directions::n_cardinal_directions);
        }


        void link_neighbours(grid<T> &g) {

            for (int i = 0;i<directions::n_directions;i++) {
                neighbours[i] = nullptr;
            }

            if (cell.x_coord > 0) {
                neighbours[directions::west] = &g[cell.y_coord][cell.x_coord-1];
                if (cell.y_coord > 0) {
                    neighbours[directions::northwest] = &g[cell.y_coord-1][cell.x_coord-1];
                }
                if (cell.y_coord < g.height-1) {
                    neighbours[directions::southwest] = &g[cell.y_coord+1][cell.x_coord-1];
                }
            }
            if (cell.x_coord < g.width-1) {
                neighbours[directions::east] = &g[cell.y_coord][cell.x_coord+1];
                if (cell.y_coord > 0) {
                    neighbours[directions::northeast] = &g[cell.y_coord-1][cell.x_coord+1];
                }
                if (cell.y_coord < g.height-1) {
                    neighbours[directions::southeast] = &g[cell.y_coord+1][cell.x_coord+1];
                }
            }
            if (cell.y_coord > 0) {
                neighbours[directions::north] = &g[cell.y_coord-1][cell.x_coord];
            }
            if (cell.y_coord < g.height-1) {
                neighbours[directions::south] = &g[cell.y_coord+1][cell.x_coord];
            }

        }
    };

    std::vector<cell_internal> cell_internals_;
    std::vector<T*> border_;

    cell_internal &get_cell_internal(const unsigned int j, const unsigned int i) {
        return cell_internals_[j + i * this->width];
    }

    typedef typename std::vector<cell_internal>::iterator iterator_internal;
    
    public:
   
    typedef typename simple_grid<T>::iterator iterator;

    grid(const int width, const int height) :
        simple_grid<T>(width, height)
    {
        
        for (iterator it = this->cells_.begin(); it != this->cells_.end(); ++it) {
            cell_internals_.push_back(*it);
        }

        /* Link neighbours */
        for (iterator_internal it = cell_internals_.begin();
            it != cell_internals_.end();
            ++it)
        {
            it->link_neighbours(*this);    
        }

        /* Compute border */
        /* top */
        for (int i = 0; i < width; ++i) {
            border_.push_back(&((*this)[0][i]));
        }
        /* right */
        for (int i = 1; i < height - 1; ++i) {
            border_.push_back(&(*this)[i][width-1]);
        }

        /* bottom */
        for (int i = width - 1; i >= 0; --i) {
            border_.push_back(&(*this)[height-1][i]);
        }

        /* left */
        for (int i = height - 2; i >= 1; --i) {
            border_.push_back(&(*this)[i][0]);
        }

    }

    const std::vector<T* const> & get_border() {return border_;}

    typedef dereference_iterator<typename std::vector<T*>::iterator, T> border_iterator;
    border_iterator border_begin() {
        border_.begin();
        return border_iterator(border_.begin());
    }
    border_iterator border_end() {
        return border_iterator(border_.end());
    }

    typedef typename cell_internal::neighbour_iterator neighbour_iterator;
    neighbour_iterator neighbour_begin(T &cell) {
        return get_cell_internal(cell.y_coord, cell.x_coord).neighbour_begin();
    }
    neighbour_iterator neighbour_end(T &cell) {
        return get_cell_internal(cell.y_coord, cell.x_coord).neighbour_end();
    }

    int get_distance_to_edge(T &cell) {
        return std::min(
            std::min(cell.x_coord, this->width - cell.x_coord - 1),
            std::min(cell.y_coord, this->height - cell.y_coord - 1)
        );
    }
};

#endif