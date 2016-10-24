#ifndef VERTEX_DECLARATION_H
#define VERTEX_DECLARATION_H

#include "SmartPointer.h"
#include "Singleton.h"
#include "Reference.h"
#include "VertexUsage.h"

#include <unordered_map>
#include <vector>

// predefine
namespace mjson
{
    class Node;
}

////////////////////////////////////////////////////////////////////
/// VertexElement
////////////////////////////////////////////////////////////////////
class VertexElement
{
public:

    VertexUsage usage;//The vertex element usage semantic.
    int         nComponent;//The number of values in the vertex element.
    int         type;
    int         tpSize;
    bool        normalized;

    VertexElement();
    VertexElement(VertexUsage usage, int nComponent);
    VertexElement(VertexUsage usage, int nComponent,
        int type, int tpsize, bool normalized);

    int size() const;
};

////////////////////////////////////////////////////////////////////
/// VertexDeclaration
////////////////////////////////////////////////////////////////////
class VertexDeclaration : public ReferenceCount
{
public:

    explicit VertexDeclaration(const std::string & name = "");
    virtual ~VertexDeclaration();

    bool load(const mjson::Node &section);

    void addElement(const VertexElement & e);
    void addElement(VertexUsage usage, int nComponent);
    const VertexElement & getElement(size_t i) const;
    size_t getNumElement() const;
    size_t getVertexSize() const;

    const std::string & getName() const { return name_; }

    void merge(VertexDeclaration * p);

private:
    std::string name_;
    size_t vertexSize_;
    std::vector<VertexElement> elements_;
};

typedef SmartPointer<VertexDeclaration> VertexDeclarationPtr;

////////////////////////////////////////////////////////////////////
/// Vertex Declaration Manager
////////////////////////////////////////////////////////////////////
class VertexDeclMgr : public Singleton<VertexDeclMgr>
{
public:
    VertexDeclMgr();
    ~VertexDeclMgr();

	bool loadFromFile(const std::string &fileName);

    void add(VertexDeclarationPtr decl);

    VertexDeclarationPtr combine(
        VertexDeclarationPtr orig, VertexDeclarationPtr extra );

    VertexDeclarationPtr get(const std::string & name);

private:
    void init();
    
    std::unordered_map<std::string, VertexDeclarationPtr> decls_;
};

#endif //VERTEX_DECLARATION_H
