#ifndef SMARTJSON_PARSER_HPP
#define SMARTJSON_PARSER_HPP

#include "sj_node.hpp"

namespace mjson
{
    class Reader;
    
    /** json parser.
     *  bnf: http://www.json.org/json-zh.html
     *  NOTICE: The format '\uxxxx' doesn't supported.
     */
    class Parser
    {
    public:
        explicit Parser(IAllocator *allocator = 0);
        ~Parser();
        
        // return one of `ResultCode`.
        bool parseFromFile(const char *fileName);
        bool parseFromData(const char *str, size_t length);
        
        Node getRoot() const{ return root_; }
        int getErrorCode() const { return errorCode_; }
        int getErrorOffset() const { return errorOffset_; }

    private:
        
        int parseDict(Node &node, Reader &reader);
        int parseArray(Node &node, Reader &reader);
        int parseNumber(Node &node, Reader &reader);
        int parseString(Node &node, Reader &reader);
        int parseTrue(Node &node, Reader &reader);
        int parseFalse(Node &node, Reader &reader);
        int parseNull(Node &node, Reader &reader);
        int parseValue(Node &node, Reader &reader);
        
        Node            root_;
        IAllocator*     allocator_;
        int 			errorCode_;
        int 			errorOffset_;
    };
}

#endif /* SMARTJSON_PARSER_HPP */
