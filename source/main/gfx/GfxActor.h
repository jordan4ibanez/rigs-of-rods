/*
    This source file is part of Rigs of Rods
    Copyright 2005-2012 Pierre-Michel Ricordel
    Copyright 2007-2012 Thomas Fischer
    Copyright 2016-2020 Petr Ohlidal

    For more information, see http://www.rigsofrods.org/

    Rigs of Rods is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3, as
    published by the Free Software Foundation.

    Rigs of Rods is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Rigs of Rods. If not, see <http://www.gnu.org/licenses/>.
*/

/// @file
/// @author Petr Ohlidal
/// @date   2017/04
/// @brief  Manager for all visuals belonging to a single actor.

#pragma once

#include "Actor.h"
#include "AutoPilot.h"
#include "Differentials.h"
#include "ForwardDeclarations.h"
#include "GfxData.h"
#include "RigDef_Prerequisites.h"
#include "ThreadPool.h" // class Task

#include <OgreAxisAlignedBox.h>
#include <OgreColourValue.h>
#include <OgreMaterial.h>
#include <OgreQuaternion.h>
#include <OgreTexture.h>
#include <OgreVector3.h>
#include <string>
#include <vector>

namespace RoR {

class GfxActor
{
    friend class ActorSpawner; // The factory

public:

    enum class VideoCamState
    {
        VCSTATE_INVALID,
        VCSTATE_DISABLED,
        VCSTATE_ENABLED_OFFLINE,
        VCSTATE_ENABLED_ONLINE,
    };

    enum class DebugViewType
    {
        DEBUGVIEW_NONE,
        DEBUGVIEW_SKELETON,
        DEBUGVIEW_NODES,
        DEBUGVIEW_BEAMS,
        DEBUGVIEW_WHEELS,
        DEBUGVIEW_SHOCKS,
        DEBUGVIEW_ROTATORS,
        DEBUGVIEW_SLIDENODES,
        DEBUGVIEW_SUBMESH,
    };

    struct SimBuffer /// Buffered simulation data
    {
        struct NodeSB
        {
            Ogre::Vector3 AbsPosition; // classic name
            bool nd_has_contact:1;
            bool nd_is_wet:1;
        };

        struct ScrewPropSB
        {
            float simbuf_sp_rudder;
            float simbuf_sp_throttle;
        };

        struct CommandKeySB
        {
            float simbuf_cmd_value;
        };

        struct AeroEngineSB
        {
            float simbuf_ae_rpm;
            float simbuf_ae_rpmpc;
            float simbuf_ae_throttle;
            float simbuf_tp_aetorque; //!< Turboprop torque, used by animation "aetorque"
            float simbuf_tp_aepitch;  //!< Turboprop pitch, used by animation "aepitch"
            float simbuf_tj_ab_thrust; //! Turbojet afterburner
            float simbuf_tj_exhaust_velo; //! Turbojet
            bool  simbuf_ae_turboprop:1; //!< This is a TurboProp/PistonProp
            bool  simbuf_ae_ignition:1;
            bool  simbuf_ae_failed:1;
            bool  simbuf_tj_afterburn:1; //! Turbojet afterburner
        };

        struct AirbrakeSB
        {
            float simbuf_ab_ratio;
        };

