// Copyright 2009-2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "triangle.h"
#include "trianglev.h"
#include "trianglev_mb.h"
#include "intersector_epilog.h"

/*! Modified Pluecker ray/triangle intersector. The test first shifts
 *  the ray origin into the origin of the coordinate system and then
 *  uses Pluecker coordinates for the intersection. Due to the shift,
 *  the Pluecker coordinate calculation simplifies and the tests get
 *  numerically stable. The edge equations are watertight along the
 *  edge for neighboring triangles. */

namespace embree
{
  namespace isa
  {
    template<int M, typename UVMapper>
    struct PlueckerHitM
    {
      __forceinline PlueckerHitM(const vfloat<M>& U, const vfloat<M>& V, const vfloat<M>& UVW, const vfloat<M>& t, const Vec3vf<M>& Ng, const UVMapper& mapUV)
        : U(U), V(V), UVW(UVW), mapUV(mapUV), vt(t), vNg(Ng) {}
      
      __forceinline void finalize() 
      {
        const vbool<M> invalid = abs(UVW) < min_rcp_input;
        const vfloat<M> rcpUVW = select(invalid,vfloat<M>(0.0f),rcp(UVW));
        vu = U * rcpUVW;
        vv = V * rcpUVW;
        mapUV(vu,vv,vNg);
      }

      __forceinline Vec2vf<M> uv() const { return Vec2vf<M>(vu,vv); }
      __forceinline vfloat<M> t () const { return vt; }
      __forceinline Vec3vf<M> Ng() const { return vNg; }
    
      __forceinline Vec2f uv (const size_t i) const { return Vec2f(vu[i],vv[i]); }
      __forceinline float t  (const size_t i) const { return vt[i]; }
      __forceinline Vec3fa Ng(const size_t i) const { return Vec3fa(vNg.x[i],vNg.y[i],vNg.z[i]); }
      
    private:
      const vfloat<M> U;
      const vfloat<M> V;
      const vfloat<M> UVW;
      const UVMapper& mapUV;
      
    public:
      vfloat<M> vu;
      vfloat<M> vv;
      vfloat<M> vt;
      Vec3vf<M> vNg;
    };

    template<int M, bool early_out = true>
    struct PlueckerIntersector1
    {
      __forceinline PlueckerIntersector1() {}

      __forceinline PlueckerIntersector1(const Ray& ray, const void* ptr) {}

      template<typename UVMapper, typename Epilog>
      __forceinline bool intersect(const vbool<M>& valid0,
                                   Ray& ray,
                                   const Vec3vf<M>& tri_v0,
                                   const Vec3vf<M>& tri_v1,
                                   const Vec3vf<M>& tri_v2,
                                   const UVMapper& mapUV,
                                   const Epilog& epilog) const
      {
        vbool<M> valid = valid0;
        
        /* calculate vertices relative to ray origin */
        const Vec3vf<M> O = Vec3vf<M>((Vec3fa)ray.org);
	const Vec3vf<M> D = Vec3vf<M>((Vec3fa)ray.dir);
        const Vec3vf<M> v0 = tri_v0-O;
        const Vec3vf<M> v1 = tri_v1-O;
        const Vec3vf<M> v2 = tri_v2-O;

        /* calculate triangle edges */
        const Vec3vf<M> e0 = v2-v0;
        const Vec3vf<M> e1 = v0-v1;
        const Vec3vf<M> e2 = v1-v2;

        /* perform edge tests */
        const vfloat<M> U = dot(cross(e0,v2+v0),D);
        const vfloat<M> V = dot(cross(e1,v0+v1),D);
        const vfloat<M> W = dot(cross(e2,v1+v2),D);
        const vfloat<M> UVW = U+V+W;
        const vfloat<M> eps = float(ulp)*abs(UVW);
#if defined(EMBREE_BACKFACE_CULLING)
        valid &= max(U,V,W) <= eps;
#else
        valid &= (min(U,V,W) >= -eps) | (max(U,V,W) <= eps);
#endif
        if (unlikely(early_out && none(valid))) return false;

        /* calculate geometry normal and denominator */
        const Vec3vf<M> Ng = stable_triangle_normal(e0,e1,e2);
        const vfloat<M> den = twice(dot(Ng,D));
        
        /* perform depth test */
        const vfloat<M> T = twice(dot(v0,Ng));
        const vfloat<M> t = rcp(den)*T;
        valid &= vfloat<M>(ray.tnear()) <= t & t <= vfloat<M>(ray.tfar);
        valid &= den != vfloat<M>(zero);
        if (unlikely(early_out && none(valid))) return false;

        /* update hit information */
        PlueckerHitM<M,UVMapper> hit(U,V,UVW,t,Ng,mapUV);
        return epilog(valid,hit);
      }

