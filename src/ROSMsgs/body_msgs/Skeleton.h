/* Software License Agreement (BSD License)
 *
 * Copyright (c) 2011, Willow Garage, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  * Neither the name of Willow Garage, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Auto-generated by genmsg_cpp from file F:\ROS\work\ws\kinect_utils\body_msgs\msg\Skeleton.msg
 *
 */


#ifndef BODY_MSGS_MESSAGE_SKELETON_H
#define BODY_MSGS_MESSAGE_SKELETON_H


#include <string>
#include <vector>
#include <map>

#include <ros/types.h>
#include <ros/serialization.h>
#include <ros/builtin_message_traits.h>
#include <ros/message_operations.h>

#include <body_msgs/SkeletonJoint.h>
#include <body_msgs/SkeletonJoint.h>
#include <body_msgs/SkeletonJoint.h>
#include <body_msgs/SkeletonJoint.h>
#include <body_msgs/SkeletonJoint.h>
#include <body_msgs/SkeletonJoint.h>
#include <body_msgs/SkeletonJoint.h>
#include <body_msgs/SkeletonJoint.h>
#include <body_msgs/SkeletonJoint.h>
#include <body_msgs/SkeletonJoint.h>
#include <body_msgs/SkeletonJoint.h>
#include <body_msgs/SkeletonJoint.h>
#include <body_msgs/SkeletonJoint.h>
#include <body_msgs/SkeletonJoint.h>
#include <body_msgs/SkeletonJoint.h>

namespace body_msgs
{
template <class ContainerAllocator>
struct Skeleton_
{
  typedef Skeleton_<ContainerAllocator> Type;

  Skeleton_()
    : playerid(0)
    , head()
    , neck()
    , right_hand()
    , left_hand()
    , right_shoulder()
    , left_shoulder()
    , right_elbow()
    , left_elbow()
    , torso()
    , left_hip()
    , right_hip()
    , left_knee()
    , right_knee()
    , left_foot()
    , right_foot()  {
    }
  Skeleton_(const ContainerAllocator& _alloc)
    : playerid(0)
    , head(_alloc)
    , neck(_alloc)
    , right_hand(_alloc)
    , left_hand(_alloc)
    , right_shoulder(_alloc)
    , left_shoulder(_alloc)
    , right_elbow(_alloc)
    , left_elbow(_alloc)
    , torso(_alloc)
    , left_hip(_alloc)
    , right_hip(_alloc)
    , left_knee(_alloc)
    , right_knee(_alloc)
    , left_foot(_alloc)
    , right_foot(_alloc)  {
    }



   typedef int32_t _playerid_type;
  _playerid_type playerid;

   typedef  ::body_msgs::SkeletonJoint_<ContainerAllocator>  _head_type;
  _head_type head;

   typedef  ::body_msgs::SkeletonJoint_<ContainerAllocator>  _neck_type;
  _neck_type neck;

   typedef  ::body_msgs::SkeletonJoint_<ContainerAllocator>  _right_hand_type;
  _right_hand_type right_hand;

   typedef  ::body_msgs::SkeletonJoint_<ContainerAllocator>  _left_hand_type;
  _left_hand_type left_hand;

   typedef  ::body_msgs::SkeletonJoint_<ContainerAllocator>  _right_shoulder_type;
  _right_shoulder_type right_shoulder;

   typedef  ::body_msgs::SkeletonJoint_<ContainerAllocator>  _left_shoulder_type;
  _left_shoulder_type left_shoulder;

   typedef  ::body_msgs::SkeletonJoint_<ContainerAllocator>  _right_elbow_type;
  _right_elbow_type right_elbow;

   typedef  ::body_msgs::SkeletonJoint_<ContainerAllocator>  _left_elbow_type;
  _left_elbow_type left_elbow;

   typedef  ::body_msgs::SkeletonJoint_<ContainerAllocator>  _torso_type;
  _torso_type torso;

   typedef  ::body_msgs::SkeletonJoint_<ContainerAllocator>  _left_hip_type;
  _left_hip_type left_hip;

   typedef  ::body_msgs::SkeletonJoint_<ContainerAllocator>  _right_hip_type;
  _right_hip_type right_hip;

   typedef  ::body_msgs::SkeletonJoint_<ContainerAllocator>  _left_knee_type;
  _left_knee_type left_knee;

   typedef  ::body_msgs::SkeletonJoint_<ContainerAllocator>  _right_knee_type;
  _right_knee_type right_knee;

   typedef  ::body_msgs::SkeletonJoint_<ContainerAllocator>  _left_foot_type;
  _left_foot_type left_foot;

   typedef  ::body_msgs::SkeletonJoint_<ContainerAllocator>  _right_foot_type;
  _right_foot_type right_foot;




