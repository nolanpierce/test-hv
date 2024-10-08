#ifndef SEGMENTS_GUARD_H
#define SEGMENTS_GUARD_H

union __interrupt_command_register_t
{
    unsigned __int64 full;
    struct
    {
        unsigned __int64 vector : 8;
        unsigned __int64 delivery_mode : 3;
        unsigned __int64 destination_mode : 1;
        unsigned __int64 delivery_status : 1;
        unsigned __int64 reserved_0 : 1;
        unsigned __int64 level : 1;
        unsigned __int64 trigger_mode : 1;
        unsigned __int64 reserved_1 : 2;
        unsigned __int64 destination_short : 2;
        unsigned __int64 reserved_3 : 35;
        unsigned __int64 destination : 8;
    } bits;
};

union __segment_selector_t
{
    struct
    {
        unsigned __int16 rpl : 2;
        unsigned __int16 table : 1;
        unsigned __int16 index : 13;
    };
    unsigned __int16 flags;
};

union __segment_access_rights_t
{
    struct
    {
        unsigned __int32 type : 4;
        unsigned __int32 descriptor_type : 1;
        unsigned __int32 dpl : 2;
        unsigned __int32 present : 1;
        unsigned __int32 reserved0 : 4;
        unsigned __int32 available : 1;
        unsigned __int32 long_mode : 1;
        unsigned __int32 default_big : 1;
        unsigned __int32 granularity : 1;
        unsigned __int32 unusable : 1;
        unsigned __int32 reserved1 : 15;
    };
    unsigned __int32 flags;
};

#endif //!SEGMENTS_GUARD_H