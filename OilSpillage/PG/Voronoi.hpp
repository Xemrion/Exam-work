#pragma once

// author: Victor Falkengaard Itzel
// copyright September 2019

#include <functional>
#include "defs.hpp"

class Voronoi {
public:
// tag dispatch
   struct EuclideanDistanceTag {};
   struct ManhattanDistanceTag {};
// ctors
   Voronoi( RNG &rng, U8 cell_size, U16 width, U16 height, EuclideanDistanceTag );
   Voronoi( RNG &rng, U8 cell_size, U16 width, U16 height, ManhattanDistanceTag );
// member constants:
   U8  const  CELL_SIZE;
   U16 const  WIDTH, HEIGHT;
// Voronoi noise data member diagram
   Vec<Size>      diagram;
   Vec<V2f> const noise;
	inline Size noise_index(   U16 x, U16 y ) const noexcept { return Size(y) * WIDTH + x; }
   inline Size diagram_index( U16 x, U16 y ) const noexcept { return Size(y) * WIDTH * U16(CELL_SIZE) + x; }
 private:
   Vec<V2f>    generate_noise( RNG &rng ) const noexcept;
   Void        generate_diagram( RNG &rng, std::function<F32(V2f const&, V2f const&)> const &distance_f ) noexcept;
};


// noise.size()    = WIDTH*HEIGHT
// diagram.size()  = WIDTH*CELL_SIZE * HEIGHT*CELL_SIZE

// noise[i]   // i = y * WIDTH + x;
// diagram[i] // i = y * WIDTH*CELL_SIZE + x;