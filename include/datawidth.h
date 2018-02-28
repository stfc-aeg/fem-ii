#ifndef DATAWIDTH_H_
#define DATAWIDTH_H_

#include "femii_include.h"

namespace Femii{
    typedef unsigned char u8;

    //  Supported data widths, default = WIDTH_UNSUPPORTED (-1) -- moved to payload as specific to these?
    enum DataWidth{

        WIDTH_UNSUPPORTED = -1, 
        WIDTH_BYTE = 0,         //could do 2^8  -- 8bits
        WIDTH_WORD = 1,         //16 bits
        WIDTH_LONG = 2,         //32 bits
        
    };
    //! Internal bi-directional mapping of Data width from string to enumerated DataWidth
    typedef boost::bimap<std::string, DataWidth> DataWidthMap;
    typedef DataWidthMap::value_type DataWidthMapEntry;
    static DataWidthMap data_width_map_;          //!< Bi-directional data width map

    //  Initialises the data_width_map and returns a string representation of the datawidth for printing
    inline static std::string init_data_width_map(DataWidth the_width){
        if(data_width_map_.size() == 0){
            data_width_map_.insert(DataWidthMapEntry("byte", WIDTH_BYTE));
            data_width_map_.insert(DataWidthMapEntry("word", WIDTH_WORD));
            data_width_map_.insert(DataWidthMapEntry("long", WIDTH_LONG));
            data_width_map_.insert(DataWidthMapEntry("unsupported width", WIDTH_UNSUPPORTED));
        }
        if (data_width_map_.right.count(the_width))
        {
            return data_width_map_.right.at(the_width);
        }
        else{
            return "undefined"; // to do exception
        }   
    }
}
#endif