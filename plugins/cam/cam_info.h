#ifndef CAM_INFO_H
#define CAM_INFO_H

namespace CAM_INFO {
     enum SideType
     {
         Inside = -1,
         On,
         Outside
     };

     enum DirectionType
     {
         Left,
         Right
     };

     enum LeadType
     {
         None,
         Normal,
         Extension,
         HalfCircle,
         QuarterCircle,
         EighthCircle
     };

     enum ToolType
     {
         Mill,
         Drill
     };
}

#endif // CAM_INFO_H