  typedef boost::shared_ptr< ::body_msgs::Skeleton_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::body_msgs::Skeleton_<ContainerAllocator> const> ConstPtr;
  boost::shared_ptr<std::map<std::string, std::string> > __connection_header;

}; // struct Skeleton_

typedef ::body_msgs::Skeleton_<std::allocator<void> > Skeleton;

typedef boost::shared_ptr< ::body_msgs::Skeleton > SkeletonPtr;
typedef boost::shared_ptr< ::body_msgs::Skeleton const> SkeletonConstPtr;

// constants requiring out of line definition



template<typename ContainerAllocator>
std::ostream& operator<<(std::ostream& s, const ::body_msgs::Skeleton_<ContainerAllocator> & v)
{
ros::message_operations::Printer< ::body_msgs::Skeleton_<ContainerAllocator> >::stream(s, "", v);
return s;
}

} // namespace body_msgs

namespace ros
{
namespace message_traits
{



// BOOLTRAITS {'IsFixedSize': True, 'IsMessage': True, 'HasHeader': False}
// {'geometric_shapes_msgs': ['F:/ROS/work/ws/geometric_shapes_msgs/msg'], 'std_msgs': ['F:/ROS/work/ws/std_msgs/msg'], 'body_msgs': ['F:/ROS/work/ws/kinect_utils/body_msgs/msg', 'F:/ROS/work/ws/kinect_utils/body_msgs/msg'], 'geometry_msgs': ['F:/ROS/work/ws/geometry_msgs/msg'], 'sensor_msgs': ['F:/ROS/work/ws/sensor_msgs/msg']}

// !!!!!!!!!!! ['__class__', '__delattr__', '__dict__', '__doc__', '__eq__', '__format__', '__getattribute__', '__hash__', '__init__', '__module__', '__ne__', '__new__', '__reduce__', '__reduce_ex__', '__repr__', '__setattr__', '__sizeof__', '__str__', '__subclasshook__', '__weakref__', '_parsed_fields', 'constants', 'fields', 'full_name', 'has_header', 'header_present', 'names', 'package', 'parsed_fields', 'short_name', 'text', 'types']




template <class ContainerAllocator>
struct IsFixedSize< ::body_msgs::Skeleton_<ContainerAllocator> >
  : TrueType
  { };

template <class ContainerAllocator>
struct IsFixedSize< ::body_msgs::Skeleton_<ContainerAllocator> const>
  : TrueType
  { };

template <class ContainerAllocator>
struct IsMessage< ::body_msgs::Skeleton_<ContainerAllocator> >
  : TrueType
  { };

template <class ContainerAllocator>
struct IsMessage< ::body_msgs::Skeleton_<ContainerAllocator> const>
  : TrueType
  { };

template <class ContainerAllocator>
struct HasHeader< ::body_msgs::Skeleton_<ContainerAllocator> >
  : FalseType
  { };

template <class ContainerAllocator>
struct HasHeader< ::body_msgs::Skeleton_<ContainerAllocator> const>
  : FalseType
  { };


template<class ContainerAllocator>
struct MD5Sum< ::body_msgs::Skeleton_<ContainerAllocator> >
{
  static const char* value()
  {
    return "0c41aa8101907706f9e2c5e6f1a31dfd";
  }

  static const char* value(const ::body_msgs::Skeleton_<ContainerAllocator>&) { return value(); }
  static const uint64_t static_value1 = 0x0c41aa8101907706ULL;
  static const uint64_t static_value2 = 0xf9e2c5e6f1a31dfdULL;
};

template<class ContainerAllocator>
struct DataType< ::body_msgs::Skeleton_<ContainerAllocator> >
{
  static const char* value()
  {
    return "body_msgs/Skeleton";
  }

  static const char* value(const ::body_msgs::Skeleton_<ContainerAllocator>&) { return value(); }
};

template<class ContainerAllocator>
struct Definition< ::body_msgs::Skeleton_<ContainerAllocator> >
{
  static const char* value()
  {
    return "int32 playerid\n\
body_msgs/SkeletonJoint head\n\
body_msgs/SkeletonJoint neck\n\
body_msgs/SkeletonJoint right_hand\n\
body_msgs/SkeletonJoint left_hand\n\
body_msgs/SkeletonJoint right_shoulder\n\
body_msgs/SkeletonJoint left_shoulder\n\
body_msgs/SkeletonJoint right_elbow\n\
body_msgs/SkeletonJoint left_elbow\n\
body_msgs/SkeletonJoint torso\n\
body_msgs/SkeletonJoint left_hip\n\
body_msgs/SkeletonJoint right_hip\n\
body_msgs/SkeletonJoint left_knee\n\
body_msgs/SkeletonJoint right_knee\n\
body_msgs/SkeletonJoint left_foot\n\
body_msgs/SkeletonJoint right_foot\n\
\n\
================================================================================\n\
MSG: body_msgs/SkeletonJoint\n\
geometry_msgs/Point position\n\
float32 confidence\n\
================================================================================\n\
MSG: geometry_msgs/Point\n\
# This contains the position of a point in free space\n\
float64 x\n\
float64 y\n\
float64 z\n\
\n\
";
  }