      template<typename UVMapper, typename Epilog>
      __forceinline bool intersect(Ray& ray,
                                   const Vec3vf<M>& tri_v0,
                                   const Vec3vf<M>& tri_v1,
                                   const Vec3vf<M>& tri_v2,
                                   const UVMapper& mapUV,
                                   const Epilog& epilog) const
      {
        return intersect(true,ray,tri_v0,tri_v1,tri_v2,mapUV,epilog);
      }

      template<typename Epilog>
      __forceinline bool intersect(Ray& ray,
                                   const Vec3vf<M>& tri_v0,
                                   const Vec3vf<M>& tri_v1,
                                   const Vec3vf<M>& tri_v2,
                                   const Epilog& epilog) const
      {
        return intersect(ray,tri_v0,tri_v1,tri_v2,UVIdentity<M>(),epilog);
      }

      template<typename Epilog>
      __forceinline bool intersect(const vbool<M>& valid0,
                                   Ray& ray,
                                   const Vec3vf<M>& tri_v0,
                                   const Vec3vf<M>& tri_v1,
                                   const Vec3vf<M>& tri_v2,
                                   const Epilog& epilog) const
      {
        return intersect(valid0,ray,tri_v0,tri_v1,tri_v2,UVIdentity<M>(),epilog);
      }
    };

    template<int K, typename UVMapper>
    struct PlueckerHitK
    {
      __forceinline PlueckerHitK(const vfloat<K>& U, const vfloat<K>& V, const vfloat<K>& UVW, const vfloat<K>& t, const Vec3vf<K>& Ng, const UVMapper& mapUV)
        : U(U), V(V), UVW(UVW), t(t), Ng(Ng), mapUV(mapUV) {}
      
      __forceinline std::tuple<vfloat<K>,vfloat<K>,vfloat<K>,Vec3vf<K>> operator() () const
      {
        const vbool<K> invalid = abs(UVW) < min_rcp_input;
        const vfloat<K> rcpUVW = select(invalid,vfloat<K>(0.0f),rcp(UVW));
        vfloat<K> u = U * rcpUVW;
        vfloat<K> v = V * rcpUVW;
        Vec3vf<K> vNg = Ng;
        mapUV(u,v,vNg);
        return std::make_tuple(u,v,t,vNg);
      }
      
    private:
      const vfloat<K> U;
      const vfloat<K> V;
      const vfloat<K> UVW;
      const vfloat<K> t;
      const Vec3vf<K> Ng;
      const UVMapper& mapUV;
    };
    
    template<int M, int K>
    struct PlueckerIntersectorK
    {
      __forceinline PlueckerIntersectorK(const vbool<K>& valid, const RayK<K>& ray) {}

      /*! Intersects K rays with one of M triangles. */
      template<typename UVMapper, typename Epilog>
      __forceinline vbool<K> intersectK(const vbool<K>& valid0,
                                        RayK<K>& ray,
                                        const Vec3vf<K>& tri_v0,
                                        const Vec3vf<K>& tri_v1,
                                        const Vec3vf<K>& tri_v2,
                                        const UVMapper& mapUV,
                                        const Epilog& epilog) const
      {
        /* calculate vertices relative to ray origin */
        vbool<K> valid = valid0;
        const Vec3vf<K> O = ray.org;
        const Vec3vf<K> D = ray.dir;
        const Vec3vf<K> v0 = tri_v0-O;
        const Vec3vf<K> v1 = tri_v1-O;
        const Vec3vf<K> v2 = tri_v2-O;

        /* calculate triangle edges */
        const Vec3vf<K> e0 = v2-v0;
        const Vec3vf<K> e1 = v0-v1;
        const Vec3vf<K> e2 = v1-v2;

        /* perform edge tests */
        const vfloat<K> U = dot(Vec3vf<K>(cross(e0,v2+v0)),D);
        const vfloat<K> V = dot(Vec3vf<K>(cross(e1,v0+v1)),D);
        const vfloat<K> W = dot(Vec3vf<K>(cross(e2,v1+v2)),D);
        const vfloat<K> UVW = U+V+W;
        const vfloat<K> eps = float(ulp)*abs(UVW);
#if defined(EMBREE_BACKFACE_CULLING)
        valid &= max(U,V,W) <= eps;
#else
        valid &= (min(U,V,W) >= -eps) | (max(U,V,W) <= eps);
#endif
        if (unlikely(none(valid))) return false;

         /* calculate geometry normal and denominator */
        const Vec3vf<K> Ng = stable_triangle_normal(e0,e1,e2);
        const vfloat<K> den = twice(dot(Vec3vf<K>(Ng),D));

        /* perform depth test */
        const vfloat<K> T = twice(dot(v0,Vec3vf<K>(Ng)));
        const vfloat<K> t = rcp(den)*T;
        valid &= ray.tnear() <= t & t <= ray.tfar;
        valid &= den != vfloat<K>(zero);
        if (unlikely(none(valid))) return false;
        
        /* calculate hit information */
        PlueckerHitK<K,UVMapper> hit(U,V,UVW,t,Ng,mapUV);
        return epilog(valid,hit);
      }

