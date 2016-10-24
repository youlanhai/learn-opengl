#include "VertexDeclaration.h"
#include "Vertex.h"
#include "glconfig.h"
#include "LogTool.h"
#include "FileSystem.h"
#include <smartjson/sj_parser.hpp>

namespace
{
    std::unordered_map<std::string, VertexUsage> AttrMap;
    std::unordered_map<std::string, VertexElement> TypeMap;

    bool registerUsage()
    {
#define REGISTER_ATTR(ATTR, USAGE) \
AttrMap[ATTR] = VertexUsage::USAGE

        REGISTER_ATTR("a_position",    POSITION);
        REGISTER_ATTR("a_normal",      NORMAL);
        REGISTER_ATTR("a_color",       COLOR);
        REGISTER_ATTR("a_tangent",     TANGENT);
        REGISTER_ATTR("a_biNormal",    BINORMAL);
        REGISTER_ATTR("a_blendWeights", BLENDWEIGHTS);
        REGISTER_ATTR("a_blendIndices",BLENDINDICES);
        REGISTER_ATTR("a_pSize",       PSIZE);
        REGISTER_ATTR("a_tessFactor",  TESSFACTOR);
        REGISTER_ATTR("a_positionT",   POSITIONT);
        REGISTER_ATTR("a_fog",         FOG);
        REGISTER_ATTR("a_depth",       DEPTH);
        REGISTER_ATTR("a_sample",      SAMPLE);
        REGISTER_ATTR("a_texcoord",    TEXCOORD0);
        REGISTER_ATTR("a_texcoord0",   TEXCOORD0);
        REGISTER_ATTR("a_texcoord1",   TEXCOORD1);
        REGISTER_ATTR("a_texcoord2",   TEXCOORD2);
        REGISTER_ATTR("a_texcoord3",   TEXCOORD3);
        REGISTER_ATTR("a_texcoord4",   TEXCOORD4);
        REGISTER_ATTR("a_texcoord5",   TEXCOORD5);
        REGISTER_ATTR("a_texcoord6",   TEXCOORD6);
        REGISTER_ATTR("a_texcoord7",   TEXCOORD7);

#undef REGISTER_ATTR

#define REGISTER_TYPE(NAME, COM, TYPE, TP_SIZE, NML) \
TypeMap[NAME] = VertexElement(VertexUsage::NONE, COM, TYPE, TP_SIZE, NML)

        REGISTER_TYPE( "float1",    1, GL_FLOAT, 4, false );
	    REGISTER_TYPE( "float2",    2, GL_FLOAT, 4, false );
	    REGISTER_TYPE( "float3",    3, GL_FLOAT, 4, false );
	    REGISTER_TYPE( "float4",    4, GL_FLOAT, 4, false );
	    REGISTER_TYPE( "ubyte4",    4, GL_UNSIGNED_BYTE, 1, false );
	    REGISTER_TYPE( "ubyte4n",   4, GL_UNSIGNED_BYTE, 1, true );
        REGISTER_TYPE( "short2",    2, GL_SHORT, 2, false );
	    REGISTER_TYPE( "short4",    4, GL_SHORT, 2, false );
	    REGISTER_TYPE( "short2n",   2, GL_SHORT, 2, true );
	    REGISTER_TYPE( "short4n",   4, GL_SHORT, 2, true );
        REGISTER_TYPE( "ushort2n",  2, GL_UNSIGNED_SHORT, 2, true );
	    REGISTER_TYPE( "ushort4n",  4, GL_UNSIGNED_SHORT, 2, true );

#undef REGISTER_TYPE

        return true;
    }

    bool _token = registerUsage();


}

VertexUsage vertexAttr2Usage(const std::string & attr)
{
    auto it = AttrMap.find(attr);
    if(it != AttrMap.end())
        return it->second;

    return VertexUsage::NONE;
}

bool vertexType2Element(const std::string & type, VertexElement & e)
{
    auto it = TypeMap.find(type);
    if(it != TypeMap.end())
    {
        e = it->second;
        return true;
    }

    return false;
}

//////////////////////////////////////////////////////////////////
///
//////////////////////////////////////////////////////////////////
VertexElement::VertexElement()
    : nComponent(0)
    , type(GL_FLOAT)
    , tpSize(sizeof(float))
    , normalized(GL_TRUE)
{}

VertexElement::VertexElement(VertexUsage usage_, int nComponent_)
    : usage(usage_)
    , nComponent(nComponent_)
    , type(GL_FLOAT)
    , tpSize(sizeof(float))
    , normalized(GL_TRUE)
{}

VertexElement::VertexElement(VertexUsage usage_, int nComponent_,
    int type_, int tpsize_, bool normalized_)
    : usage(usage_)
    , nComponent(nComponent_)
    , type(type_)
    , tpSize(tpsize_)
    , normalized(normalized_)
{}

int VertexElement::size() const
{
    return tpSize * nComponent;
}

//////////////////////////////////////////////////////////////////
///
//////////////////////////////////////////////////////////////////
VertexDeclaration::VertexDeclaration(const std::string & name)
    : name_(name)
    , vertexSize_(0)
{

}

VertexDeclaration::~VertexDeclaration()
{
}