        std::unique_ptr<NodeSB>     simbuf_nodes;
        Ogre::Vector3               simbuf_pos                = Ogre::Vector3::ZERO;
        Ogre::Vector3               simbuf_node0_velo         = Ogre::Vector3::ZERO;
        float                       simbuf_rotation           = 0;
        float                       simbuf_tyre_pressure      = 0;
        bool                        simbuf_tyre_pressurizing  = false;
        Ogre::AxisAlignedBox        simbuf_aabb               = Ogre::AxisAlignedBox::BOX_NULL;
        std::string                 simbuf_net_username;
        int                         simbuf_net_colornum;
        int                         simbuf_gear               = 0;
        int                         simbuf_autoshift          = 0;
        float                       simbuf_wheel_speed        = 0;
        float                       simbuf_engine_rpm         = 0;
        float                       simbuf_engine_crankfactor = 0;
        float                       simbuf_engine_turbo_psi   = 0;
        float                       simbuf_engine_accel       = 0;
        float                       simbuf_engine_torque      = 0;
        float                       simbuf_inputshaft_rpm     = 0;     // Land vehicle only
        float                       simbuf_drive_ratio        = 0;     // Land vehicle only
        bool                        simbuf_beaconlight_active = false;
        bool                        simbuf_smoke_enabled      = false;
        float                       simbuf_hydro_dir_state    = 0;     // State of steering actuator ('hydro'), for steeringwheel display
        float                       simbuf_hydro_aileron_state = 0;
        float                       simbuf_hydro_elevator_state = 0;
        float                       simbuf_hydro_aero_rudder_state = 0;
        int                         simbuf_cur_cinecam        = 0;
        std::vector<ScrewPropSB>    simbuf_screwprops;
        std::vector<CommandKeySB>   simbuf_commandkey;
        std::vector<AeroEngineSB>   simbuf_aeroengines;
        std::vector<AirbrakeSB>     simbuf_airbrakes;
        DiffType                    simbuf_diff_type          = DiffType::SPLIT_DIFF;
        bool                        simbuf_parking_brake      = false;
        float                       simbuf_brake              = 0;
        float                       simbuf_clutch             = 0;
        int                         simbuf_aero_flap_state    = 0;
        int                         simbuf_airbrake_state     = 0;
        float                       simbuf_wing4_aoa          = 0;
        bool                        simbuf_headlight_on       = 0;
        Ogre::Vector3               simbuf_direction          = Ogre::Vector3::ZERO;         //!< Output of `Actor::getDirection()`
        float                       simbuf_top_speed          = 0;
        // Gameplay state
        ActorState                  simbuf_actor_state        = ActorState::LOCAL_SLEEPING;
        bool                        simbuf_physics_paused     = false;
        // Autopilot
        int                         simbuf_ap_heading_mode    = Autopilot::HEADING_NONE;
        int                         simbuf_ap_heading_value   = 0;
        int                         simbuf_ap_alt_mode        = Autopilot::ALT_NONE;
        int                         simbuf_ap_alt_value       = 1000; // from AutoPilot::reset()
        bool                        simbuf_ap_ias_mode        = false;
        int                         simbuf_ap_ias_value       = 150; // from AutoPilot::reset()
        bool                        simbuf_ap_gpws_mode       = false;
        bool                        simbuf_ap_ils_available   = false;
        float                       simbuf_ap_ils_vdev        = 0;
        float                       simbuf_ap_ils_hdev        = 0;
        int                         simbuf_ap_vs_value        = 0;
    };

    struct Attributes    //!< Actor visual attributes
    {
        float            xa_speedo_highest_kph;
        bool             xa_speedo_use_engine_max_rpm;
        int              xa_num_gears; //!< Gearbox
        float            xa_engine_max_rpm;
        NodeNum_t        xa_camera0_pos_node;
        NodeNum_t        xa_camera0_roll_node;
        int              xa_driveable;
        bool             xa_has_autopilot;
        bool             xa_has_engine;
        Ogre::MaterialPtr xa_help_mat;
        Ogre::TexturePtr  xa_help_tex;
    };

    GfxActor(Actor* actor, ActorSpawner* spawner, std::string ogre_resource_group,
        std::vector<NodeGfx>& gfx_nodes, RoR::Renderdash* renderdash);

    ~GfxActor();

    // Adding elements

    void                 AddMaterialFlare(int flare_index, Ogre::MaterialPtr mat);
    void                 RegisterCabMaterial(Ogre::MaterialPtr mat, Ogre::MaterialPtr mat_trans);
    void                 RegisterCabMesh(Ogre::Entity* ent, Ogre::SceneNode* snode, FlexObj* flexobj);
    void                 AddRod(int beam_index, int node1_index, int node2_index, const char* material_name, bool visible, float diameter_meters);
    void                 SetWheelVisuals(uint16_t index, WheelGfx wheel_gfx);
    void                 RegisterAirbrakes();
    void                 RegisterProps(std::vector<Prop> const& props, int driverseat_prop_idx);
    void                 AddFlexbody(FlexBody* fb) { m_flexbodies.push_back(fb); }
    void                 SortFlexbodies();

    // Visual changes

    void                 SetMaterialFlareOn(int flare_index, bool state_on);
    void                 SetCabLightsActive(bool state_on);
    void                 SetVideoCamState(VideoCamState state);
    void                 ScaleActor(Ogre::Vector3 relpos, float ratio);
    void                 ToggleDebugView();
    void                 CycleDebugViews();
    void                 ResetFlexbodies();
    void                 SetRenderdashActive(bool active);
    void                 SetBeaconsEnabled(bool beacon_light_is_active);
    void                 SetDebugView(DebugViewType dv);

    // Visibility

    void                 SetRodsVisible(bool visible);
    void                 SetFlexbodyVisible (bool visible);
    void                 SetWheelsVisible(bool value);
    void                 SetAllMeshesVisible(bool value);
    void                 SetWingsVisible(bool visible);
    void                 SetCastShadows(bool value);
    void                 SetFlexbodiesVisible(bool visible);
    void                 SetPropsVisible(bool visible);
    void                 SetAeroEnginesVisible(bool visible);


    // Visual updates

