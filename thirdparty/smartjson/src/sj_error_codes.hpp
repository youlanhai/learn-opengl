#ifndef SMARTJSON_ERROR_CODES_HPP
#define SMARTJSON_ERROR_CODES_HPP

namespace mjson
{
    enum ResultCode
    {
        RC_OK,
        RC_END_OF_FILE,
        RC_OPEN_FILE_ERROR,
        RC_INVALID_JSON,
        RC_INVALID_DICT,
        RC_INVALID_KEY,
        RC_INVALID_ARRAY,
        RC_INVALID_STRING,
        RC_INVALID_NUMBER,
        RC_INVALID_NULL,
        RC_INVALID_TRUE,
        RC_INVALID_FALSE,
        RC_INVALID_TYPE,
    };
}

#endif //SMARTJSON_ERROR_CODES_HPP