  static const char* value(const ::body_msgs::Skeleton_<ContainerAllocator>&) { return value(); }
};

} // namespace message_traits
} // namespace ros

namespace ros
{
namespace serialization
{

  template<class ContainerAllocator> struct Serializer< ::body_msgs::Skeleton_<ContainerAllocator> >
  {
    template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
    {
      stream.next(m.playerid);
      stream.next(m.head);
      stream.next(m.neck);
      stream.next(m.right_hand);
      stream.next(m.left_hand);
      stream.next(m.right_shoulder);
      stream.next(m.left_shoulder);
      stream.next(m.right_elbow);
      stream.next(m.left_elbow);
      stream.next(m.torso);
      stream.next(m.left_hip);
      stream.next(m.right_hip);
      stream.next(m.left_knee);
      stream.next(m.right_knee);
      stream.next(m.left_foot);
      stream.next(m.right_foot);
    }

    ROS_DECLARE_ALLINONE_SERIALIZER;
  }; // struct Skeleton_

} // namespace serialization
} // namespace ros

namespace ros
{
namespace message_operations
{

template<class ContainerAllocator>
struct Printer< ::body_msgs::Skeleton_<ContainerAllocator> >
{
  template<typename Stream> static void stream(Stream& s, const std::string& indent, const ::body_msgs::Skeleton_<ContainerAllocator>& v)
  {
    s << indent << "playerid: ";
    Printer<int32_t>::stream(s, indent + "  ", v.playerid);
    s << indent << "head: ";
    s << std::endl;
    Printer< ::body_msgs::SkeletonJoint_<ContainerAllocator> >::stream(s, indent + "  ", v.head);
    s << indent << "neck: ";
    s << std::endl;
    Printer< ::body_msgs::SkeletonJoint_<ContainerAllocator> >::stream(s, indent + "  ", v.neck);
    s << indent << "right_hand: ";
    s << std::endl;
    Printer< ::body_msgs::SkeletonJoint_<ContainerAllocator> >::stream(s, indent + "  ", v.right_hand);
    s << indent << "left_hand: ";
    s << std::endl;
    Printer< ::body_msgs::SkeletonJoint_<ContainerAllocator> >::stream(s, indent + "  ", v.left_hand);
    s << indent << "right_shoulder: ";
    s << std::endl;
    Printer< ::body_msgs::SkeletonJoint_<ContainerAllocator> >::stream(s, indent + "  ", v.right_shoulder);
    s << indent << "left_shoulder: ";
    s << std::endl;
    Printer< ::body_msgs::SkeletonJoint_<ContainerAllocator> >::stream(s, indent + "  ", v.left_shoulder);
    s << indent << "right_elbow: ";
    s << std::endl;
    Printer< ::body_msgs::SkeletonJoint_<ContainerAllocator> >::stream(s, indent + "  ", v.right_elbow);
    s << indent << "left_elbow: ";
    s << std::endl;
    Printer< ::body_msgs::SkeletonJoint_<ContainerAllocator> >::stream(s, indent + "  ", v.left_elbow);
    s << indent << "torso: ";
    s << std::endl;
    Printer< ::body_msgs::SkeletonJoint_<ContainerAllocator> >::stream(s, indent + "  ", v.torso);
    s << indent << "left_hip: ";
    s << std::endl;
    Printer< ::body_msgs::SkeletonJoint_<ContainerAllocator> >::stream(s, indent + "  ", v.left_hip);
    s << indent << "right_hip: ";
    s << std::endl;
    Printer< ::body_msgs::SkeletonJoint_<ContainerAllocator> >::stream(s, indent + "  ", v.right_hip);
    s << indent << "left_knee: ";
    s << std::endl;
    Printer< ::body_msgs::SkeletonJoint_<ContainerAllocator> >::stream(s, indent + "  ", v.left_knee);
    s << indent << "right_knee: ";
    s << std::endl;
    Printer< ::body_msgs::SkeletonJoint_<ContainerAllocator> >::stream(s, indent + "  ", v.right_knee);
    s << indent << "left_foot: ";
    s << std::endl;
    Printer< ::body_msgs::SkeletonJoint_<ContainerAllocator> >::stream(s, indent + "  ", v.left_foot);
    s << indent << "right_foot: ";
    s << std::endl;
    Printer< ::body_msgs::SkeletonJoint_<ContainerAllocator> >::stream(s, indent + "  ", v.right_foot);
  }
};

} // namespace message_operations
} // namespace ros

#endif // BODY_MSGS_MESSAGE_SKELETON_H
