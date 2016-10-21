#include "Reference.h"

int g_ref_counter = 0;

ReferenceCount::ReferenceCount()
    : referenceCount_(0)
{
    ++g_ref_counter;
}

ReferenceCount::~ReferenceCount()
{
    --g_ref_counter;
}
