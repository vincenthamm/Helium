#pragma once

#include "SceneGraph/PivotTransform.h"
#include "SceneGraph/CurveControlPoint.h"
#include "SceneGraph/PropertiesGenerator.h"
#include "SceneGraph/VertexResource.h"

namespace Helium
{
    struct Vector3;
    struct Matrix4;

    namespace SceneGraph
    {
        class PrimitiveLocator;
        class PrimitiveCone;

        class CurveType
        {
        public:
            enum Enum
            {
                Linear,
                BSpline,
                CatmullRom,
            };

            REFLECT_DECLARE_ENUMERATION( CurveType );

            static void EnumerateEnum( Reflect::Enumeration& info )
            {
                info.AddElement(Linear,     TXT( "Linear" ) );
                info.AddElement(BSpline,    TXT( "BSpline" ) );
                info.AddElement(CatmullRom, TXT( "CatmullRom" ) );
            }
        };

        class ControlPointLabel
        {
        public:
            enum Enum
            {
                None,
                CurveAndIndex,
                IndexOnly
            };

            REFLECT_DECLARE_ENUMERATION( ControlPointLabel );

            static void EnumerateEnum( Reflect::Enumeration& info )
            {
                info.AddElement( None,          TXT( "None" ) );
                info.AddElement( CurveAndIndex, TXT( "Curve and Index" ) );
                info.AddElement( IndexOnly,     TXT( "Index only" ) );
            }
        };

        class Curve : public PivotTransform
        {
        public:
            REFLECT_DECLARE_OBJECT( Curve, PivotTransform );
            static void PopulateStructure( Reflect::Structure& comp );
            static void InitializeType();
            static void CleanupType();

        public:
            Curve();
            ~Curve();

            virtual void Initialize() HELIUM_OVERRIDE;

            // Curve Type
            int GetCurveType() const;
            void SetCurveType( int value );

            // Curve closed
            bool GetClosed() const;
            void SetClosed( bool value );

            // Curve resolution
            uint32_t GetResolution() const;
            void SetResolution( uint32_t value );

            // Control point labeling
            int GetControlPointLabel() const;
            void SetControlPointLabel( int value );

            // the single closest control point
            int ClosestControlPoint( PickVisitor* pick );

            // the two closest control points
            bool ClosestControlPoints(PickVisitor* pick, std::pair<uint32_t, uint32_t>& result);

            // the closest point on the computed curve
            int32_t ClosestPoint( PickVisitor* pick );

            // the closest point on the computed curve to the provided point
            int32_t ClosestPoint(Vector3& pos);

            // the number of control points the determine the computed curve
            uint32_t GetNumberControlPoints() const;

            // get control point by index
            CurveControlPoint* GetControlPointByIndex( uint32_t index );

            // linear seach for the index of a particular point
            int32_t GetIndexForControlPoint( CurveControlPoint* pc );

            // insert new control point
            UndoCommandPtr InsertControlPointAtIndex( uint32_t index, CurveControlPoint* pc );

            // remove existing control point
            UndoCommandPtr RemoveControlPointAtIndex( uint32_t index );

            // reverse curve direction
            UndoCommandPtr ReverseControlPoints();

            void ProjectPointOnCurve( const Vector3& point, Vector3& projectedPoint ) const;
            float32_t DistanceSqrToCurve( const Vector3& point ) const;
            float32_t DistanceToCurve( const Vector3& point ) const;

            //
            // Resources
            //

            virtual void Create() HELIUM_OVERRIDE;
            virtual void Delete() HELIUM_OVERRIDE;
            virtual void Populate(PopulateArgs* args);


            //
            // Evaluate/Render/Pick
            //

            virtual UndoCommandPtr CenterTransform() HELIUM_OVERRIDE;

            virtual void Evaluate( GraphDirection direction ) HELIUM_OVERRIDE;
            float32_t CalculateCurveLength() const;

            virtual void Render( RenderVisitor* render ) HELIUM_OVERRIDE;
            virtual bool Pick( PickVisitor* pick ) HELIUM_OVERRIDE;

        private:
            void ChildChangingParents( const ParentChangingArgs& args );

        protected:
            // Reflected
            bool                    m_Closed;               // Is the curve closed?
            CurveType               m_Type;                 // The degree of the curve basis function
            uint32_t				m_Resolution;           // The resolution of the points to compute
            ControlPointLabel       m_ControlPointLabel;    // How to display labels for control points
            V_Vector3				m_Points;               // The 3D locations of the computed curve points

            // Non-reflected
            PrimitiveLocator*		m_Locator;
            PrimitiveCone*			m_Cone;
            static Helium::Color    s_Material;
            static Helium::Color    s_HullMaterial;
            VertexResourcePtr       m_Vertices;
        };

        typedef Helium::StrongPtr<Curve> CurvePtr;
    }
}