    void                 UpdateVideoCameras(float dt_sec);
    void                 UpdateParticles(float dt_sec);
    void                 UpdateRods();
    void                 UpdateWheelVisuals();
    void                 UpdateFlexbodies();
    void                 UpdateDebugView();
    void                 UpdateCabMesh();
    void                 UpdateWingMeshes();
    void                 UpdateBeaconFlare(Prop & prop, float dt, bool is_player_actor);
    void                 UpdateProps(float dt, bool is_player_actor);
    void                 UpdatePropAnimations(float dt, bool is_player_connected);
    void                 UpdateAirbrakes();
    void                 UpdateCParticles();
    void                 UpdateAeroEngines();
    void                 UpdateNetLabels(float dt);
    void                 UpdateFlares(float dt_sec, bool is_player);
    void                 UpdateRenderdashRTT ();

    // Internal updates

    void                 UpdateSimDataBuffer(); //!< Copies sim. data from `Actor` to `GfxActor` for later update
    void                 FinishWheelUpdates();
    void                 FinishFlexbodyTasks();

    // Helpers

    bool                 IsActorLive() const; //!< Should the visuals be updated for this actor?
    bool                 IsActorInitialized() const  { return m_initialized; } //!< Temporary TODO: Remove once the spawn routine is fixed
    void                 InitializeActor() { m_initialized = true; } //!< Temporary TODO: Remove once the spawn routine is fixed
    void                 CalculateDriverPos(Ogre::Vector3& out_pos, Ogre::Quaternion& out_rot);
    int                  GetActorId() const;
    int                  GetActorState() const;
    int                  GetNumFlexbodies() const { return static_cast<int>(m_flexbodies.size()); }
    ActorType            GetActorDriveable() const;
    Attributes&          GetAttributes() { return m_attr; }
    Ogre::MaterialPtr&   GetCabTransMaterial() { return m_cab_mat_visual_trans; }
    VideoCamState        GetVideoCamState() const { return m_vidcam_state; }
    DebugViewType        GetDebugView() const { return m_debug_view; }
    SimBuffer &          GetSimDataBuffer() { return m_simbuf; }
    SimBuffer::NodeSB*   GetSimNodeBuffer() { return m_simbuf.simbuf_nodes.get(); }
    std::set<GfxActor*>  GetLinkedGfxActors() { return m_linked_gfx_actors; }
    Ogre::String         GetResourceGroup() { return m_custom_resource_group; }
    Actor*               GetActor() { return m_actor; } // Watch out for multithreading with this!
    int                  FetchNumBeams() const ;
    int                  FetchNumNodes() const ;
    int                  FetchNumWheelNodes() const ;
    bool                 HasDriverSeatProp() const { return m_driverseat_prop_index != -1; }
    void                 CalcPropAnimation(const int flag_state, float& cstate, int& div, float timer,
                                              const float lower_limit, const float upper_limit, const float option3);

private:

    static Ogre::Quaternion SpecialGetRotationTo(const Ogre::Vector3& src, const Ogre::Vector3& dest);

    Actor*                      m_actor;

    std::string                 m_custom_resource_group;
    std::vector<FlareMaterial>  m_flare_materials;
    VideoCamState               m_vidcam_state;
    std::vector<VideoCamera>    m_videocameras;
    DebugViewType               m_debug_view;
    DebugViewType               m_last_debug_view;
    std::vector<NodeGfx>        m_gfx_nodes;
    std::vector<AirbrakeGfx>    m_gfx_airbrakes;
    std::vector<Prop>           m_props;
    std::vector<FlexBody*>      m_flexbodies;
    int                         m_driverseat_prop_index;
    Attributes                  m_attr;
    DustPool*                   m_particles_drip;
    DustPool*                   m_particles_misc; // This is "dust" in RoR::GfxScene; handles dust, vapour and tyre smoke
    DustPool*                   m_particles_splash;
    DustPool*                   m_particles_ripple;
    DustPool*                   m_particles_sparks;
    DustPool*                   m_particles_clump;
    std::vector<Rod>            m_rods;
    std::vector<WheelGfx>       m_wheels;
    Ogre::SceneNode*            m_rods_parent_scenenode;
    RoR::Renderdash*            m_renderdash;
    std::vector<std::shared_ptr<Task>> m_flexwheel_tasks;
    std::vector<std::shared_ptr<Task>> m_flexbody_tasks;
    bool                        m_beaconlight_active;
    float                       m_prop_anim_crankfactor_prev;
    float                       m_prop_anim_shift_timer;
    int                         m_prop_anim_prev_gear;
    std::set<GfxActor*>         m_linked_gfx_actors;

    bool                        m_initialized;

    SimBuffer                   m_simbuf;

    // Old cab mesh
    FlexObj*                    m_cab_mesh;
    Ogre::SceneNode*            m_cab_scene_node;
    Ogre::Entity*               m_cab_entity;

    // Cab materials and their features
    Ogre::MaterialPtr           m_cab_mat_visual; //!< Updated in-place from templates
    Ogre::MaterialPtr           m_cab_mat_visual_trans;
    Ogre::MaterialPtr           m_cab_mat_template_plain;
    Ogre::MaterialPtr           m_cab_mat_template_emissive;
};

} // Namespace RoR
