#ifndef OBBOXRENDERABLE_H_
#define OBBOXRENDERABLE_H_
 
#include <OgreSimpleRenderable.h>
#include <OgreAxisAlignedBox.h>
 
namespace Ogre {
	class OBBoxRenderable : public SimpleRenderable
	{
	    VertexData vertexes;
	    public:
	        OBBoxRenderable();
	        void setupVertices(const AxisAlignedBox& aab);
	        virtual ~OBBoxRenderable();
	        Real getSquaredViewDepth(const Camera*)const;
	        Real getBoundingRadius()const;
	        virtual void getWorldTransforms (Matrix4 *xform)const;
	};
}
 
#endif /*OBBOXRENDERABLE_H_*/