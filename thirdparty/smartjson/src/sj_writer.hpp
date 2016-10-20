#ifndef SMARTJSON_WRITER_HPP
#define SMARTJSON_WRITER_HPP

#include <iostream>

namespace mjson
{
    class Node;
    
    class Writer
    {
    public:
        Writer(const char *tab = "\t", const char *eol = "\n");
        
        void write(const Node &node, std::ostream &out);
        
        void writeNull(const Node &node, std::ostream &out);
        void writeBool(const Node &node, std::ostream &out);
        void writeInt(const Node &node, std::ostream &out);
        void writeFloat(const Node &node, std::ostream &out);
        void writeString(const Node &node, std::ostream &out);
        void writeNode(const Node &node, std::ostream &out, int depth);
        void writeArray(const Node &node, std::ostream &out, int depth);
        void writeDict(const Node &node, std::ostream &out, int depth);
        
    private:
        const char*     tab_;
        const char*     eol_;
    };
}

#endif /* SMARTJSON_WRITER_HPP */
