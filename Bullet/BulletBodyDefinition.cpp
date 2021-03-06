
#include "BulletPch.h"
#include "Bullet/BulletBodyDefinition.h"
#include "Bullet/BulletShapes.h"

#include "Reflect/TranslatorDeduction.h"

using namespace Helium;

HELIUM_IMPLEMENT_ASSET(Helium::BulletBodyDefinition, Bullet, 0);

void Helium::BulletBodyDefinition::PopulateStructure( Reflect::Structure& comp )
{
    //TODO: m_Shapes won't properly register because BulletShape isn't recognized
    comp.AddField(&BulletBodyDefinition::m_Shapes, TXT( "m_Shapes" ));
    comp.AddField(&BulletBodyDefinition::m_Restitution, TXT( "m_Restitution" ));
    comp.AddField(&BulletBodyDefinition::m_Constrain2d, "m_Constrain2d");
}

Helium::BulletBodyDefinition::BulletBodyDefinition()
    : m_Constrain2d(false)
{

}
