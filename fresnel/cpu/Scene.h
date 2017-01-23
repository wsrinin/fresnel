// Copyright (c) 2016-2017 The Regents of the University of Michigan
// This file is part of the Fresnel project, released under the BSD 3-Clause License.

#ifndef SCENE_H_
#define SCENE_H_

#include "embree_platform.h"
#include <embree2/rtcore.h>
#include <embree2/rtcore_ray.h>
#include <pybind11/pybind11.h>

#include "Device.h"
#include "common/Material.h"
#include "common/Camera.h"

namespace fresnel { namespace cpu {

//! Thin wrapper for RTCScene
/*! Handle construction and deletion of the scene, and python lifetime as an exported class.

    Store the per geometry id materials in Scene. Embree guarantees that geometry ids will be small, increasing,
    and possibly reused when geometry is deleted. Therefore, we can efficiently store materials in a std::vector.
*/
class Scene
    {
    public:
        //! Constructor
        Scene(std::shared_ptr<Device> device);
        //! Destructor
        ~Scene();

        //! Access the RTCScene
        RTCScene& getRTCScene()
            {
            return m_scene;
            }

        //! Access the Device
        std::shared_ptr<Device> getDevice()
            {
            return m_device;
            }

        //! Set the material for a given geometry id
        void setMaterial(unsigned int geom_id, const Material& material)
            {
            if (geom_id >= m_materials.size())
                m_materials.resize(geom_id+1);

            m_materials[geom_id] = material;
            }

        //! Get the material for a given geometry id
        const Material& getMaterial(unsigned int geom_id)
            {
            if (geom_id >= m_materials.size())
                m_materials.resize(geom_id+1);

            return m_materials[geom_id];
            }

        //! Set the outline material for a given geometry id
        void setOutlineMaterial(unsigned int geom_id, const Material& material)
            {
            if (geom_id >= m_outline_materials.size())
                m_outline_materials.resize(geom_id+1);

            m_outline_materials[geom_id] = material;
            }

        //! Get the outline material for a given geometry id
        const Material& getOutlineMaterial(unsigned int geom_id)
            {
            if (geom_id >= m_outline_materials.size())
                m_outline_materials.resize(geom_id+1);

            return m_outline_materials[geom_id];
            }

        //! Set the outline width for a given geometry id
        void setOutlineWidth(unsigned int geom_id, float width)
            {
            if (geom_id >= m_outline_widths.size())
                m_outline_widths.resize(geom_id+1);

            m_outline_widths[geom_id] = width;
            }

        //! Get the outline material for a given geometry id
        float getOutlineWidth(unsigned int geom_id)
            {
            if (geom_id >= m_outline_widths.size())
                m_outline_widths.resize(geom_id+1);

            return m_outline_widths[geom_id];
            }

        //! Set the camera
        void setCamera(const Camera& camera)
            {
            m_camera = camera;
            }

        //! Get the camera
        const Camera& getCamera() const
            {
            return m_camera;
            }

        //! Set the background color
        void setBackgroundColor(const RGB<float>& c)
            {
            m_background_color = c;
            }

        //! Get the background color
        RGB<float> getBackgroundColor() const
            {
            return m_background_color;
            }

        //! Set the background alpha
        void setBackgroundAlpha(float a)
            {
            m_background_alpha = a;
            }

        //! Get the background alpha
        float getBackgroundAlpha() const
            {
            return m_background_alpha;
            }

        //! Set the light direction
        void setLightDirection(const vec3<float>& v)
            {
            vec3<float> l = v;
            l = l / sqrtf(dot(l,l));
            m_light_direction = l;
            }

        //! Get the light direction
        vec3<float> getLightDirection() const
            {
            return m_light_direction;
            }
    private:
        RTCScene m_scene;                   //!< Store the scene
        std::shared_ptr<Device> m_device;   //!< The device the scene is attached to

        std::vector<Material> m_materials;          //!< Materials associated with geometry ids
        std::vector<Material> m_outline_materials;  //!< Materials associated with geometry ids
        std::vector<float> m_outline_widths;        //!< Materials associated with geometry ids

        RGB<float> m_background_color;              //!< The background color
        float m_background_alpha;                   //!< Background alpha
        vec3<float> m_light_direction;              //!< The light direction
        Camera m_camera;                            //!< The camera
    };

//! Export Scene to python
void export_Scene(pybind11::module& m);

} } // end namespace fresnel::cpu

#endif