      template<typename Epilog>
      __forceinline vbool<K> intersectK(const vbool<K>& valid0,
                                        RayK<K>& ray,
                                        const Vec3vf<K>& tri_v0,
                                        const Vec3vf<K>& tri_v1,
                                        const Vec3vf<K>& tri_v2,
                                        const Epilog& epilog) const
      {
        return intersectK(valid0,ray,tri_v0,tri_v1,tri_v2,UVIdentity<K>(),epilog);
      }

      /*! Intersect k'th ray from ray packet of size K with M triangles. */
      template<typename UVMapper, typename Epilog>
      __forceinline bool intersect(RayK<K>& ray, size_t k,
                                   const Vec3vf<M>& tri_v0,
                                   const Vec3vf<M>& tri_v1,
                                   const Vec3vf<M>& tri_v2,
                                   const UVMapper& mapUV,
                                   const Epilog& epilog) const
      {
        /* calculate vertices relative to ray origin */
        const Vec3vf<M> O = broadcast<vfloat<M>>(ray.org,k);
        const Vec3vf<M> D = broadcast<vfloat<M>>(ray.dir,k);
        const Vec3vf<M> v0 = tri_v0-O;
        const Vec3vf<M> v1 = tri_v1-O;
        const Vec3vf<M> v2 = tri_v2-O;

        /* calculate triangle edges */
        const Vec3vf<M> e0 = v2-v0;
        const Vec3vf<M> e1 = v0-v1;
        const Vec3vf<M> e2 = v1-v2;

        /* perform edge tests */
        const vfloat<M> U = dot(cross(e0,v2+v0),D);
        const vfloat<M> V = dot(cross(e1,v0+v1),D);
        const vfloat<M> W = dot(cross(e2,v1+v2),D);
        const vfloat<M> UVW = U+V+W;
        const vfloat<M> eps = float(ulp)*abs(UVW);
#if defined(EMBREE_BACKFACE_CULLING)
        vbool<M> valid = max(U,V,W) <= eps;
#else
        vbool<M> valid = (min(U,V,W) >= -eps) | (max(U,V,W) <= eps);
#endif
        if (unlikely(none(valid))) return false;

        /* calculate geometry normal and denominator */
        const Vec3vf<M> Ng = stable_triangle_normal(e0,e1,e2);
        const vfloat<M> den = twice(dot(Ng,D));
        
        /* perform depth test */
        const vfloat<M> T = twice(dot(v0,Ng));
        const vfloat<M> t = rcp(den)*T;
        valid &= vfloat<M>(ray.tnear()[k]) <= t & t <= vfloat<M>(ray.tfar[k]);
        if (unlikely(none(valid))) return false;

        /* avoid division by 0 */
        valid &= den != vfloat<M>(zero);
        if (unlikely(none(valid))) return false;

        /* update hit information */
        PlueckerHitM<M,UVMapper> hit(U,V,UVW,t,Ng,mapUV);
        return epilog(valid,hit);
      }

      template<typename Epilog>
      __forceinline bool intersect(RayK<K>& ray, size_t k,
                                   const Vec3vf<M>& tri_v0,
                                   const Vec3vf<M>& tri_v1,
                                   const Vec3vf<M>& tri_v2,
                                   const Epilog& epilog) const
      {
        return intersect(ray,k,tri_v0,tri_v1,tri_v2,UVIdentity<M>(),epilog);
      }
    };
  }
}
