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
 * Auto-generated by genmsg_cpp from file F:\ROS\work\ws\geometry_msgs\msg\Twist.msg
 *
 */


#ifndef GEOMETRY_MSGS_MESSAGE_TWIST_H
#define GEOMETRY_MSGS_MESSAGE_TWIST_H


#include <string>
#include <vector>
#include <map>

#include <ros/types.h>
#include <ros/serialization.h>
#include <ros/builtin_message_traits.h>
#include <ros/message_operations.h>

#include <geometry_msgs/Vector3.h>
#include <geometry_msgs/Vector3.h>

namespace geometry_msgs
{
template <class ContainerAllocator>
struct Twist_
{
  typedef Twist_<ContainerAllocator> Type;

  Twist_()
    : linear()
    , angular()  {
    }
  Twist_(const ContainerAllocator& _alloc)
    : linear(_alloc)
    , angular(_alloc)  {
    }



   typedef  ::geometry_msgs::Vector3_<ContainerAllocator>  _linear_type;
  _linear_type linear;

   typedef  ::geometry_msgs::Vector3_<ContainerAllocator>  _angular_type;
  _angular_type angular;




  typedef boost::shared_ptr< ::geometry_msgs::Twist_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::geometry_msgs::Twist_<ContainerAllocator> const> ConstPtr;
  boost::shared_ptr<std::map<std::string, std::string> > __connection_header;

}; // struct Twist_

typedef ::geometry_msgs::Twist_<std::allocator<void> > Twist;

typedef boost::shared_ptr< ::geometry_msgs::Twist > TwistPtr;
typedef boost::shared_ptr< ::geometry_msgs::Twist const> TwistConstPtr;

// constants requiring out of line definition



template<typename ContainerAllocator>
std::ostream& operator<<(std::ostream& s, const ::geometry_msgs::Twist_<ContainerAllocator> & v)
{
ros::message_operations::Printer< ::geometry_msgs::Twist_<ContainerAllocator> >::stream(s, "", v);
return s;
}

} // namespace geometry_msgs

namespace ros
{
namespace message_traits
{



// BOOLTRAITS {'IsFixedSize': True, 'IsMessage': True, 'HasHeader': False}
// {'std_msgs': ['F:/ROS/work/ws/std_msgs/msg'], 'geometry_msgs': ['F:/ROS/work/ws/geometry_msgs/msg', 'F:/ROS/work/ws/geometry_msgs/msg']}

// !!!!!!!!!!! ['__class__', '__delattr__', '__dict__', '__doc__', '__eq__', '__format__', '__getattribute__', '__hash__', '__init__', '__module__', '__ne__', '__new__', '__reduce__', '__reduce_ex__', '__repr__', '__setattr__', '__sizeof__', '__str__', '__subclasshook__', '__weakref__', '_parsed_fields', 'constants', 'fields', 'full_name', 'has_header', 'header_present', 'names', 'package', 'parsed_fields', 'short_name', 'text', 'types']




template <class ContainerAllocator>
struct IsFixedSize< ::geometry_msgs::Twist_<ContainerAllocator> >
  : TrueType
  { };

template <class ContainerAllocator>
struct IsFixedSize< ::geometry_msgs::Twist_<ContainerAllocator> const>
  : TrueType
  { };

template <class ContainerAllocator>
struct IsMessage< ::geometry_msgs::Twist_<ContainerAllocator> >
  : TrueType
  { };

template <class ContainerAllocator>
struct IsMessage< ::geometry_msgs::Twist_<ContainerAllocator> const>
  : TrueType
  { };

template <class ContainerAllocator>
struct HasHeader< ::geometry_msgs::Twist_<ContainerAllocator> >
  : FalseType
  { };

template <class ContainerAllocator>
struct HasHeader< ::geometry_msgs::Twist_<ContainerAllocator> const>
  : FalseType
  { };


template<class ContainerAllocator>
struct MD5Sum< ::geometry_msgs::Twist_<ContainerAllocator> >
{
  static const char* value()
  {
    return "9f195f881246fdfa2798d1d3eebca84a";
  }

  static const char* value(const ::geometry_msgs::Twist_<ContainerAllocator>&) { return value(); }
  static const uint64_t static_value1 = 0x9f195f881246fdfaULL;
  static const uint64_t static_value2 = 0x2798d1d3eebca84aULL;
};

template<class ContainerAllocator>
struct DataType< ::geometry_msgs::Twist_<ContainerAllocator> >
{
  static const char* value()
  {
    return "geometry_msgs/Twist";
  }

  static const char* value(const ::geometry_msgs::Twist_<ContainerAllocator>&) { return value(); }
};

template<class ContainerAllocator>
struct Definition< ::geometry_msgs::Twist_<ContainerAllocator> >
{
  static const char* value()
  {
    return "# This expresses velocity in free space broken into it's linear and angular parts. \n\
Vector3  linear\n\
Vector3  angular\n\
\n\
================================================================================\n\
MSG: geometry_msgs/Vector3\n\
# This represents a vector in free space. \n\
\n\
float64 x\n\
float64 y\n\
float64 z\n\
";
  }

  static const char* value(const ::geometry_msgs::Twist_<ContainerAllocator>&) { return value(); }
};

} // namespace message_traits
} // namespace ros

namespace ros
{
namespace serialization
{

  template<class ContainerAllocator> struct Serializer< ::geometry_msgs::Twist_<ContainerAllocator> >
  {
    template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
    {
      stream.next(m.linear);
      stream.next(m.angular);
    }

    ROS_DECLARE_ALLINONE_SERIALIZER;
  }; // struct Twist_

} // namespace serialization
} // namespace ros

namespace ros
{
namespace message_operations
{

template<class ContainerAllocator>
struct Printer< ::geometry_msgs::Twist_<ContainerAllocator> >
{
  template<typename Stream> static void stream(Stream& s, const std::string& indent, const ::geometry_msgs::Twist_<ContainerAllocator>& v)
  {
    s << indent << "linear: ";
    s << std::endl;
    Printer< ::geometry_msgs::Vector3_<ContainerAllocator> >::stream(s, indent + "  ", v.linear);
    s << indent << "angular: ";
    s << std::endl;
    Printer< ::geometry_msgs::Vector3_<ContainerAllocator> >::stream(s, indent + "  ", v.angular);
  }
};

} // namespace message_operations
} // namespace ros

#endif // GEOMETRY_MSGS_MESSAGE_TWIST_H
