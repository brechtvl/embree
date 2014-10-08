// ======================================================================== //
// Copyright 2009-2014 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

#include "subdivpatchdispl1.h"
#include "quadquad4x4.h"
#include "common/ray.h"
#include "geometry/filter.h"

namespace embree
{
  struct SubdivPatchDispl1Intersector1
  {
    typedef SubdivPatchDispl1 Primitive;

    struct Precalculations {
      __forceinline Precalculations (const Ray& ray) {}
    };

    static __forceinline void intersect(const Precalculations& pre, Ray& ray, Primitive& prim, const void* geom, size_t& lazy_node)
    {
      lazy_node = prim.initialize();
    }
    
    static __forceinline bool occluded(const Precalculations& pre, Ray& ray, Primitive& prim, const void* geom, size_t& lazy_node)
    {
      lazy_node = prim.initialize();
      return false;
    }
  };
}
