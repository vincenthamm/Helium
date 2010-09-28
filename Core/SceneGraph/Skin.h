#pragma once

#include "Core/SceneGraph/SceneNode.h"

namespace Helium
{
    namespace SceneGraph
    {
        class Mesh;
        class Scene;
        class Transform;
        typedef std::vector< Transform* > V_TransformDumbPtr;

        // Influence stores the object ID indices and weights of transforms that influence or deform a vertex
        class CORE_API Influence : public Reflect::Element
        {
        public:
            // The object ID indices of the transforms that influence this vertex
            std::vector< u32 > m_Objects;

            // The weights of the transforms that influence this vertex (CORRESPONDS TO objects)
            std::vector< f32 > m_Weights;

            REFLECT_DECLARE_CLASS(Influence, Reflect::Element);

            static void EnumerateClass( Reflect::Compositor<Influence>& comp );
        };

        typedef Helium::SmartPtr<Influence> InfluencePtr;
        typedef std::vector<InfluencePtr> V_Influence;

        class Skin : public SceneNode
        {
        public:
            REFLECT_DECLARE_CLASS( Skin, SceneNode );
            static void EnumerateClass( Reflect::Compositor<Skin>& comp );
            static void InitializeType();
            static void CleanupType();

            Skin();
            ~Skin();

            virtual void Initialize(Scene* scene) HELIUM_OVERRIDE;
            virtual void Evaluate(GraphDirection direction) HELIUM_OVERRIDE;

        private:
            void BlendMatrix(const Transform* transform, const Influence* influence, Math::Matrix4& matrix);

        protected:
            // Reflected
            Helium::TUID        m_MeshID;               // The mesh we are skinning
            V_Influence         m_Influences;           // The influence data for each vertex of the mesh
            Helium::V_TUID      m_InfluenceObjectIDs;   // The influence objects required to deform this mesh, indexed by each influence item above
            std::vector< u32 >  m_InfluenceIndices;     // The influence to use for each vertex (CORRESPONDS TO m_Positions Indices in the referenced mesh)

            // Non-reflected
            Mesh*               m_Mesh;
            V_TransformDumbPtr  m_InfluenceObjects;
            Math::V_Matrix4     m_DeformMatrices;
            Math::V_Matrix4     m_SkinMatrices;
        };
    }
}