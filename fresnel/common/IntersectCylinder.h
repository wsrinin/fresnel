// Copyright (c) 2016-2017 The Regents of the University of Michigan
// This file is part of the Fresnel project, released under the BSD 3-Clause License.

#ifndef __INTERSECT_CYLINDER_H__
#define __INTERSECT_CYLINDER_H__

#include "IntersectSphere.h"
#include <iostream>

// need to declare these class methods with __device__ qualifiers when building in nvcc
// DEVICE is __host__ __device__ when included in nvcc and blank when included into the host compiler
#undef DEVICE
#ifdef NVCC
#define DEVICE __host__ __device__
#else
#define DEVICE
#endif

namespace fresnel {

const float cylinder_epsilon = 1e-4f;

//! Ray-cylinder intersection test
/*! \param t [out] Intersection t value along ray
    \param d_edge [out] Distance from shape edge in the view plane
    \param N [out] Normal vector
    \param o Ray origin
    \param d Ray direction (normalized)
    \param A Cylinder starting point
    \param B Cylinder ending point
    \param r Cylinder radius

    \returns True if the ray intersects the cylinder, False if it does not.

    Output arguments \a d and \a N are set when the intersection routine returns true.
    \a t may be set even if there is no intersection.
*/
DEVICE inline bool intersect_ray_cylinder(float& t,
                                          float& d_edge,
                                          vec3<float>& N,
                                          const vec3<float>& o,
                                          const vec3<float>& d,
                                          const vec3<float>& A,
                                          const vec3<float>& B,
                                          const float r)
    {
    // translate to a coordinate system where A is the origin
    const vec3<float> Oa = o-A;
    const vec3<float> C = B-A;

    float cdotc_inv = 1.0f / dot(C,C);
    float cdotd = dot(C,d);
    float cdotOa = dot(C,Oa);

    // solve the quadratic equation for the intersection
    float a = (1.0f - cdotd*cdotd * cdotc_inv);
    float minus_b = -2.0f*(dot(Oa,d) - cdotOa*cdotd * cdotc_inv);
    float c = dot(Oa,Oa) - cdotOa*cdotOa * cdotc_inv - r*r;

    float det = minus_b*minus_b - 4*a*c;

    // no solution when determinant is negative
    if (det < 0)
        return false;

    // solve the quadratic equation
    det=fast::sqrt(det);

    // base case - both cases intersect behind the origin
    bool hit=false;
    vec3<float> P;
    float pdotc;

    // first case
    t = (minus_b - det) / (2.0f * a);
    if (t > cylinder_epsilon)
        {
        // find the hit point
        P = Oa + t * d;
        pdotc = dot(P, C);

        // cap the cylinder
        if (pdotc >= 0 && pdotc <= dot(C,C))
            hit = true;
        }

    // second case (origin is inside the cylinder)
    // only trigger the 2nd case if the first didn't find an intersection
    if (!hit)
        {
        t = (minus_b + det) / (2.0f * a);
        if (t > cylinder_epsilon)
            {
            // find the hit point
            P = Oa + t * d;
            pdotc = dot(P, C);

            // cap the cylinder
            if (pdotc >= 0 && pdotc <= dot(C,C))
                hit = true;
            }
        }

    if (hit)
        {
        // TODO: determine d_edge.
        d_edge = HUGE_VALF;

        // determine normal.
        N = (P - pdotc * cdotc_inv * C);

        // point normal toward the ray origin (double-sided surfaces)
        if (dot(N,d) > 0)
            N = -N;
        }

    return hit;
    }

//! Ray-spherocylinder intersection test
/*! \param t [out] Intersection t value along ray
    \param d_edge [out] Distance from shape edge in the view plane
    \param N [out] Normal vector
    \param color_index [out] Index of the color to select
    \param o Ray origin
    \param d Ray direction (normalized)
    \param A Cylinder starting point
    \param B Cylinder ending point
    \param r Cylinder radius

    \returns True if the ray intersects the spherocylinder, False if it does not.

    Output arguments \a d and \a N are set when the intersection routine returns true.
    \a t may be set even if there is no intersection.
*/
DEVICE inline bool intersect_ray_spherocylinder(float& t,
                                                float& d_edge,
                                                vec3<float>& N,
                                                unsigned int& color_index,
                                                const vec3<float>& o,
                                                const vec3<float>& d,
                                                const vec3<float>& A,
                                                const vec3<float>& B,
                                                const float r)
    {
    t = HUGE_VALF;

    float t_sc = 0;
    float d_edge_sc = HUGE_VALF;
    vec3<float> N_sc;

    bool hit_A = false, hit_B = false, hit_cyl = false;

    hit_cyl = intersect_ray_cylinder(t_sc, d_edge_sc, N_sc, o, d, A, B, r);
    if (hit_cyl && t_sc < t)
        {
        t = t_sc;
        d_edge = d_edge_sc;
        N = N_sc;
        }

    hit_A = intersect_ray_sphere(t_sc, d_edge_sc, N_sc, o, d, A, r);
    if (hit_A && (t_sc < t))
        {
        t = t_sc;
        d_edge = d_edge_sc;
        N = N_sc;
        }

    hit_B = intersect_ray_sphere(t_sc, d_edge_sc, N_sc, o, d, B, r);
    if (hit_B && (t_sc < t))
        {
        t = t_sc;
        d_edge = d_edge_sc;
        N = N_sc;
        }

    // determine color index
    const vec3<float> Oa = o-A;
    const vec3<float> C = B-A;
    const vec3<float> P = Oa + t * d;
    const float pdotc = dot(P, C);

    if (pdotc < dot(C,C) / 2.0f)
        color_index = 0;
    else
        color_index = 1;

    return hit_A || hit_B || hit_cyl;
    }

}

#undef DEVICE

#endif