bool VertexDeclaration::load(const mjson::Node &section)
{
	if (!section.isDict())
	{
		return false;
	}

    for(auto it = section.memberBegin(); it != section.memberEnd(); ++it)
    {
		std::string usage = it->key.asStdString();
		const mjson::Node config = it->value;

		if (!config.isString())
		{
			return false;
		}

        VertexElement element;
		std::string type = config["type"].asStdString();
		if(!vertexType2Element(type, element))
        {
            LOG_ERROR("VertexDeclaration '%s' found unsupported type '%s'",
                name_.c_str(), type.c_str());
            return false;
        }

        element.usage = vertexAttr2Usage(usage);
        if(element.usage == VertexUsage::NONE)
        {
            LOG_ERROR("VertexDeclaration '%s' found unsupported usage '%s'",
                name_.c_str(), usage.c_str());
            return false;
        }
        
        this->addElement(element);
    }

    return true;
}

void VertexDeclaration::addElement(const VertexElement & e)
{
    elements_.push_back(e);
    vertexSize_ += e.size();
}

void VertexDeclaration::addElement(VertexUsage usage, int nComponent)
{
    elements_.push_back(VertexElement(usage, nComponent));
    vertexSize_ += elements_.back().size();
}

const VertexElement & VertexDeclaration::getElement(size_t i) const
{
    return elements_[i];
}

size_t VertexDeclaration::getNumElement() const
{
    return elements_.size();
}

size_t VertexDeclaration::getVertexSize() const
{
    return vertexSize_;
}

void VertexDeclaration::merge(VertexDeclaration * p)
{
    assert(p);

    for(size_t i = 0; i < p->getNumElement(); ++i)
    {
        this->addElement(p->getElement(i));
    }
}

////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////
IMPLEMENT_SINGLETON(VertexDeclMgr);

VertexDeclMgr::VertexDeclMgr()
{
    init();
}

VertexDeclMgr::~VertexDeclMgr()
{

}

void VertexDeclMgr::init()
{
    //add internal vertex format
    VertexDeclarationPtr decl;

    decl = new VertexDeclaration(VertexXYZ::getType());
    decl->addElement(VertexUsage::POSITION, 3);
    add(decl);

    decl = new VertexDeclaration(VertexXYZColor::getType());
    decl->addElement(VertexUsage::POSITION, 3);
    decl->addElement(VertexUsage::COLOR, 4);
    add(decl);

    decl = new VertexDeclaration(VertexXYZN::getType());
    decl->addElement(VertexUsage::POSITION, 3);
    decl->addElement(VertexUsage::NORMAL, 3);
    add(decl);

    decl = new VertexDeclaration(VertexXYZUV::getType());
    decl->addElement(VertexUsage::POSITION, 3);
    decl->addElement(VertexUsage::TEXCOORD0, 2);
    add(decl);

    decl = new VertexDeclaration(VertexXYZNUV::getType());
    decl->addElement(VertexUsage::POSITION, 3);
    decl->addElement(VertexUsage::NORMAL, 3);
    decl->addElement(VertexUsage::TEXCOORD0, 2);
    add(decl);

    decl = new VertexDeclaration(VertexXYZColorUV::getType());
    decl->addElement(VertexUsage::POSITION, 3);
    decl->addElement(VertexUsage::COLOR, 4);
    decl->addElement(VertexUsage::TEXCOORD0, 2);
    add(decl);
}

bool VertexDeclMgr::loadFromFile(const std::string & fileName)
{
	std::string data;
	if (!FileSystem::instance()->readFile(data, fileName))
	{
		return false;
	}

	mjson::Parser parser;
	if (!parser.parseFromData(data.c_str(), data.size()))
	{
		return false;
	}

	mjson::Node root = parser.getRoot();
	if (!root.isDict())
	{
		return false;
	}

	for (auto it = root.memberBegin(); it != root.memberEnd(); ++it)
	{
		VertexDeclarationPtr decl = new VertexDeclaration(it->key.asStdString());
		if (decl->load(it->value))
		{
			this->add(decl);
		}
	}
	return true;
}

void VertexDeclMgr::add(VertexDeclarationPtr decl)
{
    auto it = decls_.find(decl->getName());
    if (it != decls_.end())
    {
        LOG_ERROR("the vertex declaration '%s' has been exist!", decl->getName().c_str());
        return;
    }
    decls_.insert(std::make_pair(decl->getName(), decl));
}

VertexDeclarationPtr VertexDeclMgr::combine(
    VertexDeclarationPtr orig, VertexDeclarationPtr extra )
{
    std::string newName = orig->getName() + "_" + extra->getName();

    // see if the new decl has been created before..
    auto it = decls_.find( newName );

    // not found...
    if (it == decls_.end())
    {
	    // build a new declaration.
	    VertexDeclarationPtr vd = new VertexDeclaration( newName );

        vd->merge(orig.get());
        vd->merge(extra.get());

		decls_[newName] = vd;
		return vd;
    }

    return it->second;
}

VertexDeclarationPtr VertexDeclMgr::get(const std::string & name)
{
    //search it in the cache
    auto it = decls_.find(name);
    if(it != decls_.end())
        return it->second;

    return nullptr;
}

